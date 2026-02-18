/*****************************************************************************/
/**
 * @file    NullShader.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/07
 * @brief   Shader Objects Implementations with Null Driver.
 *
 * Shader Objects Implementations with Null Driver.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "NullShader.h"

namespace geEngineSDK {

  NullShader::~NullShader() {
    release();
  }

  void
  NullShader::release() {
    m_pBlob -= 1;
    m_pShader -= 1;
  }

  void
  NullShader::setDebugName(const String& name) {}

}
