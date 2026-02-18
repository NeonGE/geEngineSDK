/*****************************************************************************/
/**
 * @file    NullInputLayout.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/02/17
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
#include "gePrerequisitesRenderAPINull.h"
#include <geInputLayout.h>

namespace geEngineSDK {

  class NullInputLayout : public InputLayout
  {
   public:
    NullInputLayout() = default;
    virtual ~NullInputLayout() {
      release();
    }

    void
    release() override {
      m_inputLayout -= 1;
    }

   protected:
    friend class NullRenderAPI;
    friend class NullInputLayoutManager;

    int32 m_inputLayout = 0;
  };
  
} // namespace geEngineSDK
