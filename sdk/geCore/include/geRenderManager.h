/*****************************************************************************/
/**
 * @file    geRenderManager.h
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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"
#include "geGraphicsInterfaces.h"
#include <geModule.h>

namespace geEngineSDK {
  namespace RenderState {
    namespace Sampler {
      GE_CORE_EXPORT extern const uint32 DEFAULT;

      GE_CORE_EXPORT extern const uint32 LINEAR_CLAMP;
      GE_CORE_EXPORT extern const uint32 POINT_CLAMP;
      GE_CORE_EXPORT extern const uint32 ANISO_CLAMP;

      GE_CORE_EXPORT extern const uint32 LINEAR_WRAP;
      GE_CORE_EXPORT extern const uint32 POINT_WRAP;
      GE_CORE_EXPORT extern const uint32 ANISO_WRAP;

      GE_CORE_EXPORT extern const uint32 LINEAR_MIRROR;
      GE_CORE_EXPORT extern const uint32 POINT_MIRROR;
      GE_CORE_EXPORT extern const uint32 ANISO_MIRROR;
    }

    namespace Blend {
      GE_CORE_EXPORT extern const uint32 DEFAULT;

      //No blending, just overwrite the render target
      GE_CORE_EXPORT extern const uint32 OPAQUE;
      
      GE_CORE_EXPORT extern const uint32 ALPHA_BLEND; //Alpha blending with source alpha
      GE_CORE_EXPORT extern const uint32 TRANSPARENT; //Alias for alpha blend

      //Alpha blending with premultiplied alpha, source alpha is expected to be
      //already multiplied in the source color
      GE_CORE_EXPORT extern const uint32 PREMULTIPLIED_ALPHA;

      GE_CORE_EXPORT extern const uint32 ADDITIVE; //Additive blending with source alpha

      GE_CORE_EXPORT extern const uint32 MULTIPLY; 
      GE_CORE_EXPORT extern const uint32 MODULATE; //Alias for multiply

      GE_CORE_EXPORT extern const uint32 DISABLE_FRAME_BUFFER;
    }

    namespace Raster {
      GE_CORE_EXPORT extern const uint32 DEFAULT;

      GE_CORE_EXPORT extern const uint32 CULL_BACK_WIREFRAME;
      GE_CORE_EXPORT extern const uint32 CULL_FRONT_WIREFRAME;
      GE_CORE_EXPORT extern const uint32 CULL_NONE_WIREFRAME;

      GE_CORE_EXPORT extern const uint32 CULL_BACK_SOLID;
      GE_CORE_EXPORT extern const uint32 CULL_FRONT_SOLID;
      GE_CORE_EXPORT extern const uint32 CULL_NONE_SOLID;
    }

    namespace Depth {
      //Default depth stencil state, useful for most cases like opaque objects
      GE_CORE_EXPORT extern const uint32 DEFAULT;

      //Depth disabled, useful for transparent objects
      GE_CORE_EXPORT extern const uint32 DEPTH_DISABLED;

      //Alias for default depth state, depth test enabled with less and depth write
      GE_CORE_EXPORT extern const uint32 DEPTH_WRITE_LESS;

      //Depth test enabled with less equal and depth write, useful for skyboxes
      //when you want to render them after all the opaque objects but still want
      //them to pass the depth test
      GE_CORE_EXPORT extern const uint32 DEPTH_WRITE_LESS_EQUAL;

      //Depth test enabled with less but no depth write, useful for decals and
      //debug rendering when you want to render them after all the opaque objects
      //but don't want them to write to the depth buffer
      GE_CORE_EXPORT extern const uint32 DEPTH_TEST_LESS_NO_WRITE;

      //Depth test enabled with less equal but no depth write, useful for transparent
      //objects, skyboxes and decals when you want to render them after all the opaque
      //objects but don't want them to write to the depth buffer
      GE_CORE_EXPORT extern const uint32 DEPTH_TEST_LESS_EQUAL_NO_WRITE;

      //Depth test enabled with always and no depth write, useful for debug rendering
      //when you want to render something on top of everything else but don't want
      //it to write to the depth buffer
      GE_CORE_EXPORT extern const uint32 DEPTH_ALWAYS_NO_WRITE;

      //Useful for stencil operations when you want to write a reference value
      //to the stencil buffer and mark the pixels that pass the stencil
      GE_CORE_EXPORT extern const uint32 STENCIL_WRITE_REPLACE;

      //Stencil test enabled with equal and no depth write, useful for stencil
      //operations when you want to test against a reference value in the stencil
      //buffer but don't want to write to the depth buffer
      GE_CORE_EXPORT extern const uint32 STENCIL_TEST_EQUAL_NO_WRITE;

      //Stencil test enabled with not equal and no depth write, useful for outlining
      //objects, inverse masking and selection highlighting when you want to test
      //against a reference value in the stencil buffer but don't want to write
      //to the depth buffer
      GE_CORE_EXPORT extern const uint32 STENCIL_TEST_NOT_EQUAL_NO_WRITE;

      //Depth test enabled with less equal and stencil write replace, useful for
      //shadow volumes when you want to write a reference value to the stencil buffer
      GE_CORE_EXPORT extern const uint32 DEPTH_WRITE_STENCIL_REPLACE;

      //Depth test enabled with less and stencil test equal but no depth write,
      //useful for shadow volumes when you want to test against a reference value
      //in the stencil buffer
      GE_CORE_EXPORT extern const uint32 DEPTH_TEST_STENCIL_EQUAL_NO_WRITE;
      
      //Depth test enabled with less equal and stencil test equal but no depth write,
      //useful for shadow volumes when you want to test against a reference value
      //in the stencil buffer and still want the pixels to pass the depth test
      //if they are equal to the current depth buffer value, this is useful for
      //avoiding z-fighting issues with shadow volumes
      GE_CORE_EXPORT extern const uint32 STENCIL_SHADOW_VOLUME_ZFAIL;
    }

  }

  class GE_CORE_EXPORT RenderManager : public Module<RenderManager>
  {
   public:
    RenderManager() = default;
    ~RenderManager() = default;

    void
    onStartUp() override;

    void
    onShutDown() override;

    //Function that returns the sampler state with the given id
    const SPtr<SamplerState>&
    getSamplerState(uint32 id);

    //Function that returns the blend state with the given id
    const SPtr<BlendState>&
    getBlendState(uint32 id);

    //Function that returns the raster state with the given id
    const SPtr<RasterizerState>&
    getRasterizerState(uint32 id);

    //Function that returns the depth stencil state with the given id
    const SPtr<DepthStencilState>&
    getDepthStencilState(uint32 id);

    //Funtion to get the current sampler state
    const SPtr<SamplerState>&
    getCurrentSamplerState();

    //Funtion to get the current blend state
    const SPtr<BlendState>&
    getCurrentBlendState();

    //Funtion to get the current raster state
    const SPtr<RasterizerState>&
    getCurrentRasterState();

    //Funtion to get the current depth stencil state
    const SPtr<DepthStencilState>&
    getCurrentDepthStencilState();

   private:
    void
    _createDefaultSamplerStates();

    void
    _createDefaultBlendSates();

    void
    _createDefaultRasterStates();

    void
    _createDefaultDepthStates();

   private:
    UnorderedMap<uint32, SPtr<SamplerState>> m_samplerStates;
    UnorderedMap<uint32, SPtr<BlendState>> m_blendStates;
    UnorderedMap<uint32, SPtr<RasterizerState>> m_rasterStates;
    UnorderedMap<uint32, SPtr<DepthStencilState>> m_depthStates;

    uint32 m_currentSamplerState = 0;
    uint32 m_currentBlendState = 0;
    uint32 m_currentRasterState = 0;
    uint32 m_currentDepthState = 0;
  };

}
