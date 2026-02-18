/*****************************************************************************/
/**
 * @file    NullRenderAPI.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/29
 * @brief   Graphics API Implementation with Null Driver.
 *
 * Graphics API Implementation with Null Driver.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geRenderAPI.h"
#include "NullRenderAPI.h"
#include "NullGraphicsBuffer.h"

#include <geGameConfig.h>
#include <geMath.h>
#include <geDebug.h>
#include <d3dcompiler.h>

namespace geEngineSDK {
  using std::pair;
  using std::make_pair;

  bool
  NullRenderAPI::initRenderAPI(WindowHandle scrHandle, bool bFullScreen) {
    GE_UNREFERENCED_PARAMETER(scrHandle);
    m_bFullScreen = bFullScreen;
    m_pBackBufferTexture = ge_shared_ptr_new<NullTexture>();
    return true;
  }

  NullRenderAPI::~NullRenderAPI() {}

  bool
  NullRenderAPI::resizeSwapChain(uint32 newWidth, uint32 newHeight) {
    if(m_pBackBufferTexture)
      m_pBackBufferTexture->release();

    m_pBackBufferTexture = ge_shared_ptr_new<NullTexture>();
    m_pBackBufferTexture->m_desc.width = newWidth;
    m_pBackBufferTexture->m_desc.height = newHeight;
    m_pBackBufferTexture->m_desc.mipLevels = 1;
    m_pBackBufferTexture->m_desc.format = GRAPHICS_FORMAT::kR8G8B8A8_UNORM;

    return true;
  }

  bool
  NullRenderAPI::isMSAAFormatSupported(const GRAPHICS_FORMAT::E,
                                       int32&,
                                       int32&) const {
    return false;
  }

  void
  NullRenderAPI::msaaResolveRenderTarget(const WeakSPtr<Texture>&,
                                         const WeakSPtr<Texture>&)
  {}

  void
  NullRenderAPI::reportLiveObjects()
  {}
  
  SPtr<Texture>
  NullRenderAPI::createTexture(uint32 width,
                               uint32 height,
                               GRAPHICS_FORMAT::E format,
                               uint32 bindFlags,
                               uint32 mipLevels,
                               RESOURCE_USAGE::E usage,
                               uint32 cpuAccessFlags,
                               uint32 sampleCount,
                               bool /*isMSAA*/,
                               bool isCubeMap,
                               uint32 arraySize) {
    auto pTexture = ge_shared_ptr_new<NullTexture>();

    pTexture->m_desc.width = width;
    pTexture->m_desc.height = height;
    pTexture->m_desc.format = format;
    pTexture->m_desc.bindFlags = bindFlags;
    pTexture->m_desc.mipLevels = mipLevels;
    pTexture->m_desc.usage = usage;
    pTexture->m_desc.cpuAccessFlags = cpuAccessFlags;
    pTexture->m_desc.sampleDesc.count = sampleCount;
    pTexture->m_desc.sampleDesc.quality = 0;
    pTexture->m_desc.arraySize = arraySize;
    pTexture->m_bIsCubeMap = isCubeMap;
    pTexture->m_textureObj += 1;

    return pTexture;
  }

  SPtr<VertexDeclaration>
  NullRenderAPI::createVertexDeclaration(const Vector<VertexElement>& elements) {
    if (elements.empty()) {
      GE_LOG(kError,
             RenderAPI,
             "NullRenderAPI::createVertexDeclaration called with no elements.");
      return nullptr;
    }
    return ge_shared_ptr_new<VertexDeclaration>(elements);
  }

  SPtr<StreamOutputDeclaration>
  NullRenderAPI::createStreamOutputDeclaration(const Vector<StreamOutputElement>& elements) {
    if (elements.empty()) {
      GE_LOG(kError,
             RenderAPI,
             "NullRenderAPI::createVertexDeclaration called with no elements.");
      return nullptr;
    }
    return ge_shared_ptr_new<StreamOutputDeclaration>(elements);
  }

  SPtr<InputLayout>
  NullRenderAPI::createInputLayout(const WeakSPtr<VertexDeclaration>& descArray,
                                   const WeakSPtr<VertexShader>& pVS) {
    if(descArray.expired() || pVS.expired()) {
      GE_LOG(kError,
             RenderAPI,
             "NullRenderAPI::createInputLayout called with Invalid Parameters");
      return nullptr;
    }
    
    auto inputLayout = ge_shared_ptr_new<NullInputLayout>();
    inputLayout->m_inputLayout += 1;
    auto pDesc = descArray.lock();
    inputLayout->m_vertexDeclaration = pDesc;

    return inputLayout;
  }

  SPtr<InputLayout>
  NullRenderAPI::createInputLayoutFromShader(const WeakSPtr<VertexShader>& pVS) {
    if (pVS.expired()) {
      GE_LOG(kError,
             RenderAPI,
             "Vertex Shader is expired.");
      return nullptr;
    }

    //TODO: Should get the shader reflection?
    return nullptr;
  }

  /*************************************************************************/
  // Create Buffers
  /*************************************************************************/
  SPtr<VertexBuffer>
  NullRenderAPI::createVertexBuffer(const SPtr<VertexDeclaration>& pDecl,
                                    const SIZE_T sizeInBytes,
                                    const void* /*pInitialData*/,
                                    const uint32 usage) {
    auto pVB = ge_shared_ptr_new<NullVertexBuffer>();

    pVB->m_pBuffer += 1;
    pVB->m_Desc.byteWidth = cast::st<uint32>(sizeInBytes);
    pVB->m_Desc.usage = usage;
    pVB->m_pVertexDeclaration = pDecl;

    return pVB;
  }

  SPtr<StreamOutputBuffer>
  NullRenderAPI::createStreamOutputBuffer(const SPtr<StreamOutputDeclaration>& pDecl,
                                          const SIZE_T sizeInBytes,
                                          const uint32 usage) {
    auto pSOB = ge_shared_ptr_new<NullStreamOutputBuffer>();

    auto& soProps = pDecl->getProperties();
    uint32 byteStride = soProps.getComponentCountForOutputSlot(0) * sizeof(float);

    pSOB->m_pStreamOutputDeclaration = pDecl;
    pSOB->m_pBuffer += 1;
    pSOB->m_Desc.byteWidth = cast::st<uint32>(sizeInBytes);
    pSOB->m_Desc.structureByteStride = byteStride;
    pSOB->m_Desc.usage = usage;

    return pSOB;
  }

  SPtr<IndexBuffer>
  NullRenderAPI::createIndexBuffer(const SIZE_T sizeInBytes,
                                   const void* /*pInitialData*/,
                                   const INDEX_BUFFER_FORMAT::E format,
                                   const uint32 usage) {
    auto pIB = ge_shared_ptr_new<NullIndexBuffer>();
    
    pIB->m_pBuffer += 1;
    pIB->m_Desc.byteWidth = cast::st<uint32>(sizeInBytes);
    pIB->m_Desc.usage = usage;

    if(format == INDEX_BUFFER_FORMAT::R32_UINT) {
      pIB->m_indexFormat = GRAPHICS_FORMAT::kR32_UINT;
    }
    else {
      pIB->m_indexFormat = GRAPHICS_FORMAT::kR16_UINT;
    }

    return pIB;
  }

  SPtr<ConstantBuffer>
  NullRenderAPI::createConstantBuffer(const SIZE_T sizeInBytes,
                                      const void* /*pInitialData*/,
                                      const uint32 usage) {
    auto pCB = ge_shared_ptr_new<NullConstantBuffer>();
    pCB->m_pBuffer += 1;
    pCB->m_Desc.byteWidth = cast::st<uint32>(sizeInBytes);
    pCB->m_Desc.usage = usage;

    return pCB;
  }

  SPtr<RasterizerState>
  NullRenderAPI::createRasterizerState(const RASTERIZER_DESC&) {
    auto pRS = ge_shared_ptr_new<NullRasterizerState>();
    pRS->m_pRasterizerState += 1;
    return pRS;
  }

  SPtr<DepthStencilState>
  NullRenderAPI::createDepthStencilState(const DEPTH_STENCIL_DESC&) {
    auto pDSS = ge_shared_ptr_new<NullDepthStencilState>();
    pDSS->m_pDepthStencilState += 1;
    return pDSS;
  }

  SPtr<BlendState>
  NullRenderAPI::createBlendState(const BLEND_DESC&,
                                  const Vector4,
                                  const uint32) {
    auto pBS = ge_shared_ptr_new<NullBlendState>();
    pBS->m_pBlendState += 1;
    return pBS;
  }

  SPtr<SamplerState>
  NullRenderAPI::createSamplerState(const SAMPLER_DESC&) {
    auto pSS = ge_shared_ptr_new<NullSamplerState>();
    pSS->m_pSampler += 1;
    return pSS;
  }

  /*************************************************************************/
  // Create Shaders
  /*************************************************************************/
  SPtr<VertexShader>
  NullRenderAPI::createVertexShader(CREATE_SHADER_PARAMS) {
    GE_UNREFERENCED_PARAMETER(fileName);
    GE_UNREFERENCED_PARAMETER(pMacro);
    GE_UNREFERENCED_PARAMETER(szEntryPoint);
    GE_UNREFERENCED_PARAMETER(szShaderModel);

    auto vShader = ge_shared_ptr_new<NullShader>();
    vShader->m_pShader += 1;
    vShader->m_pBlob += 1;
    return vShader;
  }

  SPtr<PixelShader>
  NullRenderAPI::createPixelShader(CREATE_SHADER_PARAMS) {
    GE_UNREFERENCED_PARAMETER(fileName);
    GE_UNREFERENCED_PARAMETER(pMacro);
    GE_UNREFERENCED_PARAMETER(szEntryPoint);
    GE_UNREFERENCED_PARAMETER(szShaderModel);

    auto vShader = ge_shared_ptr_new<NullShader>();
    vShader->m_pShader += 1;
    vShader->m_pBlob += 1;
    return vShader;
  }

  SPtr<GeometryShader>
  NullRenderAPI::createGeometryShader(CREATE_SHADER_PARAMS) {
    GE_UNREFERENCED_PARAMETER(fileName);
    GE_UNREFERENCED_PARAMETER(pMacro);
    GE_UNREFERENCED_PARAMETER(szEntryPoint);
    GE_UNREFERENCED_PARAMETER(szShaderModel);

    auto vShader = ge_shared_ptr_new<NullShader>();
    vShader->m_pShader += 1;
    vShader->m_pBlob += 1;
    return vShader;
  }

  SPtr<GeometryShader>
  NullRenderAPI::createGeometryShaderWithStreamOutput(CREATE_SHADER_PARAMS,
                   const SPtr<StreamOutputDeclaration>& pDecl) {
    GE_UNREFERENCED_PARAMETER(fileName);
    GE_UNREFERENCED_PARAMETER(pMacro);
    GE_UNREFERENCED_PARAMETER(szEntryPoint);
    GE_UNREFERENCED_PARAMETER(szShaderModel);
    GE_UNREFERENCED_PARAMETER(pDecl);

    auto vShader = ge_shared_ptr_new<NullShader>();
    vShader->m_pShader += 1;
    vShader->m_pBlob += 1;
    return vShader;
  }

  SPtr<HullShader>
  NullRenderAPI::createHullShader(CREATE_SHADER_PARAMS) {
    GE_UNREFERENCED_PARAMETER(fileName);
    GE_UNREFERENCED_PARAMETER(pMacro);
    GE_UNREFERENCED_PARAMETER(szEntryPoint);
    GE_UNREFERENCED_PARAMETER(szShaderModel);

    auto vShader = ge_shared_ptr_new<NullShader>();
    vShader->m_pShader += 1;
    vShader->m_pBlob += 1;
    return vShader;
  }

  SPtr<DomainShader>
  NullRenderAPI::createDomainShader(CREATE_SHADER_PARAMS) {
    GE_UNREFERENCED_PARAMETER(fileName);
    GE_UNREFERENCED_PARAMETER(pMacro);
    GE_UNREFERENCED_PARAMETER(szEntryPoint);
    GE_UNREFERENCED_PARAMETER(szShaderModel);

    auto vShader = ge_shared_ptr_new<NullShader>();
    vShader->m_pShader += 1;
    vShader->m_pBlob += 1;
    return vShader;
  }

  SPtr<ComputeShader>
  NullRenderAPI::createComputeShader(CREATE_SHADER_PARAMS) {
    GE_UNREFERENCED_PARAMETER(fileName);
    GE_UNREFERENCED_PARAMETER(pMacro);
    GE_UNREFERENCED_PARAMETER(szEntryPoint);
    GE_UNREFERENCED_PARAMETER(szShaderModel);

    auto vShader = ge_shared_ptr_new<NullShader>();
    vShader->m_pShader += 1;
    vShader->m_pBlob += 1;
    return vShader;
  }

  /*************************************************************************/
  // Write Functions
  /*************************************************************************/

  void
  NullRenderAPI::writeToResource(const WeakSPtr<GraphicsResource>&,
                                 uint32,
                                 const GRAPHICS_BOX*,
                                 const void*,
                                 uint32,
                                 uint32,
                                 uint32)
  {}

  MappedSubresource
  NullRenderAPI::mapToRead(const WeakSPtr<GraphicsResource>&,
                           uint32,
                           uint32) {
    MappedSubresource mappedSubresource;
    mappedSubresource.pData = nullptr;
    mappedSubresource.rowPitch = 0;
    mappedSubresource.depthPitch = 0;

    return mappedSubresource;
  }

  void
  NullRenderAPI::unmap(const WeakSPtr<GraphicsResource>&, uint32)
  {}

  void
  NullRenderAPI::copyResource(const WeakSPtr<GraphicsResource>&,
                              const WeakSPtr<GraphicsResource>&)
  {}

  void
  NullRenderAPI::generateMips(const WeakSPtr<Texture>&)
  {}

  void
  NullRenderAPI::clearRenderTarget(const WeakSPtr<Texture>&, const LinearColor&)
  {}

  void
  NullRenderAPI::clearDepthStencil(const WeakSPtr<Texture>&, uint32, float, uint8)
  {}

  void
  NullRenderAPI::discardView(WeakSPtr<Texture>)
  {}

  void
  NullRenderAPI::present()
  {}

  void
  NullRenderAPI::setImmediateContext()
  {}

  void
  NullRenderAPI::setTopology(PRIMITIVE_TOPOLOGY::E)
  {}

  void
  NullRenderAPI::setViewports(const Vector<GRAPHICS_VIEWPORT>&)
  {}

  void
  NullRenderAPI::setInputLayout(const WeakSPtr<InputLayout>&)
  {}

  void
  NullRenderAPI::setRasterizerState(const WeakSPtr<RasterizerState>&)
  {}

  void
  NullRenderAPI::setDepthStencilState(const WeakSPtr<DepthStencilState>&, uint32)
  {}

  void
  NullRenderAPI::setBlendState(const WeakSPtr<BlendState>&)
  {}

  void
  NullRenderAPI::setVertexBuffer(const WeakSPtr<VertexBuffer>&, uint32, uint32)
  {}

  void
  NullRenderAPI::setIndexBuffer(const WeakSPtr<IndexBuffer>&, uint32)
  {}

  /*************************************************************************/
  // Set Shaders
  /*************************************************************************/
  void
  NullRenderAPI::vsSetProgram(const WeakSPtr<VertexShader>&) {}

  void
  NullRenderAPI::psSetProgram(const WeakSPtr<PixelShader>&) {}

  void
  NullRenderAPI::gsSetProgram(const WeakSPtr<GeometryShader>&) {}

  void
  NullRenderAPI::hsSetProgram(const WeakSPtr<HullShader>&) {}

  void
  NullRenderAPI::dsSetProgram(const WeakSPtr<DomainShader>&) {}

  void
  NullRenderAPI::csSetProgram(const WeakSPtr<ComputeShader>&) {}

  /*************************************************************************/
  // Set Shaders Resources
  /*************************************************************************/
  void
  NullRenderAPI::vsSetShaderResource(const WeakSPtr<Texture>&, const uint32) {}

  void
  NullRenderAPI::psSetShaderResource(const WeakSPtr<Texture>&, const uint32) {}

  void
  NullRenderAPI::gsSetShaderResource(const WeakSPtr<Texture>&, const uint32) {}

  void
  NullRenderAPI::hsSetShaderResource(const WeakSPtr<Texture>&, const uint32) {}

  void
  NullRenderAPI::dsSetShaderResource(const WeakSPtr<Texture>&, const uint32) {}

  void
  NullRenderAPI::csSetShaderResource(const WeakSPtr<Texture>&, const uint32) {}

  /*************************************************************************/
  // Set Unordered Access Views
  /*************************************************************************/
  void
  NullRenderAPI::csSetUnorderedAccessViews(const Vector<UAVTarget>&, const uint32) {}

  /*************************************************************************/
  // Set Constant Buffers
  /*************************************************************************/
  void
  NullRenderAPI::vsSetConstantBuffer(const WeakSPtr<ConstantBuffer>&, const uint32) {}

  void
  NullRenderAPI::psSetConstantBuffer(const WeakSPtr<ConstantBuffer>&, const uint32) {}

  void
  NullRenderAPI::gsSetConstantBuffer(const WeakSPtr<ConstantBuffer>&, const uint32) {}

  void
  NullRenderAPI::hsSetConstantBuffer(const WeakSPtr<ConstantBuffer>&, const uint32) {}

  void
  NullRenderAPI::dsSetConstantBuffer(const WeakSPtr<ConstantBuffer>&, const uint32) {}

  void
  NullRenderAPI::csSetConstantBuffer(const WeakSPtr<ConstantBuffer>&, const uint32) {}

  /*************************************************************************/
  // Set Samplers
  /*************************************************************************/
  void
  NullRenderAPI::vsSetSampler(const WeakSPtr<SamplerState>&, const uint32) {}

  void
  NullRenderAPI::psSetSampler(const WeakSPtr<SamplerState>&, const uint32) {}

  void
  NullRenderAPI::gsSetSampler(const WeakSPtr<SamplerState>&, const uint32) {}

  void
  NullRenderAPI::hsSetSampler(const WeakSPtr<SamplerState>&, const uint32) {}

  void
  NullRenderAPI::dsSetSampler(const WeakSPtr<SamplerState>&, const uint32) {}

  void
  NullRenderAPI::csSetSampler(const WeakSPtr<SamplerState>&, const uint32) {}

  /*************************************************************************/
  // Set Render Targets
  /*************************************************************************/
  void
  NullRenderAPI::setRenderTargets(const Vector<RenderTarget>&,
                                  const WeakSPtr<Texture>&)
  {}

  void
  NullRenderAPI::setStreamOutputTarget(const WeakSPtr<StreamOutputBuffer>&)
  {}

  SPtr<PipelineState>
  NullRenderAPI::savePipelineState() const {
    return nullptr;
  }

  void
  NullRenderAPI::restorePipelineState(const WeakSPtr<PipelineState>&) {}

  void
  NullRenderAPI::draw(uint32, uint32) {}

  void
  NullRenderAPI::drawIndexed(uint32, uint32, int32) {}

  void
  NullRenderAPI::drawInstanced(uint32, uint32, uint32, uint32) {}

  void
  NullRenderAPI::drawAuto() {}

  void
  NullRenderAPI::dispatch(uint32, uint32, uint32) {}

  GraphicsInfo
  NullRenderAPI::getDevice() const {
    GraphicsInfo info;
    return info;
  }

  WeakSPtr<Texture>
  NullRenderAPI::getBackBuffer() const {
    return m_pBackBufferTexture;
  }

  WeakSPtr<RasterizerState>
  NullRenderAPI::getCurrentRasterizerState() const {
    return WeakSPtr<RasterizerState>();
  }

  WeakSPtr<DepthStencilState>
  NullRenderAPI::getCurrentDepthStencilState() const {
    return WeakSPtr<DepthStencilState>();
  }

  WeakSPtr<BlendState>
  NullRenderAPI::getCurrentBlendState() const {
    return WeakSPtr<BlendState>();
  }

  WeakSPtr<SamplerState>
  NullRenderAPI::getCurrentSamplerState(uint32) const {
    return WeakSPtr<SamplerState>();
  }

}
