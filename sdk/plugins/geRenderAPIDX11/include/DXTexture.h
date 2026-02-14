/*****************************************************************************/
/**
 * @file    DXTexture.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/26
 * @brief   DirectX Implementation of the Texture Object.
 *
 * DirectX Implementation of the Texture Object.
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
#include <gePrerequisitesRenderAPIDX11.h>
#include <geTexture.h>

namespace geEngineSDK {
  
  class DXTexture : public Texture
  {
   public:
    DXTexture() = default;
    virtual ~DXTexture();

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

    void
    _setDesc(D3D11_TEXTURE2D_DESC& desc);

    void*
    _getGraphicsResource() const override;
    
   private:
#if USING(GE_DEBUG_MODE)
    String m_debugName;
#endif

   protected:
    friend class DX11RenderAPI;

    D3DTexture2D* m_pTexture = nullptr;
    D3DDepthStencilView* m_pDSV = nullptr;
    D3DDepthStencilView* m_pRO_DSV = nullptr;
    
    Vector<D3DShaderResourceView*> m_ppSRV;
    Vector<D3DRenderTargetView*> m_ppRTV;
    Vector<D3DUnorderedAccessView*> m_ppUAV;
  };
} // namespace geEngineSDK
