/*****************************************************************************/
/**
 * @file    geMemorySerializer.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/03
 * @brief   Encodes/decodes an IReflectable object from/to memory.
 *
 * Encodes/decodes an IReflectable object from/to memory.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"

namespace geEngineSDK {
  struct SerializationContext;

  using std::function;

  class GE_UTILITIES_EXPORT MemorySerializer
  {
    struct BufferPiece
    {
      uint8* buffer;
      uint32 size;
    };

   public:
    MemorySerializer() = default;
    ~MemorySerializer() = default;

    /**
     * @brief Parses the provided object, serializes all of its data as
     *        specified by its RTTIType and returns the data in the form of raw
     *        memory.
     * @param[in] object        Object to encode.
     * @param[in] bytesWritten  Output value containing the total number of
     *            bytes it took to encode the object.
     * @param[in] allocator     Determines how is memory allocated. If not
     *            specified the default allocator is used.
     * @param[in] shallow       Determines how to handle referenced objects.
     *            If true then references will not be encoded and will be set
     *            to null. If false then references will be encoded as well and
     *            restored upon decoding.
     * @param[in] params        Optional parameters to be passed to the
     *            serialization callbacks on the objects being serialized.
     * @return  A buffer containing the encoded object. It is up to the user to
     *          release the buffer memory when no longer needed.
     */
    uint8*
    encode(IReflectable* object,
           uint32& bytesWritten,
           function<void*(SIZE_T)> allocator = nullptr,
           bool shallow = false,
           SerializationContext* context = nullptr);

    /**
     * @brief Deserializes an IReflectable object by reading the binary data
              from the provided memory location.
     * @param[in] buffer      Previously allocated buffer to store the data in.
     * @param[in] bufferSize  Size of the @p buffer in bytes.
     * @param[in] params      Optional parameters to be passed to the
                  serialization callbacks on the objects being serialized.
     */
    SPtr<IReflectable>
    decode(uint8* buffer,
           uint32 bufferSize,
           SerializationContext* context = nullptr);

   private:
    Vector<BufferPiece> m_bufferPieces;

    /**
     * @brief Called by the binary serializer whenever the buffer gets full.
     */
    uint8*
    flushBuffer(uint8* bufferStart, uint32 bytesWritten, uint32& newBufferSize);

   private:
    static CONSTEXPR const uint32 WRITE_BUFFER_SIZE = 16384;
  };
}
