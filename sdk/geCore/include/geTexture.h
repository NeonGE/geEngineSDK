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
     * @brief getDimensions() - Get the dimensions of the texture
     * @return Vector3 - The dimensions of the texture,
     *                   x and y are the width and height
     *                   z is the number of mipmaps
     */
    virtual Vector3
    getDimensions() const = 0;

    /**
     * @brief getDesc() - Get the description of the texture
     * @return TEXTURE2D_DESC - The description of the texture
     */
    virtual const TEXTURE2D_DESC&
    getDesc() const;

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

   protected:
    friend class RenderAPI;

    bool m_bHaveAlpha = false;
    bool m_bIsCubeMap = false;
    TEXTURE2D_DESC m_desc;
  };
}
