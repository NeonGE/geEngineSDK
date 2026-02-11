/*****************************************************************************/
/**
 * @file    geZipFileSystem.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/15
 * @brief   Zip File System.
 *
 * Zip File System. This system provides an interface for managing ZIP files.
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
#include "geZipDataStream.h"

#include <geDebug.h>
#include <geStringID.h>

namespace geEngineSDK {

  class GE_CORE_EXPORT ZipFileSystem
  {
   public:
    explicit ZipFileSystem(const Path& zipPath);
    virtual ~ZipFileSystem();

    bool
    exists(const Path& path) const;

    SPtr<DataStream>
    open(const Path& path);

    Vector<Path>
    getAllFiles() const;

   private:
    void
    _buildIndex();

    void* m_zipHandle = nullptr;
    Path m_zipPath;
    UnorderedMap<String, ZipFileData> m_fileIndex;
  };

}
