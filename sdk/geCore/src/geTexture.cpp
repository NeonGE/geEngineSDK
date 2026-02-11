/*****************************************************************************/
/**
 * @file    geTexture.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/26
 * @brief   Texture Interface.
 *
 * Texture Interface.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geTexture.h"
#include "geDebug.h"

namespace geEngineSDK {

  const TEXTURE2D_DESC&
  Texture::getDesc() const {
    return m_desc;
  }

} // namespace geEngineSDK
