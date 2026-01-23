/*****************************************************************************/
/**
 * @file    geDataStream.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/09/19
 * @brief   General purpose class used for encapsulating the reading and
 *          writing of data
 *
 * General purpose class used for encapsulating the reading and writing of data
 * from and to various sources using a common interface.
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
#include "gePrerequisitesUtilities.h"
#include <istream>

namespace geEngineSDK {
  /**
   * @brief Supported encoding types for strings.
   */
  namespace STRING_ENCODING {
    enum E {
      kUTF8 = 1,
      kUTF16 = 2
    };
  }

  namespace ACCESS_MODE {
    enum E {
      kREAD = 1,
      kWRITE = 2
    };
  }

  /**
   * @brief	General purpose class used for encapsulating the reading and writing
   *        of data from and to various sources using a common interface.
   */
  class GE_UTILITIES_EXPORT DataStream
  {
   public:
    /**
     * @brief Creates an unnamed stream.
     */
    DataStream(uint16 accessMode = ACCESS_MODE::kREAD)
      : m_access(accessMode)
    {}

    /**
     * @brief Creates a named stream.
     */
    DataStream(const String& name, uint16 accessMode = ACCESS_MODE::kREAD) 
      : m_name(name),
        m_access(accessMode) {}

    virtual ~DataStream() = default;

    const String&
    getName(void) {
      return m_name;
    }

    uint16
    getAccessMode() const {
      return m_access;
    }

    virtual bool
    isReadable() const {
      return (m_access & ACCESS_MODE::kREAD) != 0;
    }

    virtual bool
    isWriteable() const {
      return (m_access & ACCESS_MODE::kWRITE) != 0;
    }

    virtual bool
    isFile() const = 0;

    /**
     * @brief Reads data from the buffer and copies it to the specified value.
     */
    template<typename T>
    DataStream&
    operator>>(T& val){
      read(static_cast<void*>(&val), sizeof(T));
      return *this;
    }

    /**
     * @brief Read the requisite number of bytes from the stream, stopping at
     *        the end of the file.
     * @param[in] buf Pre-allocated buffer to read the data into.
     * @param[in] count Number of bytes to read.
     * @return Number of bytes actually read.
     * @note Stream must be created with READ access mode.
     */
    virtual SIZE_T
    read(void* buf, SIZE_T count) = 0;

    /**
     * @brief Write the requisite number of bytes to the stream.
     * @param[in] buf Buffer containing bytes to write.
     * @param[in] count Number of bytes to write.
     * @return Number of bytes actually written.
     * @note Stream must be created with WRITE access mode.
     */
    virtual SIZE_T
    write(const void* buf, SIZE_T count) {
      GE_UNREFERENCED_PARAMETER(buf);
      GE_UNREFERENCED_PARAMETER(count);
      return 0;
    }

    /**
     * @brief Writes the provided narrow string to the steam. String is
     *        converted to the required encoding before being written.
     * @param[in] string String containing narrow characters to write, encoded as UTF8.
     * @param[in] encoding Encoding to convert the string to before writing.
     */
    virtual void
    writeString(const String& string, STRING_ENCODING::E encoding = STRING_ENCODING::kUTF8);

    /**
     * @brief Writes the provided wide string to the steam. String is converted
     *        to the required encoding before being written.
     * @param[in] string String containing wide characters to write, encoded as
     *            specified by platform for wide characters.
     * @param[in]	encoding	Encoding to convert the string to before writing.
     */
    virtual void
    writeString(const WString& string, STRING_ENCODING::E encoding = STRING_ENCODING::kUTF16);

    /**
     * @brief Returns a string containing the entire stream.
     * @return String data encoded as UTF-8.
     * @note  This is a convenience method for text streams only, allowing you
     *        to retrieve a String object containing all the data in the stream.
     */
    virtual String
    getAsString();

    /**
     * @brief Returns a wide string containing the entire stream.
     * @return  Wide string encoded as specified by current platform.
     * @note  This is a convenience method for text streams only, allowing you
     *        to retrieve a WString object containing all the data in the stream.
     */
    virtual WString
    getAsWString();

    /**
     * @brief Skip a defined number of bytes. This can also be a negative value,
     *        in which case the file pointer rewinds a defined number of bytes.
     */
    virtual void
    skip(SIZE_T count) = 0;

    /**
     * @brief Repositions the read point to a specified byte.
     */
    virtual void
    seek(SIZE_T pos) = 0;

    /**
     * @brief Returns the current byte offset from beginning.
     */
    virtual SIZE_T
    tell() const = 0;

    /**
     * @brief Returns true if the stream has reached the end.
     */
    virtual bool
    isEOF() const = 0;

    /**
     * @brief Returns the total size of the data to be read from the stream, or
     *        0 if this is indeterminate for this stream.
     */
    SIZE_T
    size() const { return m_size; }

    /**
     * @brief Creates a copy of this stream.
     * @param[in] copyData  If true the internal stream data will be copied as well,
     *            otherwise it will just reference the data from the original stream
     *            (in which case the caller must ensure the original stream outlives
     *            the clone). This is not relevant for file streams.
     */
    virtual SPtr<DataStream>
    clone(bool copyData = true) const = 0;

    /**
     * @brief Close the stream. This makes further operations invalid.
     */
    virtual void
    close() = 0;

   protected:
    static const uint32 streamTempSize;

    String m_name;
    SIZE_T m_size = 0;
    uint16 m_access;
  };

  /**
   * @brief Data stream for handling data from memory.
   */
  class GE_UTILITIES_EXPORT MemoryDataStream : public DataStream
  {
   public:
    /**
     * @brief Allocates a new chunk of memory and wraps it in a stream.
     * @param[in]  size  Size of the memory chunk in bytes.
     */
    MemoryDataStream(SIZE_T size);

    /**
     * @brief Wrap an existing memory chunk in a stream.
     * @param[in] memory  Memory to wrap the data stream around.
     * @param[in] inSize    Size of the memory chunk in bytes.
     * @param[in] freeOnClose Should the memory buffer be freed when the data
     *            stream goes out of scope.
     */
    MemoryDataStream(void* memory, SIZE_T inSize, bool freeOnClose = true);

    /**
     * @brief Create a stream which pre-buffers the contents of another stream.
     *        Data from the other buffer will be entirely read and stored in an
     *        internal buffer.
     * @param[in] sourceStream  Stream to read data from.
     */
    MemoryDataStream(DataStream& sourceStream);

    /**
     * @brief Create a stream which pre-buffers the contents of another stream.
     *        Data from the other buffer will be entirely read and stored in an
     *        internal buffer.
     * @param[in]	sourceStream		Stream to read data from.
     */
    explicit MemoryDataStream(const SPtr<DataStream>& sourceStream);

    ~MemoryDataStream();

    bool
    isFile() const override {
      return false;
    }

    /**
     * @brief Get a pointer to the start of the memory block this stream holds.
     */
    uint8*
    getPtr() const {
      return m_data;
    }

    /**
     * @brief Get a pointer to the current position in the memory block this stream holds.
     */
    uint8*
    getCurrentPtr() const {
      return m_pos;
    }

    /**
     * @brief @copydoc DataStream::read
     */
    SIZE_T
    read(void* buf, SIZE_T count) override;

    /**
     * @brief @copydoc DataStream::write
     */
    SIZE_T
    write(const void* buf, SIZE_T count) override;

    /**
     * @brief @copydoc DataStream::skip
     */
    void
    skip(SIZE_T count) override;

    /**
     * @brief @copydoc DataStream::seek
     */
    void
    seek(SIZE_T pos) override;

    /**
     * @brief @copydoc DataStream::tell
     */
    SIZE_T
    tell() const override;

    /**
     * @brief @copydoc DataStream::isEOF
     */
    bool
    isEOF() const override;

    /**
     * @brief @copydoc DataStream::clone
     */
    SPtr<DataStream>
    clone(bool copyData = true) const override;

    /**
     * @brief @copydoc DataStream::close
     */
    void
    close() override;

   protected:
    uint8* m_data;
    uint8* m_pos;
    uint8* m_end;

    bool m_freeOnClose;
  };

  /**
   * @brief Data stream for handling data from standard streams.
   */
  class GE_UTILITIES_EXPORT FileDataStream : public DataStream
  {
   public:
    /**
     * @brief Construct a file stream.
     * @param[in] filePath  Path of the file to open.
     * @param[in] accessMode  Determines should the file be opened in read,
     *            write or read/write mode.
     * @param[in] freeOnClose Determines should the internal stream be freed
     *            once the data stream is closed or goes out of scope.
     */
    FileDataStream(const Path& filePath,
                   ACCESS_MODE::E accessMode = ACCESS_MODE::kREAD,
                   bool freeOnClose = true);

    ~FileDataStream();

    bool
    isFile() const override {
      return true;
    }

    /**
     * @brief @copydoc DataStream::read
     */
    SIZE_T
    read(void* buf, SIZE_T count) override;

    /**
     * @brief @copydoc DataStream::write
     */
    SIZE_T
    write(const void* buf, SIZE_T count) override;

    /**
     * @brief @copydoc DataStream::skip
     */
    void
    skip(SIZE_T count) override;

    /**
     * @brief @copydoc DataStream::seek
     */
    void
    seek(SIZE_T pos) override;

    /**
     * @brief @copydoc DataStream::tell
     */
    SIZE_T
    tell() const override;

    /**
     * @brief @copydoc DataStream::isEOF
     */
    bool
    isEOF() const override;

    /**
     * @brief @copydoc DataStream::clone
     */
    SPtr<DataStream>
    clone(bool copyData = true) const override;

    /**
     * @brief @copydoc DataStream::close
     */
    void
    close() override;

    /**
     * @brief Returns the path of the file opened by the stream.
     */
    const Path& getPath() const {
      return m_path;
    }

   protected:
    Path m_path;
    SPtr<std::istream> m_pInStream;
    SPtr<std::ifstream> m_pFStreamRO;
    SPtr<std::fstream> m_pFStream;
    bool m_freeOnClose;
  };
}
