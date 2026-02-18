/*****************************************************************************/
/**
 * @file    NullTexture.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/26
 * @brief   Null Driver Implementation of the Texture Object.
 *
 * Null Driver Implementation of the Texture Object.
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
#include <gePrerequisitesRenderAPINull.h>
#include <geTexture.h>

namespace geEngineSDK {
  class NullTexture : public Texture
  {
   public:
    NullTexture() = default;
    virtual ~NullTexture();

    void
    moveFrom(Resource& other) override;

    void
    release() override;

    Vector3
    getDimensions() const override;

    void
    setDebugName(const String& name) override;

    /*************************************************************************/
    // Implementation of the Resource Interface
    /*************************************************************************/
    bool
    load(const Path& filePath) override {
      GE_UNREFERENCED_PARAMETER(filePath);
      return false;
    }

    void
    unload() override {}

    bool
    isLoaded() const override {
      return false;
    }

    const String&
    getName() const override {
#if USING(GE_DEBUG_MODE)
      return m_debugName;
#else
      static const String emptyName("");
      return emptyName;
#endif
    }

    SIZE_T
    getMemoryUsage() const override;

    void*
    _getGraphicsResource() const override {
      return nullptr;
    }

   private:
#if USING(GE_DEBUG_MODE)
    String m_debugName;
#endif

   protected:
    friend class NullRenderAPI;
    int32 m_textureObj = 0;
  };
} // namespace geEngineSDK
