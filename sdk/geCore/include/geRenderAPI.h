/*****************************************************************************/
/**
 * @file    geRenderAPI.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/26
 * @brief   Graphics API Interface and Module.
 *
 * Graphics API Interface and Module.
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
#include "geGraphicsInterfaces.h"
#include "geVertexDeclaration.h"
#include "geInputLayout.h"
#include "geShader.h"
#include "geTexture.h"

#include <geModule.h>
#include <geColor.h>
#include <geVector4.h>
#include <geNumericLimits.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT RenderAPI : public Module<RenderAPI>
  {
   public:
    RenderAPI();
    virtual ~RenderAPI();

    /**
     * @brief Initializes the rendering API with the specified screen handle
     *        and display mode.
     * @param scrHandle A pointer to the screen or window handle to be used
     *        for rendering.
     * @param bFullScreen A boolean value indicating whether to initialize in
     *        full screen mode (true) or windowed mode (false).
     * @return Returns true if the rendering API was successfully initialized;
     *         otherwise, returns false.
     */
    virtual bool
    initRenderAPI(WindowHandle scrHandle, bool bFullScreen) = 0;

    virtual bool
    resizeSwapChain(uint32 newWidth, uint32 newHeight) = 0;

    GE_NODISCARD virtual bool
    isMSAAFormatSupported(const GRAPHICS_FORMAT::E format,
                          int32& samplesPerPixel,
                          int32& sampleQuality) const = 0;

    virtual void
    msaaResolveRenderTarget(const WeakSPtr<Texture>& pSrc,
                            const WeakSPtr<Texture>& pDst) = 0;

    virtual void
    reportLiveObjects() = 0;

    GE_NODISCARD virtual WeakSPtr<Texture>
    getBackBuffer() const = 0;

    GE_NODISCARD virtual WeakSPtr<RasterizerState>
    getCurrentRasterizerState() const = 0;

    GE_NODISCARD virtual WeakSPtr<DepthStencilState>
    getCurrentDepthStencilState() const = 0;

    GE_NODISCARD virtual WeakSPtr<BlendState>
    getCurrentBlendState() const = 0;

    GE_NODISCARD virtual WeakSPtr<SamplerState>
    getCurrentSamplerState(uint32 samplerSlot = 0) const = 0;

    /*************************************************************************/
    // Create Objects
    /*************************************************************************/

    GE_NODISCARD virtual SPtr<Texture>
    createTexture(uint32 width,
                  uint32 height,
                  GRAPHICS_FORMAT::E format,
                  uint32 bindFlags = BIND_FLAG::SHADER_RESOURCE,
                  uint32 mipLevels = 1,
                  RESOURCE_USAGE::E usage = RESOURCE_USAGE::DEFAULT,
                  uint32 cpuAccessFlags = 0,
                  uint32 sampleCount = 1,
                  bool isMSAA = false,
                  bool isCubeMap = false,
                  uint32 arraySize = 1) = 0;

    GE_NODISCARD SPtr<Texture>
    createDepthStencilTexture(uint32 width,
                              uint32 height,
                              GRAPHICS_FORMAT::E format,
                              uint32 sampleCount = 1,
                              bool isMSAA = false);

    GE_NODISCARD SPtr<Texture>
    createRenderTargetTexture(uint32 width,
                              uint32 height,
                              GRAPHICS_FORMAT::E format,
                              uint32 sampleCount = 1,
                              bool isMSAA = false);

    GE_NODISCARD virtual SPtr<VertexDeclaration>
    createVertexDeclaration(const Vector<VertexElement>& elements) = 0;

    GE_NODISCARD virtual SPtr<StreamOutputDeclaration>
    createStreamOutputDeclaration(const Vector<StreamOutputElement>& elements) = 0;

    GE_NODISCARD virtual SPtr<InputLayout>
    createInputLayout(const WeakSPtr<VertexDeclaration>& descArray,
                      const WeakSPtr<VertexShader>& pVS) = 0;

    GE_NODISCARD virtual SPtr<InputLayout>
    createInputLayoutFromShader(const WeakSPtr<VertexShader>& pVS) = 0;

    /*************************************************************************/
    // Create Buffers
    /*************************************************************************/
    GE_NODISCARD virtual SPtr<VertexBuffer>
    createVertexBuffer(const SPtr<VertexDeclaration>& pDecl,
                       const SIZE_T sizeInBytes,
                       const void* pInitialData = nullptr,
                       const uint32 usage = RESOURCE_USAGE::DEFAULT) = 0;

    GE_NODISCARD SPtr<VertexBuffer>
    createVertexBuffer(const SPtr<VertexDeclaration>& pDecl,
                       const Vector<byte>& content,
                       const uint32 usage = RESOURCE_USAGE::DEFAULT) {
      return createVertexBuffer(pDecl, content.size(), content.data(), usage);
    }

    GE_NODISCARD virtual SPtr<StreamOutputBuffer>
    createStreamOutputBuffer(const SPtr<StreamOutputDeclaration>& pDecl,
                             const SIZE_T sizeInBytes,
                             const uint32 usage = RESOURCE_USAGE::DEFAULT) = 0;

    GE_NODISCARD virtual SPtr<IndexBuffer>
    createIndexBuffer(const SIZE_T sizeInBytes,
                      const void* pInitialData = nullptr,
                      const INDEX_BUFFER_FORMAT::E format = INDEX_BUFFER_FORMAT::R32_UINT,
                      const uint32 usage = RESOURCE_USAGE::DEFAULT) = 0;

    GE_NODISCARD virtual SPtr<ConstantBuffer>
    createConstantBuffer(const SIZE_T sizeInBytes,
                         const void* pInitialData = nullptr,
                         const uint32 usage = RESOURCE_USAGE::DEFAULT) = 0;

    /*************************************************************************/
    // Create State objects
    /*************************************************************************/
    GE_NODISCARD virtual SPtr<RasterizerState>
    createRasterizerState(const RASTERIZER_DESC& rasterDesc) = 0;

    GE_NODISCARD virtual SPtr<DepthStencilState>
    createDepthStencilState(const DEPTH_STENCIL_DESC& depthStencilDesc) = 0;

    GE_NODISCARD virtual SPtr<BlendState>
    createBlendState(const BLEND_DESC& blendDesc,
                     Vector4 blendFactors = Vector4::ZERO,
                     uint32 sampleMask = NumLimit::MAX_UINT32) = 0;

    GE_NODISCARD virtual SPtr<SamplerState>
    createSamplerState(const SAMPLER_DESC& samplerDesc) = 0;

    /*************************************************************************/
    // Create Shaders
    /*************************************************************************/

#define CREATE_SHADER_PARAMS const Path& fileName,\
                             const Vector<ShaderMacro>& pMacro,\
                             const String& szEntryPoint,\
                             const String& szShaderModel

    GE_NODISCARD virtual SPtr<VertexShader>
    createVertexShader(CREATE_SHADER_PARAMS) = 0;

    GE_NODISCARD virtual SPtr<PixelShader>
    createPixelShader(CREATE_SHADER_PARAMS) = 0;

    GE_NODISCARD virtual SPtr<GeometryShader>
    createGeometryShader(CREATE_SHADER_PARAMS) = 0;

    GE_NODISCARD virtual SPtr<GeometryShader>
    createGeometryShaderWithStreamOutput(CREATE_SHADER_PARAMS,
                                         const SPtr<StreamOutputDeclaration>& pDecl) = 0;

    GE_NODISCARD virtual SPtr<HullShader>
    createHullShader(CREATE_SHADER_PARAMS) = 0;

    GE_NODISCARD virtual SPtr<DomainShader>
    createDomainShader(CREATE_SHADER_PARAMS) = 0;

    GE_NODISCARD virtual SPtr<ComputeShader>
    createComputeShader(CREATE_SHADER_PARAMS) = 0;

    /*************************************************************************/
    // Write Functions
    /*************************************************************************/
    virtual void
    writeToResource(const WeakSPtr<GraphicsResource>& pResource,
                    uint32 dstSubRes,
                    const GRAPHICS_BOX* pDstBox,
                    const void* pSrcData,
                    uint32 srcRowPitch,
                    uint32 srcDepthPitch = 0,
                    uint32 copyFlags = 0) = 0;

    GE_NODISCARD virtual MappedSubresource
    mapToRead(const WeakSPtr<GraphicsResource>& pResource,
              uint32 subResource = 0,
              uint32 mapFlags = 0) = 0;

    virtual void
    unmap(const WeakSPtr<GraphicsResource>& pResource,
          uint32 subResource = 0) = 0;

    virtual void
    copyResource(const WeakSPtr<GraphicsResource>& pSrcObj,
                 const WeakSPtr<GraphicsResource>& pDstObj) = 0;

    virtual void
    generateMips(const WeakSPtr<Texture>& pTexture) = 0;

    virtual void
    clearRenderTarget(const WeakSPtr<Texture>& pRenderTarget,
                      const LinearColor& color = LinearColor::Black) = 0;

    void
    clearRenderTarget(const RenderTarget& pRenderTarget,
                      const LinearColor color = LinearColor::Black);

    virtual void
    clearDepthStencil(const WeakSPtr<Texture>& pDepthStencilView,
                      uint32 flags = CLEAR_FLAG::DEPTH | CLEAR_FLAG::STENCIL,
                      float depthVal = 1.0f,
                      uint8 stencilVal = 0U) = 0;

    virtual void
    discardView(WeakSPtr<Texture> pTexture) = 0;

    virtual void
    present() = 0;

    /*************************************************************************/ 
    // Set Objects
    /*************************************************************************/

    virtual void
    setImmediateContext() = 0;

    virtual void
    setTopology(PRIMITIVE_TOPOLOGY::E topologyType) = 0;

    virtual void
    setViewports(const Vector<GRAPHICS_VIEWPORT>& viewports) = 0;

    virtual void
    setInputLayout(const WeakSPtr<InputLayout>& pInputLayout) = 0;

    virtual void
    setRasterizerState(const WeakSPtr<RasterizerState>& pRasterizerState) = 0;

    virtual void
    setDepthStencilState(const WeakSPtr<DepthStencilState>& pDepthStencilState,
                         uint32 stencilRef = 0) = 0;

    virtual void
    setBlendState(const WeakSPtr<BlendState>& pBlendState) = 0;

    virtual void
    setVertexBuffer(const WeakSPtr<VertexBuffer>& pVertexBuffer,
                    uint32 startSlot = 0,
                    uint32 offset = 0) = 0;

    virtual void
    setIndexBuffer(const WeakSPtr<IndexBuffer>& pIndexBuffer,
                   uint32 offset = 0) = 0;

    /*************************************************************************/
    // Set Shaders
    /*************************************************************************/
    virtual void
    vsSetProgram(const WeakSPtr<VertexShader>& pInShader) = 0;
    
    virtual void
    psSetProgram(const WeakSPtr<PixelShader>& pInShader) = 0;
    
    virtual void
    gsSetProgram(const WeakSPtr<GeometryShader>& pInShader) = 0;
    
    virtual void
    hsSetProgram(const WeakSPtr<HullShader>& pInShader) = 0;
    
    virtual void
    dsSetProgram(const WeakSPtr<DomainShader>& pInShader) = 0;
    
    virtual void
    csSetProgram(const WeakSPtr<ComputeShader>& pInShader) = 0;

    /*************************************************************************/
    // Set Shaders Resources
    /*************************************************************************/
    virtual void
    vsSetShaderResource(const WeakSPtr<Texture>& pTexture,
                        const uint32 startSlot = 0) = 0;

    virtual void
    psSetShaderResource(const WeakSPtr<Texture>& pTexture,
                        const uint32 startSlot = 0) = 0;

    virtual void
    gsSetShaderResource(const WeakSPtr<Texture>& pTexture,
                        const uint32 startSlot = 0) = 0;

    virtual void
    hsSetShaderResource(const WeakSPtr<Texture>& pTexture,
                        const uint32 startSlot = 0) = 0;

    virtual void
    dsSetShaderResource(const WeakSPtr<Texture>& pTexture,
                        const uint32 startSlot = 0) = 0;

    virtual void
    csSetShaderResource(const WeakSPtr<Texture>& pTexture,
                        const uint32 startSlot = 0) = 0;

    /*************************************************************************/
    // Set Unordered Access Views
    /*************************************************************************/
    virtual void
    csSetUnorderedAccessViews(const Vector<UAVTarget>& pUAVs,
                              const uint32 startSlot = 0) = 0;

    /*************************************************************************/
    // Set Constant Buffers
    /*************************************************************************/
    virtual void
    vsSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                        const uint32 startSlot = 0) = 0;

    virtual void
    psSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                        const uint32 startSlot = 0) = 0;

    virtual void
    gsSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                        const uint32 startSlot = 0) = 0;

    virtual void
    hsSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                        const uint32 startSlot = 0) = 0;

    virtual void
    dsSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                        const uint32 startSlot = 0) = 0;

    virtual void
    csSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                        const uint32 startSlot = 0) = 0;

    /*************************************************************************/
    // Set Samplers
    /*************************************************************************/
    virtual void
    vsSetSampler(const WeakSPtr<SamplerState>& pSampler,
                 const uint32 startSlot = 0) = 0;

    virtual void
    psSetSampler(const WeakSPtr<SamplerState>& pSampler,
                 const uint32 startSlot = 0) = 0;

    virtual void
    gsSetSampler(const WeakSPtr<SamplerState>& pSampler,
                 const uint32 startSlot = 0) = 0;

    virtual void
    hsSetSampler(const WeakSPtr<SamplerState>& pSampler,
                 const uint32 startSlot = 0) = 0;

    virtual void
    dsSetSampler(const WeakSPtr<SamplerState>& pSampler,
                 const uint32 startSlot = 0) = 0;

    virtual void
    csSetSampler(const WeakSPtr<SamplerState>& pSampler,
                 const uint32 startSlot = 0) = 0;

    /*************************************************************************/
    // Set Render Targets
    /*************************************************************************/
    virtual void
    setRenderTargets(const Vector<RenderTarget>& pTargets,
                     const WeakSPtr<Texture>& pDepthStencilView) = 0;

    virtual void
    setStreamOutputTarget(const WeakSPtr<StreamOutputBuffer>& pBuffer) = 0;

    /*************************************************************************/
    // State Management Functions
    /*************************************************************************/
    virtual SPtr<PipelineState>
    savePipelineState() const = 0;

    virtual void
    restorePipelineState(const WeakSPtr<PipelineState>& pState) = 0;

    /*************************************************************************/
    // Draw Functions
    /*************************************************************************/
    virtual void
    draw(uint32 vertexCount, uint32 startVertexLocation = 0) = 0;

    virtual void
    drawIndexed(uint32 indexCount,
                uint32 startIndexLocation = 0,
                int32 baseVertexLocation = 0) = 0;

    virtual void
    drawInstanced(uint32 vertexCountPerInstance,
                  uint32 instanceCount,
                  uint32 startVertexLocation = 0,
                  uint32 startInstanceLocation = 0) = 0;

    virtual void
    drawAuto() = 0;

    virtual void
    dispatch(uint32 threadGroupCountX,
             uint32 threadGroupCountY = 1,
             uint32 threadGroupCountZ = 1) = 0;

    /*************************************************************************/
    // Getter Functions
    /*************************************************************************/
    virtual GraphicsInfo
    getDevice() const = 0;
  };

  GE_LOG_CATEGORY(RenderAPI, 100);
}
