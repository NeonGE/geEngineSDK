/*****************************************************************************/
/**
 * @file    DXInputLayout.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/07
 * @brief   Object that describes the memory layout of a Vertex Buffer in DX.
 *
 * Object that describes the memory layout of a Vertex Buffer in DX.
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
#include <geInputLayout.h>

namespace geEngineSDK {

  class DXInputLayout : public InputLayout
  {
   public:
    DXInputLayout() = default;
    virtual ~DXInputLayout() {
      release();
    }

    void
    release() override {
      safeRelease(m_inputLayout);
    }

   protected:
    friend class DX11RenderAPI;
    friend class DXInputLayoutManager;

    ID3D11InputLayout* m_inputLayout = nullptr;
  };
  
} // namespace geEngineSDK
