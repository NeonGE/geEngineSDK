/*****************************************************************************/
/**
 * @file    geFloat32.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2014/07/17
 * @brief   32 bits float with access to elements
 *
 * 32 bits float with access to individual elements
 * 23 bits Mantissa , 8 bits Exponent, 1 bit Sign
 *
 * @bug	    No known bugs.
 * 
 * @update  2025/12/12 Converted to use std::bit_cast and avoid unions.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePlatformDefines.h"
#include "gePlatformTypes.h"

namespace geEngineSDK {
  /**
   * @brief 32 bit float components
   */
  class Float32
  {
   public:
    static_assert(sizeof(float) == 4, "Float32 requires 32-bit float");

    /**
     * @brief Constructor
     * @param InValue value of the float.
     * @return  What does this function returns.
     */
    explicit Float32(float inValue = 0.0f) _NOEXCEPT
      : m_encoded(bit_cast<uint32_t>(inValue))
    {}

    explicit Float32(uint32 intValue) _NOEXCEPT
      : m_encoded(intValue)
    {}

    Float32&
    operator=(float FP32Value) _NOEXCEPT {
      set(FP32Value);
      return *this;
    }

    Float32&
    operator=(const Float32& FP32Value) _NOEXCEPT = default;

    operator float() const _NOEXCEPT {
      return bit_cast<float>(m_encoded);
    }

    void
    set(float FP32Value) _NOEXCEPT {
      m_encoded = bit_cast<uint32_t>(FP32Value);
    }

    uint32
    integerValue() const _NOEXCEPT {
      return m_encoded;
    }

    /**
     * @brief Get IEEE-754 sign bit (0 or 1).
     */
    uint32
    sign() const _NOEXCEPT {
      return (m_encoded >> 31) & 0x1u;
    }

    /**
     * @brief Get IEEE-754 exponent (8 bits).
     */
    uint32
    exponent() const _NOEXCEPT {
      return (m_encoded >> 23) & 0xFFu;
    }

    /**
     * @brief Get IEEE-754 mantissa (23 bits).
     */
    uint32
    mantissa() const _NOEXCEPT {
      return m_encoded & 0x7FFFFFu;
    }

   private:
    uint32 m_encoded;
  };
}
