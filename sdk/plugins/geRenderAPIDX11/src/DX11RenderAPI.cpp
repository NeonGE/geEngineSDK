/*****************************************************************************/
/**
 * @file    DX11RenderAPI.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/29
 * @brief   Graphics API Implementation with DirectX11.
 *
 * Graphics API Implementation with DirectX11.
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
#include "DX11RenderAPI.h"
#include "DXGraphicsBuffer.h"
#include "DXTranslateUtils.h"

#include <geGameConfig.h>
#include <geMath.h>
#include <geDebug.h>
#include <geFileSystem.h>
#include <geDataStream.h>
#include <d3dcompiler.h>

namespace geEngineSDK {

  using std::pair;
  using std::make_pair;

  class D3DIncludeHandler : public ID3DInclude
  {
   public:
    D3DIncludeHandler(const Vector<Path>& includeDirs)
      : m_includePaths(includeDirs)
    {}

    virtual ~D3DIncludeHandler() = default;

    //Called when an #include is encountered
    HRESULT __stdcall
    Open(D3D_INCLUDE_TYPE IncludeType,
         LPCSTR pFileName,
         LPCVOID pParentData,
         LPCVOID* ppData,
         UINT* pBytes) noexcept override {
      GE_UNREFERENCED_PARAMETER(pParentData);

      Path filePath;
      if (IncludeType & D3D_INCLUDE_LOCAL || IncludeType & D3D_INCLUDE_SYSTEM) {
        //Search for the file in the include paths
        filePath = _findFile(pFileName);
        if (filePath == Path::BLANK) {
          GE_LOG(kError,
                 RenderAPI,
                 "Failed to find {0} in include folders",
                 pFileName);
          return E_FAIL;
        }
      }
      else {
        return E_FAIL;
      }

      //Open the file and read its content into memory
      auto fileStream = FileSystem::openFile(filePath);
      if (!fileStream) {
        GE_LOG(kError,
               RenderAPI,
               "Failed to open shader include file: {0}",
               filePath.toString());
        return E_FAIL; //Failed to open the include file
      }

#if USING(GE_DEBUG_MODE)
      auto msg = StringUtil::format("Included file from include folder: {0} \n",
                                    filePath.toString().c_str());
      g_debug().log(msg, LogVerbosity::kInfo);
#endif

      auto size = fileStream->size();
      char* data = reinterpret_cast<char*>(ge_alloc<char>(size * sizeof(char)));
      fileStream->read(data, size);

      *ppData = data;
      *pBytes = static_cast<UINT>(size);

      return S_OK;
    }

    //Called when the file is no longer needed
    HRESULT __stdcall
    Close(LPCVOID pData) noexcept override {
      ge_free(const_cast<void*>(pData));
      return S_OK;
    }

   private:
    Path
    _findFile(const String& fileName) {
      //Get working directory
      static Path workingDir = FileSystem::getWorkingDirectoryPath();

      //Search all paths for the file
      for (const Path& path : m_includePaths) {
        Path fullSearchPath = path.getAbsolute(workingDir);
        fullSearchPath.append(fileName);
        if (FileSystem::isFile(fullSearchPath)) {
          return fullSearchPath;
        }
      }

      return Path::BLANK;  //File not found
    }

    Vector<Path> m_includePaths;
  };

  bool
  DX11RenderAPI::initRenderAPI(WindowHandle scrHandle, bool bFullScreen) {
    auto hWnd = reinterpret_cast<HWND>(scrHandle);
    m_bFullScreen = bFullScreen;

    Vector<IDXGIAdapter1*> vecAdapters;
    IDXGIAdapter1* pAdapter = nullptr;
    IDXGIFactory1* pFactory = nullptr;

    //Get from the configuration file the index of the adapter to use
    auto& config = GameConfig::instance();
    int32 adapterIndex = config.get<int32>("RenderAPI", "AdapterIndex", 0);

    //Enumerate all the graphic adapters
    CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory);
    while (DXGI_ERROR_NOT_FOUND !=
           pFactory->EnumAdapters1(UINT(vecAdapters.size()), &pAdapter)) {
      vecAdapters.push_back(pAdapter);
    }

    safeRelease(pFactory);

    if(vecAdapters.empty()) {
      //No adapters found, cannot initialize the render API
      GE_EXCEPT(RenderingAPIException,
                "No graphic adapters found. Cannot initialize the render API.");
      return false;
    }

    //Check that the adapter index is valid
    adapterIndex = Math::max(0, adapterIndex);
    if (adapterIndex >= static_cast<int32>(vecAdapters.size())) {
      GE_LOG(kWarning, RenderAPI, "Invalid adapter index, use the first one");
      adapterIndex = 0;
    }

    //Get the adapter description
    DXGI_ADAPTER_DESC1 aDesc1;
    vecAdapters[adapterIndex]->GetDesc1(&aDesc1);
    memcpy(&m_selectedAdapterDesc, &aDesc1, sizeof(m_selectedAdapterDesc));

    Vector<D3D_FEATURE_LEVEL> featureLevels =
    {
#if !USING(DX_VERSION_11_0)
      D3D_FEATURE_LEVEL_11_1
#else
      D3D_FEATURE_LEVEL_11_0
#endif
    };
    D3D_FEATURE_LEVEL selectedFeatureLevel;

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;

    uint32 deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if USING(GE_DEBUG_MODE)
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    //Create a device and immediate device context
    throwIfFailed(D3D11CreateDevice(vecAdapters[adapterIndex],
                                    D3D_DRIVER_TYPE_UNKNOWN,
                                    nullptr,
                                    deviceFlags,
                                    &featureLevels[0],
                                    static_cast<UINT>(featureLevels.size()),
                                    D3D11_SDK_VERSION,
                                    &device,
                                    &selectedFeatureLevel,
                                    &deviceContext));

    m_pDevice = getAs<D3DDevice>(device);

#if USING(GE_DEBUG_MODE)
    m_pDebug = getAs<ID3D11Debug>(device);
#endif
    m_pImmediateDC = getAs<D3DDeviceContext>(deviceContext);
    GE_ASSERT(m_pDevice);
    GE_ASSERT(m_pImmediateDC);

    //Create a swap chain
#if USING(DX_VERSION_11_0)
    DXGI_SWAP_CHAIN_DESC scDesc;
#else
    DXGI_SWAP_CHAIN_DESC1 scDesc;
#endif
    ge_zero_out(scDesc);

#if USING(DX_VERSION_11_0)
    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.OutputWindow = hWnd;
    scDesc.BufferDesc.RefreshRate.Numerator = 60;
    scDesc.BufferDesc.RefreshRate.Denominator = 1;
    scDesc.Windowed = !m_bFullScreen;
    auto& scDesc_Width = scDesc.BufferDesc.Width;
    auto& scDesc_Height = scDesc.BufferDesc.Height;
#else
    scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    scDesc.Stereo = false;
    scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    auto& scDesc_Width = scDesc.Width;
    auto& scDesc_Height = scDesc.Height;
#endif

#if USING(DX_VERSION_11_3)
    //scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
#endif

    scDesc.BufferCount = config.get<uint32>("RenderAPI", "BufferCount", 2);
    scDesc.SampleDesc.Count = 1;
    scDesc.SampleDesc.Quality = 0;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    if (!m_bFullScreen) {
      RECT rc;
      GetClientRect(hWnd, &rc);
      scDesc_Width = rc.right - rc.left;
      scDesc_Height = rc.bottom - rc.top;
    }
    else {
      //Get the screen resolution from the adapter description
      IDXGIOutput* pOutput = nullptr;
      if (vecAdapters[adapterIndex]->EnumOutputs(0, &pOutput) == DXGI_ERROR_NOT_FOUND) {
        //No outputs found, use the default screen resolution
        scDesc_Width = GetSystemMetrics(SM_CXSCREEN);
        scDesc_Height = GetSystemMetrics(SM_CYSCREEN);
      }
      else {
        DXGI_OUTPUT_DESC outDesc;
        pOutput->GetDesc(&outDesc);
        scDesc_Width = outDesc.DesktopCoordinates.right - outDesc.DesktopCoordinates.left;
        scDesc_Height = outDesc.DesktopCoordinates.bottom - outDesc.DesktopCoordinates.top;
        safeRelease(pOutput);
      }
    }

#if !USING(DX_VERSION_11_0)
    String scalingMode = config.get<String>("RenderAPI", "Scaling", "None");
    StringUtil::toUpperCase(scalingMode);
    if (scalingMode == "STRETCH") {
      scDesc.Scaling = DXGI_SCALING_STRETCH;
    }
    else if (scalingMode == "ASPECT") {
      scDesc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;
    }
    else {
      scDesc.Scaling = DXGI_SCALING_NONE;
    }

    String flipMode = config.get<String>("RenderAPI", "FlipMode", "None");
    StringUtil::toUpperCase(flipMode);
    if (flipMode == "SEQUENTIAL") {
      scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    }
    else {
      scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    }
#endif

    DXGIDevice* dxgiDevice = getAs<DXGIDevice>(m_pDevice);

    IDXGIAdapter* dxgiAdapter = nullptr;
    dxgiDevice->GetAdapter(&dxgiAdapter);

    DXFactory* dxgiFactory = nullptr;
    dxgiAdapter->GetParent(__uuidof(DXFactory), reinterpret_cast<void**>(&dxgiFactory));
#if USING(DX_VERSION_11_0)
    throwIfFailed(dxgiFactory->CreateSwapChain(m_pDevice, &scDesc, &m_pSwapChain));
#else
    IDXGISwapChain1* pSwapChain = nullptr;
    throwIfFailed(dxgiFactory->CreateSwapChainForHwnd(m_pDevice,
                                                      hWnd,
                                                      &scDesc,
                                                      nullptr,
                                                      nullptr,
                                                      &pSwapChain));
    m_pSwapChain = getAs<D3DSwapChain>(pSwapChain);
    safeRelease(pSwapChain);
#endif

    uint32 MaximumFrameLatency = config.get<uint32>("RenderAPI", "MaximumFrameLatency", 1);
    throwIfFailed(dxgiDevice->SetMaximumFrameLatency(MaximumFrameLatency));

    setImmediateContext();

    //Get the required interfaces for the screen and targets
    _updateBackBufferTexture();

    GRAPHICS_VIEWPORT scDesc_Viewport;
    scDesc_Viewport.x = 0.0f;
    scDesc_Viewport.y = 0.0f;
    scDesc_Viewport.width = float(m_pBackBufferTexture->getDesc().width);
    scDesc_Viewport.height = float(m_pBackBufferTexture->getDesc().height);
    scDesc_Viewport.zNear = 0.0f;
    scDesc_Viewport.zFar = 1.0f;

    setViewports({ scDesc_Viewport });

    setRenderTargets({ {m_pBackBufferTexture, 0} }, WeakSPtr<Texture>());

#if USING(GE_DEBUG_MODE)
    String objectName = "DX Main Device";
    m_pDevice->SetPrivateData(WKPDID_D3DDebugObjectName,
                              static_cast<uint32>(objectName.size()),
                              objectName.c_str());
    
    objectName = "DX Immediate Context";
    m_pImmediateDC->SetPrivateData(WKPDID_D3DDebugObjectName,
                                   static_cast<uint32>(objectName.size()),
                                   objectName.c_str());

    objectName = "DX Swap Chain";
    m_pSwapChain->SetPrivateData(WKPDID_D3DDebugObjectName,
                                 static_cast<uint32>(objectName.size()),
                                 objectName.c_str());
#endif

    //Release all the adapters
    for (auto& adapter : vecAdapters) {
      safeRelease(adapter);
    }

    //Cleanup all the temporal objects in order
    safeRelease(dxgiAdapter);
    safeRelease(dxgiFactory);
    safeRelease(deviceContext);
    safeRelease(device);
    safeRelease(dxgiDevice);

    return true;
  }

  DX11RenderAPI::~DX11RenderAPI() {
    //Cleanup all the member objects in order
    m_pBackBufferTexture = nullptr;
    safeRelease(m_pSwapChain);

    m_pActiveContext = nullptr;
    safeRelease(m_pImmediateDC);

#if USING(GE_DEBUG_MODE)
    reportLiveObjects();
    safeRelease(m_pDebug);
#endif

    safeRelease(m_pDevice);
  }

  bool
  DX11RenderAPI::resizeSwapChain(uint32 newWidth, uint32 newHeight) {
    if (!m_pSwapChain) {
      return false;
    }

    m_pBackBufferTexture->release();
    m_pImmediateDC->ClearState();

    DXGI_SWAP_CHAIN_DESC scDesc;
    m_pSwapChain->GetDesc(&scDesc);

    throwIfFailed(m_pSwapChain->ResizeBuffers(scDesc.BufferCount,
                                              newWidth,
                                              newHeight,
                                              scDesc.BufferDesc.Format,
                                              DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
    _updateBackBufferTexture();

    return true;
  }

  bool
  DX11RenderAPI::isMSAAFormatSupported(const GRAPHICS_FORMAT::E format,
                                       int32& samplesPerPixel,
                                       int32& sampleQuality) const {
    GE_ASSERT(m_pDevice);

    SPtr<DXTexture> pBackBuffer = ge_shared_ptr_new<DXTexture>();

#if USING(GE_CPP17_OR_LATER)
    static UnorderedMap<GRAPHICS_FORMAT::E, Optional<pair<int32, int32>>> s_msaaCache;
    auto it = s_msaaCache.find(format);
    if (it != s_msaaCache.end()) {
      if(it->second.has_value()) {
        samplesPerPixel = it->second->first;
        sampleQuality = it->second->second;
        return true;
      }
      return true;
    }
#endif

    samplesPerPixel = 1;
    sampleQuality = 0;
    const DXGI_FORMAT dxFormat = TranslateUtils::get(format);

    if (dxFormat == DXGI_FORMAT_UNKNOWN) {
      return false;
    }

    //Iterate to determine the maximum sample count supported for the format
    for (int32 i = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i > 0; --i) {
      uint32 quality;
      HRESULT hr = E_FAIL;
#if USING(DX_VERSION_11_0)
      hr = m_pDevice->CheckMultisampleQualityLevels(dxFormat, i, &quality);
#else
      hr = m_pDevice->CheckMultisampleQualityLevels1(dxFormat, i, 0, &quality);
#endif
      if (SUCCEEDED(hr)) {
        if (quality > 0) {
          samplesPerPixel = i;
          sampleQuality = quality - 1;
#if USING(GE_CPP17_OR_LATER)
          s_msaaCache[format] = make_pair(samplesPerPixel, sampleQuality);
#endif
          return true;
        }
      }
    }
#if USING(GE_CPP17_OR_LATER)
    s_msaaCache[format] = NullOpt;
#endif
    return false;
  }

  void
  DX11RenderAPI::msaaResolveRenderTarget(const WeakSPtr<Texture>& pSrc,
                                         const WeakSPtr<Texture>& pDst) {
    GE_ASSERT(m_pActiveContext);

    if (pDst.expired() || pSrc.expired()) {
      return;
    }

    auto pDstObj = reinterpret_cast<DXTexture*>(pDst.lock().get());
    auto pSrcObj = reinterpret_cast<DXTexture*>(pSrc.lock().get());

    auto dstFormat = TranslateUtils::get(pDstObj->getDesc().format);

    m_pActiveContext->ResolveSubresource(pDstObj->m_pTexture, 0,
                                         pSrcObj->m_pTexture, 0,
                                         dstFormat);
  }

  void
  DX11RenderAPI::reportLiveObjects(){
#if USING(GE_DEBUG_MODE)
    if (!m_pDebug) {
      return;
    }
    //Report live objects
    m_pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif
  }
  
  SPtr<Texture>
  DX11RenderAPI::createTexture(uint32 width,
                               uint32 height,
                               GRAPHICS_FORMAT::E format,
                               uint32 bindFlags,
                               uint32 mipLevels,
                               RESOURCE_USAGE::E usage,
                               uint32 cpuAccessFlags,
                               uint32 sampleCount,
                               bool isMSAA,
                               bool isCubeMap,
                               uint32 arraySize) {
    GE_ASSERT(m_pDevice);

    auto pTexture = ge_shared_ptr_new<DXTexture>();

    DXGI_FORMAT inFormat = TranslateUtils::get(format);
    DXGI_FORMAT tex_format = inFormat;
    DXGI_FORMAT srv_format = tex_format;
    DXGI_FORMAT dsv_format = tex_format;

    if (bindFlags & D3D11_BIND_DEPTH_STENCIL) {
      bindFlags |= D3D11_BIND_SHADER_RESOURCE;

      switch (inFormat)
      {
      case DXGI_FORMAT_D32_FLOAT:
        tex_format = DXGI_FORMAT_R32_TYPELESS;
        srv_format = DXGI_FORMAT_R32_FLOAT;
        dsv_format = DXGI_FORMAT_D32_FLOAT;
        break;

      case DXGI_FORMAT_D24_UNORM_S8_UINT:
        tex_format = DXGI_FORMAT_R24G8_TYPELESS;
        srv_format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        dsv_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        break;

      case DXGI_FORMAT_D16_UNORM:
        tex_format = DXGI_FORMAT_R16_TYPELESS;
        srv_format = DXGI_FORMAT_R16_UNORM;
        dsv_format = DXGI_FORMAT_D16_UNORM;
        break;

      default:
        return nullptr;
      }
    }

    uint32 realArraySize = arraySize * (isCubeMap ? 6 : 1);

    if (usage == RESOURCE_USAGE::DYNAMIC) {
      cpuAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }

    D3D11_TEXTURE2D_DESC tDesc;
    tDesc.Height = height;
    tDesc.Width = width;
    tDesc.MipLevels = mipLevels;
    tDesc.ArraySize = realArraySize;
    tDesc.Format = tex_format;
    tDesc.SampleDesc.Count = sampleCount;
    tDesc.SampleDesc.Quality = 0;
    tDesc.Usage = static_cast<D3D11_USAGE>(usage);
    tDesc.BindFlags = bindFlags;
    tDesc.CPUAccessFlags = cpuAccessFlags;
    tDesc.MiscFlags = isCubeMap ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

    bool autogenMipmaps = false;
    if (mipLevels != 1 && usage != RESOURCE_USAGE::STAGING) {
      //Check if the format supports mipmaps
      uint32 fmtSupport = 0;
      HRESULT hr = m_pDevice->CheckFormatSupport(tex_format, &fmtSupport);
      if (SUCCEEDED(hr) && (fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN)) {
        //If we want mipmaps, we REQUIRE that it is binded as a render target too
        tDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
        tDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        if (mipLevels == 0) {
          mipLevels = static_cast<uint32>(Math::log2(float(Math::max(width, height)))) + 1;
          autogenMipmaps = true;
        }
      }
    }

    throwIfFailed(m_pDevice->CreateTexture2D(&tDesc,
                                             nullptr,
                                             &pTexture->m_pTexture));

    if (bindFlags & D3D11_BIND_RENDER_TARGET && !isCubeMap) {
#if !USING(DX_VERSION_11_3) && !USING(DX_VERSION_11_4)
      D3D11_RENDER_TARGET_VIEW_DESC rDesc;
#else
      D3D11_RENDER_TARGET_VIEW_DESC1 rDesc;
#endif
      rDesc.Format = tex_format;
      rDesc.ViewDimension = (sampleCount > 1 || isMSAA) ?
        D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;

      pTexture->m_ppRTV.resize(mipLevels);
      for (uint32 i = 0; i < mipLevels; ++i) {
        rDesc.Texture2D.MipSlice = i;
#if USING(DX_VERSION_11_3) || USING(DX_VERSION_11_4)
        rDesc.Texture2D.PlaneSlice = 0;
        throwIfFailed(m_pDevice->CreateRenderTargetView1(pTexture->m_pTexture,
                                                         &rDesc,
                                                         &pTexture->m_ppRTV[i]));
#else
        throwIfFailed(m_pDevice->CreateRenderTargetView(pTexture->m_pTexture,
                                                        &rDesc,
                                                        &pTexture->m_ppRTV[i]));
#endif          
      }
    }

    if (bindFlags & D3D11_BIND_DEPTH_STENCIL && !isCubeMap) {
      D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
      ge_zero_out(descDSV);

      descDSV.Flags = 0;
      descDSV.Format = dsv_format;
      descDSV.ViewDimension = (sampleCount > 1 || isMSAA) ?
        D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
      descDSV.Texture2D.MipSlice = 0;

      throwIfFailed(m_pDevice->CreateDepthStencilView(pTexture->m_pTexture,
        &descDSV,
        &pTexture->m_pDSV));

      descDSV.Flags |= D3D11_DSV_READ_ONLY_DEPTH;
      if (DXGI_FORMAT_D24_UNORM_S8_UINT == dsv_format) {
        descDSV.Flags |= D3D11_DSV_READ_ONLY_STENCIL;
      }

      throwIfFailed(m_pDevice->CreateDepthStencilView(pTexture->m_pTexture,
        &descDSV,
        &pTexture->m_pRO_DSV));
    }

    if (bindFlags & D3D11_BIND_UNORDERED_ACCESS && !isCubeMap) {
      D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
      uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
      uavDesc.Format = tex_format;

      pTexture->m_ppUAV.resize(mipLevels);
      for (uint32 i = 0; i < mipLevels; ++i) {
        uavDesc.Texture2D.MipSlice = i;
        throwIfFailed(m_pDevice->CreateUnorderedAccessView(pTexture->m_pTexture,
                                                           &uavDesc,
                                                           &pTexture->m_ppUAV[i]));
      }
    }

    if (bindFlags & D3D11_BIND_SHADER_RESOURCE) {
      pTexture->m_ppSRV.resize(1); //mipLevels

      D3D11_SHADER_RESOURCE_VIEW_DESC sDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC();
      sDesc.Format = srv_format;

      if (isCubeMap) {
        if (arraySize > 1) {
          sDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
          sDesc.TextureCubeArray.First2DArrayFace = 0;
          sDesc.TextureCubeArray.NumCubes = arraySize;
          sDesc.TextureCubeArray.MostDetailedMip = 0;
          sDesc.TextureCubeArray.MipLevels = mipLevels;
        }
        else {
          sDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
          sDesc.TextureCube.MipLevels = mipLevels;
          sDesc.TextureCube.MostDetailedMip = 0;
          sDesc.TextureCubeArray.MipLevels = mipLevels;
        }
      }
      else {
        sDesc.ViewDimension = (sampleCount > 1 || isMSAA) ?
          D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
        sDesc.Texture2D.MostDetailedMip = 0;
        sDesc.Texture2D.MipLevels = mipLevels;
      }

      //for (uint32 i = 0; i < mipLevels; ++i)
      uint32 i = 0;
      {
        throwIfFailed(m_pDevice->CreateShaderResourceView(pTexture->m_pTexture,
                                                          &sDesc,
                                                          &pTexture->m_ppSRV[i]));
      }
    }

    if (autogenMipmaps) {
      generateMips(pTexture);
    }

    pTexture->m_desc = TranslateUtils::get(tDesc);
    pTexture->m_bIsCubeMap = isCubeMap;

    return pTexture;
  }

  SPtr<VertexDeclaration>
  DX11RenderAPI::createVertexDeclaration(const Vector<VertexElement>& elements) {
    GE_ASSERT(m_pDevice);
    if (elements.empty()) {
      GE_LOG(kError,
             RenderAPI,
             "DX11RenderAPI::createVertexDeclaration called with no elements.");
      return nullptr;
    }
    return ge_shared_ptr_new<VertexDeclaration>(elements);
  }

  SPtr<StreamOutputDeclaration>
  DX11RenderAPI::createStreamOutputDeclaration(const Vector<StreamOutputElement>& elements) {
    GE_ASSERT(m_pDevice);
    if (elements.empty()) {
      GE_LOG(kError,
             RenderAPI,
             "DX11RenderAPI::createVertexDeclaration called with no elements.");
      return nullptr;
    }
    return ge_shared_ptr_new<StreamOutputDeclaration>(elements);
  }

  SPtr<InputLayout>
  DX11RenderAPI::createInputLayout(const WeakSPtr<VertexDeclaration>& descArray,
                                   const WeakSPtr<VertexShader>& pVS) {
    GE_ASSERT(m_pDevice);

    if(descArray.expired() || pVS.expired()) {
      GE_LOG(kError,
             RenderAPI,
             "DX11RenderAPI::createInputLayout called with Invalid Parameters");
      return nullptr;
    }
    
    auto inputLayout = ge_shared_ptr_new<DXInputLayout>();

    auto pDesc = descArray.lock();
    auto vs = reinterpret_cast<DXShader*>(pVS.lock().get());

    //Translate the vertex elements to D3D11_INPUT_ELEMENT_DESC
    auto& declElements = pDesc->getProperties().getElements();
    Vector<D3D11_INPUT_ELEMENT_DESC> dxDescArray;

    int32 maxStreamIndex = -1;
    for(auto& elem : declElements) {
      dxDescArray.emplace_back();
      D3D11_INPUT_ELEMENT_DESC& desc = dxDescArray.back();

      desc.SemanticName = TranslateUtils::toString(elem.getSemantic());
      desc.SemanticIndex = elem.getSemanticIndex();
      desc.Format = TranslateUtils::get(elem.getType());
      desc.InputSlot = elem.getStreamIndex();
      desc.AlignedByteOffset = static_cast<WORD>(elem.getOffset());

      if (elem.getInstanceStepRate() == 0) {
        desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        desc.InstanceDataStepRate = 0;
      }
      else {
        desc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
        desc.InstanceDataStepRate = elem.getInstanceStepRate();
      }

      maxStreamIndex = Math::max(maxStreamIndex, int32(elem.getStreamIndex()));
    }

    HRESULT hr = m_pDevice->CreateInputLayout(&dxDescArray[0],
                                              static_cast<uint32>(dxDescArray.size()),
                                              vs->m_pBlob->GetBufferPointer(),
                                              vs->m_pBlob->GetBufferSize(),
                                              &inputLayout->m_inputLayout);
    if (FAILED(hr)) {
      GE_LOG(kError,
             RenderAPI,
             "Failed to create Input Layout.");
      return nullptr;
    }
    
    //Should we set the vertex declaration here?
    inputLayout->m_vertexDeclaration = pDesc;

    return inputLayout;
  }

  SPtr<InputLayout>
  DX11RenderAPI::createInputLayoutFromShader(const WeakSPtr<VertexShader>& pVS) {
    GE_ASSERT(m_pDevice);

    if (pVS.expired()) {
      GE_LOG(kError,
             RenderAPI,
             "Vertex Shader is expired.");
      return nullptr;
    }

    //Use the reflection API to get the input layout
    ID3D11ShaderReflection* pReflector = nullptr;
    {//This scope is to unlock the Vertex Shader after reflection
      auto pDXVS = reinterpret_cast<DXShader*>(pVS.lock().get());
      throwIfFailed(D3DReflect(pDXVS->m_pBlob->GetBufferPointer(),
                               pDXVS->m_pBlob->GetBufferSize(),
                               __uuidof(ID3D11ShaderReflection),
                               reinterpret_cast<void**>(&pReflector)));

    }

    D3D11_SHADER_DESC shaderDesc;
    pReflector->GetDesc(&shaderDesc);

    Vector<D3D11_INPUT_ELEMENT_DESC> descArray;
    descArray.reserve(shaderDesc.InputParameters);
    
    Vector<VertexElement> vertexElements;
    vertexElements.reserve(shaderDesc.InputParameters);

    //Loop through all the input parameters and create the input element descriptors
    uint32 offset = 0;
    for (uint32 i = 0; i < shaderDesc.InputParameters; ++i) {
      D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
      throwIfFailed(pReflector->GetInputParameterDesc(i, &paramDesc));

      //We ignore the system value semantics, as they are not used in the vertex shader
      if (StringUtil::startsWith(String(paramDesc.SemanticName), "sv_")) {
        continue;
      }

      vertexElements.emplace_back(paramDesc.Stream,
                                  offset,
                                  TranslateUtils::getInputType(paramDesc.ComponentType,
                                    paramDesc.Mask),
                                  TranslateUtils::get(paramDesc.SemanticName),
                                  paramDesc.SemanticIndex);

      offset += vertexElements.back().getSize();
    }

    safeRelease(pReflector);

    //Create the vertex declaration from the elements
    auto pVertexDecl = createVertexDeclaration(vertexElements);
    
    return createInputLayout(pVertexDecl, pVS);
  }

  /*************************************************************************/
  // Create Buffers
  /*************************************************************************/
  void
  DX11RenderAPI::_createBuffer(uint32 bindFlags,
                               SIZE_T sizeInBytes,
                               const void* pInitialData,
                               const uint32 usage,
                               const uint32 byteStride,
                               ID3D11Buffer** outBuffer,
                               D3D11_BUFFER_DESC& outDesc) const {
    GE_ASSERT(m_pDevice && outBuffer && sizeInBytes > 0 && bindFlags != 0);
    
    ge_zero_out(outDesc);
    outDesc.Usage = static_cast<D3D11_USAGE>(usage);
    outDesc.ByteWidth = static_cast<UINT>(sizeInBytes);
    outDesc.BindFlags = bindFlags;
    outDesc.CPUAccessFlags = usage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;
    outDesc.MiscFlags = 0;
    outDesc.StructureByteStride = byteStride;

    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = pInitialData;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    throwIfFailed(m_pDevice->CreateBuffer(&outDesc,
                                          pInitialData ? &InitData : nullptr,
                                          outBuffer));
  }

  SPtr<VertexBuffer>
  DX11RenderAPI::createVertexBuffer(const SPtr<VertexDeclaration>& pDecl,
                                    const SIZE_T sizeInBytes,
                                    const void* pInitialData,
                                    const uint32 usage) {
    GE_ASSERT(m_pDevice);
    auto pVB = ge_shared_ptr_new<DXVertexBuffer>();

    _createBuffer(D3D11_BIND_VERTEX_BUFFER,
                  sizeInBytes,
                  pInitialData,
                  usage,
                  pDecl->getProperties().getVertexSize(0),
                  &pVB->m_pBuffer,
                  pVB->m_Desc);

    pVB->m_pVertexDeclaration = pDecl;

    return pVB;
  }

  SPtr<StreamOutputBuffer>
  DX11RenderAPI::createStreamOutputBuffer(const SPtr<StreamOutputDeclaration>& pDecl,
                                          const SIZE_T sizeInBytes,
                                          const uint32 usage) {
    GE_ASSERT(m_pDevice);
    auto pSOB = ge_shared_ptr_new<DXStreamOutputBuffer>();

    auto& soProps = pDecl->getProperties();
    uint32 byteStride = soProps.getComponentCountForOutputSlot(0) * sizeof(float);

    _createBuffer(D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_VERTEX_BUFFER,
                  sizeInBytes,
                  nullptr, //Stream output buffers are not initialized
                  usage,
                  byteStride,
                  &pSOB->m_pBuffer,
                  pSOB->m_Desc);

    pSOB->m_pStreamOutputDeclaration = pDecl;

    return pSOB;
  }

  SPtr<IndexBuffer>
  DX11RenderAPI::createIndexBuffer(const SIZE_T sizeInBytes,
                                   const void* pInitialData,
                                   const INDEX_BUFFER_FORMAT::E format,
                                   const uint32 usage) {
    GE_ASSERT(m_pDevice);
    auto pIB = ge_shared_ptr_new<DXIndexBuffer>();

    _createBuffer(D3D11_BIND_INDEX_BUFFER,
                  sizeInBytes,
                  pInitialData,
                  usage,
                  format == INDEX_BUFFER_FORMAT::R32_UINT ? sizeof(uint32) : sizeof(uint16),
                  &pIB->m_pBuffer,
                  pIB->m_Desc);

    if(format == INDEX_BUFFER_FORMAT::R32_UINT) {
      pIB->m_indexFormat = GRAPHICS_FORMAT::kR32_UINT;
    }
    else {
      pIB->m_indexFormat = GRAPHICS_FORMAT::kR16_UINT;
    }

    return pIB;
  }

  SPtr<ConstantBuffer>
  DX11RenderAPI::createConstantBuffer(const SIZE_T sizeInBytes,
                                      const void* pInitialData,
                                      const uint32 usage) {
    GE_ASSERT(m_pDevice);
    auto pCB = ge_shared_ptr_new<DXConstantBuffer>();

    _createBuffer(D3D11_BIND_CONSTANT_BUFFER,
                  sizeInBytes,
                  pInitialData,
                  usage,
                  0, //Constant buffers don't have a byte stride
                  &pCB->m_pBuffer,
                  pCB->m_Desc);

    return pCB;
  }

  SPtr<RasterizerState>
  DX11RenderAPI::createRasterizerState(const RASTERIZER_DESC& rasterDesc) {
    GE_ASSERT(m_pDevice);

    auto pRS = ge_shared_ptr_new<DXRasterizerState>();

#if USING(DX_VERSION_11_0)
    D3D11_RASTERIZER_DESC desc;
#elif USING(DX_VERSION_11_1) || USING(DX_VERSION_11_2)
    D3D11_RASTERIZER_DESC1 desc;
#else
    D3D11_RASTERIZER_DESC2 desc;
#endif

    ge_zero_out(desc);  //This is important to avoid uninitialized values
    memcpy(&desc, &rasterDesc, sizeof(desc));

#if USING(DX_VERSION_11_0)
    throwIfFailed(m_pDevice->CreateRasterizerState(&desc, &pRS->m_pRasterizerState));
#elif USING(DX_VERSION_11_1) || USING(DX_VERSION_11_2)
    throwIfFailed(m_pDevice->CreateRasterizerState1(&desc, &pRS->m_pRasterizerState));
#else
    throwIfFailed(m_pDevice->CreateRasterizerState2(&desc, &pRS->m_pRasterizerState));
#endif

    return pRS;
  }

  SPtr<DepthStencilState>
  DX11RenderAPI::createDepthStencilState(const DEPTH_STENCIL_DESC& depthStencilDesc) {
    GE_ASSERT(m_pDevice);

    auto pDSS = ge_shared_ptr_new<DXDepthStencilState>();
    D3D11_DEPTH_STENCIL_DESC desc;
    memcpy(&desc, &depthStencilDesc, sizeof(desc));

    throwIfFailed(m_pDevice->CreateDepthStencilState(&desc, &pDSS->m_pDepthStencilState));
    return pDSS;
  }

  SPtr<BlendState>
  DX11RenderAPI::createBlendState(const BLEND_DESC& blendDesc,
                                  const Vector4 blendFactors,
                                  const uint32 sampleMask) {
    GE_ASSERT(m_pDevice);

    auto pBS = ge_shared_ptr_new<DXBlendState>();
    
#if USING(DX_VERSION_11_0)
    D3D11_BLEND_DESC desc;
    ge_zero_out(desc);  //This is important to avoid uninitialized values

    //We need to copy the blendDesc manually because it's defined like D3D11_BLEND_DESC1
    desc.AlphaToCoverageEnable = blendDesc.alphaToCoverageEnable;
    desc.IndependentBlendEnable = blendDesc.independentBlendEnable;
    for(uint32 i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
      auto& rtIn = blendDesc.renderTarget[i];
      auto& rtOut = desc.RenderTarget[i];

      rtOut.BlendEnable = rtIn.blendEnable;
      rtOut.SrcBlend = static_cast<D3D11_BLEND>(rtIn.srcBlend);
      rtOut.DestBlend = static_cast<D3D11_BLEND>(rtIn.destBlend);
      rtOut.BlendOp = static_cast<D3D11_BLEND_OP>(rtIn.blendOp);
      rtOut.SrcBlendAlpha = static_cast<D3D11_BLEND>(rtIn.srcBlendAlpha);
      rtOut.DestBlendAlpha = static_cast<D3D11_BLEND>(rtIn.destBlendAlpha);
      rtOut.BlendOpAlpha = static_cast<D3D11_BLEND_OP>(rtIn.blendOpAlpha);
      rtOut.RenderTargetWriteMask = rtIn.renderTargetWriteMask;
    }

    throwIfFailed(m_pDevice->CreateBlendState(&desc, &pBS->m_pBlendState));
#else
    D3D11_BLEND_DESC1 desc;
    memcpy(&desc, &blendDesc, sizeof(desc));
    throwIfFailed(m_pDevice->CreateBlendState1(&desc, &pBS->m_pBlendState));
#endif
    pBS->m_blendFactors = blendFactors;
    pBS->m_sampleMask = sampleMask;

    return pBS;
  }

  SPtr<SamplerState>
  DX11RenderAPI::createSamplerState(const SAMPLER_DESC& samplerDesc) {
    GE_ASSERT(m_pDevice);
    auto pSS = ge_shared_ptr_new<DXSamplerState>();

    D3D11_SAMPLER_DESC desc;
    memcpy(&desc, &samplerDesc, sizeof(desc));

    throwIfFailed(m_pDevice->CreateSamplerState(&desc, &pSS->m_pSampler));

    return pSS;
  }

  /*************************************************************************/
  // Create Shaders
  /*************************************************************************/
  bool
  _compileFromFile(const Path& fileName,
                   const Vector<ShaderMacro>& pMacros,
                   const String szEntryPoint,
                   const String szShaderModel,
                   ID3DBlob** pBlob) {
    HRESULT hr = S_OK;
    int32 dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS ;
#if USING(GE_DEBUG_MODE)
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    static D3DIncludeHandler includeHandler({
      "Data/Engine/Shaders/",
      "Data/Shaders/"
    });

    //Add shader preprocessor definitions
    Vector<D3D_SHADER_MACRO> defines;
    defines.reserve(pMacros.size() + 1);
    for (const auto& macro : pMacros)
    {
      D3D_SHADER_MACRO def;
      def.Name = macro.name.c_str();
      def.Definition = macro.definition.c_str();
      defines.push_back(def);
    }

    defines.push_back({ nullptr, nullptr });

    ID3DBlob* pErrorBlob = nullptr;
    WString file = fileName.toPlatformString();
    hr = D3DCompileFromFile(file.c_str(),
                            defines.data(),
                            &includeHandler,
                            szEntryPoint.c_str(),
                            szShaderModel.c_str(),
                            dwShaderFlags,
                            0,
                            pBlob,
                            &pErrorBlob);

    if (FAILED(hr)) {
      if (nullptr != pErrorBlob) {
        GE_LOG(kError,
               RenderAPI,
               reinterpret_cast<char*>(pErrorBlob->GetBufferPointer()));
        safeRelease(pErrorBlob);
      }

      return false;
    }

    safeRelease(pErrorBlob);

    return true;
  }

  SPtr<VertexShader>
  DX11RenderAPI::createVertexShader(CREATE_SHADER_PARAMS) {
    GE_ASSERT(m_pDevice);
    auto vShader = ge_shared_ptr_new<DXShader>();

    if (!_compileFromFile(fileName, pMacro, szEntryPoint, szShaderModel, &vShader->m_pBlob)) {
      GE_LOG(kError,
             RenderAPI,
             "Could not compile VertexShader Shader from {1}", fileName);
      return nullptr;
    }

    HRESULT hr = m_pDevice->CreateVertexShader(vShader->m_pBlob->GetBufferPointer(),
                              vShader->m_pBlob->GetBufferSize(),
                              nullptr,
                              reinterpret_cast<ID3D11VertexShader**>(&vShader->m_pShader));
    if (FAILED(hr)) {
      GE_LOG(kError,
             RenderAPI,
             "Failed to create VertexShader Shader '{1}' from '{2}'",
             szEntryPoint,
             fileName);
      return nullptr;
    }

    return vShader;
  }

  SPtr<PixelShader>
  DX11RenderAPI::createPixelShader(CREATE_SHADER_PARAMS) {
    GE_ASSERT(m_pDevice);
    auto vShader = ge_shared_ptr_new<DXShader>();

    if (!_compileFromFile(fileName, pMacro, szEntryPoint, szShaderModel, &vShader->m_pBlob)) {
      GE_LOG(kError,
             RenderAPI,
             "Could not compile PixelShader Shader from {1}", fileName);
      return nullptr;
    }

    HRESULT hr = m_pDevice->CreatePixelShader(vShader->m_pBlob->GetBufferPointer(),
                              vShader->m_pBlob->GetBufferSize(),
                              nullptr,
                              reinterpret_cast<ID3D11PixelShader**>(&vShader->m_pShader));
    if (FAILED(hr)) {
      GE_LOG(kError,
             RenderAPI,
             "Failed to create PixelShader Shader '{1}' from '{2}'",
             szEntryPoint,
             fileName);
      return nullptr;
    }

    return vShader;
  }

  SPtr<GeometryShader>
  DX11RenderAPI::createGeometryShader(CREATE_SHADER_PARAMS) {
    GE_ASSERT(m_pDevice);
    auto vShader = ge_shared_ptr_new<DXShader>();

    if (!_compileFromFile(fileName, pMacro, szEntryPoint, szShaderModel, &vShader->m_pBlob)) {
      GE_LOG(kError,
             RenderAPI,
             "Could not compile GeometryShader Shader from {1}", fileName);
      return nullptr;
    }

    HRESULT hr = m_pDevice->CreateGeometryShader(vShader->m_pBlob->GetBufferPointer(),
                              vShader->m_pBlob->GetBufferSize(),
                              nullptr,
                              reinterpret_cast<ID3D11GeometryShader**>(&vShader->m_pShader));
    if (FAILED(hr)) {
      GE_LOG(kError,
             RenderAPI,
             "Failed to create GeometryShader Shader '{1}' from '{2}'",
             szEntryPoint,
             fileName);
      return nullptr;
    }

    return vShader;
  }

  SPtr<GeometryShader>
  DX11RenderAPI::createGeometryShaderWithStreamOutput(CREATE_SHADER_PARAMS,
                   const SPtr<StreamOutputDeclaration>& pDecl) {
    GE_ASSERT(m_pDevice);

    //First we create the Stream Output Descriptor
    Vector<D3D11_SO_DECLARATION_ENTRY> pDeclArray;    
    const auto& elements = pDecl->getProperties().getElements();
    pDeclArray.resize(elements.size());

    for (uint32 i = 0; i < elements.size(); ++i) {
      D3D11_SO_DECLARATION_ENTRY& entry = pDeclArray[i];
      auto& elem = elements[i];

      entry.Stream = elem.getStreamIndex();
      entry.SemanticName = TranslateUtils::toString(elem.getSemantic());
      entry.SemanticIndex = elem.getSemanticIndex();
      entry.StartComponent = elem.getComponentStart();
      entry.ComponentCount = elem.getComponentCount();
      entry.OutputSlot = 0; //We only support one output slot
    }

    auto vShader = ge_shared_ptr_new<DXShader>();

    if (!_compileFromFile(fileName, pMacro, szEntryPoint, szShaderModel, &vShader->m_pBlob)) {
      GE_LOG(kError,
        RenderAPI,
        "Could not compile GeometryShader Shader from {1}", fileName);
      return nullptr;
    }

    UINT stride = pDecl->getProperties().getComponentCountForOutputSlot(0) * sizeof(float);

    HRESULT hr = m_pDevice->CreateGeometryShaderWithStreamOutput(
                              vShader->m_pBlob->GetBufferPointer(),
                              vShader->m_pBlob->GetBufferSize(),
                              pDeclArray.data(),
                              static_cast<UINT>(pDeclArray.size()),
                              &stride,
                              1, //We don't use the buffer strides
                              D3D11_SO_NO_RASTERIZED_STREAM,
                              nullptr,
                   reinterpret_cast<ID3D11GeometryShader**>(&vShader->m_pShader));
    if (FAILED(hr)) {
      GE_LOG(kError,
             RenderAPI,
             "Failed CreateGeometryShaderWithStreamOutput '{1}' from '{2}'",
             szEntryPoint,
             fileName);
      return nullptr;
    }

    return vShader;

  }

  SPtr<HullShader>
  DX11RenderAPI::createHullShader(CREATE_SHADER_PARAMS) {
    GE_ASSERT(m_pDevice);
    auto vShader = ge_shared_ptr_new<DXShader>();

    if (!_compileFromFile(fileName, pMacro, szEntryPoint, szShaderModel, &vShader->m_pBlob)) {
      GE_LOG(kError,
             RenderAPI,
             "Could not compile HullShader Shader from {1}", fileName);
      return nullptr;
    }

    HRESULT hr = m_pDevice->CreateHullShader(vShader->m_pBlob->GetBufferPointer(),
                              vShader->m_pBlob->GetBufferSize(),
                              nullptr,
                              reinterpret_cast<ID3D11HullShader**>(&vShader->m_pShader));
    if (FAILED(hr)) {
      GE_LOG(kError,
             RenderAPI,
             "Failed to create GeometryShader Shader '{1}' from '{2}'",
             szEntryPoint,
             fileName);
      return nullptr;
    }

    return vShader;
  }

  SPtr<DomainShader>
  DX11RenderAPI::createDomainShader(CREATE_SHADER_PARAMS) {
    GE_ASSERT(m_pDevice);
    auto vShader = ge_shared_ptr_new<DXShader>();

    if (!_compileFromFile(fileName, pMacro, szEntryPoint, szShaderModel, &vShader->m_pBlob)) {
      GE_LOG(kError,
             RenderAPI,
             "Could not compile DomainShader Shader from {1}", fileName);
      return nullptr;
    }

    HRESULT hr = m_pDevice->CreateDomainShader(vShader->m_pBlob->GetBufferPointer(),
                              vShader->m_pBlob->GetBufferSize(),
                              nullptr,
                              reinterpret_cast<ID3D11DomainShader**>(&vShader->m_pShader));
    if (FAILED(hr)) {
      GE_LOG(kError,
             RenderAPI,
             "Failed to create DomainShader Shader '{1}' from '{2}'",
             szEntryPoint,
             fileName);
      return nullptr;
    }

    return vShader;
  }

  SPtr<ComputeShader>
  DX11RenderAPI::createComputeShader(CREATE_SHADER_PARAMS) {
    GE_ASSERT(m_pDevice);
    auto vShader = ge_shared_ptr_new<DXShader>();

    if (!_compileFromFile(fileName, pMacro, szEntryPoint, szShaderModel, &vShader->m_pBlob)) {
      GE_LOG(kError,
             RenderAPI,
             "Could not compile ComputeShader Shader from {1}", fileName);
      return nullptr;
    }

    HRESULT hr = m_pDevice->CreateComputeShader(vShader->m_pBlob->GetBufferPointer(),
                              vShader->m_pBlob->GetBufferSize(),
                              nullptr,
                              reinterpret_cast<ID3D11ComputeShader**>(&vShader->m_pShader));
    if (FAILED(hr)) {
      GE_LOG(kError,
             RenderAPI,
             "Failed to create ComputeShader Shader '{1}' from '{2}'",
             szEntryPoint,
             fileName);
      return nullptr;
    }

    return vShader;
  }

  /*************************************************************************/
  // Write Functions
  /*************************************************************************/

  void
  DX11RenderAPI::writeToResource(const WeakSPtr<GraphicsResource>& pResource,
                                 uint32 dstSubRes,
                                 const GRAPHICS_BOX* pDstBox,
                                 const void* pSrcData,
                                 uint32 srcRowPitch,
                                 uint32 srcDepthPitch,
                                 uint32 copyFlags) {
    GE_ASSERT(m_pActiveContext);

    if (pResource.expired()) {
      return;
    }

    auto pTex = pResource.lock();
    auto pGraphRes =
      reinterpret_cast<ID3D11Resource*>(pTex->_getGraphicsResource());
    GE_ASSERT(pGraphRes);

#if USING(DX_VERSION_11_0)
    GE_UNREFERENCED_PARAMETER(copyFlags);
    m_pActiveContext->UpdateSubresource(pGraphRes,
                                        dstSubRes,
                                        pDstBox ? nullptr :
                                        reinterpret_cast<const D3D11_BOX*>(pDstBox),
                                        pSrcData,
                                        srcRowPitch,
                                        srcDepthPitch);
#else
    m_pActiveContext->UpdateSubresource1(pGraphRes,
                                         dstSubRes,
                                         pDstBox ? nullptr :
                                         reinterpret_cast<const D3D11_BOX*>(pDstBox),
                                         pSrcData,
                                         srcRowPitch,
                                         srcDepthPitch,
                                         copyFlags);
#endif
  }

  MappedSubresource
  DX11RenderAPI::mapToRead(const WeakSPtr<GraphicsResource>& pResource,
                           uint32 subResource,
                           uint32 mapFlags) {
    GE_ASSERT(m_pActiveContext);

    MappedSubresource mappedSubresource;
    mappedSubresource.pData = nullptr;

    if (pResource.expired()) {
      return mappedSubresource;
    }

    auto pTex = pResource.lock();
    auto pGraphRes =
      reinterpret_cast<ID3D11Resource*>(pTex->_getGraphicsResource());
    GE_ASSERT(pGraphRes);

    ge_zero_out(mappedSubresource);
    throwIfFailed(m_pActiveContext->Map(pGraphRes,
                          subResource,
                          D3D11_MAP_READ,
                          mapFlags,
                          reinterpret_cast<D3D11_MAPPED_SUBRESOURCE*>(&mappedSubresource)));
    if (mappedSubresource.pData == nullptr) {
      GE_LOG(kError, RenderAPI, "Failed to map texture.");
    }

    return mappedSubresource;
  }

  void
  DX11RenderAPI::unmap(const WeakSPtr<GraphicsResource>& pResource,
                       uint32 subResource) {
    GE_ASSERT(m_pActiveContext);
    if (pResource.expired()) {
      return;
    }

    auto pTex = pResource.lock();
    auto pGraphRes =
      reinterpret_cast<ID3D11Resource*>(pTex->_getGraphicsResource());
    GE_ASSERT(pGraphRes);

    m_pActiveContext->Unmap(pGraphRes, subResource);
  }

  void
  DX11RenderAPI::copyResource(const WeakSPtr<GraphicsResource>& pSrcObj,
                              const WeakSPtr<GraphicsResource>& pDstObj) {
    GE_ASSERT(m_pActiveContext);

    if (pSrcObj.expired() || pDstObj.expired()) {
      return;
    }

    auto pSrc = pSrcObj.lock();
    auto pDst = pDstObj.lock();

    auto pSrcDst = reinterpret_cast<ID3D11Resource*>(pSrc->_getGraphicsResource());
    auto pResDst = reinterpret_cast<ID3D11Resource*>(pDst->_getGraphicsResource());

    GE_ASSERT(pSrcDst && pResDst);

    //Copy the resource to the destination
    m_pActiveContext->CopyResource(pResDst, pSrcDst);
  }

  void
  DX11RenderAPI::generateMips(const WeakSPtr<Texture>& pTexture) {
    GE_ASSERT(m_pActiveContext);

    if (pTexture.expired()) {
      return;
    }

    auto pObj = pTexture.lock();
    auto pDXObj = reinterpret_cast<DXTexture*>(pObj.get());

    m_pActiveContext->GenerateMips(pDXObj->m_ppSRV[0]);
  }

  void
  DX11RenderAPI::clearRenderTarget(const WeakSPtr<Texture>& pRenderTarget,
                                   const LinearColor& color) {
    GE_ASSERT(m_pActiveContext);

    if (pRenderTarget.expired()) {
      return;
    }

    auto pObj = pRenderTarget.lock();
    auto pDXObj = reinterpret_cast<DXTexture*>(pObj.get());
    GE_ASSERT(!pDXObj->m_ppRTV.empty());

    ID3D11RenderTargetView* pTarget = pDXObj->m_ppRTV[0];
    m_pActiveContext->ClearRenderTargetView(pTarget, reinterpret_cast<const FLOAT*>(&color));
  }

  void
  DX11RenderAPI::clearDepthStencil(const WeakSPtr<Texture>& pDepthStencilView,
                                   uint32 flags,
                                   float depthVal,
                                   uint8 stencilVal) {
    GE_ASSERT(m_pActiveContext);

    if (pDepthStencilView.expired()) {
      return;
    }

    auto pObj = pDepthStencilView.lock();
    auto pDXObj = reinterpret_cast<DXTexture*>(pObj.get());
    GE_ASSERT(pDXObj->m_pDSV);

    m_pActiveContext->ClearDepthStencilView(pDXObj->m_pDSV, flags, depthVal, stencilVal);
  }

  void
  DX11RenderAPI::discardView(WeakSPtr<Texture> pTexture) {
#if USING(DX_VERSION_11_0)
    GE_UNREFERENCED_PARAMETER(pTexture);
    return;
#else
    GE_ASSERT(m_pActiveContext);

    if (pTexture.expired()) {
      return;
    }

    auto pObj = pTexture.lock();
    auto pDXObj = reinterpret_cast<DXTexture*>(pObj.get());

    ID3D11View* pView = nullptr;
    if (!pDXObj->m_ppRTV.empty()) {
      pView = pDXObj->m_ppRTV[0];
    }
    else if (pDXObj->m_pDSV) {
      pView = pDXObj->m_pDSV;
    }
    else {
      GE_ASSERT(false && "Texture has no RTV or DSV to discard.");
      return;
    }

    m_pActiveContext->DiscardView1(pView, nullptr, 0);
#endif
  }

  void
  DX11RenderAPI::present() {
    GE_ASSERT(m_pSwapChain && m_pActiveContext && m_pBackBufferTexture);

#if USING(DX_VERSION_11_0)
    m_pSwapChain->Present(1, 0);
#else
    static DXGI_PRESENT_PARAMETERS presentParams = {};
    m_pSwapChain->Present1(1, 0, &presentParams);
#endif
  }

  void
  DX11RenderAPI::setImmediateContext() {
    m_pActiveContext = m_pImmediateDC;
  }

  void
  DX11RenderAPI::setTopology(PRIMITIVE_TOPOLOGY::E topologyType) {
    GE_ASSERT(m_pActiveContext);
    m_pActiveContext->IASetPrimitiveTopology(
      static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topologyType));
  }

  void
  DX11RenderAPI::setViewports(const Vector<GRAPHICS_VIEWPORT>& viewports) {
    GE_ASSERT(m_pActiveContext);
    
    uint32 numViewports = static_cast<uint32>(viewports.size());
    GE_ASSERT(numViewports > 0 &&
              numViewports <= D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE);

    //This conteiner is used to store the converted viewports
    static Vector<D3D11_VIEWPORT>
      dxViewports(D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE);

    memcpy(&dxViewports[0], viewports.data(), sizeof(D3D11_VIEWPORT) * numViewports);
    m_pActiveContext->RSSetViewports(numViewports, &dxViewports[0]);
  }

  void
  DX11RenderAPI::setInputLayout(const WeakSPtr<InputLayout>& pInputLayout) {
    GE_ASSERT(m_pActiveContext);

    ID3D11InputLayout* pLayout = nullptr;
    if (!pInputLayout.expired()) {
      auto pObj = reinterpret_cast<DXInputLayout*>(pInputLayout.lock().get());
      pLayout = pObj->m_inputLayout;
    }

    m_pActiveContext->IASetInputLayout(pLayout);
  }

  void
  DX11RenderAPI::setRasterizerState(const WeakSPtr<RasterizerState>& pRasterizerState) {
    GE_ASSERT(m_pActiveContext);

    D3DRasterizerState* pRS = nullptr;
    if (!pRasterizerState.expired()) {
      auto pRSState = reinterpret_cast<DXRasterizerState*>(pRasterizerState.lock().get());
      pRS = pRSState->m_pRasterizerState;
    }

    m_pActiveContext->RSSetState(pRS);
  }

  void
  DX11RenderAPI::setDepthStencilState(const WeakSPtr<DepthStencilState>& pDepthStencilState,
                                      uint32 stencilRef) {
    GE_ASSERT(m_pActiveContext);

    ID3D11DepthStencilState* pDSS = nullptr;
    if (!pDepthStencilState.expired()) {
      auto pDSSState = reinterpret_cast<DXDepthStencilState*>(pDepthStencilState.lock().get());
      pDSS = pDSSState->m_pDepthStencilState;
    }

    m_pActiveContext->OMSetDepthStencilState(pDSS, stencilRef);
  }

  void
  DX11RenderAPI::setBlendState(const WeakSPtr<BlendState>& pBlendState) {
    GE_ASSERT(m_pActiveContext);

    ID3D11BlendState1* pBS = nullptr;
    Vector4 blendFactors(geEngineSDK::FORCE_INIT::kForceInitToZero);
    uint32 sampleMask = 0xffffffff;

    if (!pBlendState.expired()) {
      auto pBlend = reinterpret_cast<DXBlendState*>(pBlendState.lock().get());
      pBS = pBlend->m_pBlendState;
      blendFactors = pBlend->m_blendFactors;
      sampleMask = pBlend->m_sampleMask;
    }

    m_pActiveContext->OMSetBlendState(pBS, &blendFactors[0], sampleMask);
  }

  void
  DX11RenderAPI::setVertexBuffer(const WeakSPtr<VertexBuffer>& pVertexBuffer,
                                 uint32 startSlot,
                                 uint32 offset) {
    GE_ASSERT(m_pActiveContext);

    ID3D11Buffer* pBuffer = nullptr;
    UINT stride = 0;
    UINT offsetInBytes = offset;

    if (!pVertexBuffer.expired()) {
      auto pVB = reinterpret_cast<DXVertexBuffer*>(pVertexBuffer.lock().get());
      pBuffer = pVB->m_pBuffer;
      stride = pVB->m_pVertexDeclaration->getProperties().getVertexSize(0);
    }

    m_pActiveContext->IASetVertexBuffers(startSlot, 1, &pBuffer, &stride, &offsetInBytes);
  }

  void
  DX11RenderAPI::setIndexBuffer(const WeakSPtr<IndexBuffer>& pIndexBuffer,
                                  uint32 offset) {
    GE_ASSERT(m_pActiveContext);

    ID3D11Buffer* pBuffer = nullptr;
    DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;
    UINT offsetInBytes = offset;

    if (!pIndexBuffer.expired()) {
      auto pIB = reinterpret_cast<DXIndexBuffer*>(pIndexBuffer.lock().get());
      pBuffer = pIB->m_pBuffer;
      format = static_cast<DXGI_FORMAT>(pIB->m_indexFormat);
    }

    m_pActiveContext->IASetIndexBuffer(pBuffer, format, offsetInBytes);
  }

  /*************************************************************************/
  // Set Shaders
  /*************************************************************************/
  template<DX11RenderAPI::ShaderStage Stage, typename TShader>
  void
  DX11RenderAPI::_setProgram(const WeakSPtr<TShader>& pInShader) {
    GE_ASSERT(m_pActiveContext);

    using Traits = ShaderTraits<Stage>;
    typename Traits::ShaderInterface* pShader = nullptr;

    if (!pInShader.expired()) {
      auto pObj = reinterpret_cast<DXShader*>(pInShader.lock().get());
      pShader = reinterpret_cast<typename Traits::ShaderInterface*>(pObj->m_pShader);
    }

    (m_pActiveContext->*Traits::SetProgramFn)(pShader, nullptr, 0);
  }

  void
  DX11RenderAPI::vsSetProgram(const WeakSPtr<VertexShader>& pInShader) {
    _setProgram<ShaderStage::Vertex>(pInShader);
  }

  void
  DX11RenderAPI::psSetProgram(const WeakSPtr<PixelShader>& pInShader) {
    _setProgram<ShaderStage::Pixel>(pInShader);
  }

  void
  DX11RenderAPI::gsSetProgram(const WeakSPtr<GeometryShader>& pInShader) {
    _setProgram<ShaderStage::Geometry>(pInShader);
  }

  void
  DX11RenderAPI::hsSetProgram(const WeakSPtr<HullShader>& pInShader) {
    _setProgram<ShaderStage::Hull>(pInShader);
  }

  void
  DX11RenderAPI::dsSetProgram(const WeakSPtr<DomainShader>& pInShader) {
    _setProgram<ShaderStage::Domain>(pInShader);
  }

  void
  DX11RenderAPI::csSetProgram(const WeakSPtr<ComputeShader>& pInShader) {
    _setProgram<ShaderStage::Compute>(pInShader);
  }

  template<DX11RenderAPI::ShaderStage Stage>
  void DX11RenderAPI::_setShaderResource(const WeakSPtr<Texture>& pTexture,
                                         const uint32 startSlot) {
    GE_ASSERT(m_pActiveContext);

    ID3D11ShaderResourceView* pSRV = nullptr;
    if (!pTexture.expired()) {
      auto pTx = reinterpret_cast<DXTexture*>(pTexture.lock().get());
      pSRV = pTx->m_ppSRV[0];
    }

    (m_pActiveContext->*ShaderTraits<Stage>::SetSRVFn)(startSlot, 1, &pSRV);
  }

  /*************************************************************************/
  // Set Shaders Resources
  /*************************************************************************/
  void
  DX11RenderAPI::vsSetShaderResource(const WeakSPtr<Texture>& pTexture,
                                     const uint32 startSlot) {
    _setShaderResource<ShaderStage::Vertex>(pTexture, startSlot);
  }

  void
  DX11RenderAPI::psSetShaderResource(const WeakSPtr<Texture>& pTexture,
                                     const uint32 startSlot) {
    _setShaderResource<ShaderStage::Pixel>(pTexture, startSlot);
  }

  void
  DX11RenderAPI::gsSetShaderResource(const WeakSPtr<Texture>& pTexture,
                                     const uint32 startSlot) {
    _setShaderResource<ShaderStage::Geometry>(pTexture, startSlot);
  }

  void
  DX11RenderAPI::hsSetShaderResource(const WeakSPtr<Texture>& pTexture,
                                     const uint32 startSlot) {
    _setShaderResource<ShaderStage::Hull>(pTexture, startSlot);
  }

  void
  DX11RenderAPI::dsSetShaderResource(const WeakSPtr<Texture>& pTexture,
                                     const uint32 startSlot) {
    _setShaderResource<ShaderStage::Domain>(pTexture, startSlot);
  }

  void
  DX11RenderAPI::csSetShaderResource(const WeakSPtr<Texture>& pTexture,
                                     const uint32 startSlot) {
    _setShaderResource<ShaderStage::Compute>(pTexture, startSlot);
  }

  /*************************************************************************/
  // Set Unordered Access Views
  /*************************************************************************/
  void
  DX11RenderAPI::csSetUnorderedAccessViews(const Vector<UAVTarget>& pUAVs,
                                           const uint32 startSlot) {
    GE_ASSERT(m_pActiveContext);

    static Vector<ID3D11UnorderedAccessView*> pDXUAVs(D3D11_1_UAV_SLOT_COUNT);
    uint32 numTargets = static_cast<uint32>(pUAVs.size());

    for(uint32 i = 0; i < numTargets; ++i) {
      const UAVTarget& target = pUAVs[i];
      if (target.pUAVTarget.expired()) {
        pDXUAVs[i] = nullptr;
        continue;
      }

      auto pObj = target.pUAVTarget.lock();
      auto pDXObj = reinterpret_cast<DXTexture*>(pObj.get());

      GE_ASSERT(pDXObj->m_ppUAV.size() > SIZE_T(target.mipLevel));
      ID3D11UnorderedAccessView* pUAV = pDXObj->m_ppUAV[target.mipLevel];
      pDXUAVs[i] = pUAV;
    }

    m_pActiveContext->CSSetUnorderedAccessViews(startSlot,
                                                numTargets,
                                                pDXUAVs.data(),
                                                nullptr);
  }

  /*************************************************************************/
  // Set Constant Buffers
  /*************************************************************************/
  template<DX11RenderAPI::ShaderStage Stage>
  void
    DX11RenderAPI::_setConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
      const uint32 startSlot) {
    GE_ASSERT(m_pActiveContext);

    ID3D11Buffer* pDXBuffer = nullptr;
    if (!pBuffer.expired()) {
      auto pCB = reinterpret_cast<DXConstantBuffer*>(pBuffer.lock().get());
      pDXBuffer = pCB->m_pBuffer;
    }

    (m_pActiveContext->*ShaderTraits<Stage>::SetCBuffFn)(startSlot, 1, &pDXBuffer);
  }

  void
  DX11RenderAPI::vsSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                                     const uint32 startSlot) {
    _setConstantBuffer<ShaderStage::Vertex>(pBuffer, startSlot);
  }

  void
  DX11RenderAPI::psSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                                     const uint32 startSlot) {
    _setConstantBuffer<ShaderStage::Pixel>(pBuffer, startSlot);
  }

  void
  DX11RenderAPI::gsSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                                     const uint32 startSlot) {
    _setConstantBuffer<ShaderStage::Geometry>(pBuffer, startSlot);
  }

  void
  DX11RenderAPI::hsSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                                     const uint32 startSlot) {
    _setConstantBuffer<ShaderStage::Hull>(pBuffer, startSlot);
  }

  void
  DX11RenderAPI::dsSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                                     const uint32 startSlot) {
    _setConstantBuffer<ShaderStage::Domain>(pBuffer, startSlot);
  }

  void
  DX11RenderAPI::csSetConstantBuffer(const WeakSPtr<ConstantBuffer>& pBuffer,
                                     const uint32 startSlot) {
    _setConstantBuffer<ShaderStage::Compute>(pBuffer, startSlot);
  }

  /*************************************************************************/
  // Set Samplers
  /*************************************************************************/
  template<DX11RenderAPI::ShaderStage Stage>
  void
  DX11RenderAPI::_setSampler(const WeakSPtr<SamplerState>& pSampler,
                             const uint32 startSlot) {
    GE_ASSERT(m_pActiveContext);

    ID3D11SamplerState* pSS = nullptr;
    if (!pSampler.expired()) {
      auto pObj = reinterpret_cast<DXSamplerState*>(pSampler.lock().get());
      pSS = pObj->m_pSampler;
    }

    (m_pActiveContext->*ShaderTraits<Stage>::SetSamplerFn)(startSlot, 1, &pSS);
  }

  void
  DX11RenderAPI::vsSetSampler(const WeakSPtr<SamplerState>& pSampler,
                              const uint32 startSlot) {
    _setSampler<ShaderStage::Vertex>(pSampler, startSlot);
  }

  void
  DX11RenderAPI::psSetSampler(const WeakSPtr<SamplerState>& pSampler,
                              const uint32 startSlot) {
    _setSampler<ShaderStage::Pixel>(pSampler, startSlot);
  }

  void
  DX11RenderAPI::gsSetSampler(const WeakSPtr<SamplerState>& pSampler,
                              const uint32 startSlot) {
    _setSampler<ShaderStage::Geometry>(pSampler, startSlot);
  }

  void
  DX11RenderAPI::hsSetSampler(const WeakSPtr<SamplerState>& pSampler,
                              const uint32 startSlot) {
    _setSampler<ShaderStage::Hull>(pSampler, startSlot);
  }

  void
  DX11RenderAPI::dsSetSampler(const WeakSPtr<SamplerState>& pSampler,
                              const uint32 startSlot) {
    _setSampler<ShaderStage::Domain>(pSampler, startSlot);
  }

  void
  DX11RenderAPI::csSetSampler(const WeakSPtr<SamplerState>& pSampler,
                              const uint32 startSlot) {
    _setSampler<ShaderStage::Compute>(pSampler, startSlot);
  }

  /*************************************************************************/
  // Set Render Targets
  /*************************************************************************/
  void
  DX11RenderAPI::setRenderTargets(const Vector<RenderTarget>& pTargets,
                                  const WeakSPtr<Texture>& pDepthStencilView) {
    GE_ASSERT(m_pActiveContext);

    static Vector<ID3D11RenderTargetView*>
      pRTVs(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

    uint32 numTargets = static_cast<uint32>(pTargets.size());

    for(uint32 i = 0; i < numTargets; ++i) {
      const RenderTarget& target = pTargets[i];
      if (target.pRenderTarget.expired()) {
        pRTVs[i] = nullptr;
        continue;
      }

      auto pObj = target.pRenderTarget.lock();
      auto pDXObj = reinterpret_cast<DXTexture*>(pObj.get());

      GE_ASSERT(pDXObj->m_ppRTV.size() > SIZE_T(target.mipLevel));
      ID3D11RenderTargetView* pRTV = pDXObj->m_ppRTV[target.mipLevel];
      pRTVs[i] = pRTV;
    }

    ID3D11DepthStencilView* pDS = nullptr;
    if (!pDepthStencilView.expired()) {
      auto pObj = pDepthStencilView.lock();
      auto pDXObj = reinterpret_cast<DXTexture*>(pObj.get());
      pDS = pDXObj->m_pDSV;
    }

    m_pActiveContext->OMSetRenderTargets(numTargets, pRTVs.data(), pDS);
  }

  void
  DX11RenderAPI::setStreamOutputTarget(const WeakSPtr<StreamOutputBuffer>& pBuffer) {
    GE_ASSERT(m_pActiveContext);

    ID3D11Buffer* pDXBuffer = nullptr;
    if (!pBuffer.expired()) {
      auto pObj = reinterpret_cast<DXStreamOutputBuffer*>(pBuffer.lock().get());
      pDXBuffer = pObj->m_pBuffer;
    }

    UINT offset = 0;
    m_pActiveContext->SOSetTargets(1, &pDXBuffer, &offset);
  }

  SPtr<PipelineState>
  DX11RenderAPI::savePipelineState() const {
    GE_ASSERT(m_pActiveContext);

    //First create a new PipelineState object
    auto pBkState = ge_shared_ptr_new<DXPipelineState>();

    pBkState->m_scissorRectsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    pBkState->m_viewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

    m_pActiveContext->RSGetScissorRects(&pBkState->m_scissorRectsCount,
                                        pBkState->m_scissorRects);
    m_pActiveContext->RSGetViewports(&pBkState->m_viewportsCount, pBkState->m_viewports);
    m_pActiveContext->RSGetState(&pBkState->m_rasterState);

    m_pActiveContext->OMGetBlendState(&pBkState->m_blendState,
                                      pBkState->m_blendFactor,
                                      &pBkState->m_sampleMask);
    m_pActiveContext->OMGetDepthStencilState(&pBkState->m_depthStencilState,
                                             &pBkState->m_stencilRef);

    m_pActiveContext->PSGetShaderResources(0, 1, &pBkState->m_psShaderResource);
    m_pActiveContext->PSGetSamplers(0, 1, &pBkState->m_psSampler);

    pBkState->m_psInstancesCount = 256;
    pBkState->m_vsInstancesCount = 256;
    pBkState->m_gsInstancesCount = 256;
    m_pActiveContext->PSGetShader(&pBkState->m_ps,
                                  pBkState->m_psInstances,
                                  &pBkState->m_psInstancesCount);
    m_pActiveContext->VSGetShader(&pBkState->m_vs,
                                  pBkState->m_vsInstances,
                                  &pBkState->m_vsInstancesCount);
    m_pActiveContext->VSGetConstantBuffers(0, 1, &pBkState->m_vsConstantBuffer);
    m_pActiveContext->GSGetShader(&pBkState->m_gs,
                                  pBkState->m_gsInstances,
                                  &pBkState->m_gsInstancesCount);

    m_pActiveContext->IAGetPrimitiveTopology(&pBkState->m_primitiveTopology);
    m_pActiveContext->IAGetIndexBuffer(&pBkState->m_indexBuffer,
                                       &pBkState->m_indexBufferFormat,
                                       &pBkState->m_indexBufferOffset);
    m_pActiveContext->IAGetVertexBuffers(0,
                                         1,
                                         &pBkState->m_vertexBuffer,
                                         &pBkState->m_vertexBufferStride,
                                         &pBkState->m_vertexBufferOffset);

    m_pActiveContext->IAGetInputLayout(&pBkState->m_inputLayout);

    return pBkState;
  }

  void
  DX11RenderAPI::restorePipelineState(const WeakSPtr<PipelineState>& pState) {
    GE_ASSERT(m_pActiveContext);

    if (pState.expired()) {
      return;
    }

    auto pOldState = reinterpret_cast<DXPipelineState*>(pState.lock().get());

    m_pActiveContext->RSSetScissorRects(pOldState->m_scissorRectsCount,
                                        pOldState->m_scissorRects);
    m_pActiveContext->RSSetViewports(pOldState->m_viewportsCount, pOldState->m_viewports);
    m_pActiveContext->RSSetState(pOldState->m_rasterState);

    m_pActiveContext->OMSetBlendState(pOldState->m_blendState,
                                      pOldState->m_blendFactor,
                                      pOldState->m_sampleMask);
    m_pActiveContext->OMSetDepthStencilState(pOldState->m_depthStencilState,
                                             pOldState->m_stencilRef);

    m_pActiveContext->PSSetShaderResources(0, 1, &pOldState->m_psShaderResource);
    m_pActiveContext->PSSetSamplers(0, 1, &pOldState->m_psSampler);
    m_pActiveContext->PSSetShader(pOldState->m_ps,
                                  pOldState->m_psInstances,
                                  pOldState->m_psInstancesCount);

    m_pActiveContext->VSSetShader(pOldState->m_vs,
                                  pOldState->m_vsInstances,
                                  pOldState->m_vsInstancesCount);
    m_pActiveContext->VSSetConstantBuffers(0, 1, &pOldState->m_vsConstantBuffer);
    m_pActiveContext->GSSetShader(pOldState->m_gs,
                                  pOldState->m_gsInstances,
                                  pOldState->m_gsInstancesCount);
    
    m_pActiveContext->IASetPrimitiveTopology(pOldState->m_primitiveTopology);
    m_pActiveContext->IASetIndexBuffer(pOldState->m_indexBuffer,
                                       pOldState->m_indexBufferFormat,
                                       pOldState->m_indexBufferOffset);
    m_pActiveContext->IASetVertexBuffers(0,
                                         1,
                                         &pOldState->m_vertexBuffer,
                                         &pOldState->m_vertexBufferStride,
                                         &pOldState->m_vertexBufferOffset);
    m_pActiveContext->IASetInputLayout(pOldState->m_inputLayout);
  }

  void
  DX11RenderAPI::draw(uint32 vertexCount, uint32 startVertexLocation) {
    GE_ASSERT(m_pActiveContext);
    m_pActiveContext->Draw(vertexCount, startVertexLocation);
  }

  void
  DX11RenderAPI::drawIndexed(uint32 indexCount,
                             uint32 startIndexLocation,
                             int32 baseVertexLocation) {
    GE_ASSERT(m_pActiveContext);
    m_pActiveContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
  }

  void
  DX11RenderAPI::drawInstanced(uint32 vertexCountPerInstance,
                               uint32 instanceCount,
                               uint32 startVertexLocation,
                               uint32 startInstanceLocation) {
    GE_ASSERT(m_pActiveContext);
    m_pActiveContext->DrawInstanced(vertexCountPerInstance,
                                    instanceCount,
                                    startVertexLocation,
                                    startInstanceLocation);
  }

  void
  DX11RenderAPI::drawAuto() {
    GE_ASSERT(m_pActiveContext);
    m_pActiveContext->DrawAuto();
  }

  void
  DX11RenderAPI::dispatch(uint32 threadGroupCountX,
                          uint32 threadGroupCountY,
                          uint32 threadGroupCountZ) {
    GE_ASSERT(m_pActiveContext);
    m_pActiveContext->Dispatch(threadGroupCountX,
                               threadGroupCountY,
                               threadGroupCountZ);
  }

  GraphicsInfo
  DX11RenderAPI::getDevice() const {
    GraphicsInfo info;

    //On DirectX11, we send the device and device context pointers
    info.pDevicePtr = cast::re<void*>(m_pDevice);
    info.pCommandPtr = cast::re<void*>(m_pActiveContext);

    return info;
  }

  void
  DX11RenderAPI::_updateBackBufferTexture() {
    GE_ASSERT(m_pDevice && m_pSwapChain);

    //Release the old back buffer and creates a new one
    m_pBackBufferTexture = ge_shared_ptr_new<DXTexture>();
    if(!m_pBackBufferTexture) {
      GE_EXCEPT(RenderingAPIException,
                "Failed to allocate back buffer texture.");
    }
    
    m_pBackBufferTexture->m_ppRTV.push_back(nullptr);

    //Create a render target view
    throwIfFailed(m_pSwapChain->GetBuffer(0,
      __uuidof(ID3D11Texture2D),
      reinterpret_cast<LPVOID*>(&m_pBackBufferTexture->m_pTexture)));

#if !USING(DX_VERSION_11_3) && !USING(DX_VERSION_11_4)
    throwIfFailed(m_pDevice->CreateRenderTargetView(m_pBackBufferTexture->m_pTexture,
                                                    nullptr,
                                                    &m_pBackBufferTexture->m_ppRTV[0]));
#else
    throwIfFailed(m_pDevice->CreateRenderTargetView1(m_pBackBufferTexture->m_pTexture,
                                                    nullptr,
                                                    &m_pBackBufferTexture->m_ppRTV[0]));
#endif

    D3D11_TEXTURE2D_DESC desc;
    m_pBackBufferTexture->m_pTexture->GetDesc(&desc);
    m_pBackBufferTexture->_setDesc(desc);
  }

  WeakSPtr<Texture>
  DX11RenderAPI::getBackBuffer() const {
    return m_pBackBufferTexture;
  }

  WeakSPtr<RasterizerState>
  DX11RenderAPI::getCurrentRasterizerState() const {
    GE_ASSERT(m_pActiveContext);

    ID3D11RasterizerState* pDXRS = nullptr;
    auto pRS = ge_shared_ptr_new<DXRasterizerState>();
    m_pActiveContext->RSGetState(&pDXRS);
    pRS->m_pRasterizerState = getAs<D3DRasterizerState>(pDXRS);
    safeRelease(pDXRS);

    return pRS;
  }

  WeakSPtr<DepthStencilState>
  DX11RenderAPI::getCurrentDepthStencilState() const {
    GE_ASSERT(m_pActiveContext);
    
    ID3D11DepthStencilState* pDXDSS = nullptr;
    uint32 stencilRef = 0;

    auto pDSS = ge_shared_ptr_new<DXDepthStencilState>();
    m_pActiveContext->OMGetDepthStencilState(&pDXDSS, &stencilRef);
    pDSS->m_pDepthStencilState = pDXDSS;
    safeRelease(pDXDSS);

    return pDSS;
  }

  WeakSPtr<BlendState>
  DX11RenderAPI::getCurrentBlendState() const {
    GE_ASSERT(m_pActiveContext);
    
    ID3D11BlendState* pDXBS = nullptr;
    Vector4 blendFactors(geEngineSDK::FORCE_INIT::kForceInitToZero);
    uint32 sampleMask = 0xffffffff;
    
    auto pBS = ge_shared_ptr_new<DXBlendState>();
    m_pActiveContext->OMGetBlendState(&pDXBS, &blendFactors[0], &sampleMask);
    pBS->m_pBlendState = getAs<D3DBlendState>(pDXBS);
    pBS->m_blendFactors = blendFactors;
    pBS->m_sampleMask = sampleMask;
    safeRelease(pDXBS);

    return pBS;
  }

  WeakSPtr<SamplerState>
  DX11RenderAPI::getCurrentSamplerState(uint32 samplerSlot) const {
    GE_ASSERT(m_pActiveContext);

    ID3D11SamplerState* pDXSS = nullptr;
    auto pSS = ge_shared_ptr_new<DXSamplerState>();
    m_pActiveContext->PSGetSamplers(samplerSlot, 1, &pDXSS);
    pSS->m_pSampler = pDXSS;

    return pSS;
  }

}
