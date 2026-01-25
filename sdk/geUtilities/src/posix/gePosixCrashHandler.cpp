/*****************************************************************************/
/**
 * @file    gePosixCrashHandler.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/01/25
 * @brief   Saves crash data and notifies when a crash occurs
 *
 * Poxis implementation for saving crash data and notifying the user when a
 * crash occurs (Linux, macOS).
 *
 * @bug	    No known bugs.
 */
 /*****************************************************************************/
#ifndef _WIN32
/*****************************************************************************/
/**
 * Includes
 */
 /*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geCrashHandler.h"
#include "geDebug.h"
#include "geFileSystem.h"
#include "gePath.h"

#if USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_OSX)
  #include <execinfo.h>   // backtrace, backtrace_symbols
  #include <dlfcn.h>      // dladdr
  #include <cxxabi.h>     // __cxa_demangle
#endif

namespace geEngineSDK {

  struct CrashHandler::Data
  {
    Mutex mutex;
  };

  CrashHandler::CrashHandler() {
    m_crashData = ge_new<Data>();
  }

  CrashHandler::~CrashHandler() {
    ge_delete(m_crashData);
    m_crashData = nullptr;
  }

  void
  CrashHandler::reportCrash(const String& type,
                            const String& strDescription,
                            const String& strFunction,
                            const String& strFile,
                            uint32 nLine) const {
    Lock lock(m_crashData->mutex);

    logErrorAndStackTrace(type, strDescription, strFunction, strFile, nLine);
    saveCrashLog();

    //POSIX: if we want a core dump, we just let the program crash normally.
    //This is controlled by ulimit -c in the shell.
    //Here we just exit.
  }

  String
  CrashHandler::getCrashTimestamp() {
    std::time_t t = std::time(nullptr);

    std::tm tmLocal{};
  #if defined(_POSIX_VERSION)
    localtime_r(&t, &tmLocal);
  #else
    tmLocal = *std::localtime(&t);
  #endif

    //YYYYMMDD_HHMM (same format as Windows version)
    String strYear   = toString(static_cast<uint32>(tmLocal.tm_year + 1900), 4, '0');
    String strMonth  = toString(static_cast<uint32>(tmLocal.tm_mon + 1),     2, '0');
    String strDay    = toString(static_cast<uint32>(tmLocal.tm_mday),        2, '0');
    String strHour   = toString(static_cast<uint32>(tmLocal.tm_hour),        2, '0');
    String strMinute = toString(static_cast<uint32>(tmLocal.tm_min),         2, '0');

    return StringUtil::format("{0}{1}{2}_{3}{4}",
                              strYear,
                              strMonth,
                              strDay,
                              strHour,
                              strMinute);
  }

  static String
  posix_demangle(const char* name) {
#if USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_OSX)
    if (!name || name[0] == '\0') {
      return StringUtil::BLANK;
    }

    int32 status = 0;
    char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    if (status == 0 && demangled) {
      String out = demangled;
      std::free(demangled);
      return out;
    }
#endif
    return name ? String(name) : StringUtil::BLANK;
  }

  String
  CrashHandler::getStackTrace() {
#if defined(__unix__) || defined(__APPLE__)
    void* addrs[GE_MAX_STACKTRACE_DEPTH];
    const int n = ::backtrace(addrs, static_cast<int>(GE_MAX_STACKTRACE_DEPTH));

    StringStream ss;
    ss << "Stack frames: " << n << "\n";

    for (int32 i = 0; i < n; ++i) {
      Dl_info info{};
      const bool hasInfo = (::dladdr(addrs[i], &info) != 0);

      String symName = hasInfo ? posix_demangle(info.dli_sname) : StringUtil::BLANK;

      //If there's no symbol name, just print the address
      ss << "  [" << i << "] " << addrs[i];

      if (!symName.empty()) {
        ss << "  " << symName;
      }

      if (hasInfo && info.dli_fname) {
        ss << "  (" << info.dli_fname << ")";
      }

      ss << "\n";
    }

    return ss.str();
#else
    return "Stack trace not supported on this platform.\n";
#endif
  }

} // namespace geEngineSDK

#endif // !USING(GE_PLATFORM_WINDOWS)
