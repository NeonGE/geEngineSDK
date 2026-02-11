/*****************************************************************************/
/**
 * @file    geShader.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/07
 * @brief   Shader Objects Interfaces.
 *
 * Shader Objects Interfaces.
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

namespace geEngineSDK {

  class Shader : Resource
  {
   public:
    Shader() = default;
    virtual ~Shader() = default;
    
    /**
     * @brief Releases resources held by the object.
     */
    virtual void
    release() = 0;

    virtual void
    setDebugName(const String& name) = 0;

    virtual const String&
    getDebugName() const {
      return m_debugName;
    }

   protected:
    String m_debugName;
  };

  using VertexShader = Shader;
  using PixelShader = Shader;
  using GeometryShader = Shader;
  using HullShader = Shader;
  using DomainShader = Shader;
  using ComputeShader = Shader;

} // namespace geEngineSDK
