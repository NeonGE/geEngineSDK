/*****************************************************************************/
/**
 * @file    NullGraphicsBuffer.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/02/17
 * @brief   Graphics Buffer Interface.
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
#include "gePrerequisitesRenderAPINull.h"
#include "NullGraphicsInterfaces.h"

namespace geEngineSDK {

  /**
   * @brief Represents a vertex buffer in DirectX 11.
   *        This class manages the vertex buffer used for rendering geometry.
   */
  class NullVertexBuffer : public VertexBuffer
  {
   public:
    NullVertexBuffer() = default;
    virtual ~NullVertexBuffer() {
      release();
    }

    /**
     * @brief Releases resources held by the object.
     */
    void
    release() override {
      m_pBuffer -= 1;
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
      return m_pBuffer != 0;
    }

    const String&
    getName() const override {
      static String emptyName;
      return emptyName;
    }

    SIZE_T
    getMemoryUsage() const override {
      return m_Desc.byteWidth; // Memory usage is the size of the buffer
    }

    const BUFFER_DESC&
    getDesc() const {
      return m_Desc;
    }

    void*
    _getGraphicsBuffer() const override {
      return nullptr;
    }

    void*
    _getGraphicsResource() const override {
      return nullptr;
    }

   protected:
    friend class NullRenderAPI;
    int32 m_pBuffer = 0;
    BUFFER_DESC m_Desc{};
  };

  /**
   * @brief Represents a stream output buffer in DirectX 11.
   *        This class manages the buffer used for stream output operations.
   */
  class NullStreamOutputBuffer : public StreamOutputBuffer
  {
   public:
    NullStreamOutputBuffer() = default;
    virtual ~NullStreamOutputBuffer() {
      release();
    }

    /**
     * @brief Releases resources held by the object.
     */
    void
    release() override {
      m_pBuffer -= 1;
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
      return m_pBuffer != 0;
    }

    const String&
    getName() const override {
      static String emptyName;
      return emptyName;
    }

    SIZE_T
    getMemoryUsage() const override {
      return m_Desc.byteWidth; // Memory usage is the size of the buffer
    }

    void*
    _getGraphicsBuffer() const override {
      return nullptr;
    }

    void*
    _getGraphicsResource() const override {
      return nullptr;
    }

   protected:
    friend class NullRenderAPI;
    int32 m_pBuffer = 0;
    BUFFER_DESC m_Desc{};
  };

  /**
   * @brief Represents an index buffer in DirectX 11.
   *        This class manages the index buffer used for rendering indexed geometry.
   */
  class NullIndexBuffer : public IndexBuffer
  {
   public:
    NullIndexBuffer() = default;
    virtual ~NullIndexBuffer() {
      release();
    }

    /**
     * @brief Releases resources held by the object.
     */
    void
    release() override {
      m_pBuffer -= 1;
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
      return m_pBuffer != 0;
    }

    const String&
    getName() const override {
      static String emptyName;
      return emptyName;
    }

    SIZE_T
    getMemoryUsage() const override {
      return m_Desc.byteWidth; // Memory usage is the size of the buffer
    }

    const BUFFER_DESC&
    getDesc() const {
      return m_Desc;
    }

    void*
    _getGraphicsBuffer() const override {
      return nullptr;
    }

    void*
    _getGraphicsResource() const override {
      return nullptr;
    }

  protected:
    friend class NullRenderAPI;
    int32 m_pBuffer = 0;
    BUFFER_DESC m_Desc{};
  };

  /**
   * @brief Represents a constant buffer in DirectX 11.
   *        This class manages the constant buffer used for passing data to shaders.
   */
  class NullConstantBuffer : public ConstantBuffer
  {
   public:
    NullConstantBuffer() = default;
    virtual ~NullConstantBuffer() {
      release();
    }

    /**
     * @brief Releases resources held by the object.
     */
    void
    release() override {
      m_pBuffer -= 1;
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
      return m_pBuffer != 0;
    }

    const String&
    getName() const override {
      static String emptyName;
      return emptyName;
    }

    SIZE_T
    getMemoryUsage() const override {
      return m_Desc.byteWidth; // Memory usage is the size of the buffer
    }

    void*
    _getGraphicsResource() const override {
      return nullptr;
    }

    void*
    _getGraphicsBuffer() const override {
      return nullptr;
    }

    const BUFFER_DESC&
    getDesc() const {
      return m_Desc;
    }

   protected:
    friend class NullRenderAPI;
    int32 m_pBuffer = 0;
    BUFFER_DESC m_Desc{};
  };

} // namespace geEngineSDK
