/*****************************************************************************/
/**
 * @file    geDataStream.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/09/19
 * @brief   General purpose class used for encapsulating the reading and
 *          writing of data
 *
 * General purpose class used for encapsulating the reading and writing of data
 * from and to various sources using a common interface.
 *
 * @bug -std::consume_header seems to be ignored so I manually remove the
 *      header.
 *
 *      -Regardless of not providing the std::little_endian flag it
 *      seems that is how the data is read so I manually flip it.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geDataStream.h"
#include "geDebug.h"
#include "geUnicode.h"

namespace geEngineSDK {

  const uint32 DataStream::streamTempSize = 128;

  /**
   * @brief	Checks does the provided buffer has an UTF32 byte order mark in
   *        little endian order.
   */
  bool
  isUTF32LE(const char* buffer) {
    return (0xFF == static_cast<uint8>(buffer[0]) &&
            0xFE == static_cast<uint8>(buffer[1]) &&
            0x00 == static_cast<uint8>(buffer[2]) &&
            0x00 == static_cast<uint8>(buffer[3]));
  }

  /**
   * @brief Checks does the provided buffer has an UTF32 byte order mark in
   *        big endian order.
   */
  bool
  isUTF32BE(const char* buffer) {
    return (0x00 == static_cast<uint8>(buffer[0]) &&
            0x00 == static_cast<uint8>(buffer[1]) &&
            0xFE == static_cast<uint8>(buffer[2]) &&
            0xFF == static_cast<uint8>(buffer[3]));
  }

  /**
   * @brief Checks does the provided buffer has an UTF16 byte order mark in
   *        little endian order.
   */
  bool
  isUTF16LE(const char* buffer) {
    return (0xFF == static_cast<uint8>(buffer[0]) &&
            0xFE == static_cast<uint8>(buffer[1]));
  }

  /**
   * @brief Checks does the provided buffer has an UTF16 byte order mark in
   *        big endian order.
   */
  bool
  isUTF16BE(const char* buffer) {
    return (0xFE == static_cast<uint8>(buffer[0]) &&
            0xFF == static_cast<uint8>(buffer[1]));
  }

  /**
   * @brief Checks does the provided buffer has an UTF8 byte order mark.
   */
  bool
  isUTF8(const char* buffer) {
    return (0xEF == static_cast<uint8>(buffer[0]) &&
            0xBB == static_cast<uint8>(buffer[1]) &&
            0xBF == static_cast<uint8>(buffer[2]));
  }

  void
  DataStream::writeString(const String& str, STRING_ENCODING::E encoding) {
    if (STRING_ENCODING::kUTF16 == encoding) {
      //Write BOM
      uint8 bom[2] = { 0xFF, 0xFE };
      write(bom, sizeof(bom));

      U16String u16string = UTF8::toUTF16(str);
      write(u16string.data(), u16string.length() * sizeof(char16_t));
    }
    else {
      // Write BOM
      uint8 bom[3] = { 0xEF, 0xBB, 0xBF };
      write(bom, sizeof(bom));
      write(str.data(), str.length());
    }
  }

  void
  DataStream::writeString(const WString& str, STRING_ENCODING::E encoding) {
    if (STRING_ENCODING::kUTF16 == encoding) {
      //Write BOM
      uint8 bom[2] = { 0xFF, 0xFE };
      write(bom, sizeof(bom));

      String u8string = UTF8::fromWide(str);
      U16String u16string = UTF8::toUTF16(u8string);
      write(u16string.data(), u16string.length() * sizeof(char16_t));
    }
    else {
      // Write BOM
      uint8 bom[3] = { 0xEF, 0xBB, 0xBF };
      write(bom, sizeof(bom));

      String u8string = UTF8::fromWide(str);
      write(u8string.data(), u8string.length());
    }
  }

  String
  DataStream::getAsString() {
    //Ensure read from begin of stream
    seek(0);

    //Try reading header
    uint8 headerBytes[4];
    SIZE_T numHeaderBytes = read(headerBytes, 4);

    SIZE_T dataOffset = 0;
    if (4 <= numHeaderBytes) {
      if (isUTF32LE(reinterpret_cast<char*>(headerBytes))) {
        dataOffset = 4;
      }
      else if (isUTF32BE(reinterpret_cast<char*>(headerBytes))) {
        GE_LOG(kWarning, Generic, "UTF-32 big endian decoding not supported");
        return U8STRING("");
      }
    }

    if (0 == dataOffset && 3 <= numHeaderBytes) {
      if (isUTF8(reinterpret_cast<char*>(headerBytes))) {
        dataOffset = 3;
      }
    }

    if (0 == dataOffset && 2 <= numHeaderBytes) {
      if (isUTF16LE(reinterpret_cast<char*>(headerBytes))) {
        dataOffset = 2;
      }
      else if (isUTF16BE(reinterpret_cast<char*>(headerBytes))) {
        GE_LOG(kWarning, Generic, "UTF-16 big endian decoding not supported");
        return U8STRING("");
      }
    }

    seek(dataOffset);

    //Read whats left on the buffer, not the full size
    const SIZE_T remaining = (m_size > dataOffset) ? (m_size - dataOffset) : 0;
    const SIZE_T bufSize = (remaining > 0 ? remaining : 4096);
    
    //TODO: Change this to use the stack allocator, however right now we
    //haven't initialized the stack yet on the engine
    auto* tempBuffer = static_cast<StringStream::char_type*>(ge_alloc(bufSize));

    StringStream result;
    while (!isEOF()) {
      SIZE_T numReadBytes = read(tempBuffer, bufSize);
      result.write(tempBuffer, static_cast<std::streamsize>(numReadBytes));
    }

    //TODO: Change this to use the stack allocator, however right now we
    //haven't initialized the stack yet on the engine
    ge_free(tempBuffer);

    String raw = result.str();

    switch (dataOffset)
    {
      default:
      case 0: //No BOM = assumed UTF-8
      case 3: //UTF-8
        return String(raw.data(), raw.length());
      case 2: //UTF-16
        {
          const SIZE_T byteCount = raw.length();
          if ((byteCount & 1) != 0) {
            //non-pair bytes on UTF-16 means the data is invalid
            GE_LOG(kWarning, Generic, "Invalid UTF-16 byte length");
            return U8STRING("");
          }

          const SIZE_T numElems = byteCount / sizeof(char16_t);
          U16String u16;
          u16.resize(static_cast<size_t>(numElems));
#if USING(GE_CPP17_OR_LATER)
          auto u16Data = u16.data();
#else
          auto u16Data = &u16[0];
#endif
          memcpy(u16Data, raw.data(), static_cast<size_t>(byteCount));

          return UTF8::fromUTF16(u16);
        }
      case 4: //UTF-32
        {
          const SIZE_T byteCount = raw.length();
          if ((byteCount & 3) != 0) {
            GE_LOG(kWarning, Generic, "Invalid UTF-32 byte length");
            return U8STRING("");
          }

          const SIZE_T numElems = byteCount / sizeof(char32_t);
          U32String u32;
          u32.resize(static_cast<size_t>(numElems));
#if USING(GE_CPP17_OR_LATER)
          auto u32Data = u32.data();
#else
          auto u32Data = &u32[0];
#endif
          std::memcpy(u32Data, raw.data(), static_cast<size_t>(byteCount));

          return UTF8::fromUTF32(u32);
        }
    }
  }

  WString
  DataStream::getAsWString() {
    String u8string = getAsString();
    return UTF8::toWide(u8string);
  }

  MemoryDataStream::MemoryDataStream(SIZE_T size)
    : DataStream(ACCESS_MODE::kREAD | ACCESS_MODE::kWRITE),
      m_data(nullptr),
      m_freeOnClose(true) {
    m_data = m_pos = reinterpret_cast<uint8*>(ge_alloc(size));
    m_size = size;
    m_end = m_data + m_size;

    GE_ASSERT(m_end >= m_pos);
  }

  MemoryDataStream::MemoryDataStream(void* memory, SIZE_T inSize, bool freeOnClose)
    : DataStream(ACCESS_MODE::kREAD | ACCESS_MODE::kWRITE),
      m_data(nullptr),
      m_freeOnClose(freeOnClose) {
    m_data = m_pos = static_cast<uint8*>(memory);
    m_size = inSize;
    m_end = m_data + m_size;

    GE_ASSERT(m_end >= m_pos);
  }

  MemoryDataStream::MemoryDataStream(DataStream& sourceStream)
    : DataStream(ACCESS_MODE::kREAD | ACCESS_MODE::kWRITE),
      m_data(nullptr) {
    //Copy data from incoming stream
    m_size = sourceStream.size();

    m_data = reinterpret_cast<uint8*>(ge_alloc(m_size));
    m_pos = m_data;
    m_end = m_data + sourceStream.read(m_data, m_size);
    m_freeOnClose = true;

    GE_ASSERT(m_end >= m_pos);
  }

  MemoryDataStream::MemoryDataStream(const SPtr<DataStream>& sourceStream)
    : DataStream(ACCESS_MODE::kREAD | ACCESS_MODE::kWRITE),
      m_data(nullptr) {
    //Copy data from incoming stream
    m_size = sourceStream->size();

    m_data = reinterpret_cast<uint8*>(ge_alloc(m_size));
    m_pos = m_data;
    m_end = m_data + sourceStream->read(m_data, m_size);
    m_freeOnClose = true;

    GE_ASSERT(m_end >= m_pos);
  }

  MemoryDataStream::~MemoryDataStream() {
    close();
  }

  SIZE_T
  MemoryDataStream::read(void* buf, SIZE_T count) {
    if (count == 0) {
      return 0;
    }
    GE_ASSERT(buf != nullptr);

    const SIZE_T pos = static_cast<SIZE_T>(m_pos - m_data);
    const SIZE_T end = static_cast<SIZE_T>(m_end - m_data);
    GE_ASSERT(pos <= end);

    SIZE_T cnt = count;
    const SIZE_T avail = end - pos;
    if (cnt > avail) {
      cnt = avail;
    }
    if (cnt == 0) {
      return 0;
    }

    memcpy(buf, m_pos, cnt);
    m_pos += cnt;
    return cnt;
  }

  SIZE_T
  MemoryDataStream::write(const void* buf, SIZE_T count) {
    if (!isWriteable() || count == 0) {
      return 0;
    }
    GE_ASSERT(buf != nullptr);

    const SIZE_T pos = static_cast<SIZE_T>(m_pos - m_data);
    const SIZE_T end = static_cast<SIZE_T>(m_end - m_data);
    GE_ASSERT(pos <= end);

    SIZE_T written = count;
    const SIZE_T avail = end - pos;
    if (written > avail) {
      written = avail;
    }
    if (written == 0) {
      return 0;
    }

    memcpy(m_pos, buf, written);
    m_pos += written;
    return written;
  }

  void
  MemoryDataStream::skip(SIZE_T count) {
    const SIZE_T pos = static_cast<SIZE_T>(m_pos - m_data);
    const SIZE_T end = static_cast<SIZE_T>(m_end - m_data);
    GE_ASSERT(pos <= end);

    const SIZE_T avail = end - pos;
    GE_ASSERT(count <= avail);

    m_pos += count;
  }

  void
  MemoryDataStream::seek(SIZE_T pos) {
    GE_ASSERT(m_data + pos <= m_end);
    m_pos = m_data + pos;
  }

  SIZE_T
  MemoryDataStream::tell() const {
    GE_ASSERT(m_pos >= m_data);
    GE_ASSERT(m_pos <= m_end);

    const ptrdiff_t d = m_pos - m_data;
    return static_cast<SIZE_T>(d);
  }

  bool
  MemoryDataStream::isEOF() const {
    return m_pos >= m_end;
  }

  SPtr<DataStream>
  MemoryDataStream::clone(bool copyData) const {
    if (!copyData) {
      return ge_shared_ptr_new<MemoryDataStream>(m_data, m_size, false);
    }

    //Bugfix: Needs to be called like this to avoid slicing
    
    //This version works but gives a warning about deprecated cast
    //return ge_shared_ptr_new<MemoryDataStream>((DataStream&)(*this));
    
    //This version does not work because it tries to cast away constness
    //return ge_shared_ptr_new<MemoryDataStream>(static_cast<const DataStream&>(*this));

    //This version works without warnings
    return ge_shared_ptr_new<MemoryDataStream>(
      static_cast<DataStream&>(*const_cast<MemoryDataStream*>(this)));
  }

  void
  MemoryDataStream::close() {
    if (nullptr != m_data) {
      if (m_freeOnClose) {
        ge_free(m_data);
      }
      m_data = nullptr;
    }
  }

  FileDataStream::FileDataStream(const Path& filePath,
                                 ACCESS_MODE::E accessMode,
                                 bool freeOnClose)
    : DataStream(static_cast<uint16>(accessMode)),
      m_path(filePath),
      m_freeOnClose(freeOnClose) {
    //Always open in binary mode. Also, always include reading
    std::ios::openmode mode = std::ios::binary;

    if ((accessMode & ACCESS_MODE::kREAD) != 0) {
      mode |= std::ios::in;
    }

    if (((accessMode & ACCESS_MODE::kWRITE) != 0)) {
      mode |= std::ios::out;
      m_pFStream = ge_shared_ptr_new<std::fstream>();
      m_pFStream->open(filePath.toPlatformString().c_str(), mode);
      m_pInStream = m_pFStream;
    }
    else {
      m_pFStreamRO = ge_shared_ptr_new<std::ifstream>();
      m_pFStreamRO->open(filePath.toPlatformString().c_str(), mode);
      m_pInStream = m_pFStreamRO;
    }

    //Should check ensure open succeeded, in case fail for some reason.
    if (m_pInStream->fail()) {
      GE_LOG(kWarning, FileSystem, "Cannot open file: " + filePath.toString());
      return;
    }

    m_pInStream->seekg(0, std::ios_base::end);
    m_size = static_cast<SIZE_T>(m_pInStream->tellg());
    m_pInStream->seekg(0, std::ios_base::beg);
  }

  FileDataStream::~FileDataStream() {
    close();
  }

  SIZE_T
  FileDataStream::read(void* buf, SIZE_T count) {
    m_pInStream->read(reinterpret_cast<char*>(buf), static_cast<std::streamsize>(count));
    return static_cast<SIZE_T>(m_pInStream->gcount());
  }

  SIZE_T
  FileDataStream::write(const void* buf, SIZE_T count) {
    SIZE_T written = 0;
    if (isWriteable() && m_pFStream) {
      m_pFStream->write(static_cast<const char*>(buf), static_cast<std::streamsize>(count));
      written = count;
    }
    return written;
  }

  void
  FileDataStream::skip(SIZE_T count) {
    m_pInStream->clear(); //Clear fail status in case eof was set
    const auto offset = static_cast<std::istream::off_type>(count);
    m_pInStream->seekg(offset, std::ios::cur);
  }

  void
  FileDataStream::seek(SIZE_T pos) {
    m_pInStream->clear();	//Clear fail status in case eof was set
    m_pInStream->seekg(static_cast<std::streamoff>(pos), std::ios::beg);
  }

  SIZE_T
  FileDataStream::tell() const {
    m_pInStream->clear(); //Clear fail status in case eof was set
    return static_cast<SIZE_T>(m_pInStream->tellg());
  }

  bool
  FileDataStream::isEOF() const {
    //return m_pInStream->eof();
    return static_cast<SIZE_T>(m_pInStream->tellg()) >= m_size;
  }

  SPtr<DataStream>
  FileDataStream::clone(bool copyData) const {
    GE_UNREFERENCED_PARAMETER(copyData);
    return ge_shared_ptr_new<FileDataStream>(m_path,
                                             static_cast<ACCESS_MODE::E>(getAccessMode()),
                                             true);
  }

  void
  FileDataStream::close() {
    if (m_pInStream) {
      if (m_pFStreamRO) {
        m_pFStreamRO->close();
      }

      if (m_pFStream) {
        m_pFStream->flush();
        m_pFStream->close();
      }

      if (m_freeOnClose) {
        m_pInStream = nullptr;
        m_pFStreamRO = nullptr;
        m_pFStream = nullptr;
      }
    }
  }
}
