/*****************************************************************************/
/**
 * @file    geWin32PlatformUtility.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/11
 * @brief   Provides access to Windows specific utility functions
 *
 * Provides access to various Windows specific utility functions
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once
#ifdef _WIN32
/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "Win32/geMinWindows.h"
#include "gePrerequisitesUtilities.h"

namespace geEngineSDK {
  /**
   * @brief Provides access to various Windows specific utility functions.
   */
  class GE_UTILITIES_EXPORT Win32PlatformUtility
  {
   public:
    /**
     * @brief Creates a new bitmap usable by various Win32 methods from the
     *        provided pixel data. Caller must ensure to call DeleteObject()
     *        on the bitmap handle when finished.
     */
    static HBITMAP
    createBitmap(const LinearColor* pixels,
                 uint32 width,
                 uint32 height,
                 bool premultiplyAlpha);
  };
}

#endif
