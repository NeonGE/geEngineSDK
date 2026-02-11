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
  #include <signal.h>
  #include <unistd.h>     // write, _exit
  #include <cstring>      // memset
  #include <cstdio>       // snprintf
  #include <execinfo.h>   // backtrace, backtrace_symbols
  #include <dlfcn.h>      // dladdr
  #include <cxxabi.h>     // __cxa_demangle
#endif

#ifndef GE_CRASHHANDLER_UNSAFE_REPORT_IN_SIGNAL
# define GE_CRASHHANDLER_UNSAFE_REPORT_IN_SIGNAL 0
#endif

namespace geEngineSDK {

  //Alt stack for the handler to work on in case the crash was caused by stack overflow.
  //64KB should be enough for any stack trace.
  alignas(16) static uint8 g_altStackMem[64 * 1024];

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
  CrashHandler::installPosixSignalHandlers() {
    //1) Configure altstack
    stack_t ss;
    std::memset(&ss, 0, sizeof(ss));
    ss.ss_sp = g_altStackMem;
    ss.ss_size = sizeof(g_altStackMem);
    ss.ss_flags = 0;
    (void)::sigaltstack(&ss, nullptr);

    //2) Register sigaction
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = &CrashHandler::posixSignalTrampoline;
    ::sigemptyset(&sa.sa_mask);

    //SA_SIGINFO: gets siginfo_t (addr, code)
    //SA_ONSTACK: use altstack
    //SA_RESETHAND: avoids loops if the handler itself crashes
    //(e.g. due to stack overflow or heap corruption).
    //After handling the signal once, the handler is reset to default, so if it
    //happens again, the program will crash normally instead of looping in the handler.
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_RESETHAND;

    (void)::sigaction(SIGSEGV, &sa, nullptr);
    (void)::sigaction(SIGABRT, &sa, nullptr);
    (void)::sigaction(SIGFPE, &sa, nullptr);
    (void)::sigaction(SIGILL, &sa, nullptr);
#ifdef SIGBUS
    (void)::sigaction(SIGBUS, &sa, nullptr);
#endif
  }

  void
  CrashHandler::posixSignalTrampoline(int32 sig, siginfo_t* info, void* uctx) _NOEXCEPT {
    //NOTE: instance() can use heap if it still doesn't exist
    CrashHandler::instance().reportPosixSignal(sig, info, uctx);

    //Hard exit, don't try to continue execution after a fatal signal.
    _exit(128 + sig);
  }

  void
  CrashHandler::reportPosixSignal(int32 sig, siginfo_t* info, void* /*uctx*/) _NOEXCEPT {
    //In signal handler, we can only use async-signal-safe functions.
    //This means we can't use std::string, locks, heap allocations, or any complex logic.
    //We have to keep it very simple and just write a message to stderr with the signal info.
    char buf[512];
    const char* sigName = ::strsignal(sig);

    if (info && (sig == SIGSEGV
#ifdef SIGBUS
      || sig == SIGBUS
#endif
      ))
    {
      int n = std::snprintf(buf, sizeof(buf),
        "CRASH(POSIX): signal=%d (%s) addr=%p code=%d\n",
        sig, sigName ? sigName : "?", info->si_addr, info->si_code);
      if (n > 0) (void)::write(STDERR_FILENO, buf, (size_t)n);
    }
    else
    {
      int n = std::snprintf(buf, sizeof(buf),
        "CRASH(POSIX): signal=%d (%s)\n",
        sig, sigName ? sigName : "?");
      if (n > 0) (void)::write(STDERR_FILENO, buf, (size_t)n);
    }

#if GE_CRASHHANDLER_UNSAFE_REPORT_IN_SIGNAL
    //WARNING: This is unsafe and can cause deadlocks or crashes if the signal
    //was caused by heap corruption or stack overflow.
    //Only enable this if you understand the risks and want to try to get a
    //crash log in some cases where it might not be possible otherwise.
    {
      //Keep this code as simple as possible, don't do anything fancy that
      //might cause more problems in the signal handler. Just try to log the
      //signal info and stack trace.
      this->reportCrash("PosixSignal",
        "Process received a fatal POSIX signal.",
        StringUtil::BLANK,
        StringUtil::BLANK,
        0);
      this->saveCrashLog();
    }
#endif
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
    const int n = ::backtrace(addrs, GE_MAX_STACKTRACE_DEPTH);

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
