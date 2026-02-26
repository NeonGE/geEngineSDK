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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"
#include "geGraphicsTypes.h"
#include "gePath.h"
#include "geGraphicsInterfaces.h"

namespace geEngineSDK {

  class GE_CORE_EXPORT Texture : public GraphicsResource
  {
   public:
    virtual ~Texture() = default;

    /**
     * @brief getDimensions() - Get the dimensions (width, height, depth of the
     *                          texture (each only are set if the texture does
     *                          have that dimension 1D, 2D, 3D.
     * @return Vector4 - The dimensions of the texture,
     *                   x, y and z are the width, height and depth
     *                   w is the number of mipmaps
     */
    virtual Vector4
    getDimensions() const;

    /**
     * @brief getDesc() - Get the full description of the texture
     * @return TEXTURE_DESC - The description of the texture
     */
    virtual const TEXTURE_DESC&
    getDesc() const;

    /**
     * @brief getDesc() - Get the description of the texture as a 1D descriptor
     * @return TEXTURE1D_DESC - The description of the texture
     */
    virtual TEXTURE1D_DESC
    getDesc1D() const;

    /**
     * @brief getDesc() - Get the description of the texture
     * @return TEXTURE2D_DESC - The description of the texture
     */
    virtual TEXTURE2D_DESC
    getDesc2D() const;

    /**
     * @brief getDesc() - Get the description of the texture
     * @return TEXTURE2D_DESC - The description of the texture
     */
    virtual TEXTURE3D_DESC
    getDesc3D() const;

    /**
     * @brief Sets a debug name for the object.
     * @param name The debug name to assign to the object.
     *             This name is used for debugging purposes and may be
     *             displayed in graphics debuggers or logs.
     */
    virtual void
    setDebugName(const String& name) = 0;

    bool
    haveAlpha() const {
      return m_bHaveAlpha;
    }

    bool
    setAlpha(bool bAlpha) {
      m_bHaveAlpha = bAlpha;
      return m_bHaveAlpha;
    }

    bool
    isCubeMap() const {
      return m_bIsCubeMap;
    }

    virtual const void*
    getDrawingReference(const uint32 mipMap = 0) const = 0;

   protected:
    friend class RenderAPI;

    bool m_bHaveAlpha = false;
    bool m_bIsCubeMap = false;
    TEXTURE_DESC m_desc;
  };
}
