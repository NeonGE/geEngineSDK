/*****************************************************************************/
/**
 * @file    geDiskFileSystem.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/15
 * @brief   Disk File System.
 *
 * Disk File System. This system provides an interface for managing files
 * on the disk. It allows reading and writing files, checking existence,
 * and listing files in directories.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geDiskFileSystem.h"

#include <geFileSystem.h>

namespace geEngineSDK {

  DiskFileSystem::DiskFileSystem(const Path& rootPath)
    : m_root(rootPath)
  {}

  bool
  DiskFileSystem::exists(const Path& path) const {
    auto fullPath = path.getAbsolute(m_root);
    return FileSystem::exists(path);
  }

  SPtr<DataStream>
  DiskFileSystem::open(const Path& path) {
    auto fullPath = path.getAbsolute(m_root);
    if (!FileSystem::exists(fullPath)) {
      return nullptr;
    }

    return ge_shared_ptr_new<FileDataStream>(fullPath, ACCESS_MODE::kREAD);
  }

  Vector<Path>
  DiskFileSystem::getAllFiles() const {
    Vector<Path> outFiles;
    _recursiveCollectFiles(m_root, m_root, outFiles);
    return outFiles;
  }

  void
  DiskFileSystem::_recursiveCollectFiles(const Path& currentPath,
                                        const Path& relativePath,
                                        Vector<Path>& outFiles) const {
    if (!currentPath.isDirectory()) {
      outFiles.push_back(currentPath);
      return; // If it's not a directory, just add the file and return
    }
    
    Vector<Path> files, directories;
    FileSystem::getChildren(currentPath, files, directories);
    for (const auto& file : files) {
      outFiles.push_back(file.getRelative(relativePath));
    }

    for (const auto& dir : directories) {
      // Recursively collect files from subdirectories
      _recursiveCollectFiles(dir, m_root, outFiles);
    }

  }

}
