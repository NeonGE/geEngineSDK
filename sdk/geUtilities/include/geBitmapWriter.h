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
  /**
   * @brief Utility class for generating BMP images.
   */
  class GE_UTILITIES_EXPORT BitmapWriter
  {
  public:
    /**
     * @brief Generates bytes representing the BMP image format, from a set of
     *        raw RGB or RGBA pixels.
     * @param[in] input The input set of bytes in RGB or RGBA format. Starting
     *            byte represents the top left pixel of the image and following
     *            pixels need to be set going from left to right, row after row.
     * @param[out]  output Preallocated buffer where the BMP bytes will be stored.
     *              Use getBMPSize() to retrieve the size needed for this buffer.
     * @param[in] width   The width of the image in pixels.
     * @param[in] height  The height of the image in pixels.
     * @param[in] bytesPerPixel Number of bytes per pixel. 3 for RGB and 4 for
     *            RGBA images. Other values not supported.
     */
    static void
    rawPixelsToBMP(const uint8* input,
                   uint8* output,
                   uint32 width,
                   uint32 height,
                   uint32 bytesPerPixel);

    /**
     * @brief Returns the size of the BMP output buffer that needs to be
     *        allocated before calling rawPixelsToBMP().
     * @param[in] width   The width of the image in pixels.
     * @param[in] height  The height of the image in pixels.
     * @param[in] bytesPerPixel Number of bytes per pixel. 3 for RGB images
     *            and 4 for RGBA images. Other values not supported.
     *
     * @return  Size of the BMP output buffer needed to write a BMP of the
     *          specified size & bpp.
     */
    static uint32 getBMPSize(uint32 width, uint32 height, uint32 bytesPerPixel);
  };
}
