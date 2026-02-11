/*****************************************************************************/
/**
 * @file    geFileWatcher.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geFileTracker.h"

namespace geEngineSDK {
  GE_LOG_CATEGORY_IMPL(FileTracker);

  void
  FileTracker::onStartUp()
  {}

  void
  FileTracker::onShutDown() {
    stopWatching();
    clearFiles();
    m_subscribersCallbacks.clear();
  }

  void
  FileTracker::watchFiles() {
    //Monitor all files in a single thread
    while (!m_stopFlag) {
      GE_THREAD_SLEEP(500);

      ScopedLock<true> lock(m_dataMutex);
      for (auto& file : m_filesToWatch) {
        try {
          auto currentTimestamp = FileSystem::getLastModifiedTime(toString(file.m_filePath));
          if (file.m_lastModifiedTime != currentTimestamp) {
            {
              //Update the last modified time (this is only done because the time stamp
              //is not part of the hash function. But I wanted a set to avoid repetition by
              //system and; if needed, be able to track the same file on different systems).
              auto& modFile = const_cast<TrackedFile&>(file);
              modFile.m_lastModifiedTime = currentTimestamp;
            }
            auto& callback = m_subscribersCallbacks[file.m_systemID];
            if (!callback.empty()) {
              callback(file.m_filePath);
            }
          }
        }
        catch (FileNotFoundException& e) {
          GE_LOG(kWarning,
                 FileTracker,
                 "Error accessing file {0}: {1}", toString(file.m_filePath), e.what());
        }
        GE_THREAD_SLEEP(100);

        //We do this here so if the user wants to stop the thread it will stop faster
        if (m_stopFlag) {
          break;
        }
      }
    }
  }

  void
  FileTracker::addFiles(const uint32 systemID, const Vector<Path>& newFiles) {
    Path currentDir = FileSystem::getWorkingDirectoryPath();
    {
      ScopedLock<true> lock(m_dataMutex);

      if (m_subscribersCallbacks.find(systemID) == m_subscribersCallbacks.end()) {
        //If the system is not subscribed, we do not track its files
        GE_LOG(kWarning,
          FileTracker,
          "System with ID {} is not subscribed to file changes.", systemID);
        return;
      }

      for (const auto& file : newFiles) {
        auto fileStr = file.getAbsolute(currentDir).toPlatformString();
        TrackedFile trackedFile;
        trackedFile.m_systemID = systemID;
        trackedFile.m_filePath = fileStr;
        trackedFile.m_lastModifiedTime = FileSystem::getLastModifiedTime(file);
        if (m_filesToWatch.insert(trackedFile).second) {
          // If the file was successfully added
        }
      }

    }
  }

  FileTracker&
  geEngineSDK::g_fileWatcher() {    
    return FileTracker::instance();
  }

} // namespace geEngineSDK
