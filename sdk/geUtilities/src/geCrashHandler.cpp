/*****************************************************************************/
/**
 * @file    geCrashHandler.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/10
 * @brief   Generic crash data notifications code
 *
 * Generic crash data notifications code
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geDebug.h"
#include "geFileSystem.h"
#include "gePath.h"

namespace geEngineSDK {
  const String CrashHandler::s_crashReportFolder = "Reports";
  const String CrashHandler::s_crashLogName = U8STRING("geEngine_Log.html");
  const String CrashHandler::s_fatalErrorMsg = 
    "A fatal error occurred and the program has to terminate!";

  CrashHandler& g_crashHandler() {
    return CrashHandler::instance();
  }

  const Path&
  CrashHandler::getCrashFolder() {
    static const Path path = FileSystem::getWorkingDirectoryPath() +
                             Path(s_crashReportFolder) +
                             Path(getCrashTimestamp());

    static bool first = true;
    if (first) {
      FileSystem::createDir(path);
      first = false;
    }

    return path;
  }

  void
  CrashHandler::logErrorAndStackTrace(const String& message,
                                      const String& stackTrace) const {
    StringStream errorMessage;
    errorMessage << s_fatalErrorMsg << std::endl;
    errorMessage << message;
    errorMessage << "\n\nStack trace: \n";
    errorMessage << stackTrace;

    g_debug().log(errorMessage.str(), LogVerbosity::kFatal);
  }

  void
  CrashHandler::logErrorAndStackTrace(const String& type,
                                      const String& strDescription,
                                      const String& strFunction,
                                      const String& strFile,
                                      uint32 nLine) const {
    StringStream errorMessage;
    errorMessage << "  - Error: " << type << std::endl;
    errorMessage << "  - Description: " << strDescription << std::endl;
    errorMessage << "  - In function: " << strFunction << std::endl;
    errorMessage << "  - In file: " << strFile << ":" << nLine;
    logErrorAndStackTrace(errorMessage.str(), getStackTrace());
  }

  void
  CrashHandler::saveCrashLog() const {
    g_debug().saveLog(getCrashFolder() + Path(s_crashLogName), SAVED_LOG_TYPE::kHTML);
  }
}
