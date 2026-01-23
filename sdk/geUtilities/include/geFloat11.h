/*****************************************************************************/
/**
 * @file    geFloat11.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/06/03
 * @brief   Float 11 class according to OpenGL packed_float extension.
 *
 * Declaration of a class to use 11 bits floats with access to elements
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
   * @brief Class to use 11 bits floats with elements access
   * @note 5 mantissa bits and 5 exponent bits.
   */
  class Float11
  {
   public:
    Float11(const uint32 encoded = 0) _NOEXCEPT
      : m_encoded(encoded)
    {}

    Float11(const Float11& FP11Value) _NOEXCEPT = default;

    explicit Float11(float FP32Value) _NOEXCEPT {
      set(FP32Value);
    }

    Float11&
    operator=(float FP32Value) _NOEXCEPT {
      set(FP32Value);
      return *this;
    }

    Float11&
    operator=(const Float11& FP11Value) _NOEXCEPT = default;

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
          // encoded layout: [exp:5 (all ones)=31] [mant:6]
          m_encoded = 1984u |
                      (((fp32Bits >> 17) |
                        (fp32Bits >> 11) |
                        (fp32Bits >> 6) |
                        fp32Bits) & 63u);
        }
        else if (fp32Sign) { // Negative infinity clamped to 0 (no negatives allowed)
          m_encoded = 0u;
        }
        else { // Positive infinity -> special pattern
          m_encoded = 1984u;
        }
        return;
      }

      // Negative values are clamped to zero (no signed values in this packed format)
      if (fp32Sign) {
        m_encoded = 0u;
        return;
      }

      // Too large, clamp to max value
      if (0x477E0000u < fp32Bits) {
        m_encoded = 1983u;
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

      // Round and pack (11 bits: 5 exponent + 6 mantissa)
      m_encoded = static_cast<uint32>(((val + 0xFFFFu + ((val >> 17) & 1u)) >> 17) & 2047u);
    }

    /**
     * @brief Convert from Fp10 to Fp32.
     */
    float
    getFloat() const _NOEXCEPT {
      const uint32 fp11 = m_encoded & 0x7FFu; // lower 11 bits
      const uint32 mant = fp11 & 0x3Fu;       // 6-bit mantissa
      const uint32 exp = (fp11 >> 6) & 0x1Fu; // 5-bit exponent

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
          // Normalize until the implicit leading 1 is in place for a 6-bit mantissa.
          while (0u == (mantissa & 0x40u)) {
            --exponent;
            mantissa <<= 1u;
          }
          // Remove the implicit leading bit leaving the lower 6 bits
          mantissa &= 0x3Fu;
        }
        else { // Zero
          exponent = static_cast<uint32>(-112);
        }

        const uint32 bits = ((exponent + 112u) << 23) | (mantissa << 17);
        return Float32(bits);
      }
    }

    CONSTEXPR uint32
    integerValue() const _NOEXCEPT {
      return m_encoded & 0x7FFu;
    }

    CONSTEXPR uint32
    exponent() const _NOEXCEPT {
      return (m_encoded >> 6) & 0x1Fu;
    }

    CONSTEXPR uint32
    mantissa() const _NOEXCEPT {
      return m_encoded & 0x3Fu;
    }

   private:
    uint32 m_encoded;
  };
}
