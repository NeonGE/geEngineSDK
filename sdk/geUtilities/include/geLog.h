/*****************************************************************************/
/**
 * @file    geLog.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/09/19
 * @brief   Used for logging messages
 *
 * Can categorize messages according to channels, save the log to a
 * file and send out callbacks when a new message is added
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
#include "geTime.h"

namespace geEngineSDK {
  using std::move;
  using std::time;
  using std::time_t;

  /**
   * @brief Represents verbosity level at which a specific log message will be
   *        displayed.
   */
  enum class LogVerbosity
  {
    /**
     * Fatal error: Application cannot recover from and will crash.
     */
    kFatal,

    /**
     * An error happened that will not result in an immediate crash but may
     * cause serious problems.
     */

    kError,

    /**
     * Something went wrong but the application will not crash, although
     * invalid behavior might be observed.
     */
    kWarning,

    /**
     * An informational message will be logged, can be used for debugging and
     * tracing.
     */
    kInfo,

    /**
     * Same as Info, but the message will only be logged to the log file and
     * not any console output.
     */
    kLog,

    /**
     * Messages that can provide additional information and warnings, but are
     * too spammy to be displayed under normal circumstances.
     */
    kVerbose,

    /**
     * Same as Verbose, but for even spammier messages.
     */
    kVeryVerbose,
    
    /**
     * Meta-type encompassing all verbosity types. Should not be used for
     * logging directly.
     */
    kAny
  };

  /**
   * @brief A single log entry, containing a message and a channel the message
   *        was recorded on.
   */
  class GE_UTILITIES_EXPORT LogEntry
  {
   public:
    LogEntry() = default;
    LogEntry(String msg, LogVerbosity verbosity, uint32 category)
      : m_msg(std::move(msg)),
        m_verbosity(verbosity),
        m_category(category),
        m_localTime(time(nullptr))
    {}

    /**
     * @brief Determines how important is the message and when should it be
     *        displayed.
     */
    LogVerbosity
    getVerbosity() const {
      return m_verbosity;
    }

    /**
     * @brief Category of the system the message originated from.
     */
    uint32
    getCategory() const {
      return m_category;
    }

    /**
     * @brief Text of the message.
     */
    const String&
    getMessage() const {
      return m_msg;
    }

    /**
     * @brief Local time of message being registered as a text
     */
    const time_t&
    getLocalTime() const {
      return m_localTime;
    }

   private:
    String m_msg;
    LogVerbosity m_verbosity = LogVerbosity::kLog;
    uint32 m_category = 0U;
    time_t m_localTime = 0;
  };

  /**
   * @brief Used for logging messages. Can categorize messages according to
   *        channels, save the log to a file and send out callbacks when a new
   *        message is added.
   * @note  Thread safe.
   */
  class GE_UTILITIES_EXPORT Log
  {
   public:
    Log() = default;
    ~Log();

    /**
     * @brief Logs a new message.
     * @param[in] message   The message describing the log entry.
     * @param[in] verbosity Verbosity of the message, determining its
     *                      importance.
     * @param[in] category  Category of the message, determining which system
     *                      is it relevant to.
     */
    void
    logMsg(const String& message, LogVerbosity verbosity, uint32 category);

    /**
     * @brief Removes all log entries.
     */
    void
    clear();

    /**
     * @brief Removes all log entries for a specific verbosity level and/or
     *        category. Specify -1 to clear all verbosity levels and/or
     *        categories.
     */
    void
    clear(LogVerbosity verbosity, uint32 category);

    /**
     * @brief Returns all existing log entries.
     */
    Vector<LogEntry>
    getEntries() const;

    /**
     * @brief Returns the latest unread entry from the log queue, and removes
     *        the entry from the unread entries list.
     * @param[out] entry Entry that was retrieved, or undefined if no entries exist.
     * @return true if an unread entry was retrieved, false otherwise.
     */
    bool
    getUnreadEntry(LogEntry& entry);

    /**
     * @brief Returns the last available log entry.
     * @param[out] entry Entry that was retrieved, or undefined if no entries exist.
     * @return true if an entry was retrieved, false otherwise.
     */
    bool
    getLastEntry(LogEntry& entry);

    /**
     * @brief Returns a hash value that is modified whenever entries in the log
     *        change. This can be used for checking for changes by external systems.
     */
    uint64
    getHash() const {
      return m_hash;
    }

    /**
     * @brief Checks if the category with the specified ID exists.
     * @param[in] id  Number representing the category's ID.
     * @return  true if exists, otherwise false.
     */
    static bool
    categoryExists(uint32 id);

    /**
     * @brief Returns the number of registered log categories.
     */
    static uint32
    getNumCategories() {
      return static_cast<uint32>(s_categories.size());
    }

    /**
     * @brief Get the name of the category based on its ID.
     * @param[in] id    Number representing the category's ID.
     * @param[in] name  Variable in which the name will be written.
     * @return  If found will write the name and return true.
     *          Otherwise will write the name "Unknown" and return false.
     */
    static bool
    getCategoryName(uint32 id, String& name);

    /**
     * @brief Registers the new category with the specified ID and name.
     * @param[in] id    Number representing the category's ID.
     * @param[in] name  Name of the category.
     * @return  false if the ID is already taken, otherwise true.
     */
    static bool
    _registerCategory(uint32 id, const char* name);

   private:
    friend class Debug;

    /**
     * @brief Returns all log entries, including those marked as unread.
     */
    Vector<LogEntry>
    getAllEntries() const;

    Vector<LogEntry> m_entries;
    Queue<LogEntry> m_unreadEntries;
    uint64 m_hash = 0;
    mutable RecursiveMutex m_mutex;
    static UnorderedMap<uint32, String> s_categories;
  };
}
