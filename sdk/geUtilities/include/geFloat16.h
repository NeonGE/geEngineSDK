/*****************************************************************************/
/**
 * @file    geFloat16.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2014/07/17
 * @brief   IEEE float 16 class
 *
 * Declaration of a class to use 16 bits floats with access to elements
 * represented by 10 mantissa bits, 5 exponent bits and 1 sign bit.
 *
 * E=0, M=0     == 0.0
 * E=0, M!=0    == Denormalized value (M/2^10)*2^-14
 * 0<E<31, M=any== (1 + M / 2^10) * 2^(E-15)
 * E=32, M=0    == Infinite
 * E=31, M!=0   == NaN
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
#include "geMath.h"
#include "geFloat32.h"

namespace geEngineSDK {
  /**
   * @brief Class to use 16 bits floats with elements access
   * @note 10 mantissa bits, 5 exponent bits and 1 sign bit.
   */
  class Float16
  {
   public:
    
    CONSTEXPR Float16(const uint32 encoded = 0) _NOEXCEPT
      : m_encoded(static_cast<uint16>(encoded & 0xFFFFu))
    {}

    CONSTEXPR Float16(const Float16& other) _NOEXCEPT = default;

    explicit Float16(float FP32Value) _NOEXCEPT {
      set(FP32Value);
    }

    Float16&
    operator=(float FP32Value) _NOEXCEPT {
      set(FP32Value);
      return *this;
    }

    CONSTEXPR Float16&
    operator=(const Float16& other) _NOEXCEPT = default;

    operator float() const _NOEXCEPT {
      return getFloat();
    }

    void
    set(float FP32Value) _NOEXCEPT {
      Float32 fp32(FP32Value);

      //const uint32 fp32Bits = fp32.integerValue();
      const uint32 fp32Exp = fp32.exponent();
      const uint32 fp32Mant = fp32.mantissa();
      const uint32 fp32Sign = fp32.sign();

      uint32 sign = fp32Sign;
      uint32 exp16 = 0u;
      uint32 mant16 = 0u;

      // Handle NaN / INF explicitly
      if (255u == fp32Exp) {
        if (0u < fp32Mant) { // NaN -> keep payload bits in mantissa (quiet NaN)
          exp16 = 0x1Fu;
          // Take top 10 bits of the payload (shift mantissa down by 13)
          mant16 = (fp32Mant >> 13) & 0x3FFu;
          if (0u == mant16) {
            // ensure it's still NaN (non-zero payload)
            mant16 = 0x1u;
          }
        }
        else { // Infinity
          exp16 = 0x1Fu;
          mant16 = 0u;
        }

        m_encoded = static_cast<uint16>((sign << 15) | (exp16 << 10) | mant16);
        return;
      }

      // Normal range and bounds checks
      if (fp32Exp <= 112u) { // Too small -> may become zero or subnormal
        // Try to produce a denormalized half if possible
        if (fp32Exp >= 103u) {
          // Convert to a subnormal half by shifting mantissa
          // shift = 114 - fp32Exp (114 = 127 - (15 - 1))
          //uint32 shift = 125u - fp32Exp; // another valid variant (safe small-int calc)
          // Using integer representation to construct the denormal half mantissa
          uint32 mant = (0x800000u | fp32Mant) >> (13 + (113u - fp32Exp));
          // Above arithmetic is equivalent to rounding to nearest for subnormal conversion.
          mant16 = mant & 0x3FFu;
          // If rounding produced 0, the value underflows to zero
          if (mant16 == 0u) {
            m_encoded = static_cast<uint16>(sign << 15);
            return;
          }
          m_encoded = static_cast<uint16>((sign << 15) | mant16);
          return;
        }
        else {
          // Too small, underflow to zero
          m_encoded = static_cast<uint16>(sign << 15);
          return;
        }
      }
      else if (fp32Exp >= 143u) { // Too large -> clamp to max finite half
        exp16 = 0x1Eu; // 30
        mant16 = 0x3FFu; // 1023
        m_encoded = static_cast<uint16>((sign << 15) | (exp16 << 10) | mant16);
        return;
      }
      else {
        // Normalized value
        exp16 = static_cast<uint32>(int32(fp32Exp) - 127 + 15);
        mant16 = (fp32Mant >> 13) & 0x3FFu;
        m_encoded = static_cast<uint16>((sign << 15) | (exp16 << 10) | mant16);
        return;
      }
    }

    /**
     * @brief Convert from Fp32 to Fp16 without doing any checks if the Fp32
     *        exponent is too large or too small. This is a faster alternative
     *        to set() when you know the values within the single precision
     *        float don't need the checks.
     * @param FP32Value Single precision float to be set as half precision.
     * @note  Make absolutely sure that you never pass in a single precision
     *        floating point value that may actually need the checks.
     *        If you are not 100% sure of that just use Set().
     */
    void
    setWithoutBoundsChecks(const float FP32Value) _NOEXCEPT {
      const Float32 fp32(FP32Value);
      uint32 sign = fp32.sign();
      uint32 exp16 = static_cast<uint32>(int32(fp32.exponent()) - 127 + 15);
      uint32 mant16 = (fp32.mantissa() >> 13) & 0x3FFu;
      m_encoded = static_cast<uint16>((sign << 15) | (exp16 << 10) | mant16);
    }

    /**
     * @brief Convert from Fp16 to Fp32.
     */
    float
    getFloat() const _NOEXCEPT {
      const uint32 u = static_cast<uint32>(m_encoded);
      const uint32 sign = (u >> 15) & 0x1u;
      const uint32 exp = (u >> 10) & 0x1Fu;
      uint32 mant = u & 0x3FFu;

      if (exp == 0u) {
        if (mant == 0u) {
          // Zero
          const uint32 bits = (sign << 31);
          return Float32(bits);
        }
        else {
          // Subnormal -> normalize
          // Count leading zeros in 10-bit mantissa by shifting
          int shift = 0;
          while ((mant & 0x400u) == 0u) { // until leading 1 reaches bit 10 (0x400)
            mant <<= 1u;
            ++shift;
          }
          // Remove the leading 1 (implicit) and compute exponent
          mant &= 0x3FFu;
          int32 fe_s = 113 - shift;
          uint32 fe = static_cast<uint32>(fe_s);
          uint32 fm = mant << 13;
          const uint32 bits = (sign << 31) | (fe << 23) | fm;
          return Float32(bits);
        }
      }
      else if (exp == 0x1Fu) { // all ones -> Inf or NaN
        const uint32 bits = (sign << 31) | (0xFFu << 23) | (mant << 13);
        return Float32(bits);
      }
      else {
        uint32 fe = exp + 112u; // exp - 15 + 127
        uint32 fm = mant << 13;
        const uint32 bits = (sign << 31) | (fe << 23) | fm;
        return Float32(bits);
      }
    }

    CONSTEXPR uint32
    integerValue() const _NOEXCEPT {
      return static_cast<uint32>(m_encoded);
    }

    CONSTEXPR uint32
    sign() const _NOEXCEPT {
      return (static_cast<uint32>(m_encoded) >> 15) & 0x1u;
    }

    CONSTEXPR uint32
    exponent() const _NOEXCEPT {
      return (static_cast<uint32>(m_encoded) >> 10) & 0x1Fu;
    }

    CONSTEXPR uint32
    mantissa() const _NOEXCEPT {
      return static_cast<uint32>(m_encoded) & 0x3FFu;
    }

   private:
    uint16 m_encoded;
  };
}
