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
 * 
 * @update  2026/06/06 Fixed all the structure to be more robust and acurate.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geBitmapWriter.h"

namespace geEngineSDK {

#pragma pack(push, 1)
  struct BMPFileHeader
  {
    uint16 bfType;      // 'BM' = 0x4D42
    uint32 bfSize;      // Total file size in bytes
    uint16 bfReserved1; // 0
    uint16 bfReserved2; // 0
    uint32 bfOffBits;   // Offset to pixel data (54)
  };

  struct BMPInfoHeader
  {
    uint32 biSize;          // 40
    int32  biWidth;
    int32  biHeight;        // + => bottom-up
    uint16 biPlanes;        // 1
    uint16 biBitCount;      // 24 or 32
    uint32 biCompression;   // 0 = BI_RGB
    uint32 biSizeImage;     // Pixel data size incl padding
    int32  biXPelsPerMeter; // 3780 ~= 96 DPI
    int32  biYPelsPerMeter; // 3780 ~= 96 DPI
    uint32 biClrUsed;       // 0
    uint32 biClrImportant;  // 0
  };
#pragma pack(pop)

  static_assert(sizeof(BMPFileHeader) == 14, "BMPFileHeader must be 14 bytes");
  static_assert(sizeof(BMPInfoHeader) == 40, "BMPInfoHeader must be 40 bytes");

  static inline uint32
  calcRowPitch(uint32 width, uint32 bytesPerPixel) {
    //BMP rows are aligned to 4 bytes
    const uint32 raw = width * bytesPerPixel;
    const uint32 pad = (4u - (raw & 3u)) & 3u;
    return raw + pad;
  }

  uint32
  BitmapWriter::getBMPSize(uint32 width,
                           uint32 height,
                           uint32 srcBytesPerPixel,
                           bool force32bpp) {
    //Output is 24-bit unless we have alpha (src >= 4) or caller forces 32-bit
    const uint32 outBpp = (force32bpp || srcBytesPerPixel >= 4) ? 4u : 3u;
    const uint32 rowPitch = calcRowPitch(width, outBpp);
    const uint32 dataSize = height * rowPitch;

    return uint32_t(sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + dataSize);
  }

  // Writes a full BMP (header + pixels) to 'dst'.
  // Returns bytes written (same as getBMPSize(...)) or 0 on invalid args.
  uint32
  BitmapWriter::rawPixelsToBMP(const uint8* src,
                               const uint32 width,
                               const uint32 height,
                               const uint32 srcBytesPerPixel,
                               uint8* dst,
                               uint32 dstCapacity,
                               PIXEL_ORDER::E srcOrder,
                               bool force32bpp)
  {
    if (!src || !dst || width == 0 || height == 0) {
      return 0;
    }

    if (srcBytesPerPixel < 1 || srcBytesPerPixel > 4){
      return 0; // Keep it simple: only 1-4 bpp supported. 1=gray, 2=gray+alpha, 3=RGB, 4=RGBA
    }

    const uint32 outBytesPerPixel = (force32bpp || srcBytesPerPixel >= 4) ? 4u : 3u;
    const uint32 rowPitch = calcRowPitch(width, outBytesPerPixel);
    const uint32 dataSize = height * rowPitch;
    const uint32 totalSize = uint32(sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + dataSize);

    if (dstCapacity < totalSize) {
      return 0;
    }

    BMPFileHeader fileH{};
    fileH.bfType = 0x4D42; // 'BM'
    fileH.bfSize = totalSize;
    fileH.bfReserved1 = 0;
    fileH.bfReserved2 = 0;
    fileH.bfOffBits = uint32_t(sizeof(BMPFileHeader) + sizeof(BMPInfoHeader));

    BMPInfoHeader infoH{};
    infoH.biSize = sizeof(BMPInfoHeader);
    infoH.biWidth =  cast::st<int32>(width);
    infoH.biHeight = cast::st<int32>(height); // bottom-up
    infoH.biPlanes = 1;
    infoH.biBitCount = cast::st<uint16>(outBytesPerPixel * 8u); // 24/32
    infoH.biCompression = 0; // BI_RGB
    infoH.biSizeImage = dataSize;
    infoH.biXPelsPerMeter = 3780;
    infoH.biYPelsPerMeter = 3780;
    infoH.biClrUsed = 0;
    infoH.biClrImportant = 0;

    //Write headers
    uint8* out = dst;
    memcpy(out, &fileH, sizeof(fileH)); out += sizeof(fileH);
    memcpy(out, &infoH, sizeof(infoH)); out += sizeof(infoH);

    //Pixel conversion
    //BMP expects BGR (24) or BGRA (32)
    //We write bottom-up: first row in file is bottom row.
    const uint32 srcRowBytes = width * srcBytesPerPixel;
    const uint32 outRowBytes = width * outBytesPerPixel;
    const uint32 padding = rowPitch - outRowBytes;

    for (uint32 y = 0; y < height; ++y) {
      const uint8* srcRow = src + cast::st<size_t>(y) * srcRowBytes;
      uint8* dstRow = out + cast::st<size_t>(height - 1 - y) * rowPitch;

      //Fast-path when formats match exactly AND already BGR/BGRA
      const bool srcIsBGR = (srcOrder == PIXEL_ORDER::BGR && srcBytesPerPixel == 3);
      const bool srcIsBGRA = (srcOrder == PIXEL_ORDER::BGRA && srcBytesPerPixel == 4);

      if (outBytesPerPixel == 3 && srcIsBGR) {
        memcpy(dstRow, srcRow, outRowBytes);
      }
      else if (outBytesPerPixel == 4 && srcIsBGRA) {
        memcpy(dstRow, srcRow, outRowBytes);
      }
      else {
        //Convert per pixel
        for (uint32 x = 0; x < width; ++x) {
          const uint8* sp = srcRow + cast::st<size_t>(x) * srcBytesPerPixel;
          uint8* dp = dstRow + cast::st<size_t>(x) * outBytesPerPixel;

          uint8 r = 0, g = 0, b = 0, a = 255;

          switch (srcBytesPerPixel)
          {
          case 4:
            if (srcOrder == PIXEL_ORDER::RGBA) { r = sp[0]; g = sp[1]; b = sp[2]; a = sp[3]; }
            else /*BGRA*/ { b = sp[0]; g = sp[1]; r = sp[2]; a = sp[3]; }
            break;

          case 3:
            if (srcOrder == PIXEL_ORDER::RGB) { r = sp[0]; g = sp[1]; b = sp[2]; }
            else /*BGR*/ { b = sp[0]; g = sp[1]; r = sp[2]; }
            break;

          case 2:
            //Interpret as LA (luma+alpha) or RG? -> we make it robust
            //replicate 1st byte to RGB and 2nd byte is alpha
            r = g = b = sp[0];
            a = sp[1];
            break;

          case 1:
            r = g = b = sp[0];
            a = 255;
            break;
          }

          //BMP output is BGR/BGRA
          dp[0] = b;
          dp[1] = g;
          dp[2] = r;
          if (outBytesPerPixel == 4) {
            dp[3] = a;
          }
        }
      }

      if (padding) {
        memset(dstRow + outRowBytes, 0, padding);
      }
    }

    return totalSize;
  }
}
