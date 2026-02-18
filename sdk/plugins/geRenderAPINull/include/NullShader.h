/*****************************************************************************/
/**
 * @file    NullShader.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/02/17
 * @brief   Shader Objects Implementations with Null Driver.
 *
 * Shader Objects Implementations with Null Driver.
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
#include <geShader.h>

namespace geEngineSDK {

  class NullShader : public Shader
  {
   public:
    NullShader() = default;
    virtual ~NullShader();

    void
    release() override;

    void
    setDebugName(const String& name) override;

    bool
    load(const Path& filePath) override {
      GE_UNREFERENCED_PARAMETER(filePath);
      return false;
    }

    void
    unload() override {}

    bool
    isLoaded() const override {
      return m_pShader != 0;
    }

    const String&
    getName() const override {
      return m_debugName;
    }

    SIZE_T
    getMemoryUsage() const override {
      return 0;
    }

   protected:
    friend class NullRenderAPI;

    int32 m_pShader = 0;
    int32 m_pBlob = 0;
  };
  
}
