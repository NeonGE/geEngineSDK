/*****************************************************************************/
/**
 * @file    NullGraphicsManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/02/17
 * @brief   Graphics API implementation on a Null Driver.
 *
 * Graphics API implementation on a Null Driver.
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
#include <geRenderAPI.h>

#include "NullInputLayout.h"
#include "NullTexture.h"
#include "NullShader.h"

namespace geEngineSDK {
  class NullRenderAPI : public RenderAPI
  {
   public:
    NullRenderAPI() = default;
    virtual ~NullRenderAPI();
    
    bool
    initRenderAPI(WindowHandle scrHandle, bool bFullScreen) override;

    bool
    resizeSwapChain(uint32 newWidth, uint32 newHeight) override;

    bool
    isMSAAFormatSupported(const GRAPHICS_FORMAT::E format,
                          int32& samplesPerPixel,
                          int32& sampleQuality) const override;

    void
    msaaResolveRenderTarget(const WeakSPtr<Texture>& pSrc,
                            const WeakSPtr<Texture>& pDst) override;

    void
    reportLiveObjects() override;

    //************************************************************************/
    // Get methods
    //************************************************************************/
    WeakSPtr<Texture>
    getBackBuffer() const override;

    WeakSPtr<RasterizerState>
    getCurrentRasterizerState() const override;

    WeakSPtr<DepthStencilState>
    getCurrentDepthStencilState() const override;

    WeakSPtr<BlendState>
    getCurrentBlendState() const override;

    WeakSPtr<SamplerState>
    getCurrentSamplerState(uint32 samplerSlot = 0) const override;

    /*************************************************************************/
    // Create methods
    /*************************************************************************/
    SPtr<Texture>
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
                  uint32 arraySize = 1) override;

    SPtr<VertexDeclaration>
    createVertexDeclaration(const Vector<VertexElement>& elements) override;

    SPtr<StreamOutputDeclaration>
    createStreamOutputDeclaration(const Vector<StreamOutputElement>& elements) override;

    SPtr<InputLayout>
    createInputLayout(const WeakSPtr<VertexDeclaration>& descArray,
                      const WeakSPtr<VertexShader>& pVS) override;

    SPtr<InputLayout>
    createInputLayoutFromShader(const WeakSPtr<VertexShader>& pVS) override;

    /*************************************************************************/
    // Create Buffers
    /*************************************************************************/
   public:
    SPtr<VertexBuffer>
    createVertexBuffer(const SPtr<VertexDeclaration>& pDecl, 
                       const SIZE_T sizeInBytes,
                       const void* pInitialData = nullptr,
                       const uint32 usage = RESOURCE_USAGE::DEFAULT) override;

    SPtr<StreamOutputBuffer>
    createStreamOutputBuffer(const SPtr<StreamOutputDeclaration>& pDecl,
                             const SIZE_T sizeInBytes,
                             const uint32 usage = RESOURCE_USAGE::DEFAULT) override;

    SPtr<IndexBuffer>
    createIndexBuffer(const SIZE_T sizeInBytes,
                      const void* pInitialData = nullptr,
                      const INDEX_BUFFER_FORMAT::E format = INDEX_BUFFER_FORMAT::R32_UINT,
                      const uint32 usage = RESOURCE_USAGE::DEFAULT) override;

    SPtr<ConstantBuffer>
    createConstantBuffer(const SIZE_T sizeInBytes,
                         const void* pInitialData = nullptr,
                         const uint32 usage = RESOURCE_USAGE::DEFAULT) override;

    /*************************************************************************/
    // Create Pipeline State Objects
    /*************************************************************************/
    SPtr<RasterizerState>
    createRasterizerState(const RASTERIZER_DESC& rasterDesc) override;

    SPtr<DepthStencilState>
    createDepthStencilState(const DEPTH_STENCIL_DESC& depthStencilDesc) override;

    SPtr<BlendState>
    createBlendState(const BLEND_DESC& blendDesc,
                     const Vector4 blendFactors = Vector4::ZERO,
                     const uint32 sampleMask = NumLimit::MAX_UINT32) override;

    SPtr<SamplerState>
    createSamplerState(const SAMPLER_DESC& samplerDesc) override;

    /*************************************************************************/
    // Create Shaders
    /*************************************************************************/

    SPtr<VertexShader>
    createVertexShader(CREATE_SHADER_PARAMS) override;

    SPtr<PixelShader>
    createPixelShader(CREATE_SHADER_PARAMS) override;

    SPtr<GeometryShader>
    createGeometryShader(CREATE_SHADER_PARAMS) override;

    SPtr<GeometryShader>
    createGeometryShaderWithStreamOutput(CREATE_SHADER_PARAMS,
                                         const SPtr<StreamOutputDeclaration>& pDecl) override;

    SPtr<HullShader>
    createHullShader(CREATE_SHADER_PARAMS) override;

    SPtr<DomainShader>
    createDomainShader(CREATE_SHADER_PARAMS) override;

    SPtr<ComputeShader>
    createComputeShader(CREATE_SHADER_PARAMS) override;

    /*************************************************************************/
    // Write Functions
    /*************************************************************************/
    void
    writeToResource(const WeakSPtr<GraphicsResource>& pResource,
                    uint32 dstSubRes,
                    const GRAPHICS_BOX* pDstBox,
                    const void* pSrcData,
                    uint32 srcRowPitch,
                    uint32 srcDepthPitch,
                    uint32 copyFlags = 0) override;

    MappedSubresource
    mapToRead(const WeakSPtr<GraphicsResource>& pTexture,
              uint32 subResource = 0,
              uint32 mapFlags = 0) override;

    void
    unmap(const WeakSPtr<GraphicsResource>& pTexture,
          uint32 subResource = 0) override;

    void
    copyResource(const WeakSPtr<GraphicsResource>& pSrcObj,
                 const WeakSPtr<GraphicsResource>& pDstObj) override;

    void
    generateMips(const WeakSPtr<Texture>& pTexture) override;

    void
    clearRenderTarget(const WeakSPtr<Texture>& pRenderTarget,
                      const LinearColor& color = LinearColor::Black) override;

    void
    clearDepthStencil(const WeakSPtr<Texture>& pDepthStencilView,
                      uint32 flags = CLEAR_FLAG::DEPTH | CLEAR_FLAG::STENCIL,
                      float depthVal = 1.0f,
                      uint8 stencilVal = 0U) override;

    void
    discardView(WeakSPtr<Texture> pTexture) override;

    void
    present() override;

    /*************************************************************************/
    // Set Objects
    /*************************************************************************/

    void
    setImmediateContext() override;

    void
    setTopology(PRIMITIVE_TOPOLOGY::E topologyType) override;

    void
    setViewports(const Vector<GRAPHICS_VIEWPORT>& viewports) override;

    void
    setInputLayout(const WeakSPtr<InputLayout>& pInputLayout) override;

    void
    setRasterizerState(const WeakSPtr<RasterizerState>& pRasterizerState) override;

    void
    setDepthStencilState(const WeakSPtr<DepthStencilState>& pDepthStencilState,
                         uint32 stencilRef = 0) override;

    void
    setBlendState(const WeakSPtr<BlendState>& pBlendState) override;

    void
    setVertexBuffer(const WeakSPtr<VertexBuffer>& pVertexBuffer,
                    uint32 startSlot = 0,
                    uint32 offset = 0) override;

    void
    setIndexBuffer(const WeakSPtr<IndexBuffer>& pIndexBuffer,
                   uint32 offset = 0) override;

    /*************************************************************************/
    // Set Shaders
    /*************************************************************************/
   public:
    void
    vsSetProgram(const WeakSPtr<VertexShader>& pInShader) override;
    
    void
    psSetProgram(const WeakSPtr<PixelShader>& pInShader) override;
    
    void
    gsSetProgram(const WeakSPtr<GeometryShader>& pInShader) override;
    
    void
    hsSetProgram(const WeakSPtr<HullShader>& pInShader) override;
    
    void
    dsSetProgram(const WeakSPtr<DomainShader>& pInShader) override;
    
    void
    csSetProgram(const WeakSPtr<ComputeShader>& pInShader) override;

    /*************************************************************************/
    // Set Shaders Resources
    /*************************************************************************/
    void
    vsSetShaderResource(const WeakSPtr<Texture>& pTexture,
                        const uint32 startSlot = 0) override;

    void
    psSetShaderResource(const WeakSPtr<Texture>& pTexture,
                        const uint32 startSlot = 0) override;

    void
    gsSetShaderResource(const WeakSPtr<Texture>& pTexture,
                        const uint32 startSlot = 0) override;

    void
    hsSetShaderResource(const WeakSPtr<Texture>& pTexture,
                        const uint32 startSlot = 0) override;

    void
    dsSetShaderResource(const WeakSPtr<Texture>& pTexture,
                        const uint32 startSlot = 0) override;

    void
    csSetShaderResource(const WeakSPtr<Texture>& pTexture,
                        const uint32 startSlot = 0) override;

    /*************************************************************************/
    // Set Unordered Access Views
    /*************************************************************************/
    void
    csSetUnorderedAccessViews(const Vector<UAVTarget>& pUAVs,
                              const uint32 startSlot = 0) override;


    /*************************************************************************/
    // Set Constant Buffers
    /*************************************************************************/
    void
    vsSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                        const uint32 startSlot = 0) override;

    void
    psSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                        const uint32 startSlot = 0) override;

    void
    gsSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                        const uint32 startSlot = 0) override;

    void
    hsSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                        const uint32 startSlot = 0) override;

    void
    dsSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                        const uint32 startSlot = 0) override;

    void
    csSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                        const uint32 startSlot = 0) override;

    /*************************************************************************/
    // Set Samplers
    /*************************************************************************/
    void
    vsSetSampler(const WeakSPtr<SamplerState>& pSampler,
                 const uint32 startSlot = 0) override;

    void
    psSetSampler(const WeakSPtr<SamplerState>& pSampler,
                 const uint32 startSlot = 0) override;

    void
    gsSetSampler(const WeakSPtr<SamplerState>& pSampler,
                 const uint32 startSlot = 0) override;

    void
    hsSetSampler(const WeakSPtr<SamplerState>& pSampler,
                 const uint32 startSlot = 0) override;

    void
    dsSetSampler(const WeakSPtr<SamplerState>& pSampler,
                 const uint32 startSlot = 0) override;

    void
    csSetSampler(const WeakSPtr<SamplerState>& pSampler,
                 const uint32 startSlot = 0) override;

    /*************************************************************************/
    // Set Render Targets
    /*************************************************************************/
    void
    setRenderTargets(const Vector<RenderTarget>& pTargets,
                     const WeakSPtr<Texture>& pDepthStencilView) override;

    void
    setStreamOutputTarget(const WeakSPtr<StreamOutputBuffer>& pBuffer) override;

    /*************************************************************************/
    // State Management Functions
    /*************************************************************************/
    SPtr<PipelineState>
    savePipelineState() const override;

    void
    restorePipelineState(const WeakSPtr<PipelineState>& pState) override;

    /*************************************************************************/
    // Draw Functions
    /*************************************************************************/
    void
    draw(uint32 vertexCount, uint32 startVertexLocation = 0) override;

    void
    drawIndexed(uint32 indexCount,
                uint32 startIndexLocation = 0,
                int32 baseVertexLocation = 0) override;

    void
    drawInstanced(uint32 vertexCountPerInstance,
                  uint32 instanceCount,
                  uint32 startVertexLocation = 0,
                  uint32 startInstanceLocation = 0) override;

    void
    drawAuto() override;

    void
    dispatch(uint32 threadGroupCountX,
             uint32 threadGroupCountY = 1,
             uint32 threadGroupCountZ = 1) override;

    /*************************************************************************/
    // Getter Functions
    /*************************************************************************/
    GraphicsInfo
    getDevice() const override;

   private:
    bool m_bFullScreen = false;
    SPtr<NullTexture> m_pBackBufferTexture;
  };
} // namespace geEngineSDK
