/*****************************************************************************/
/**
 * @file    DXGraphicsBuffer.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/26
 * @brief   Graphics Buffer Interface .
 *
 * Graphics Buffer Interface. This interface defines the methods and properties
 * To unify the management of graphics buffers across different graphics APIs.
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
#include "DXGraphicsInterfaces.h"

namespace geEngineSDK {

  /**
   * @brief Represents a vertex buffer in DirectX 11.
   *        This class manages the vertex buffer used for rendering geometry.
   */
  class DXVertexBuffer : public VertexBuffer
  {
   public:
    DXVertexBuffer() = default;
    virtual ~DXVertexBuffer() {
      release();
    }

    /**
     * @brief Releases resources held by the object.
     */
    void
    release() override {
      safeRelease(m_pBuffer);
    }

    bool
    load(const Path& filePath) override {
      GE_UNREFERENCED_PARAMETER(filePath);
      return false; // Vertex buffers are usually created in code, not loaded from files
    }

    void
    unload() override
    {}

    bool
    isLoaded() const override {
      return m_pBuffer != nullptr;
    }

    const String&
    getName() const override {
      static String emptyName;
      return emptyName;
    }

    SIZE_T
    getMemoryUsage() const override {
      return m_Desc.ByteWidth; // Memory usage is the size of the buffer
    }

    void*
    _getGraphicsResource() const override {
      return m_pBuffer;
    }

    void*
    _getGraphicsBuffer() const override {
      return m_pBuffer;
    }

    const D3D11_BUFFER_DESC&
    getDesc() const {
      return m_Desc;
    }

   protected:
    friend class DX11RenderAPI;
    ID3D11Buffer* m_pBuffer = nullptr;
    D3D11_BUFFER_DESC m_Desc{};
  };

  /**
   * @brief Represents a stream output buffer in DirectX 11.
   *        This class manages the buffer used for stream output operations.
   */
  class DXStreamOutputBuffer : public StreamOutputBuffer
  {
   public:
    DXStreamOutputBuffer() = default;
    virtual ~DXStreamOutputBuffer() {
      release();
    }

    /**
     * @brief Releases resources held by the object.
     */
    void
    release() override {
      safeRelease(m_pBuffer);
    }

    bool
    load(const Path& filePath) override {
      GE_UNREFERENCED_PARAMETER(filePath);
      return false; // Stream output buffers are usually created in code, not loaded from files
    }

    void
    unload() override
    {}

    bool
    isLoaded() const override {
      return m_pBuffer != nullptr;
    }

    const String&
    getName() const override {
      static String emptyName;
      return emptyName;
    }

    SIZE_T
    getMemoryUsage() const override {
      return m_Desc.ByteWidth; // Memory usage is the size of the buffer
    }

    void*
    _getGraphicsResource() const override {
      return m_pBuffer;
    }

    void*
    _getGraphicsBuffer() const override {
      return m_pBuffer;
    }

   protected:
    friend class DX11RenderAPI;
    ID3D11Buffer* m_pBuffer = nullptr;
    D3D11_BUFFER_DESC m_Desc{};
  };

  /**
   * @brief Represents an index buffer in DirectX 11.
   *        This class manages the index buffer used for rendering indexed geometry.
   */
  class DXIndexBuffer : public IndexBuffer
  {
   public:
    DXIndexBuffer() = default;
    virtual ~DXIndexBuffer() {
      release();
    }

    /**
     * @brief Releases resources held by the object.
     */
    void
    release() override {
      safeRelease(m_pBuffer);
    }

    bool
    load(const Path& filePath) override {
      GE_UNREFERENCED_PARAMETER(filePath);
      return false; // Vertex buffers are usually created in code, not loaded from files
    }

    void
    unload() override
    {}

    bool
    isLoaded() const override {
      return m_pBuffer != nullptr;
    }

    const String&
    getName() const override {
      static String emptyName;
      return emptyName;
    }

    SIZE_T
    getMemoryUsage() const override {
      return m_Desc.ByteWidth; // Memory usage is the size of the buffer
    }

    void*
    _getGraphicsResource() const override {
      return m_pBuffer;
    }

    void*
    _getGraphicsBuffer() const override {
      return m_pBuffer;
    }

    const D3D11_BUFFER_DESC&
      getDesc() const {
      return m_Desc;
    }

  protected:
    friend class DX11RenderAPI;
    ID3D11Buffer* m_pBuffer = nullptr;
    D3D11_BUFFER_DESC m_Desc{};
  };

  /**
   * @brief Represents a constant buffer in DirectX 11.
   *        This class manages the constant buffer used for passing data to shaders.
   */
  class DXConstantBuffer : public ConstantBuffer
  {
  public:
    DXConstantBuffer() = default;
    virtual ~DXConstantBuffer() {
      release();
    }

    /**
     * @brief Releases resources held by the object.
     */
    void
    release() override {
      safeRelease(m_pBuffer);
    }

    bool
    load(const Path& filePath) override {
      GE_UNREFERENCED_PARAMETER(filePath);
      return false; // Vertex buffers are usually created in code, not loaded from files
    }

    void
    unload() override
    {}

    bool
    isLoaded() const override {
      return m_pBuffer != nullptr;
    }

    const String&
    getName() const override {
      static String emptyName;
      return emptyName;
    }

    SIZE_T
    getMemoryUsage() const override {
      return m_Desc.ByteWidth; // Memory usage is the size of the buffer
    }

    void*
    _getGraphicsResource() const override {
      return m_pBuffer;
    }

    void*
    _getGraphicsBuffer() const override {
      return m_pBuffer;
    }

    const D3D11_BUFFER_DESC&
    getDesc() const {
      return m_Desc;
    }

   protected:
    friend class DX11RenderAPI;
    ID3D11Buffer* m_pBuffer = nullptr;
    D3D11_BUFFER_DESC m_Desc{};
  };

} // namespace geEngineSDK
