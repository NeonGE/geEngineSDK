/*****************************************************************************/
/**
 * @file    geBitmapWriter.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/09/21
 * @brief   Utility class for generating BMP images.
 *
 * Utility class for generating BMP images.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geBitmapWriter.h"

namespace geEngineSDK {
  //Align to 2 byte boundary so we don't get extra 2 bytes for this struct
#	pragma pack(push, 2)
  struct BMP_HEADER
  {
    uint16 BM;
    uint32 size_of_file;
    uint32 reserve;
    uint32 offset_of_pixel_data;
    uint32 size_of_header;
    uint32 width;
    uint32 hight;
    uint16 num_of_color_plane;
    uint16 num_of_bit_per_pix;
    uint32 compression;
    uint32 size_of_pix_data;
    uint32 h_resolution;
    uint32 v_resolution;
    uint32 num_of_color_in_palette;
    uint32 important_colors;
  };
#	pragma pack(pop)

  void
  BitmapWriter::rawPixelsToBMP(const uint8* input,
                               uint8* output,
                               uint32 width,
                               uint32 height,
                               uint32 bytesPerPixel) {
    uint16 bmpBytesPerPixel = 3;
    if (4 < bytesPerPixel) {
      bmpBytesPerPixel = 4;
    }

    uint32 padding = (width * bmpBytesPerPixel) % 4;
    if (0 != padding) {
      padding = 4 - padding;
    }

    uint32 rowPitch = (width * bmpBytesPerPixel) + padding;
    uint32 dataSize = height * rowPitch;

    BMP_HEADER header;
    header.BM = 0x4d42;
    header.size_of_file = sizeof(header) + dataSize;
    header.reserve = 0000;
    header.offset_of_pixel_data = 54;
    header.size_of_header = 40;
    header.width = width;
    header.hight = height;
    header.num_of_color_plane = 1;
    header.num_of_bit_per_pix = bmpBytesPerPixel * 8;
    header.compression = 0;
    header.size_of_pix_data = dataSize;
    header.h_resolution = 2835;
    header.v_resolution = 2835;
    header.num_of_color_in_palette = 0;
    header.important_colors = 0;

    //Write header
    memcpy(output, &header, sizeof(header));
    output += sizeof(header);

    //Write bytes
    uint32 widthBytes = width * bytesPerPixel;

    //BPP matches so we can just copy directly
    if (bmpBytesPerPixel == bytesPerPixel) {
      for (int32 y = height - 1; y >= 0; --y) {
        uint8* outputPtr = output + y * rowPitch;
        memcpy(outputPtr, input, widthBytes);
        memset(outputPtr + widthBytes, 0, padding);
        input += widthBytes;
      }
    }
    else if (bmpBytesPerPixel < bytesPerPixel) {
      //More bytes in source than supported in BMP, just truncate excess data
      for (int32 y = height - 1; y >= 0; --y) {
        uint8* outputPtr = output + y * rowPitch;
        for (uint32 x = 0; x<width; ++x) {
          memcpy(outputPtr, input, bmpBytesPerPixel);
          outputPtr += bmpBytesPerPixel;
          input += bytesPerPixel;
        }

        memset(outputPtr, 0, padding);
      }
    }
    else {
      //More bytes in BMP than in source (BMP must be 24bit minimum)
      for (int32 y = height - 1; y >= 0; --y) {
        uint8* outputPtr = output + y * rowPitch;
        for (uint32 x = 0; x < width; ++x) {
          memcpy(outputPtr, input, bytesPerPixel);

          //Fill the empty bytes with the last available byte from input
          uint32 remainingBytes = bmpBytesPerPixel - bytesPerPixel;
          while (0 < remainingBytes) {
            memcpy(outputPtr + (bmpBytesPerPixel - remainingBytes), input, 1);
            --remainingBytes;
          }

          outputPtr += bmpBytesPerPixel;
          input += bytesPerPixel;
        }

        memset(outputPtr, 0, padding);
      }
    }
  }

  uint32
  BitmapWriter::getBMPSize(uint32 width, uint32 height, uint32 bytesPerPixel) {
    uint16 bmpBytesPerPixel = 3;
    if (4 < bytesPerPixel) {
      bmpBytesPerPixel = 4;
    }

    uint32 padding = (width * bmpBytesPerPixel) % 4;
    if (padding != 0) {
      padding = 4 - padding;
    }

    uint32 rowPitch = (width * bmpBytesPerPixel) + padding;
    uint32 dataSize = height * rowPitch;

    return sizeof(BMP_HEADER) + dataSize;
  }
}
