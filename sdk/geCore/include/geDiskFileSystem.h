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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"

#include <geDataStream.h>
#include <geDebug.h>
#include <geStringID.h>

namespace geEngineSDK {

  class GE_CORE_EXPORT DiskFileSystem
  {
   public:
    explicit DiskFileSystem(const Path& rootPath);
    virtual ~DiskFileSystem() = default;

    bool
    exists(const Path& path) const;

    SPtr<DataStream>
    open(const Path& path);

    Vector<Path>
    getAllFiles() const;

   private:
    void
    _recursiveCollectFiles(const Path& currentPath,
                           const Path& relativePath,
                           Vector<Path>& outFiles) const;

     Path m_root;
  };

}
