/*****************************************************************************/
/**
 * @file    NullTexture.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/26
 * @brief   DirectX Implementation of the Texture Object.
 *
 * DirectX Implementation of the Texture Object.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "NullTexture.h"
#include "NullTranslateUtils.h"

#include <geVector3.h>

namespace geEngineSDK {

  NullTexture::~NullTexture() {
    release();
  }

  void
  NullTexture::moveFrom(Resource& other) {
    if (this != &other) {
      release();

      //Cast to NullTexture to access the specific members
      NullTexture& dxOther = static_cast<NullTexture&>(other);
      m_bHaveAlpha = dxOther.m_bHaveAlpha;
      m_bIsCubeMap = dxOther.m_bIsCubeMap;
      m_desc = dxOther.m_desc;

      setPath(dxOther.getPath());
      setCookedPath(dxOther.getCookedPath());
    }
  }

  void
  NullTexture::release() {
    m_textureObj -= 1;
    m_desc.bindFlags = 0;
  }

  Vector3
  NullTexture::getDimensions() const {
    return Vector3(cast::st<float>(m_desc.width),
                   cast::st<float>(m_desc.height),
                   cast::st<float>(m_desc.mipLevels));
  }

  void
  NullTexture::setDebugName(const String& name) {
#if USING(GE_DEBUG_MODE)
    m_debugName = name;
#else
    GE_UNREFERENCED_PARAMETER(name);
#endif
    return;
  }

  SIZE_T
  NullTexture::getMemoryUsage() const {
    SIZE_T sizeInMemory = 0;
    if (m_textureObj) {
      const SIZE_T& width = m_desc.width;
      const SIZE_T& height = m_desc.height;
      const SIZE_T& mipLevels = m_desc.mipLevels;
      const SIZE_T bpp = 32; //TODO: We might want to get this from the format

      for (SIZE_T i = 0; i < mipLevels; ++i) {
        sizeInMemory += (width >> i) * (height >> i) * (bpp >> 3);
      }
    }

    return sizeInMemory;
  }

}
