/*****************************************************************************/
/**
 * @file    DXGraphicsBuffer.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
 /*****************************************************************************/
#include "DXGraphicsBuffer.h"

namespace geEngineSDK {

    DXVertexBuffer::~DXVertexBuffer() {
      release();
    }

    void
    DXVertexBuffer::release() {
      safeRelease(m_pBuffer);
    }

    DXIndexBuffer::~DXIndexBuffer() {
      release();
    }

    void
    DXIndexBuffer::release() {
      safeRelease(m_pBuffer);
    }

    DXConstantBuffer::~DXConstantBuffer() {
      release();
    }

    void
    DXConstantBuffer::release() {
      safeRelease(m_pBuffer);
    }

    DXStreamOutputBuffer::~DXStreamOutputBuffer() {
      release();
    }

    void
    DXStreamOutputBuffer::release() {
      safeRelease(m_pBuffer);
    }

}
