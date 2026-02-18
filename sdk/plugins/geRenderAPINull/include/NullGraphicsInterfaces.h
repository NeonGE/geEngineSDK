/*****************************************************************************/
/**
 * @file    geGraphicsInterfaces.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/26
 * @brief   Graphics Interfaces Implementation with Null Driver.
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
#include "gePrerequisitesRenderAPINull.h"
#include <geGraphicsInterfaces.h>
#include <geVector4.h>
#include <geNumericLimits.h>

namespace geEngineSDK
{

  class NullRasterizerState : public RasterizerState
  {
   public:
    virtual ~NullRasterizerState() {
      release();
    }

    void
    release() override {
      m_pRasterizerState -= 1;
    }

    void
    setDebugName(const String&) override {}

   protected:
    friend class NullRenderAPI;

    int32 m_pRasterizerState = 0;
  };

  class NullDepthStencilState : public DepthStencilState
  {
   public:
    virtual ~NullDepthStencilState() {
      release();
    }

    void
    release() override {
      m_pDepthStencilState -= 1;
    }

    void
    setDebugName(const String&) override {}

   protected:
    friend class NullRenderAPI;
    int32 m_pDepthStencilState = 0;
  };

  class NullBlendState : public BlendState
  {
   public:
    virtual ~NullBlendState() {
      release();
    }

    void
    release() override {
      m_pBlendState -= 1;
    }

    void
    setDebugName(const String&) override {}

   protected:
    friend class NullRenderAPI;
    int32 m_pBlendState = 0;
    uint32 m_sampleMask = NumLimit::MAX_UINT32;
    Vector4 m_blendFactors = Vector4::ZERO;
  };

  class NullSamplerState : public SamplerState
  {
   public:
    virtual ~NullSamplerState() {
      release();
    }

    void
    release() override {
      m_pSampler -= 1;
    }
    
    void
    setDebugName(const String&) override {}

   protected:
    friend class NullRenderAPI;
    int32 m_pSampler = 0;
  };

  class NullPipelineState : public PipelineState
  {
   public:
    NullPipelineState() {}

    virtual ~NullPipelineState() {
      release();
    }

    void
    release() override {}

   protected:
    friend class NullRenderAPI;
  };

} // namespace geEngineSDK
