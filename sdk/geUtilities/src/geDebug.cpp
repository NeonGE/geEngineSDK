/*****************************************************************************/
/**
 * @file    geDebug.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/09/19
 * @brief   Utility class providing various debug functionality.
 *
 * Utility class providing various debug functionality.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geDebug.h"
#include "geException.h"
#include "geBitmapWriter.h"
#include "geFileSystem.h"
#include "geDataStream.h"
#include "geTime.h"

using std::cout;
using std::endl;

#if USING(GE_PLATFORM_WINDOWS) && USING(GE_COMPILER_MSVC)
#	include "Win32/geMinWindows.h"
#	include <iostream>

void
logToIDEConsole(const geEngineSDK::String& message, const char* channel) {
  static geEngineSDK::Mutex mutex;
  geEngineSDK::Lock lock(mutex);

  OutputDebugStringA("[");
  OutputDebugStringA(channel);
  OutputDebugStringA("] ");
  OutputDebugStringA(message.c_str());
  OutputDebugStringA("\n");

  //Also default output in case we're running without debugger attached
  cout << message << endl;
}
#else
void
logToIDEConsole(const geEngineSDK::String& message, const char* channel) {
  cout << "[" << channel << "] " << message << endl;
}
#endif

namespace geEngineSDK {
  /**
   * This definition needs to be before any category implementation
   */
  UnorderedMap<uint32, String> Log::s_categories;

  GE_LOG_CATEGORY_IMPL(Uncategorized);
  GE_LOG_CATEGORY_IMPL(FileSystem);
  GE_LOG_CATEGORY_IMPL(RTTI);
  GE_LOG_CATEGORY_IMPL(Generic);
  GE_LOG_CATEGORY_IMPL(Platform);
  GE_LOG_CATEGORY_IMPL(Serialization);

  void
  Debug::log(const String& message, LogVerbosity verbosity, uint32 category) {
    m_log.logMsg(message, verbosity, category);

    if (LogVerbosity::kLog != verbosity) {
      switch (verbosity)
      {
      case LogVerbosity::kFatal:
        logToIDEConsole(message, "FATAL");
        break;
      case LogVerbosity::kError:
        logToIDEConsole(message, "ERROR");
        break;
      case LogVerbosity::kWarning:
        logToIDEConsole(message, "WARNING");
        break;
      default:
      case LogVerbosity::kInfo:
        logToIDEConsole(message, "INFO");
        break;
      case LogVerbosity::kVerbose:
        logToIDEConsole(message, "VERBOSE");
        break;
      case LogVerbosity::kVeryVerbose:
        logToIDEConsole(message, "VERY_VERBOSE");
        break;
      }
    }
  }

  void
  Debug::writeAsBMP(uint8* rawPixels,
                    uint32 bytesPerPixel,
                    uint32 width,
                    uint32 height,
                    const Path& filePath,
                    bool overwrite) const {
    if (FileSystem::isFile(filePath)) {
      if (overwrite) {
        FileSystem::remove(filePath);
      }
      else {
        GE_EXCEPT(FileNotFoundException,
                  "File already exists at specified location: " + filePath.toString());
      }
    }

    auto ds = FileSystem::createAndOpenFile(filePath);

    auto bmpDataSize = BitmapWriter::getBMPSize(width, height, bytesPerPixel);
    auto bmpBuffer = ge_newN<uint8>(bmpDataSize);

    BitmapWriter::rawPixelsToBMP(rawPixels, bmpBuffer, width, height, bytesPerPixel);

    ds->write(bmpBuffer, bmpDataSize);
    ds->close();

    ge_deleteN(bmpBuffer, bmpDataSize);
  }

  void
  Debug::_triggerCallbacks() {
    LogEntry entry;
    while (m_log.getUnreadEntry(entry)) {
      onLogEntryAdded(entry);
    }

    uint64 hash = m_log.getHash();
    if (m_logHash != hash) {
      onLogModified();
      m_logHash = hash;
    }
  }

  void
  Debug::saveLog(const Path& path, SAVED_LOG_TYPE::E type) const {
    switch (type)
    {
      default:
      case SAVED_LOG_TYPE::kHTML:
        saveHtmlLog(path);
        break;
      case SAVED_LOG_TYPE::kTextual:
        saveTextLog(path);
        break;
    }
  }

  void
  Debug::saveHtmlLog(const Path& path) const {
    static const char* style =
      R"(<link rel="stylesheet" type="text/css" href="../css/debug.css">)";

    static const char* htmlPreStyleHeader = 
R"(<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0 Transitional//EN'>
<html>
  <head>
    <title>geEngine: Information Log</title>
      <link rel='shortcut icon' href=''>)";

    static const char* htmlPostStyleHeader = R"(
  </head>
  <body>)";

    static const char* htmlEntriesTableHeader = R"(
    <div class="wrapper">
      <div class="table">
        <div class="row header blue">
          <div class="cell"> Type </div>
          <div class="cell"> Time </div>
          <div class="cell"> Description </div>
        </div>)";

    static const char* htmlFooter = R"(
        </div>
      </div>
    </body>
</html>)";

    StringStream stream;
    stream << htmlPreStyleHeader;
    stream << style;
    stream << htmlPostStyleHeader;
    stream << "<h1>geEngine Log</h1>\n";
    stream << "<h2>System information</h2>\n";

    //Write header information
    stream << "<p>geEngine version: ";
    stream << GE_VERSION_MAJOR << "." << GE_VERSION_MINOR << "." << GE_VERSION_PATCH;
    stream << "<br>\n";

    if (Time::isStarted()) {
      stream << "Started on: " << g_time().getAppStartUpDateString(false) << "<br>\n";
    }

    SystemInfo systemInfo = PlatformUtility::getSystemInfo();
    stream << "OS version: " << systemInfo.osName << " ";
    stream << (systemInfo.osIs64Bit ? "64-bit" : "32-bit") << "<br>\n";
    stream << "<h3>CPU information:</h3>\n";
    stream << "CPU vendor: " << systemInfo.cpuManufacturer << "<br>\n";
    stream << "CPU name: " << systemInfo.cpuModel << "<br>\n";
    stream << "CPU clock speed: " << systemInfo.cpuClockSpeedMhz << "MHz <br>\n";
    stream << "CPU core count: " << systemInfo.cpuNumCores << "<br>\n";
    stream << "Memory amount: " << systemInfo.memoryAmountMb << " MB" << "</p>\n";

    if (systemInfo.gpuInfo.numGPUs == 1) {
      stream << "<p>GPU List: " << systemInfo.gpuInfo.names[0] << "<br>\n";
    }
    else {
      for (uint32 i = 0; i < systemInfo.gpuInfo.numGPUs; ++i) {
        stream << "GPU #" << i << ": " << systemInfo.gpuInfo.names[i] << "<br>\n";
      }
    }
    stream << "</p>\n";

    //Write log entries
    stream << "<h2>Log entries</h2>\n";
    stream << htmlEntriesTableHeader;

    Vector<LogEntry> entries = m_log.getAllEntries();
    for (auto& entry : entries) {
      String channelName;

      switch (entry.getVerbosity())
      {
        case LogVerbosity::kFatal:
        case LogVerbosity::kError:
          stream << R"(<div class="row red">)" << endl;
          stream << R"(<div class="cell">Error</div>)" << endl;
          break;
        case LogVerbosity::kWarning:
          stream << R"(<div class="row yellow">)" << endl;
          stream << R"(<div class="cell">Warning</div>)" << endl;
          break;
        default:
        case LogVerbosity::kInfo:
        case LogVerbosity::kLog:
        case LogVerbosity::kVerbose:
        case LogVerbosity::kVeryVerbose:
          stream << R"(<div class="row green">)" << endl;
          stream << R"(<div class="cell">Debug</div>)" << endl;
          break;
      }

      stream << R"(<div>)" << entry.getLocalTime() << "</div>" << endl;
      String parsedMessage = StringUtil::replaceAll(entry.getMessage(), "\n", "<br>\n");
      stream << R"(<div class="cell">)" << parsedMessage << "</div>" << endl;
      stream << R"(</div>)" << endl;
    }

    stream << htmlFooter;

    auto fileStream = FileSystem::createAndOpenFile(path);
    fileStream->writeString(stream.str());
  }

  /**
   * @brief Internal function to get the given number of spaces, so that the 
   *        log looks properly indented
   */ 
  String
  _getSpacesIndentation(SIZE_T numSpaces) {
    String tmp;
    for (uint8 i = 0; i < numSpaces; ++i) {
      tmp.append(" ");
    }
    return tmp;
  }

  void
  Debug::saveTextLog(const Path& path) const
  {
    StringStream stream;
    stream << "geEngine Log\n";
    stream << 
      "===================================================================================\n";
    stream << "System information\n";

    //Write header information
    stream << "geEngine version: ";
    stream << GE_VERSION_MAJOR << "." << GE_VERSION_MINOR << "." << GE_VERSION_PATCH;
    stream << "\n";

    if (Time::isStarted()) {
      stream << "Started on: " << g_time().getAppStartUpDateString(false) << "\n\n";
    }

    SystemInfo systemInfo = PlatformUtility::getSystemInfo();
    stream << "OS version: " << systemInfo.osName << " ";
    stream << (systemInfo.osIs64Bit ? "64-bit" : "32-bit") << "\n";
    stream << "CPU information:\n";
    stream << "CPU vendor: " << systemInfo.cpuManufacturer << "\n";
    stream << "CPU name: " << systemInfo.cpuModel << "\n";
    stream << "CPU clock speed: " << systemInfo.cpuClockSpeedMhz << "MHz \n";
    stream << "CPU core count: " << systemInfo.cpuNumCores << "\n";
    stream << "Memory amount: " << systemInfo.memoryAmountMb << " MB" << "\n";

    if (systemInfo.gpuInfo.numGPUs == 1) {
      stream << "\nGPU List: " << systemInfo.gpuInfo.names[0] << "\n";
      stream <<
      "===================================================================================\n";
    }
    else {
      for (uint32 i = 0; i < systemInfo.gpuInfo.numGPUs; ++i) {
        stream << "GPU #" << i << ": " << systemInfo.gpuInfo.names[i] << "\n";
      }
    }
    stream << "\n\n";

    //Write log entries
    stream << "Log entries\n";
    stream <<
      "===================================================================================\n";

    Vector<LogEntry> entries = m_log.getAllEntries();
    for (auto& entry : entries) {
      String builtMsg;
      builtMsg.append(toString(entry.getLocalTime(),
                               false,
                               true,
                               TIME_TO_STRING_CONVERSION_TYPE::kFull));
      builtMsg.append(" ");

      switch (entry.getVerbosity())
      {
        case LogVerbosity::kFatal:
          builtMsg.append("[FATAL]");
          break;
        case LogVerbosity::kError:
          builtMsg.append("[ERROR]");
          break;
        case LogVerbosity::kWarning:
          builtMsg.append("[WARNING]");
          break;
        case LogVerbosity::kInfo:
          builtMsg.append("[INFO]");
          break;
        case LogVerbosity::kLog:
          builtMsg.append("[LOG]");
          break;
        case LogVerbosity::kVerbose:
          builtMsg.append("[VERBOSE]");
          break;
        case LogVerbosity::kVeryVerbose:
          builtMsg.append("[VERY_VERBOSE]");
          break;
        case LogVerbosity::kAny:
          break;
      }

      String categoryName;
      m_log.getCategoryName(entry.getCategory(), categoryName);
      builtMsg.append(" <" + categoryName + ">");

      builtMsg.append(" | ");

      String tmpSpaces = _getSpacesIndentation(builtMsg.length());

      String parsedMessage = StringUtil::replaceAll(entry.getMessage(),
                                                    "\n\t\t",
                                                    "\n" + tmpSpaces);
      builtMsg.append(parsedMessage);
      stream << builtMsg << "\n";
    }

    SPtr<DataStream> fileStream = FileSystem::createAndOpenFile(path);
    fileStream->writeString(stream.str());
  }

  GE_UTILITIES_EXPORT Debug& g_debug() {
    static Debug debug;
    return debug;
  }
}
