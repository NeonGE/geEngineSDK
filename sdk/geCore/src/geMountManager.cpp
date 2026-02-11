/*****************************************************************************/
/**
 * @file    geMountManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/24
 * @brief   Mount Manager.
 *
 * Mount Manager. This system provides an interface for managing mounted ZIP
 * files and other file systems.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geMountManager.h"

namespace geEngineSDK {

  void
  MountManager::mount(const SPtr<ZipFileSystem>& zipFs) {
    m_zipMounts.push_back(zipFs);

    for (const auto& path : zipFs->getAllFiles()) {
      _addToIndex(path, FS_TYPE::kZIP, path, zipFs.get());
    }
  }

  void
  MountManager::mount(const SPtr<DiskFileSystem>& diskFs) {
    m_diskMounts.push_back(diskFs);

    for (const auto& path : diskFs->getAllFiles()) {
      _addToIndex(path, FS_TYPE::kDISK, path, diskFs.get());
    }
  }

  void
  MountManager::_addToIndex(const Path& virtualPath,
                           FS_TYPE::E type,
                           const Path& internalPath,
                           void* backend) {
    String key = virtualPath.toString();
    StringUtil::toLowerCase(key);
    m_fileIndex[key] = { virtualPath, internalPath, type, backend };
  }

  bool
  MountManager::exists(const Path& path) const {
    String key = path.toString();
    StringUtil::toLowerCase(key);
    return m_fileIndex.find(key) != m_fileIndex.end();
  }

  SPtr<DataStream>
  MountManager::open(const Path& path) {
    String key = path.toString();
    StringUtil::toLowerCase(key);
    auto it = m_fileIndex.find(key);
    if (it == m_fileIndex.end()) {
      return nullptr;
    }

    const FileEntry& entry = it->second;

    switch (entry.sourceType) {
    case FS_TYPE::kZIP:
      return cast::st<ZipFileSystem*>(entry.backend)->open(entry.internalPath);
    case FS_TYPE::kDISK:
      return cast::st<DiskFileSystem*>(entry.backend)->open(entry.internalPath);
    default:
      return nullptr;
    }
  }

  Path
  MountManager::getRealPath(const Path& virtualPath) const {
    String key = virtualPath.toString();
    StringUtil::toLowerCase(key);
    auto it = m_fileIndex.find(key);
    if (it == m_fileIndex.end()) {
      return Path(); // Not found
    }

    const FileEntry& entry = it->second;
    return entry.internalPath;
  }

  void
  MountManager::clear() {
    m_zipMounts.clear();
    m_diskMounts.clear();
    m_fileIndex.clear();
  }

}
