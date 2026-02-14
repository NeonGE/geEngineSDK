/*****************************************************************************/
/**
 * @file    DXShader.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/07
 * @brief   Shader Objects Implementations with DX.
 *
 * Shader Objects Implementations with DX.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "DXShader.h"

namespace geEngineSDK {

  DXShader::~DXShader() {
    release();
  }

  void
  DXShader::release() {
    safeRelease(m_pBlob);
    safeRelease(m_pShader);
  }

  void
  DXShader::setDebugName(const String& name) {
#if USING(GE_DEBUG_MODE)
    m_debugName = name;
    if (m_pShader) {
      m_pShader->SetPrivateData(WKPDID_D3DDebugObjectName,
                                static_cast<UINT>(name.size()),
                                name.c_str());
    }
#else
    GE_UNREFERENCED_PARAMETER(name);
#endif // USING(GE_DEBUG_MODE)
  }

}
