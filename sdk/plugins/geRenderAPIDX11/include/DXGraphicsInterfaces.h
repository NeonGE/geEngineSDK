/*****************************************************************************/
/**
 * @file    geGraphicsInterfaces.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/26
 * @brief   Graphics Interfaces Implementation with DirectX11.
 *
 * Graphics Interfaces.
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
#include <geGraphicsInterfaces.h>
#include <geVector4.h>
#include <geNumericLimits.h>

namespace geEngineSDK
{

  class DXRasterizerState : public RasterizerState
  {
   public:
    virtual ~DXRasterizerState() {
      release();
    }

    void
    release() override {
      safeRelease(m_pRasterizerState);
    }

    void
    setDebugName(const String& name) override {
#if USING(GE_DEBUG_MODE)
      if (m_pRasterizerState) {
        m_pRasterizerState->SetPrivateData(WKPDID_D3DDebugObjectName,
                                           static_cast<UINT>(name.size()),
                                           name.c_str());
      }
#else
      GE_UNREFERENCED_PARAMETER(name);
#endif
    }

   protected:
    friend class DX11RenderAPI;

    D3DRasterizerState* m_pRasterizerState = nullptr;
  };

  class DXDepthStencilState : public DepthStencilState
  {
   public:
    virtual ~DXDepthStencilState() {
      release();
    }

    void
    release() override {
      safeRelease(m_pDepthStencilState);
    }

    void
    setDebugName(const String& name) override {
#if USING(GE_DEBUG_MODE)
      if (m_pDepthStencilState) {
        m_pDepthStencilState->SetPrivateData(WKPDID_D3DDebugObjectName,
                                             static_cast<UINT>(name.size()),
                                             name.c_str());
      }
#else
      GE_UNREFERENCED_PARAMETER(name);
#endif
    }

   protected:
    friend class DX11RenderAPI;
    ID3D11DepthStencilState* m_pDepthStencilState = nullptr;
  };

  class DXBlendState : public BlendState
  {
   public:
    virtual ~DXBlendState() {
      release();
    }

    void
    release() override {
      safeRelease(m_pBlendState);
    }

    void
    setDebugName(const String& name) override {
#if USING(GE_DEBUG_MODE)
      if (m_pBlendState) {
        m_pBlendState->SetPrivateData(WKPDID_D3DDebugObjectName,
                                      static_cast<UINT>(name.size()),
                                      name.c_str());
      }
#else
      GE_UNREFERENCED_PARAMETER(name);
#endif
    }

   protected:
    friend class DX11RenderAPI;
    D3DBlendState* m_pBlendState = nullptr;
    Vector4 m_blendFactors = Vector4::ZERO;
    uint32 m_sampleMask = NumLimit::MAX_UINT32;
  };

  class DXSamplerState : public SamplerState
  {
   public:
    virtual ~DXSamplerState() {
      release();
    }

    void
    release() override {
      safeRelease(m_pSampler);
    }
    
    void
    setDebugName(const String& name) override {
#if USING(GE_DEBUG_MODE)
      if (m_pSampler) {
        m_pSampler->SetPrivateData(WKPDID_D3DDebugObjectName,
                                   static_cast<UINT>(name.size()),
                                   name.c_str());
      }
#else
      GE_UNREFERENCED_PARAMETER(name);
#endif
    }
   protected:
    friend class DX11RenderAPI;
    ID3D11SamplerState* m_pSampler = nullptr;
  };

  class DXPipelineState : public PipelineState
  {
   public:
     DXPipelineState() {
       memset(&m_scissorRects, 0, sizeof(m_scissorRects));
       memset(&m_viewports, 0, sizeof(m_viewports));
       memset(&m_blendFactor, 0, sizeof(m_blendFactor));
       memset(&m_psInstances, 0, sizeof(m_psInstances));
       memset(&m_vsInstances, 0, sizeof(m_vsInstances));
       memset(&m_gsInstances, 0, sizeof(m_gsInstances));
     }

    virtual ~DXPipelineState() {
      release();
    }

    void
    release() override {
      safeRelease(m_rasterState);
      safeRelease(m_blendState);
      safeRelease(m_depthStencilState);
      safeRelease(m_psShaderResource);
      safeRelease(m_psSampler);
      safeRelease(m_ps);
      
      for (uint32 i = 0; i < m_psInstancesCount; i++) {
        safeRelease(m_psInstances[i]);
      }

      safeRelease(m_vs);
      safeRelease(m_vsConstantBuffer);
      safeRelease(m_gs);
      
      for (uint32 i = 0; i < m_vsInstancesCount; i++) {
        safeRelease(m_vsInstances[i]);
      }

      for (uint32 i = 0; i < m_gsInstancesCount; i++) {
        safeRelease(m_gsInstances[i]);
      }

      safeRelease(m_indexBuffer);
      safeRelease(m_vertexBuffer);
      safeRelease(m_inputLayout);
    }

   protected:
    friend class DX11RenderAPI;

    uint32 m_scissorRectsCount = 0;
    uint32 m_viewportsCount = 0;
    D3D11_RECT m_scissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    D3D11_VIEWPORT m_viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    ID3D11RasterizerState* m_rasterState = nullptr;
    ID3D11BlendState* m_blendState = nullptr;
    float m_blendFactor[4];
    uint32 m_sampleMask = NumLimit::MAX_UINT32;
    uint32 m_stencilRef = 0;
    ID3D11DepthStencilState* m_depthStencilState = nullptr;
    ID3D11ShaderResourceView* m_psShaderResource = nullptr;
    ID3D11SamplerState* m_psSampler = nullptr;
    ID3D11PixelShader* m_ps = nullptr;
    ID3D11VertexShader* m_vs = nullptr;
    ID3D11GeometryShader* m_gs = nullptr;
    uint32 m_psInstancesCount = 0;
    uint32 m_vsInstancesCount = 0;
    uint32 m_gsInstancesCount = 0;

    //256 is max according to PSSetShader documentation
    ID3D11ClassInstance* m_psInstances[256];
    ID3D11ClassInstance* m_vsInstances[256];
    ID3D11ClassInstance* m_gsInstances[256];

    D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    ID3D11Buffer* m_indexBuffer = nullptr;
    ID3D11Buffer* m_vertexBuffer = nullptr;
    ID3D11Buffer* m_vsConstantBuffer = nullptr;
    uint32 m_indexBufferOffset = 0;
    uint32 m_vertexBufferStride = 0;
    uint32 m_vertexBufferOffset = 0;
    DXGI_FORMAT m_indexBufferFormat = DXGI_FORMAT_UNKNOWN;
    ID3D11InputLayout* m_inputLayout = nullptr;
  };

} // namespace geEngineSDK
