/*****************************************************************************/
/**
 * @file    DXShader.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/07
 * @brief   Shader Objects Implementations with DX.
 *
 * Shader Objects Implementations with DX.
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
#include "gePrerequisitesRenderAPIDX11.h"
#include <geShader.h>

namespace geEngineSDK {

  class DXShader : public Shader
  {
   public:
    DXShader() = default;
    virtual ~DXShader();

    void
    release() override;

    void
    setDebugName(const String& name) override;

    bool
    load(const Path& filePath) override {
      GE_UNREFERENCED_PARAMETER(filePath);
      return false;
    }

    void
    unload() override {}

    bool
    isLoaded() const override {
      return m_pShader != nullptr;
    }

    const String&
    getName() const override {
      return m_debugName;
    }

    SIZE_T
    getMemoryUsage() const override {
      return 0;
    }

   protected:
    friend class DX11RenderAPI;

    ID3D11DeviceChild* m_pShader = nullptr;
    ID3DBlob* m_pBlob = nullptr;  //Saved in this to be able to reflect and decompile
  };
  
}
