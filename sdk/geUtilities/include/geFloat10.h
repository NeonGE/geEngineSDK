/*****************************************************************************/
/**
 * @file    geFloat10.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/06/03
 * @brief   Float 10 class according to OpenGL packed_float extension.
 *
 * Declaration of a class to use 10 bits floats with access to elements
 * represented by 5 mantissa bits and 5 exponent bits.
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
#include "gePrerequisitesUtilities.h"
#include "geFloat32.h"

namespace geEngineSDK {
  /**
   * @brief Class to use 10 bits floats with elements access
   * @note 5 mantissa bits and 5 exponent bits.
   */
  class Float10
  {
   public:
    Float10(const uint32 encoded = 0) _NOEXCEPT
      : m_encoded(encoded)
    {}

    Float10(const Float10& FP10Value) _NOEXCEPT = default;

    explicit Float10(float FP32Value) _NOEXCEPT {
      set(FP32Value);
    }

    Float10&
    operator=(float FP32Value) _NOEXCEPT {
      set(FP32Value);
      return *this;
    }

    Float10&
    operator=(const Float10& FP10Value) _NOEXCEPT = default;

    operator float() const _NOEXCEPT {
      return getFloat();
    }

    void
    set(float FP32Value) _NOEXCEPT {
      Float32 FP32(FP32Value);

      const uint32 fp32Bits = FP32.integerValue();
      const uint32 fp32Exp = FP32.exponent();
      const uint32 fp32Mant = FP32.mantissa();
      const uint32 fp32Sign = FP32.sign();

      // Handle NaN / INF
      if (255u == fp32Exp) {
        if (0u < fp32Mant) { // NaN -> keep some payload bits into mantissa
          m_encoded = 992u |
                      (((fp32Bits >> 18) |
                      (fp32Bits >> 13) |
                      (fp32Bits >> 3) |
                      fp32Bits) & 31u);
        }
        else if (fp32Sign) { // Negative infinity clamped to 0 (no negatives allowed)
          m_encoded = 0u;
        }
        else { // Positive infinity -> max representable special pattern
          m_encoded = 992u;
        }
        return;
      }

      // Negative values are clamped to zero (no signed values in 10-bit format used here)
      if (fp32Sign) {
        m_encoded = 0u;
        return;
      }

      // Too large, clamp to max value (matches original threshold)
      if (0x477C0000u < fp32Bits) {
        m_encoded = 991u;
        return;
      }

      uint32 val;
      if (0x38800000u > fp32Bits) {
        // Too small to be represented as a normalized float, convert to denormalized value
        uint32 shift = 113u - fp32Exp;
        val = (0x800000u | fp32Mant) >> shift;
      }
      else {
        // Re-bias exponent
        val = fp32Bits + 0xC8000000u;
      }

      m_encoded = static_cast<uint32>(((val + 0x1FFFFu + ((val >> 18) & 1u)) >> 18) & 1023u);
    }

    /**
     * @brief Convert from Fp10 to Fp32.
     */
    float
    getFloat() const _NOEXCEPT {
      const uint32 fp10 = m_encoded & 0x3FFu; // lower 10 bits
      const uint32 mant = fp10 & 0x1Fu;
      const uint32 exp = (fp10 >> 5) & 0x1Fu;

      if (31u == exp) { // INF or NAN
        const uint32 bits = 0x7f800000u | (mant << 17);
        return Float32(bits);
      }
      else {
        uint32 exponent;
        uint32 mantissa = mant;

        if (0u != exp) { // Normalized
          exponent = exp;
        }
        else if (mantissa != 0u) { // Denormalized - normalize
          exponent = 1u;
          do {
            --exponent;
            mantissa <<= 1u;
          } while (0u == (mantissa & 0x20u));
          mantissa &= 0x1Fu;
        }
        else { // Zero
          exponent = static_cast<uint32>(-112);
        }

        const uint32 bits = ((exponent + 112u) << 23) | (mantissa << 18);
        return Float32(bits);
      }
    }

    CONSTEXPR uint32
      integerValue() const _NOEXCEPT {
      return m_encoded & 0x3FFu;
    }

    CONSTEXPR uint32
      exponent() const _NOEXCEPT {
      return (m_encoded >> 5) & 0x1Fu;
    }

    CONSTEXPR uint32
      mantissa() const _NOEXCEPT {
      return m_encoded & 0x1Fu;
    }

  private:
    uint32 m_encoded;
  };
}
