/*****************************************************************************/
/**
 * @file    geRenderAPI.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/29
 * @brief   Graphics API Interface and Module.
 *
 * Graphics API Interface and Module.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/

#include <geRenderAPI.h>

namespace geEngineSDK {
  GE_LOG_CATEGORY_IMPL(RenderAPI);

  RenderAPI::RenderAPI()
  {}

  RenderAPI::~RenderAPI()
  {}

  SPtr<Texture>
  RenderAPI::createDepthStencilTexture(uint32 width,
                                       uint32 height,
                                       GRAPHICS_FORMAT::E format,
                                       uint32 sampleCount,
                                       bool isMSAA)  {
    return createTexture(width, height, format, BIND_FLAG::DEPTH_STENCIL, 1,
                         RESOURCE_USAGE::DEFAULT, 0, sampleCount, isMSAA);
  }

  SPtr<Texture>
  RenderAPI::createRenderTargetTexture(uint32 width,
                                       uint32 height,
                                       GRAPHICS_FORMAT::E format,
                                       uint32 sampleCount,
                                       bool isMSAA) {
    return createTexture(width, height, format,
                         BIND_FLAG::RENDER_TARGET | BIND_FLAG::SHADER_RESOURCE,
                         1, RESOURCE_USAGE::DEFAULT, 0, sampleCount, isMSAA);
  }

  void
  RenderAPI::clearRenderTarget(const RenderTarget& pRenderTarget,
                               const LinearColor color) {
    clearRenderTarget(pRenderTarget.pRenderTarget, color);
  }

}
