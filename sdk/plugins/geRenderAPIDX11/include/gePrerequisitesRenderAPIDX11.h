/*****************************************************************************/
/**
 * @file    gePrerequisitesRenderAPIDX11.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/29
 * @brief   Prerequisites for the Graphics API.
 *
 * Prerequisites for the Graphics API.
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

#include <gePrerequisitesCore.h>
#include <geGraphicsTypes.h>
#include <geException.h>

//This usings will be used to identify the DirectX version used in the plugin
#define DX_VERSION_11_0 NOT_IN_USE
#define DX_VERSION_11_1 NOT_IN_USE
#define DX_VERSION_11_2 NOT_IN_USE
#define DX_VERSION_11_3 NOT_IN_USE
#define DX_VERSION_11_4 IN_USE

#include <Win32/geMinWindows.h>
#if USING(DX_VERSION_11_0)
# include <d3d11.h>
using D3DDevice = ID3D11Device;
using D3DDeviceContext = ID3D11DeviceContext;
using D3DSwapChain = IDXGISwapChain;
using DXFactory = IDXGIFactory1;
using DXGIDevice = IDXGIDevice1;
using D3DRenderTargetView = ID3D11RenderTargetView;
using D3DRasterizerState = ID3D11RasterizerState;
using D3DBlendState = ID3D11BlendState;

#elif USING(DX_VERSION_11_1)
# include <d3d11_1.h>
using D3DDevice = ID3D11Device1;
using D3DDeviceContext = ID3D11DeviceContext1;
using D3DSwapChain = IDXGISwapChain1;
using DXFactory = IDXGIFactory2;
using DXGIDevice = IDXGIDevice2;
using D3DRenderTargetView = ID3D11RenderTargetView;
using D3DRasterizerState = ID3D11RasterizerState1;
using D3DBlendState = ID3D11BlendState1;

#elif USING(DX_VERSION_11_2)
# include <d3d11_2.h>
using D3DDevice = ID3D11Device2;
using D3DDeviceContext = ID3D11DeviceContext2;
using D3DSwapChain = IDXGISwapChain1;
using DXFactory = IDXGIFactory2;
using DXGIDevice = IDXGIDevice3;
using D3DRenderTargetView = ID3D11RenderTargetView;
using D3DRasterizerState = ID3D11RasterizerState1;
using D3DBlendState = ID3D11BlendState1;

#elif USING(DX_VERSION_11_3)
# include <d3d11_3.h>
using D3DDevice = ID3D11Device3;
using D3DDeviceContext = ID3D11DeviceContext3;
using D3DSwapChain = IDXGISwapChain2;
using DXFactory = IDXGIFactory2;
using DXGIDevice = IDXGIDevice3;
using D3DRenderTargetView = ID3D11RenderTargetView1;
using D3DRasterizerState = ID3D11RasterizerState2;
using D3DBlendState = ID3D11BlendState1;

#elif USING(DX_VERSION_11_4)
# include <d3d11_4.h>
using D3DDevice = ID3D11Device4;
using D3DDeviceContext = ID3D11DeviceContext4;
using D3DSwapChain = IDXGISwapChain4;
using DXFactory = IDXGIFactory2;
using DXGIDevice = IDXGIDevice3;
using D3DRenderTargetView = ID3D11RenderTargetView1;
using D3DRasterizerState = ID3D11RasterizerState2;
using D3DBlendState = ID3D11BlendState1;

#else
# error "No DirectX version defined. Please define a DirectX version to use."
#endif

using D3DTexture2D = ID3D11Texture2D;
using D3DShaderResourceView = ID3D11ShaderResourceView;
using D3DDepthStencilView = ID3D11DepthStencilView;
using D3DUnorderedAccessView = ID3D11UnorderedAccessView;

namespace geEngineSDK {

  template<typename T = RenderingAPIException>
  inline void
  throwIfFailed(HRESULT hr, const char* msg = "DirectX call failed.") {
    if (FAILED(hr)) {
      //Convert the HRESULT to a string for better error reporting
      char errorMsg[256];
      sprintf_s(errorMsg, sizeof(errorMsg), "%s\n(HRESULT: 0x%08X)", msg, hr);
      GE_EXCEPT(T, errorMsg);
    }
  }

  template<typename RetT, typename OgT>
  RetT*
  getAs(OgT* obj) {
    RetT* ret = nullptr;
    throwIfFailed(obj->QueryInterface(__uuidof(RetT), reinterpret_cast<void**>(&ret)));
    return ret;
  }

}
