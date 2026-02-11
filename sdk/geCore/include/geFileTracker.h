/*****************************************************************************/
/**
 * @file    geFileWatcher.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/26
 * @brief   File Watcher System.
 *
 * File Watcher System. This system is a subscription-based system that informs
 * the engine when a file is modified, created or deleted. It can be used to
 * automatically reload resources when they are modified.
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
#include "gePrerequisitesCore.h"

#include <geEvent.h>
#include <geModule.h>
#include <geStringID.h>
#include <geFileSystem.h>
#include <geDebug.h>

namespace geEngineSDK {

  using FILE_CHANGED_EVENT_CALLBACK = void(const PlatformString&);
  using ChangeCallback = Event<FILE_CHANGED_EVENT_CALLBACK>;

  struct TrackedFile
  {
    bool
    operator==(const TrackedFile& other) const {
      return m_systemID == other.m_systemID && m_filePath == other.m_filePath;
    }

    bool
    operator<(const TrackedFile& other) const {
      if (m_systemID < other.m_systemID) {
        return true;
      }
      if (m_systemID > other.m_systemID) {
        return false;
      }
      return m_filePath < other.m_filePath;
    }

    uint32 m_systemID = 0;          //ID of the system that is watching the file
    PlatformString m_filePath;      //Absolute path of the file being watched
    time_t m_lastModifiedTime = 0;  //Last modified time of the file
  };

  struct TrackedFileHash
  {
    std::size_t
    operator()(const TrackedFile& file) const _NOEXCEPT {
      return std::hash<uint32>()(file.m_systemID) ^
             std::hash<PlatformString>()(file.m_filePath);
    }
  };

  class GE_CORE_EXPORT FileTracker final : public Module<FileTracker>
  {
   public:
    FileTracker() = default;
    ~FileTracker() = default;

    /**
     * @brief Start monitoring the files
     */
    void
    startWatching() {
      m_stopFlag = false;
      m_monitoringThread = Thread(&FileTracker::watchFiles, this);
    }

    /**
     * @brief Stop monitoring the files
     */
    void
    stopWatching() {
      m_stopFlag = true;
      if (m_monitoringThread.joinable()) {
        m_monitoringThread.join();
      }
    }

    /**
     * @brief Subscribe to file changes for a specific system
     * @param systemID ID of the system that is subscribing to file changes
     * @param callback Callback function to be called when a file change is detected
     */
    uint32
    subscribe(const String systemName, const ChangeCallback& callback) {
      uint32 systemID = StringID(systemName).id();

      ScopedLock<true> lock(m_dataMutex);
      if (m_subscribersCallbacks.find(systemID) == m_subscribersCallbacks.end()) {
        m_subscribersCallbacks[systemID] = callback;
      }
      
      return systemID;
    }

    /**
     * @brief Unsubscribe from file changes for a specific system
     * @param systemID ID of the system that is unsubscribing from file changes
     */
    void
    unsubscribe(const StringID systemID) {
      ScopedLock<true> lock(m_dataMutex);
      m_subscribersCallbacks.erase(systemID.id());
    }

    /**
     * @brief Add files to be watched
     * @param directory
     */
    void
    addFiles(const uint32 systemID, const Vector<Path>& newFiles);

    /**
     * @brief Add directories to be watched
     * @param directory
     */
    void
    clearFiles() {
      ScopedLock<true> lock(m_dataMutex);
      m_filesToWatch.clear();
    }

   protected:
    void
    onStartUp() override;

    void
    onShutDown() override;

   private:
    /**
     * @brief Thread function that watches the files
     */
    void
    watchFiles();

    UnorderedMap<uint32, ChangeCallback> m_subscribersCallbacks;
    UnorderedSet<TrackedFile, TrackedFileHash> m_filesToWatch;

    Thread m_monitoringThread;
    Mutex m_dataMutex;
    atomic<bool> m_stopFlag{ false };
  };

  GE_CORE_EXPORT FileTracker&
  g_fileWatcher();

  GE_LOG_CATEGORY(FileTracker, 500);
}
