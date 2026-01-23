/*****************************************************************************/
/**
 * @file    geFileSystem.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/09/19
 * @brief   Utility class for dealing with files.
 *
 * Utility class for dealing with files.
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

namespace geEngineSDK {
  using std::time_t;
  using std::function;

  /**
   * @brief Utility class for dealing with files.
   */
  class GE_UTILITIES_EXPORT FileSystem
  {
   public:
    /**
     * @brief Opens a file and returns a data stream capable of reading or
     *        writing to that file.
     * @param[in] fullPath  Full path to a file.
     * @param[in] readOnly  (optional) If true, returned stream will only be readable.
     */
    static SPtr<DataStream>
    openFile(const Path& fullPath, bool readOnly = true);

    /**
     * @brief Opens a file and returns a data stream capable of reading and
     *        writing to that file. If the file doesn't exist one will be created.
     * @param[in] fullPath  Full path to a file.
     */
    static SPtr<DataStream>
    createAndOpenFile(const Path& fullPath);

    /**
     * @brief Returns the size of a file in bytes.
     * @param[in] fullPath  Full path to a file.
     */
    static uint64
    getFileSize(const Path& fullPath);

    /**
     * @brief Deletes a file or a folder at the specified path.
     * @param[in] fullPath  Full path to a file or a folder..
     * @param[in] recursively (optional) If true, folders will have their contents
     *            deleted as well. Otherwise an exception will be thrown for non-empty folders
     */
    static void
    remove(const Path& fullPath, bool recursively = true);

    /**
     * @brief Moves a file or a folder from one to another path.
     *        This can also be used as a rename operation.
     * @param[in] oldPath Full path to the old file/folder.
     * @param[in] newPath Full path to the new file/folder.
     * @param[in] overwriteExisting (optional) If true, any existing file/folder
     *            at the new location will be overwritten, otherwise an exception
     *            will be thrown if a file/folder already exists.
     */
    static void
    move(const Path& oldPath, const Path& newPath, bool overwriteExisting = true);

    /**
     * @brief Makes a copy of a file or a folder in the specified path.
     * @param[in] oldPath Full path to the old file/folder.
     * @param[in] newPath Full path to the new file/folder.
     * @param[in] overwriteExisting	(optional) If true, any existing file/folder
     *            at the new location will be overwritten, otherwise an exception
     *            will be thrown if a file/folder already exists.
     */
    static void
    copy(const Path& oldPath, const Path& newPath, bool overwriteExisting = true);

    /**
     * @brief Creates a folder at the specified path.
     * @param[in] fullPath	Full path to a full folder to create.
     */
    static void
    createDir(const Path& fullPath);

    /**
     * @brief Returns true if a file or a folder exists at the specified path.
     * @param[in] fullPath	Full path to a file or folder.
     */
    static bool
    exists(const Path& fullPath);

    /**
     * @brief Returns true if a file exists at the specified path.
     * @param[in] fullPath  Full path to a file or folder.
     */
    static bool
    isFile(const Path& fullPath);

    /**
     * @brief Returns true if a folder exists at the specified path.
     * @param[in] fullPath	Full path to a file or folder.
     */
    static bool
    isDirectory(const Path& fullPath);

    /**
     * @brief Returns all files or folders located in the specified folder.
     * @param[in] dirPath Full path to the folder to retrieve children files/folders from.
     * @param[out] files Full paths to all files located directly in specified folder.
     * @param[out] directories Full paths to all folders located directly in specified folder.
     */
    static void
    getChildren(const Path& dirPath, Vector<Path>& files, Vector<Path>& directories);

    /**
     * @brief Iterates over all files and directories in the specified folder and
     *        calls the provided callback when a file/folder is iterated over.
     * @param[in] dirPath Directory over which to iterate
     * @param[in] fileCallback  Callback to call whenever a file is found.
     *            If callback returns false iteration stops. Can be null.
     * @param[in] dirCallback Callback to call whenever a directory is found.
     *            If callback returns false iteration stops. Can be null.
     * @param[in] recursive If false then only the direct children of the provided
     *            folder will be iterated over, and if true then child directories
     *            will be recursively visited as well.
     * @return  true if iteration finished iterating over all files/folders, or
     *          false if it was interrupted by a callback returning false.
     */
    static bool
    iterate(const Path& dirPath,
            const function<bool(const Path&)>& fileCallback,
            const function<bool(const Path&)>& dirCallback = nullptr,
            bool recursive = true);

    /**
     * @brief Returns the last modified time of a file or a folder at the specified path.
     * @param[in] fullPath  Full path to a file or a folder.
     */
    static time_t
    getLastModifiedTime(const Path& fullPath);

    /**
     * @brief Returns the path to the currently working directory.
     */
    static Path
    getWorkingDirectoryPath();

    /**
     * @brief Returns the path to a directory where temporary files may be stored.
     */
    static Path
    getTempDirectoryPath();

    /**
     * @brief Returns the path to a directory where user data may be stored.
     */
    static Path
    getUserDataDirectoryPath();

    /**
     * @brief Sets the path to the engine's data directory.
     *        This is where the engine will look for its resources.
     * @param path The new path to the engine's data directory.
     */
    static void
    setEnginePath(const Path& path);

    /**
     * @brief Returns the path to the engine's data directory.
     *        This is where the engine will look for its resources.
     */
    static Path
    getEnginePath();

    /**
     * @brief Sets the path to the engine's plugins directory.
     *        This is where the engine will look for its plugins.
     * @param path The new path to the engine's plugins directory.
     */
    static void
    setPluginsPath(const Path& path);

    /**
     * @brief Returns the path to the engine's plugins directory.
     *        This is where the engine will look for its plugins.
     */
    static Path
    getPluginsPath();

    /**
     * @brief Sets the path to the application data directory.
     *        This is where the application will look for its resources.
     * @param path The new path to the application data directory.
     */
    static void
    setAppPath(const Path& path);

    /**
     * @brief Returns the path to the application data directory.
     *        This is where the application will look for its resources.
     */
    static Path
    getAppPath();

   private:
    /**
     * @brief Copy a single file. Internal function used by copy().
     */
    static void
    copyFile(const Path& from, const Path& to);
    
    /**
     * @brief Remove a single file. Internal function used by remove().
     */
    static void
    removeFile(const Path& path);
    
    /**
     * @brief Move a single file. Internal function used by move().
     */
    static void
    moveFile(const Path& oldPath, const Path& newPath);
  };

  /**
   * @brief Locks access to files on the same drive, allowing only one file to
   *        be read at a time, per drive. This prevents multiple threads
   *        accessing multiple files on the same drive at once, ruining
   *        performance on mechanical drives.
   */
  class GE_UTILITIES_EXPORT FileScheduler final
  {
   public:
    /**
     * @brief Locks access and doesn't allow other threads to get past this
     *        point until access is unlocked. Any scheduled file access should
     *        happen past this point.
     */
    static void
    lock(const Path& /*path*/) {
      //Note: File path should be analyzed and determined on which drive does
      //the path belong to. Locks can then be issued on a per-drive basis,
      //instead of having one global lock. This would allow multiple files to
      //be accessed at the same time, as long as they're on different drives.
      m_mutex.lock();
    }

    /**
     * @brief Unlocks access and allows another thread to lock file access.
     *        Must be provided with the same file path as lock().
     */
    static void
    unlock(const Path& /*path*/) {
      m_mutex.unlock();
    }

    /**
     * @brief Returns a lock object that immediately locks access (same as
     *        lock()), and then calls unlock() when it goes out of scope.
     */
    static Lock
    getLock(const Path& /*path*/) {
      return Lock(m_mutex);
    }

   private:
    static Mutex m_mutex;
  };
}
