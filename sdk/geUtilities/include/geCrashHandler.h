/*****************************************************************************/
/**
 * @file    geCrashHandler.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/10
 * @brief   Saves crash data and notifies when a crash occurs
 *
 * Saves crash data and notifies the user when a crash occurs
 *
 * @bug     Crashes are reported in the same process as the main application.
 *          This can be a problem if the crash was caused by heap. Any further
 *          use of the heap by the reporting methods will cause a silent crash,
 *          failing to log it. A more appropriate way of doing it should be to
 *          resume another process to actually handle the crash.
 */
/*****************************************************************************/
#pragma once

#define GE_MAX_STACKTRACE_DEPTH 200
#define GE_MAX_STACKTRACE_NAME_BYTES 1024

namespace geEngineSDK {
  /**
   * @brief Saves crash data and notifies the user when a crash occurs.
   */
  class GE_UTILITIES_EXPORT CrashHandler
  {
   public:
    CrashHandler();
    ~CrashHandler();

    /**
     * @brief Constructs and starts the module.
     */
    static void
    startUp() {
      if (nullptr == _instance()) {
        _instance() = ge_new<CrashHandler>();
      }
    }

    /**
     * @brief Shuts down this module and frees any resources it is using.
     */
    static void
    shutDown() {
      if (nullptr != _instance()) {
        ge_delete(_instance());
        _instance() = nullptr;
      }
    }

    /**
     * @brief Returns a reference to the module instance.
     */
    static CrashHandler&
    instance() {
      return *_instance();
    }

    /**
     * @brief Records a crash with a custom error message.
     * @param[in] type Type of the crash that occurred. e.g. "InvalidParameter"
     * @param[in] description More detailed description of the issue that caused the crash.
     * @param[in] function Optional name of the function where the error occurred.
     * @param[in] file Optional name of the source code file in which the code
     *            that crashed the program exists.
     * @param[in] line Optional source code line at which the crash was triggered at.
     */
    void
    reportCrash(const String& type,
                const String& strDescription,
                const String& strFunction = StringUtil::BLANK,
                const String& strFile = StringUtil::BLANK,
                uint32 nLine = 0) const;

#if USING(GE_PLATFORM_WINDOWS)
    /**
     * @brief Records a crash resulting from a Windows-specific SEH exception.
     * @param[in] exceptionData Exception data returned from getExceptionInformation()
     * @return Code that signals the __except exception handler on how to proceed.
     * @note Available in Windows builds only.
     */
    int
    reportCrash(void* exceptionDataPtr) const;
#endif

    /**
     * @brief Returns a string containing a current stack trace. If function
     *        can be found in the symbol table its readable name will be
     *        present in the stack trace, otherwise just its address.
     * @return String containing the call stack with each function on its own line.
     */
    static String
    getStackTrace();
   private:
    /**
     * @brief Does what it says. Internal utility function used by reportCrash().
     */
    void
    logErrorAndStackTrace(const String& message, const String& stackTrace) const;

    /**
     * @brief Does what it says. Internal utility function used by reportCrash().
     */
    void
    logErrorAndStackTrace(const String& type,
                          const String& strDescription,
                          const String& strFunction,
                          const String& strFile,
                          uint32 nLine) const;

    /**
     * @brief Does what it says. Internal utility function used by reportCrash().
     */
    void
    saveCrashLog() const;

    /**
     * @brief Returns path to the folder into which to store the crash reports.
     */
    static const Path&
    getCrashFolder();

    /**
     * @brief Returns the current time as a string timestamp.
     *        This is used to name the crash report directory.
     */
    static String
    getCrashTimestamp();

    /**
     * @brief Returns a singleton instance of this module.
     */
    static CrashHandler*&
    _instance() {
      static CrashHandler* inst = nullptr;
      return inst;
    }

    static const String s_crashReportFolder;
    static const String s_crashLogName;
    static const String s_fatalErrorMsg;

#if USING(GE_PLATFORM_WINDOWS)
    struct Data;
    Data* m_crashData;
#endif
  };

  GE_UTILITIES_EXPORT CrashHandler& g_crashHandler();
}
