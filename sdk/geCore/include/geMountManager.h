/*****************************************************************************/
/**
 * @file    geMountManager.h
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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"
#include "geZipFileSystem.h"
#include "geDiskFileSystem.h"

#include <geModule.h>

namespace geEngineSDK {

  namespace FS_TYPE {
    enum E {
      kZIP,
      kDISK
    };
  }

  class GE_CORE_EXPORT MountManager : public Module<MountManager>
  {
   public:
    MountManager() = default;
    virtual ~MountManager() = default;

    void
    mount(const SPtr<ZipFileSystem>& zipFs);

    void
    mount(const SPtr<DiskFileSystem>& diskFs);

    bool
    exists(const Path& virtualPath) const;

    SPtr<DataStream>
    open(const Path& virtualPath);

    Path
    getRealPath(const Path& virtualPath) const;

    void
    clear();

   private:
    struct FileEntry
    {
      Path virtualPath;
      Path internalPath;
      FS_TYPE::E sourceType;
      void* backend;
    };

    void
    _addToIndex(const Path& virtualPath,
                FS_TYPE::E type,
                const Path& internalPath,
                void* backend);

    Vector<SPtr<ZipFileSystem>> m_zipMounts;
    Vector<SPtr<DiskFileSystem>> m_diskMounts;

    UnorderedMap<String, FileEntry> m_fileIndex;
  };

}
