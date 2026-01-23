/*****************************************************************************/
/**
 * @file    geDebug.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/09/19
 * @brief   Utility class providing various debug functionality.
 *
 * Utility class providing various debug functionality.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geLog.h"

namespace geEngineSDK {
  class Log;

  /** Type of the log that will be saved. */
  namespace SAVED_LOG_TYPE {
    enum E
    {
      kHTML = 0,
      kTextual = 1
    };
  }

  /**
   * @brief Utility class providing various debug functionality.
   * @note  Thread safe.
   */
  class GE_UTILITIES_EXPORT Debug
  {
   public:
     Debug() = default;

    /**
     * @brief Logs a new message.
     * @param[in] message   The message describing the log entry.
     * @param[in] verbosity Verbosity of the message, determining its
     *                      importance.
     * @param[in] category  Category of the message, determining which system
     *                      is it relevant to.
     */
    void
    log(const String& message, LogVerbosity verbosity, uint32 category = 0);

    /**
     * @brief Retrieves the Log used by the Debug instance.
     */
    Log&
    getLog() {
      return m_log;
    }

    /**
     * @brief Converts raw pixels into a BMP image. See BitmapWriter for more information.
     */
    void
    writeAsBMP(uint8* rawPixels,
               uint32 bytesPerPixel,
               uint32 width,
               uint32 height,
               const Path& filePath,
               bool overwrite = true) const;

    /**
     * @brief Saves a log about the current state of the application to the
     *        specified location.
     * @param path  Absolute path to the log filename.
     */
    void
    saveLog(const Path& path,
            SAVED_LOG_TYPE::E type = SAVED_LOG_TYPE::kHTML) const;

    /**
     * @brief Saves a log about the current state of the application to the
     *        specified location as a HTML file.
     * @param path  Absolute path to the log filename.
     */
    void
    saveHtmlLog(const Path& path) const;

    /**
     * @brief Saves a log about the current state of the application to the
     *        specified location as a text file.
     * @param	path	Absolute path to the log filename.
     */
    void
    saveTextLog(const Path& path) const;

    /**
     * @brief Triggered when a new entry in the log is added.
     * @note  Sim thread only.
     */
    Event<void(const LogEntry&)> onLogEntryAdded;

    /**
     * @brief Triggered whenever one or multiple log entries were added or removed.
     *        Triggers only once per frame.
     * @note  Sim thread only.
     */
    Event<void()> onLogModified;

    void
    setLogCallback(function<bool(const String& message,
                                 LogVerbosity verbosity,
                                 uint32 category)> callback) {
      m_customLogCallback = callback;
    }

    /**
     * @brief Triggers callbacks that notify external code that a log entry was added.
     * @note  Internal method. Sim thread only.
     */
    void
    _triggerCallbacks();

   private:
    uint64 m_logHash = 0;
    Log m_log;
    function<bool(const String& message,
                  LogVerbosity verbosity,
                  uint32 category)> m_customLogCallback;
  };

  /**
   * @brief A simpler way of accessing the Debug module.
   */
  GE_UTILITIES_EXPORT Debug&
  g_debug();

#ifndef GE_LOG_VERBOSITY
# if USING(GE_DEBUG_MODE)
#   define GE_LOG_VERBOSITY LogVerbosity::kLog
# else
#   define GE_LOG_VERBOSITY LogVerbosity::kWarning
# endif
#endif

  /**
   * @brief Defines a new log category to use with GE_LOG.
   *        Each category must have a unique ID.
   */
#define GE_LOG_CATEGORY(name, id)                                             \
  struct LogCategory##name {                                                  \
    enum {                                                                    \
    _id = id                                                                  \
    };                                                                        \
    static bool s_registered;                                                 \
  };

#define GE_LOG_CATEGORY_IMPL(name)                                            \
bool LogCategory##name::s_registered = Log::_registerCategory(LogCategory##name::_id, #name);

  /** Get the ID of the log category based on its name. */
#define GE_LOG_GET_CATEGORY_ID(category) LogCategory##category::_id


#define GE_LOG(verbosity, category, message, ...)                             \
  do {                                                                        \
  using namespace ::geEngineSDK;                                              \
  IF_CONSTEXPR ((int32)LogVerbosity::verbosity <= (int32)GE_LOG_VERBOSITY) {  \
    g_debug().log(StringUtil::format(message, ##__VA_ARGS__) +                \
                          String("\n\t\t in ") +                              \
                          __PRETTY_FUNCTION__ +                               \
                          " [" + __FILE__ + ":" +                             \
                          toString(static_cast<int32>(__LINE__)) + "]\n",     \
                  LogVerbosity::verbosity, LogCategory##category::_id);       \
  }} while (0)

  GE_LOG_CATEGORY(Uncategorized, 0);
  GE_LOG_CATEGORY(FileSystem, 1);
  GE_LOG_CATEGORY(RTTI, 2);
  GE_LOG_CATEGORY(Generic, 3);
  GE_LOG_CATEGORY(Platform, 4);
  GE_LOG_CATEGORY(Serialization, 5);
}
