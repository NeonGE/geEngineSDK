/*****************************************************************************/
/**
 * @file    geZipDataStream.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/23
 * @brief   Encapsulate reading and writing of data from and to ZIP files.
 *
 * Encapsulate reading and writing of data from and to ZIP files.
 *
 * @bug	   No known bugs.
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

namespace geEngineSDK {

  /**
   * @brief Represents metadata and attributes for a file entry within a ZIP
   *        archive. We create this here to encapsulate the file's metadata
   *        and avoid direct dependency on mz_zip_file structure.
   */
  struct ZipFileData {
    String filename;
    uint64_t uncompressed_size;
    uint64_t compressed_size;
  };

  class GE_CORE_EXPORT ZipDataStream : public DataStream
  {
   public:
    ZipDataStream(void* zipHandle,
                  const ZipFileData& fileInfo,
                  uint16 accessMode = ACCESS_MODE::kREAD);

    virtual ~ZipDataStream();

    // Override methods from DataStream
    bool
      isFile() const override {
      return true;
    }

    SIZE_T
    read(void* buf, SIZE_T count) override;

    SIZE_T
    write(const void* buf, SIZE_T count) override;

    void
    skip(SIZE_T count) override;

    void
    seek(SIZE_T pos) override;

    SIZE_T
    tell() const override;

    bool
    isEOF() const override;

    SPtr<DataStream>
    clone(bool copyData = true) const override;

    void
    close() override;

   private:
    void* m_zipHandle;
    Vector<uint8> m_data; //The file will be fully loaded into memory
    SIZE_T m_pos = 0;
  };

}
