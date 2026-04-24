/*****************************************************************************/
/**
 * @file    geRenderManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/03/12
 * @brief   This file contains the declaration of the RenderManager class.
 *
 * This class is responsible for managing the render states of the engine, such
 * as sampler states, blend states, rasterizer states and depth stencil states.
 * It provides functions to get the current render states and to set new render
 * states.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geRenderManager.h"
#include <geStringID.h>
#include <geRenderAPI.h>

namespace geEngineSDK {
  namespace RenderState {
    namespace Sampler {
      const uint32 DEFAULT = StringID("ss_default").id();

      const uint32 LINEAR_CLAMP = StringID("ss_linear_clamp").id();
      const uint32 POINT_CLAMP = StringID("ss_point_clamp").id();
      const uint32 ANISO_CLAMP = StringID("ss_aniso_clamp").id();

      const uint32 LINEAR_WRAP = StringID("ss_linear_wrap").id();
      const uint32 POINT_WRAP = StringID("ss_point_wrap").id();
      const uint32 ANISO_WRAP = StringID("ss_aniso_wrap").id();

      const uint32 LINEAR_MIRROR = StringID("ss_linear_mirror").id();
      const uint32 POINT_MIRROR = StringID("ss_point_mirror").id();
      const uint32 ANISO_MIRROR = StringID("ss_aniso_mirror").id();
    }

    namespace Blend {
      const uint32 DEFAULT = StringID("bs_default").id();
      const uint32 OPAQUE = StringID("bs_opaque").id();

      const uint32 ALPHA_BLEND = StringID("bs_alpha_blend").id();
      const uint32 TRANSPARENT = ALPHA_BLEND;

      const uint32 PREMULTIPLIED_ALPHA = StringID("bs_premultiplied_alpha").id();

      const uint32 ADDITIVE = StringID("bs_additive").id();
      
      const uint32 MULTIPLY = StringID("bs_multiply").id();
      const uint32 MODULATE = MULTIPLY;

      const uint32 DISABLE_FRAME_BUFFER = StringID("bs_disable_frame_buffer").id();
    }

    namespace Depth {
      const uint32 DEFAULT =
        StringID("ds_default").id();
      const uint32 DEPTH_DISABLED =
        StringID("ds_depth_disabled").id();
      const uint32 DEPTH_WRITE_LESS = DEFAULT;
      const uint32 DEPTH_WRITE_LESS_EQUAL =
        StringID("ds_depth_write_less_equal").id();
      const uint32 DEPTH_TEST_LESS_NO_WRITE =
        StringID("ds_depth_test_less_no_write").id();
      const uint32 DEPTH_TEST_LESS_EQUAL_NO_WRITE =
        StringID("ds_depth_test_less_equal_no_write").id();
      const uint32 DEPTH_ALWAYS_NO_WRITE
        = StringID("ds_depth_always_no_write").id();
      const uint32 STENCIL_WRITE_REPLACE =
        StringID("ds_stencil_write_replace").id();
      const uint32 STENCIL_TEST_EQUAL_NO_WRITE =
        StringID("ds_stencil_test_equal_no_write").id();
      const uint32 STENCIL_TEST_NOT_EQUAL_NO_WRITE =
        StringID("ds_stencil_test_not_equal_no_write").id();
      const uint32 DEPTH_WRITE_STENCIL_REPLACE =
        StringID("ds_depth_write_stencil_replace").id();
      const uint32 DEPTH_TEST_STENCIL_EQUAL_NO_WRITE =
        StringID("ds_depth_test_stencil_equal_no_write").id();
      const uint32 STENCIL_SHADOW_VOLUME_ZFAIL =
        StringID("ds_stencil_shadow_volume_zfail").id();
    }
  }

  void
  RenderManager::_createDefaultSamplerStates() {
    using namespace RenderState::Sampler;
    auto& renderAPI = RenderAPI::instance();
    auto& renderMan = RenderManager::instance();
    auto& _samplers = renderMan.m_samplerStates;
    _samplers.clear();

    //Create the default sampler states
    SAMPLER_DESC samplerDesc(FORCE_INIT::kForceInit);
    {
      _samplers[DEFAULT] = renderAPI.createSamplerState(samplerDesc);
    }
    
    {//CLAMP samplers
      samplerDesc = SAMPLER_DESC(FORCE_INIT::kForceInit);

      //Set the address mode to clamp
      samplerDesc.addressU = TEXTURE_ADDRESS_MODE::CLAMP;
      samplerDesc.addressV = TEXTURE_ADDRESS_MODE::CLAMP;
      samplerDesc.addressW = TEXTURE_ADDRESS_MODE::CLAMP;

      //Create the clamp point sampler
      samplerDesc.filter = SAMPLER_FILTER::MIN_MAG_MIP_POINT;
      _samplers[POINT_CLAMP] = renderAPI.createSamplerState(samplerDesc);

      //Create the clamp linear sampler
      samplerDesc.filter = SAMPLER_FILTER::MIN_MAG_MIP_LINEAR;
      _samplers[LINEAR_CLAMP] = renderAPI.createSamplerState(samplerDesc);

      //Create the clamp anisotropic sampler
      samplerDesc.filter = SAMPLER_FILTER::ANISOTROPIC;
      samplerDesc.maxAnisotropy = 16;
      _samplers[ANISO_CLAMP] = renderAPI.createSamplerState(samplerDesc);
    }

    {//WRAP samplers
      samplerDesc = SAMPLER_DESC(FORCE_INIT::kForceInit);
      samplerDesc.addressU = TEXTURE_ADDRESS_MODE::WRAP;
      samplerDesc.addressV = TEXTURE_ADDRESS_MODE::WRAP;
      samplerDesc.addressW = TEXTURE_ADDRESS_MODE::WRAP;

      //Create the wrap point sampler
      samplerDesc.filter = SAMPLER_FILTER::MIN_MAG_MIP_POINT;
      _samplers[POINT_WRAP] = renderAPI.createSamplerState(samplerDesc);

      //Create the wrap linear sampler
      samplerDesc.filter = SAMPLER_FILTER::MIN_MAG_MIP_LINEAR;
      _samplers[LINEAR_WRAP] = renderAPI.createSamplerState(samplerDesc);

      //Create the wrap anisotropic sampler
      samplerDesc.filter = SAMPLER_FILTER::ANISOTROPIC;
      samplerDesc.maxAnisotropy = 16;
      _samplers[ANISO_WRAP] = renderAPI.createSamplerState(samplerDesc);
    }

    {//MIRROR samplers
      samplerDesc = SAMPLER_DESC(FORCE_INIT::kForceInit);
      samplerDesc.addressU = TEXTURE_ADDRESS_MODE::MIRROR;
      samplerDesc.addressV = TEXTURE_ADDRESS_MODE::MIRROR;
      samplerDesc.addressW = TEXTURE_ADDRESS_MODE::MIRROR;

      //Create the mirror point sampler
      samplerDesc.filter = SAMPLER_FILTER::MIN_MAG_MIP_POINT;
      _samplers[POINT_MIRROR] = renderAPI.createSamplerState(samplerDesc);

      //Create the mirror linear sampler
      samplerDesc.filter = SAMPLER_FILTER::MIN_MAG_MIP_LINEAR;
      _samplers[LINEAR_MIRROR] = renderAPI.createSamplerState(samplerDesc);

      //Create the mirror anisotropic sampler
      samplerDesc.filter = SAMPLER_FILTER::ANISOTROPIC;
      samplerDesc.maxAnisotropy = 16;
      _samplers[ANISO_MIRROR] = renderAPI.createSamplerState(samplerDesc);
    }

  }

  void
  RenderManager::_createDefaultBlendSates() {
    using namespace RenderState::Blend;
    auto& renderAPI = RenderAPI::instance();
    auto& renderMan = RenderManager::instance();
    auto& _blend = renderMan.m_blendStates;
    _blend.clear();

    //Create the default blend states
    BLEND_DESC blendDesc(FORCE_INIT::kForceInit);

    {
      blendDesc = BLEND_DESC(FORCE_INIT::kForceInit);
      _blend[DEFAULT] = renderAPI.createBlendState(blendDesc);
    }

    {
      blendDesc = BLEND_DESC(FORCE_INIT::kForceInit);
      blendDesc.renderTarget[0].blendEnable = false;
      _blend[OPAQUE] = renderAPI.createBlendState(blendDesc);
    }

    {
      blendDesc = BLEND_DESC(FORCE_INIT::kForceInit);
      blendDesc.renderTarget[0].blendEnable = true;
      blendDesc.renderTarget[0].srcBlend = BLEND::SRC_ALPHA;
      blendDesc.renderTarget[0].destBlend = BLEND::INV_SRC_ALPHA;
      blendDesc.renderTarget[0].blendOp = BLEND_OP::ADD;
      blendDesc.renderTarget[0].srcBlendAlpha = BLEND::ONE;
      blendDesc.renderTarget[0].destBlendAlpha = BLEND::INV_SRC_ALPHA;
      blendDesc.renderTarget[0].blendOpAlpha = BLEND_OP::ADD;
      _blend[ALPHA_BLEND] = renderAPI.createBlendState(blendDesc);
    }

    {
      blendDesc = BLEND_DESC(FORCE_INIT::kForceInit);
      blendDesc.renderTarget[0].blendEnable = true;
      blendDesc.renderTarget[0].srcBlend = BLEND::ONE;
      blendDesc.renderTarget[0].destBlend = BLEND::INV_SRC_ALPHA;
      blendDesc.renderTarget[0].blendOp = BLEND_OP::ADD;
      blendDesc.renderTarget[0].srcBlendAlpha = BLEND::ONE;
      blendDesc.renderTarget[0].destBlendAlpha = BLEND::INV_SRC_ALPHA;
      blendDesc.renderTarget[0].blendOpAlpha = BLEND_OP::ADD;
      _blend[PREMULTIPLIED_ALPHA] = renderAPI.createBlendState(blendDesc);
    }

    {
      blendDesc = BLEND_DESC(FORCE_INIT::kForceInit);
      blendDesc.renderTarget[0].blendEnable = true;
      blendDesc.renderTarget[0].srcBlend = BLEND::ONE;
      blendDesc.renderTarget[0].destBlend = BLEND::ONE;
      blendDesc.renderTarget[0].blendOp = BLEND_OP::ADD;
      blendDesc.renderTarget[0].srcBlendAlpha = BLEND::ONE;
      blendDesc.renderTarget[0].destBlendAlpha = BLEND::ONE;
      blendDesc.renderTarget[0].blendOpAlpha = BLEND_OP::ADD;
      _blend[ADDITIVE] = renderAPI.createBlendState(blendDesc);
    }

    {
      blendDesc = BLEND_DESC(FORCE_INIT::kForceInit);
      blendDesc.renderTarget[0].blendEnable = true;
      blendDesc.renderTarget[0].srcBlend = BLEND::DEST_COLOR;
      blendDesc.renderTarget[0].destBlend = BLEND::ZERO;
      blendDesc.renderTarget[0].blendOp = BLEND_OP::ADD;
      blendDesc.renderTarget[0].srcBlendAlpha = BLEND::ONE;
      blendDesc.renderTarget[0].destBlendAlpha = BLEND::ZERO;
      blendDesc.renderTarget[0].blendOpAlpha = BLEND_OP::ADD;
      _blend[MULTIPLY] = renderAPI.createBlendState(blendDesc);
    }
    
    {
      blendDesc = BLEND_DESC(FORCE_INIT::kForceInit);
      blendDesc.renderTarget[0].blendEnable = false;
      blendDesc.renderTarget[0].renderTargetWriteMask = 0;
      _blend[DISABLE_FRAME_BUFFER] = renderAPI.createBlendState(blendDesc);
    }
  }

  void
  RenderManager::_createDefaultRasterStates() {
    using namespace RenderState::Raster;
    auto& renderAPI = RenderAPI::instance();
    auto& renderMan = RenderManager::instance();
    auto& _raster = renderMan.m_rasterStates;
    _raster.clear();

    //Create the default raster states
    RASTERIZER_DESC blendDesc(FORCE_INIT::kForceInit);

    {
      blendDesc = RASTERIZER_DESC(FORCE_INIT::kForceInit);
      _raster[DEFAULT] = renderAPI.createRasterizerState(blendDesc);
    }

    {
      blendDesc = RASTERIZER_DESC(FORCE_INIT::kForceInit);
      blendDesc.fillMode = FILL_MODE::WIREFRAME;
      blendDesc.cullMode = CULL_MODE::BACK;
      _raster[CULL_BACK_WIREFRAME] = renderAPI.createRasterizerState(blendDesc);
    }
  
    {
      blendDesc = RASTERIZER_DESC(FORCE_INIT::kForceInit);
      blendDesc.fillMode = FILL_MODE::WIREFRAME;
      blendDesc.cullMode = CULL_MODE::FRONT;
      _raster[CULL_FRONT_WIREFRAME] = renderAPI.createRasterizerState(blendDesc);
    }

    {
      blendDesc = RASTERIZER_DESC(FORCE_INIT::kForceInit);
      blendDesc.fillMode = FILL_MODE::WIREFRAME;
      blendDesc.cullMode = CULL_MODE::NONE;
      _raster[CULL_NONE_WIREFRAME] = renderAPI.createRasterizerState(blendDesc);
    }
  
    {
      blendDesc = RASTERIZER_DESC(FORCE_INIT::kForceInit);
      blendDesc.fillMode = FILL_MODE::SOLID;
      blendDesc.cullMode = CULL_MODE::BACK;
      _raster[CULL_BACK_SOLID] = renderAPI.createRasterizerState(blendDesc);
    }

    {
      blendDesc = RASTERIZER_DESC(FORCE_INIT::kForceInit);
      blendDesc.fillMode = FILL_MODE::SOLID;
      blendDesc.cullMode = CULL_MODE::FRONT;
      _raster[CULL_FRONT_SOLID] = renderAPI.createRasterizerState(blendDesc);
    }

    {
      blendDesc = RASTERIZER_DESC(FORCE_INIT::kForceInit);
      blendDesc.fillMode = FILL_MODE::SOLID;
      blendDesc.cullMode = CULL_MODE::NONE;
      _raster[CULL_NONE_SOLID] = renderAPI.createRasterizerState(blendDesc);
    }
  }

  void
  RenderManager::_createDefaultDepthStates() {
    using namespace RenderState::Depth;
    auto& renderAPI = RenderAPI::instance();
    auto& renderMan = RenderManager::instance();
    auto& _depth = renderMan.m_depthStates;
    _depth.clear();

    DEPTH_STENCIL_DESC depthDesc(FORCE_INIT::kForceInit);

    {
      depthDesc = DEPTH_STENCIL_DESC(FORCE_INIT::kForceInit);
      _depth[DEFAULT] = renderAPI.createDepthStencilState(depthDesc);
    }

    {
      depthDesc = DEPTH_STENCIL_DESC(FORCE_INIT::kForceInit);
      depthDesc.depthEnable = false;
      _depth[DEPTH_DISABLED] = renderAPI.createDepthStencilState(depthDesc);
    }

    {
      depthDesc = DEPTH_STENCIL_DESC(FORCE_INIT::kForceInit);
      depthDesc.depthEnable = true;
      depthDesc.depthWriteMask = DEPTH_WRITE_MASK::ALL;
      depthDesc.depthFunc = COMPARISON_FUNC::LESS;
      _depth[DEPTH_WRITE_LESS] = renderAPI.createDepthStencilState(depthDesc);
    }

    {
      depthDesc = DEPTH_STENCIL_DESC(FORCE_INIT::kForceInit);
      depthDesc.depthEnable = true;
      depthDesc.depthWriteMask = DEPTH_WRITE_MASK::ALL;
      depthDesc.depthFunc = COMPARISON_FUNC::LESS_EQUAL;
      _depth[DEPTH_WRITE_LESS_EQUAL] = renderAPI.createDepthStencilState(depthDesc);
    }

    {
      depthDesc = DEPTH_STENCIL_DESC(FORCE_INIT::kForceInit);
      depthDesc.depthEnable = true;
      depthDesc.depthWriteMask = DEPTH_WRITE_MASK::ZERO;
      depthDesc.depthFunc = COMPARISON_FUNC::LESS;
      _depth[DEPTH_TEST_LESS_NO_WRITE] = renderAPI.createDepthStencilState(depthDesc);
    }

    {
      depthDesc = DEPTH_STENCIL_DESC(FORCE_INIT::kForceInit);
      depthDesc.depthEnable = true;
      depthDesc.depthWriteMask = DEPTH_WRITE_MASK::ZERO;
      depthDesc.depthFunc = COMPARISON_FUNC::LESS_EQUAL;
      _depth[DEPTH_TEST_LESS_EQUAL_NO_WRITE] = renderAPI.createDepthStencilState(depthDesc);
    }

    {
      depthDesc = DEPTH_STENCIL_DESC(FORCE_INIT::kForceInit);
      depthDesc.depthEnable = true;
      depthDesc.depthWriteMask = DEPTH_WRITE_MASK::ZERO;
      depthDesc.depthFunc = COMPARISON_FUNC::ALWAYS;
      _depth[DEPTH_ALWAYS_NO_WRITE] = renderAPI.createDepthStencilState(depthDesc);
    }

    {
      depthDesc = DEPTH_STENCIL_DESC(FORCE_INIT::kForceInit);
      depthDesc.depthEnable = false;
      depthDesc.depthWriteMask = DEPTH_WRITE_MASK::ZERO;
      depthDesc.stencilEnable = true;
      depthDesc.stencilReadMask = 0xFF;
      depthDesc.stencilWriteMask = 0xFF;

      depthDesc.frontFace.stencilFailOp = STENCIL_OP::KEEP;
      depthDesc.frontFace.stencilDepthFailOp = STENCIL_OP::KEEP;
      depthDesc.frontFace.stencilPassOp = STENCIL_OP::REPLACE;
      depthDesc.frontFace.stencilFunc = COMPARISON_FUNC::ALWAYS;

      depthDesc.backFace = depthDesc.frontFace;

      _depth[STENCIL_WRITE_REPLACE] = renderAPI.createDepthStencilState(depthDesc);
    }

    {
      depthDesc = DEPTH_STENCIL_DESC(FORCE_INIT::kForceInit);
      depthDesc.depthEnable = true;
      depthDesc.depthWriteMask = DEPTH_WRITE_MASK::ZERO;
      depthDesc.depthFunc = COMPARISON_FUNC::LESS_EQUAL;
      depthDesc.stencilEnable = true;
      depthDesc.stencilReadMask = 0xFF;
      depthDesc.stencilWriteMask = 0x00;

      depthDesc.frontFace.stencilFailOp = STENCIL_OP::KEEP;
      depthDesc.frontFace.stencilDepthFailOp = STENCIL_OP::KEEP;
      depthDesc.frontFace.stencilPassOp = STENCIL_OP::KEEP;
      depthDesc.frontFace.stencilFunc = COMPARISON_FUNC::EQUAL;

      depthDesc.backFace = depthDesc.frontFace;

      _depth[STENCIL_TEST_EQUAL_NO_WRITE] = renderAPI.createDepthStencilState(depthDesc);
    }

    {
      depthDesc = DEPTH_STENCIL_DESC(FORCE_INIT::kForceInit);
      depthDesc.depthEnable = true;
      depthDesc.depthWriteMask = DEPTH_WRITE_MASK::ZERO;
      depthDesc.depthFunc = COMPARISON_FUNC::LESS_EQUAL;
      depthDesc.stencilEnable = true;
      depthDesc.stencilReadMask = 0xFF;
      depthDesc.stencilWriteMask = 0x00;

      depthDesc.frontFace.stencilFailOp = STENCIL_OP::KEEP;
      depthDesc.frontFace.stencilDepthFailOp = STENCIL_OP::KEEP;
      depthDesc.frontFace.stencilPassOp = STENCIL_OP::KEEP;
      depthDesc.frontFace.stencilFunc = COMPARISON_FUNC::NOT_EQUAL;

      depthDesc.backFace = depthDesc.frontFace;

      _depth[STENCIL_TEST_NOT_EQUAL_NO_WRITE] = renderAPI.createDepthStencilState(depthDesc);
    }

    {
      depthDesc = DEPTH_STENCIL_DESC(FORCE_INIT::kForceInit);
      depthDesc.depthEnable = true;
      depthDesc.depthWriteMask = DEPTH_WRITE_MASK::ALL;
      depthDesc.depthFunc = COMPARISON_FUNC::LESS_EQUAL;
      depthDesc.stencilEnable = true;
      depthDesc.stencilReadMask = 0xFF;
      depthDesc.stencilWriteMask = 0xFF;

      depthDesc.frontFace.stencilFailOp = STENCIL_OP::KEEP;
      depthDesc.frontFace.stencilDepthFailOp = STENCIL_OP::KEEP;
      depthDesc.frontFace.stencilPassOp = STENCIL_OP::REPLACE;
      depthDesc.frontFace.stencilFunc = COMPARISON_FUNC::ALWAYS;

      depthDesc.backFace = depthDesc.frontFace;

      _depth[DEPTH_WRITE_STENCIL_REPLACE] = renderAPI.createDepthStencilState(depthDesc);
    }

    {
      depthDesc = DEPTH_STENCIL_DESC(FORCE_INIT::kForceInit);
      depthDesc.depthEnable = true;
      depthDesc.depthWriteMask = DEPTH_WRITE_MASK::ZERO;
      depthDesc.depthFunc = COMPARISON_FUNC::LESS_EQUAL;
      depthDesc.stencilEnable = true;
      depthDesc.stencilReadMask = 0xFF;
      depthDesc.stencilWriteMask = 0x00;

      depthDesc.frontFace.stencilFailOp = STENCIL_OP::KEEP;
      depthDesc.frontFace.stencilDepthFailOp = STENCIL_OP::KEEP;
      depthDesc.frontFace.stencilPassOp = STENCIL_OP::KEEP;
      depthDesc.frontFace.stencilFunc = COMPARISON_FUNC::EQUAL;

      depthDesc.backFace = depthDesc.frontFace;

      _depth[DEPTH_TEST_STENCIL_EQUAL_NO_WRITE] = renderAPI.createDepthStencilState(depthDesc);
    }

    {
      depthDesc = DEPTH_STENCIL_DESC(FORCE_INIT::kForceInit);
      depthDesc.depthEnable = true;
      depthDesc.depthWriteMask = DEPTH_WRITE_MASK::ZERO;
      depthDesc.depthFunc = COMPARISON_FUNC::LESS;
      depthDesc.stencilEnable = true;
      depthDesc.stencilReadMask = 0xFF;
      depthDesc.stencilWriteMask = 0xFF;

      depthDesc.backFace.stencilFailOp = STENCIL_OP::KEEP;
      depthDesc.backFace.stencilDepthFailOp = STENCIL_OP::INCR;
      depthDesc.backFace.stencilPassOp = STENCIL_OP::KEEP;
      depthDesc.backFace.stencilFunc = COMPARISON_FUNC::ALWAYS;

      depthDesc.frontFace.stencilFailOp = STENCIL_OP::KEEP;
      depthDesc.frontFace.stencilDepthFailOp = STENCIL_OP::DECR;
      depthDesc.frontFace.stencilPassOp = STENCIL_OP::KEEP;
      depthDesc.frontFace.stencilFunc = COMPARISON_FUNC::ALWAYS;

      _depth[STENCIL_SHADOW_VOLUME_ZFAIL] = renderAPI.createDepthStencilState(depthDesc);
    }
    
  }

  void
  RenderManager::onStartUp() {
    using namespace RenderState;
    auto& renderAPI = RenderAPI::instance();

    _createDefaultSamplerStates();
    _createDefaultBlendSates();
    _createDefaultRasterStates();
    _createDefaultDepthStates();    
  }

  void
  RenderManager::onShutDown() {
    m_samplerStates.clear();
    m_blendStates.clear();
    m_rasterStates.clear();
    m_depthStates.clear();
  }

  const SPtr<SamplerState>&
  RenderManager::getSamplerState(uint32 id) {
    GE_ASSERT(m_samplerStates.find(id) != m_samplerStates.end() &&
              "Sampler state not found");
    m_currentSamplerState = id;
    return m_samplerStates[id];
  }

  const SPtr<BlendState>&
  RenderManager::getBlendState(uint32 id) {
    GE_ASSERT(m_blendStates.find(id) != m_blendStates.end() &&
              "Blend state not found");
    m_currentBlendState = id;
    return m_blendStates[id];
  }

  const SPtr<RasterizerState>&
  RenderManager::getRasterizerState(uint32 id) {
    GE_ASSERT(m_rasterStates.find(id) != m_rasterStates.end() &&
              "Rasterizer state not found");
    m_currentRasterState = id;
    return m_rasterStates[id];
  }

  const SPtr<DepthStencilState>&
  RenderManager::getDepthStencilState(uint32 id) {
    GE_ASSERT(m_depthStates.find(id) != m_depthStates.end() &&
              "Depth stencil state not found");
    m_currentDepthState = id;
    return m_depthStates[id];
  }

  const SPtr<SamplerState>&
  RenderManager::getCurrentSamplerState() {
    GE_ASSERT(m_samplerStates.find(m_currentSamplerState) != m_samplerStates.end() &&
              "Sampler state not found");
    return m_samplerStates[m_currentSamplerState];
  }

  const SPtr<BlendState>&
  RenderManager::getCurrentBlendState() {
    GE_ASSERT(m_blendStates.find(m_currentBlendState) != m_blendStates.end() &&
              "Blend state not found");
    return m_blendStates[m_currentBlendState];
  }

  const SPtr<RasterizerState>&
  RenderManager::getCurrentRasterState() {
    GE_ASSERT(m_rasterStates.find(m_currentRasterState) != m_rasterStates.end() &&
              "Rasterizer state not found");
    return m_rasterStates[m_currentRasterState];
  }

  const SPtr<DepthStencilState>&
  RenderManager::getCurrentDepthStencilState() {
    GE_ASSERT(m_depthStates.find(m_currentDepthState) != m_depthStates.end() &&
              "Depth stencil state not found");
    return m_depthStates[m_currentDepthState];
  }

}
