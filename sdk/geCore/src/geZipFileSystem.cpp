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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geZipFileSystem.h"
#include "geZipDataStream.h"

#include <geDebug.h>
#include <geStringID.h>

#include <mz.h>
#include <mz_zip.h>
#include <mz_strm.h>
#include <mz_zip_rw.h>
#include <mz_strm_os.h>

namespace geEngineSDK {

  ZipFileSystem::ZipFileSystem(const Path& zipPath)
    : m_zipPath(zipPath) {
    m_zipHandle = mz_zip_reader_create();

    if (MZ_OK != mz_zip_reader_open_file(m_zipHandle,
      zipPath.toString().c_str())) {
      mz_zip_reader_delete(&m_zipHandle);
      GE_EXCEPT(FileNotFoundException,
                "No se pudo abrir el archivo ZIP: " + zipPath.toString());
    }

    _buildIndex();
  }

  ZipFileSystem::~ZipFileSystem() {
    if (m_zipHandle) {
      mz_zip_reader_close(m_zipHandle);
      mz_zip_reader_delete(&m_zipHandle);
    }
  }

  void
  ZipFileSystem::_buildIndex() {
    if (MZ_OK != mz_zip_reader_goto_first_entry(m_zipHandle)) {
      return;
    }

    do {
      mz_zip_file* fileInfo = nullptr;
      if (MZ_OK != mz_zip_reader_entry_get_info(m_zipHandle, &fileInfo)) {
        continue;
      }

      if (!fileInfo->filename || fileInfo->uncompressed_size == 0) {
        continue;
      }

      String normalized = String(fileInfo->filename);
      normalized = StringUtil::replaceAll(normalized, "\\", "/");

      //Copy the file metadata into our ZipFileData structure
      ZipFileData zipFileData;
      zipFileData.filename = fileInfo->filename;
      zipFileData.compressed_size = fileInfo->compressed_size;
      zipFileData.uncompressed_size = fileInfo->uncompressed_size;
      m_fileIndex[normalized] = zipFileData;

    }while(MZ_OK == mz_zip_reader_goto_next_entry(m_zipHandle));
  }

  bool
  ZipFileSystem::exists(const Path& path) const {
    auto fixedPath = path.toString();
    fixedPath = StringUtil::replaceAll(fixedPath, "\\", "/");
    return m_fileIndex.find(fixedPath) != m_fileIndex.end();
  }

  SPtr<DataStream>
  ZipFileSystem::open(const Path& path) {
    auto fixedPath = path.toString();
    fixedPath = StringUtil::replaceAll(fixedPath, "\\", "/");
    auto it = m_fileIndex.find(fixedPath);
    if (it == m_fileIndex.end()) {
      return nullptr;
    }

    return ge_shared_ptr_new<ZipDataStream>(m_zipHandle, it->second);
  }

  Vector<Path>
  ZipFileSystem::getAllFiles() const {
    Vector<Path> allFiles;
    for (const auto& [pathStr, _] : m_fileIndex) {
      allFiles.push_back(Path(pathStr));
    }
    return allFiles;
  }

} // namespace geEngineSDK
