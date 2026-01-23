/*****************************************************************************/
/**
 * @file    geCompression.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/15
 * @brief   Performs generic compression and decompression on raw data
 *
 * Performs generic compression and decompression on raw data
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geCompression.h"
#include "geDataStream.h"
#include "geDebug.h"
#include "externals/lz4.h"

namespace geEngineSDK {
  using std::static_pointer_cast;

  SPtr<MemoryDataStream>
  Compression::compress(SPtr<DataStream>& input,
                        function<void(float)> reportProgress) {
    //LZ4 provides a function that will tell you the maximum size of compressed
    //output based on input data via LZ4_compressBound().
    const auto maxDstSize = static_cast<SIZE_T>(LZ4_compressBound(
                                                static_cast<int32>(input->size())));

    //We will use that size for our destination boundary when allocating space.
    MemoryDataStream compData(maxDstSize);

    int32 compSize = -1; //Initially set to error
    {//Make a local copy of the input data
      MemoryDataStream inputData(input);

      //Invoke LZ4_compress_default
      compSize = LZ4_compress_default(reinterpret_cast<char*>(inputData.getPtr()),
                                      reinterpret_cast<char*>(compData.getPtr()),
                                      static_cast<int32>(inputData.size()),
                                      static_cast<int32>(maxDstSize));
    }

    //Check return_value to determine what happened.
    if (compSize < 0) {
      reportProgress(1.0f);
      GE_LOG(kError, Generic, "Failure trying to compress the data.");
      return nullptr;
    }
    else if (compSize == 0) {
      reportProgress(1.0f);
      GE_LOG(kError, Generic, "Destination buffer couldn't hold all the information.");
      return nullptr;
    }

    //Create the return buffer set to the real size of the compression
    //We will save the first the size of the real data, and then the data
    SPtr<MemoryDataStream> realComp_data = ge_shared_ptr_new<MemoryDataStream>(
                                                    compSize + sizeof(uint64));

    //This value indicates the real size of the data,
    //we need to save this for decompression purposes
    auto headerSizeData = static_cast<uint64>(input->size());

    realComp_data->write(&headerSizeData, sizeof(uint64));
    realComp_data->write(compData.getPtr(), compSize);

    //Set the buffer to the starting point
    realComp_data->seek(0);

    if (reportProgress) {
      reportProgress(1.0f);
    }

    return realComp_data;
  }

  SPtr<MemoryDataStream>
  Compression::decompress(SPtr<DataStream>& input,
                          function<void(float)> reportProgress) {
    MemoryDataStream src(input);
    
    //Read the original size of the data from the compressed buffer
    uint64 originalDataSize = 0;
    src >> originalDataSize;

    if (!originalDataSize) {
      if (reportProgress) {
        reportProgress(1.0f);
      }
      GE_LOG(kError, Generic, "Invalid compressed data");
      return nullptr;
    }

    //Create a buffer the size of the original data
    SPtr<MemoryDataStream> decompData = ge_shared_ptr_new<MemoryDataStream>
                                        (static_cast<SIZE_T>(originalDataSize));

    int32 decompSize = LZ4_decompress_safe(reinterpret_cast<char*>(src.getCurrentPtr()),
                                           reinterpret_cast<char*>(decompData->getPtr()),
                                           static_cast<int32>(src.size() - sizeof(uint64)),
                                           static_cast<int32>(originalDataSize));
    //Set the buffer to the starting point
    decompData->seek(0);

    if (decompSize < 0) {
      if (reportProgress) {
        reportProgress(1.0f);
      }
      GE_LOG(kError, Generic, "Failure trying to decompress the data.");
      return nullptr;
    }
    
    if (decompSize != static_cast<int32>(originalDataSize)) { //This should never happen
      if (reportProgress) {
        reportProgress(1.0f);
      }
      GE_LOG(kError, Generic, "Difference in data compressed and decompressed.");
      return nullptr;
    }

    if (reportProgress) {
      reportProgress(1.0f);
    }
    return decompData;
  }
}
