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
#include "geVector4.h"

namespace geEngineSDK {

  Vector4
  Texture::getDimensions() const {
    return Vector4(cast::st<float>(m_desc.width),
                   cast::st<float>(m_desc.height),
                   cast::st<float>(m_desc.depth),
                   cast::st<float>(m_desc.mipLevels));
  }

  const TEXTURE_DESC&
  Texture::getDesc() const {
    return m_desc;
  }

  TEXTURE1D_DESC
  Texture::getDesc1D() const {
    GE_ASSERT(m_desc.dimensions == 1);

    TEXTURE1D_DESC desc1D;
    desc1D.width = m_desc.width;
    desc1D.mipLevels = m_desc.mipLevels;
    desc1D.arraySize = m_desc.arraySize;
    desc1D.format = m_desc.format;
    desc1D.usage = m_desc.usage;
    desc1D.bindFlags = m_desc.bindFlags;
    desc1D.cpuAccessFlags = m_desc.cpuAccessFlags;
    desc1D.miscFlags = m_desc.miscFlags;

    return desc1D;
  }

  TEXTURE2D_DESC
  Texture::getDesc2D() const {
    GE_ASSERT(m_desc.dimensions == 2);

    TEXTURE2D_DESC desc2D;
    desc2D.width = m_desc.width;
    desc2D.height = m_desc.height;
    desc2D.mipLevels = m_desc.mipLevels;
    desc2D.arraySize = m_desc.arraySize;
    desc2D.format = m_desc.format;
    desc2D.sampleDesc.count = m_desc.sampleDesc.count;
    desc2D.sampleDesc.quality = m_desc.sampleDesc.quality;
    desc2D.usage = m_desc.usage;
    desc2D.bindFlags = m_desc.bindFlags;
    desc2D.cpuAccessFlags = m_desc.cpuAccessFlags;
    desc2D.miscFlags = m_desc.miscFlags;

    return desc2D;
  }

  TEXTURE3D_DESC
  Texture::getDesc3D() const {
    GE_ASSERT(m_desc.dimensions == 3);

    TEXTURE3D_DESC desc3D;
    desc3D.width = m_desc.width;
    desc3D.height = m_desc.height;
    desc3D.depth = m_desc.depth;
    desc3D.mipLevels = m_desc.mipLevels;
    desc3D.format = m_desc.format;
    desc3D.usage = m_desc.usage;
    desc3D.bindFlags = m_desc.bindFlags;
    desc3D.cpuAccessFlags = m_desc.cpuAccessFlags;
    desc3D.miscFlags = m_desc.miscFlags;

    return desc3D;
  }

} // namespace geEngineSDK
