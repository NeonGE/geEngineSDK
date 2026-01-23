/*****************************************************************************/
/**
 * @file    geLog.cpp
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

/*****************************************************************************/
/**
* Includes
*/
/*****************************************************************************/
#include "geLog.h"
#include "geException.h"
#include "geNumericLimits.h"

namespace geEngineSDK {
//  UnorderedMap<uint32, String> Log::s_categories;

  Log::~Log() {
    clear();
  }

  void
  Log::logMsg(const String& message, LogVerbosity verbosity, uint32 category) {
    RecursiveLock lock(m_mutex);
    m_unreadEntries.emplace(message, verbosity, category);
  }

  void
  Log::clear() {
    RecursiveLock lock(m_mutex);
    m_entries.clear();

    while (!m_unreadEntries.empty()) {
      m_unreadEntries.pop();
    }

    ++m_hash;
  }

  void
  Log::clear(LogVerbosity verbosity, uint32 category) {
    RecursiveLock lock(m_mutex);

    Vector<LogEntry> newEntries;
    for (auto& entry : m_entries) {
      if( (LogVerbosity::kAny == verbosity || verbosity == entry.getVerbosity()) &&
         (category == NumLimit::MAX_UINT32 || category == entry.getCategory())) {
        continue;
      }

      newEntries.push_back(entry);
    }

    m_entries = newEntries;

    Queue<LogEntry> newUnreadEntries;
    while (!m_unreadEntries.empty()) {
      LogEntry entry = m_unreadEntries.front();
      m_unreadEntries.pop();

      if ((LogVerbosity::kAny == verbosity || verbosity == entry.getVerbosity()) &&
        (category == NumLimit::MAX_UINT32 || category == entry.getCategory())) {
        continue;
      }

      newUnreadEntries.push(entry);
    }

    m_unreadEntries = newUnreadEntries;
    ++m_hash;
  }

  bool
  Log::getUnreadEntry(LogEntry& entry) {
    RecursiveLock lock(m_mutex);

    if (m_unreadEntries.empty()) {
      return false;
    }

    entry = m_unreadEntries.front();
    m_unreadEntries.pop();
    m_entries.push_back(entry);
    ++m_hash;

    return true;
  }

  bool
  Log::getLastEntry(LogEntry& entry) {
    if (m_entries.empty()) {
      return false;
    }

    entry = m_entries.back();
    return true;
  }

  Vector<LogEntry>
  Log::getEntries() const {
    RecursiveLock lock(m_mutex);
    return m_entries;
  }

  bool
  Log::_registerCategory(uint32 id, const char* name) {
    if (!categoryExists(id)) {
      s_categories.emplace(id, name);
      return true;
    }

    return false;
  }

  bool
  Log::categoryExists(uint32 id) {
    return s_categories.find(id) != s_categories.end();
  }

  bool
  Log::getCategoryName(uint32 id, String& name) {
    auto search = s_categories.find(id);
    if (s_categories.end() != search) {
      name = search->second;
      return true;
    }

    name = "Unknown";
    return false;
  }

  Vector<LogEntry>
  Log::getAllEntries() const {
    Vector<LogEntry> entries;
    
    {
      RecursiveLock lock(m_mutex);

      for (auto& entry : m_entries) {
        entries.push_back(entry);
      }

      Queue<LogEntry> unreadEntries = m_unreadEntries;
      while (!unreadEntries.empty()) {
        entries.push_back(unreadEntries.front());
        unreadEntries.pop();
      }
    }
    
    return entries;
  }
}
