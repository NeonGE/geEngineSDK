/*****************************************************************************/
/**
 * @file    geBitmapWriter.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/09/21
 * @brief   Utility class for generating BMP images.
 *
 * Utility  class for generating BMP images.
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
  namespace PIXEL_ORDER {
    enum E {
      RGB,
      BGR,
      RGBA,
      BGRA
    };
  }

  /**
   * @brief Utility class for generating BMP images.
   */
  class GE_UTILITIES_EXPORT BitmapWriter
  {
  public:
    /**
     * @brief Converts raw pixel data to BMP format and writes it to the provided buffer.
     * @param[in] src The source pixel data in raw format (e.g., RGBA or RGB).
     * @param[in] width The width of the image in pixels.
     * @param[in] height The height of the image in pixels.
     * @param[in] srcBytesPerPixel Number of bytes per pixel in the source data.
     * @param[out] dst The destination buffer where the BMP data will be written.
     * @param[in] dstCapacity The capacity of the destination buffer in bytes.
     * @param[in] srcOrder The order of color channels in the source data (default is RGBA).
     * @param[in] force32bpp If true, the output will be in 32 bits per pixel
     *            (BGRA) format even if the source is RGB.
     * @return The number of bytes written to the destination buffer, or 0 if the
     *         input parameters are invalid or if the destination buffer is not large enough.
     */
    static uint32
    rawPixelsToBMP(const uint8* src,
                   uint32 width,
                   uint32 height,
                   uint32 srcBytesPerPixel,
                   uint8* dst,
                   uint32 dstCapacity,
                   PIXEL_ORDER::E srcOrder = PIXEL_ORDER::RGBA,
                   bool force32bpp = false);

    /**
     * @brief Calculates the size in bytes needed to store a BMP image with the
     *        given parameters.
     * @param[in] width   The width of the image in pixels.
     * @param[in] height  The height of the image in pixels.
     * @param[in] srcBytesPerPixel Number of bytes per pixel in the source data.
     *            3 for RGB and 4 for RGBA images. Other values not supported.
     * @param[in] force32bpp If true, the output will be calculated as if the
     *            BMP is always 32 bits per pixel (BGRA), even if the source is
     *            RGB. This can be useful for ensuring a consistent output format.
     * @return The size in bytes needed to store the BMP image, including headers
     *         and pixel data with padding.
     */
    static uint32
    getBMPSize(uint32 width,
               uint32 height,
               uint32 srcBytesPerPixel,
               bool force32bpp = false);
  };
}
