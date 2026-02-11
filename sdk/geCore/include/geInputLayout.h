/*****************************************************************************/
/**
 * @file    geInputLayout.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/07
 * @brief   Object that describes the memory layout of a Vertex Buffer.
 *
 * Object that describes the memory layout of a Vertex Buffer.
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
#include "geVertexDeclaration.h"

namespace geEngineSDK {

  class GE_CORE_EXPORT InputLayout
  {
   public:
    InputLayout() = default;
    virtual ~InputLayout() = default;

    virtual void
    release() = 0;

    SPtr<VertexDeclaration>
    getVertexDeclaration() const {
      return m_vertexDeclaration;
    }

   protected:
    SPtr<VertexDeclaration> m_vertexDeclaration;
  };

} // namespace geEngineSDK
