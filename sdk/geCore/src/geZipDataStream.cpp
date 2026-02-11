/*****************************************************************************/
/**
 * @file    geZipDataStream.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/23
 * @brief   Encapsulate reading and writing of data from and to ZIP files.
 *
 * Encapsulate reading and writing of data from and to ZIP files.
 *
 * @bug	   No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geZipDataStream.h"
#include <geException.h>
#include <geMath.h>

#include <mz.h>
#include <mz_zip.h>
#include <mz_strm.h>
#include <mz_zip_rw.h>
#include <mz_strm_mem.h>
#include <mz_strm_buf.h>
#include <mz_os.h>

namespace geEngineSDK {
  ZipDataStream::ZipDataStream(void* zipHandle,
                               const ZipFileData& fileInfo,
                               uint16 accessMode)
    : DataStream(String(fileInfo.filename), accessMode),
      m_zipHandle(zipHandle),
      m_pos(0) {
    //Check the access mode as the zip file can only be opened for reading
    GE_ASSERT(accessMode == ACCESS_MODE::kREAD &&
              "ZipDataStream can only be opened for reading");
    this->m_size = cast::st<SIZE_T>(fileInfo.uncompressed_size);
    m_data.resize(m_size);

    //Get to the entry in the ZIP file
    if (MZ_OK != mz_zip_reader_locate_entry(zipHandle, fileInfo.filename.c_str(), 1)) {
      GE_EXCEPT(FileNotFoundException,
                "Couldn't find the file " +
                String(fileInfo.filename) +
                " in the specified ZIP");
    }

    //Open the entry for reading
    auto result = mz_zip_reader_entry_open(zipHandle);
    if (MZ_OK != result) {
      GE_EXCEPT(InvalidStateException,
                "Couldn't open the Zip Entry");
    }

    //Read the file data into the buffer
    int32 bytesRead = mz_zip_reader_entry_read(zipHandle,
                                               &m_data[0],
                                               static_cast<int32_t>(m_size));
    if (bytesRead < 0 || cast::st<SIZE_T>(bytesRead) != m_size) {
      mz_zip_reader_entry_close(zipHandle);
      GE_EXCEPT(InvalidStateException,
                "Couldn't read the Zip Entry data, "
                "read bytes: " + toString(bytesRead) +
                ", expected: " + toString(m_size));
    }

    mz_zip_reader_entry_close(zipHandle);
  }

  ZipDataStream::~ZipDataStream() {
    close();
  }

  SIZE_T
  ZipDataStream::read(void* buf, SIZE_T count) {
    if (isEOF()) {
      return 0;
    }

    SIZE_T available = m_size - m_pos;
    SIZE_T toRead = Math::min(count, available);

    memcpy(buf, m_data.data() + m_pos, toRead);
    m_pos += toRead;

    return toRead;
  }

  SIZE_T
  ZipDataStream::write(const void* buf, SIZE_T count) {
    GE_UNREFERENCED_PARAMETER(buf);
    GE_UNREFERENCED_PARAMETER(count);
    GE_ASSERT(false && "ZipDataStream is read-only");
    return 0; // Solo lectura
  }

  void
  ZipDataStream::skip(SIZE_T count) {
    m_pos = Math::min(m_pos + count, m_size);
  }

  void
  ZipDataStream::seek(SIZE_T pos) {
    m_pos = Math::min(pos, m_size);
  }

  SIZE_T
  ZipDataStream::tell() const {
    return m_pos;
  }

  bool
  ZipDataStream::isEOF() const {
    return m_pos >= m_size;
  }

  SPtr<DataStream>
  ZipDataStream::clone(bool copyData) const {
    if (!copyData) {
      //Returns a new stream that shares the same memory buffer
      auto newStream = ge_shared_ptr_new<MemoryDataStream>(const_cast<uint8*>(m_data.data()),
                                                           m_size,
                                                           false);
      newStream->seek(m_pos);
      return newStream;
    }
    else {
      auto newStream = ge_shared_ptr_new<MemoryDataStream>(m_size);
      memcpy(newStream->getPtr(), m_data.data(), m_size);
      newStream->seek(m_pos);
      return newStream;
    }
  }

  void
  ZipDataStream::close() {
    m_data.clear();
    m_pos = 0;
    m_size = 0;
  }

}
