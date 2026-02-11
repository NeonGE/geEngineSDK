/*****************************************************************************/
/**
 * @file    geGraphicsInterfaces.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/26
 * @brief   Graphics Interface to handle class herency and .
 *
 * Graphics Resource Interface. A graphics resource is a specific type of 
 * resource that is used in the rendering pipeline, such as textures, shaders,
 * or buffers.
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
#include "geResource.h"
#include "geVertexDeclaration.h"

namespace geEngineSDK {

  class GE_CORE_EXPORT GraphicsResource : public Resource
  {
   public:
    virtual ~GraphicsResource() = default;

    /**
     * @brief Releases resources held by the object.
     */
    virtual void
    release() = 0;

    virtual void*
    _getGraphicsResource() const = 0;
  };

  class GE_CORE_EXPORT GraphicsBuffer : public GraphicsResource
  {
   public:
    virtual ~GraphicsBuffer() = default;

    virtual void*
    _getGraphicsBuffer() const = 0;
  };

  class GE_CORE_EXPORT VertexBuffer : public GraphicsBuffer
  {
   public:
    virtual ~VertexBuffer() = default;

    virtual WeakSPtr<VertexDeclaration>
    getVertexDeclaration() const {
      return m_pVertexDeclaration;
    }

   protected:
    SPtr<VertexDeclaration> m_pVertexDeclaration;
  };

  class GE_CORE_EXPORT StreamOutputBuffer : public GraphicsBuffer
  {
   public:
    virtual ~StreamOutputBuffer() = default;
  
    /**
     * @brief Returns the stream output declaration associated with this buffer.
     */
    virtual WeakSPtr<StreamOutputDeclaration>
    getStreamOutputDeclaration() const {
      return m_pStreamOutputDeclaration;
    }
  
   protected:
    SPtr<StreamOutputDeclaration> m_pStreamOutputDeclaration;
  };

  class GE_CORE_EXPORT IndexBuffer : public GraphicsBuffer
  {
   public:
    virtual ~IndexBuffer() = default;

   protected:
    GRAPHICS_FORMAT::E m_indexFormat = GRAPHICS_FORMAT::kR32_UINT;
  };

  using ConstantBuffer = GraphicsBuffer;

  class GE_CORE_EXPORT RasterizerState
  {
   public:
    virtual ~RasterizerState() = default;

    /**
     * @brief Releases resources held by the object.
     */
    virtual void
    release() = 0;

    virtual void
    setDebugName(const String& name) = 0;
  };

  class GE_CORE_EXPORT BlendState
  {
   public:
    virtual ~BlendState() = default;

    /**
     * @brief Releases resources held by the object.
     */
    virtual void
    release() = 0;

    virtual void
    setDebugName(const String& name) = 0;
  };

  class GE_CORE_EXPORT DepthStencilState
  {
   public:
    virtual ~DepthStencilState() = default;

    /**
     * @brief Releases resources held by the object.
     */
    virtual void
    release() = 0;

    virtual void
    setDebugName(const String& name) = 0;
  };

  class GE_CORE_EXPORT SamplerState
  {
   public:
    virtual ~SamplerState() = default;

    /**
     * @brief Releases resources held by the object.
     */
    virtual void
    release() = 0;

    virtual void
    setDebugName(const String& name) = 0;
  };

  class GE_CORE_EXPORT PipelineState
  {
   public:
    PipelineState() = default;
    virtual ~PipelineState() = default;

    /**
     * @brief Releases resources held by the object.
     */
    virtual void
    release() = 0;
  };

}; // namespace geEngineSDK
