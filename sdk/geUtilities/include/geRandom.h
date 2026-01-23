/*****************************************************************************/
/**
 * @file    geRandom.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/06
 * @brief   Generates pseudo random numbers using the Xorshift128 algorithm.
 *
 * Generates pseudo random numbers using the Xorshift128 algorithm.
 * Suitable for high performance requirements.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geNumericLimits.h"
#include "geVector2.h"
#include "geVector3.h"
#include "geMath.h"

namespace geEngineSDK {
  class GE_SCRIPT_EXPORT(m:Math) Random
  {
   public:
    /**
     * @brief Initializes a new generator using the specified seed.
     */
    GE_SCRIPT_EXPORT()
    Random(uint32 seed = 0) {
      setSeed(seed);
    }

    /**
     * @brief Changes the seed of the generator to the specified value.
     */
    GE_SCRIPT_EXPORT() void
    setSeed(uint32 seed) {
      m_seed[0] = seed;
      m_seed[1] = seed * 0x1496934D + 1; // Arbitrary random numbers
      m_seed[2] = seed * 0x22F46670 + 1;
      m_seed[3] = seed * 0x395DF948 + 1;
    }

    /**
     * @brief Returns a random value in range [0, NumLimits::MAX_UINT32].
     */
    GE_SCRIPT_EXPORT() uint32
    get() const {
      //Using xorshift128 algorithm
      uint32 t = m_seed[3];
      t ^= t << 11;
      t ^= t >> 8;

      m_seed[3] = m_seed[2];
      m_seed[2] = m_seed[1];
      m_seed[1] = m_seed[0];

      const uint32 s = m_seed[0];
      t ^= s;
      t ^= s >> 19;

      m_seed[0] = t;
      return t;
    }

    /**
     * @brief Returns a random value in range [min, max].
     */
    GE_SCRIPT_EXPORT() int32
    getRange(int32 min, int32 max) const {
      GE_ASSERT(max > min);

      //Note: Not using modulo for performance
      const int32 range = max - min + 1;
      CONSTEXPR static float DELTA = 0e-5f;
      return min + static_cast<int32>(getUNorm() * (static_cast<float>(range) - DELTA));
    }

    /**
     * @brief Returns a random value in range [0, 1].
     */
    GE_SCRIPT_EXPORT() float
    getUNorm() const {
      //Mask first 23 bits and divide by 2^23-1
      return static_cast<float>(get() & 0x007FFFFF) / 8388607.0f;
    }

    /**
     * @brief Returns a random value in range [-1, 1].
     */
    GE_SCRIPT_EXPORT() float
    getSNorm() const {
      return 2.0f * getUNorm() - 1.0f;
    }

    /**
     * @brief Returns a random unit vector in three dimensions.
     */
    GE_SCRIPT_EXPORT() Vector3
    getUnitVector() const {
      //Pick a random number on a unit cube and use the result only if squared
      //size less than 1. This is faster than most other methods, and generally
      //not many iterations are required to get the required vector.
      Vector3 output;
      float sqrdSize;

      do {
        output.x = getSNorm();
        output.y = getSNorm();
        output.z = getSNorm();
        sqrdSize = output.sizeSquared();
      } while (sqrdSize > 1.0f || sqrdSize < 0.001f);

      output.normalize();
      return output;
    }

    /**
     * @brief Returns a random unit vector in two dimensions.
     */
    GE_SCRIPT_EXPORT() Vector2
    getUnitVector2D() const {
      //Pick a random number on a unit square and use the result only if
      //squared size less than 1. This is faster than most other methods, and
      //generally not many iterations are required to get the required vector.
      Vector2 output;
      float sqrdSize;

      do {
        output.x = getSNorm();
        output.y = getSNorm();
        sqrdSize = output.sizeSquared();
      } while (sqrdSize > 1.0f || sqrdSize < 0.001f);

      output.normalize();
      return output;
    }

    /**
     * @brief Returns a random point inside a unit sphere.
     */
    GE_SCRIPT_EXPORT() Vector3
    getPointInSphere() const {
      const Vector3 dir = getUnitVector();
      return dir * Math::pow(getUNorm(), 1.0f / 3.0f);
    }

    /**
     * @brief Returns a random point inside the specified range in a sphere
     *        shell of unit radius, with the specified thickness,
     *        in range [0, 1]. Thickness of 0 will generate points on the
     *        sphere surface, while thickness of 1 will generate points within
     *        the entire sphere volume. Intermediate values represent the
     *        shell, which is a volume between two concentric spheres.
     */
    GE_SCRIPT_EXPORT() Vector3
    getPointInSphereShell(float thickness) const {
      const float minRadius = 1.0f - thickness;
      const Vector3 dir = getUnitVector();
      return dir * (minRadius + thickness * Math::pow(getUNorm(), 1.0f / 3.0f));
    }

    /**
     * @brief Returns a random point inside a unit circle.
     */
    GE_SCRIPT_EXPORT() Vector2
    getPointInCircle() const {
      const Vector2 dir = getUnitVector2D();
      return dir * Math::pow(getUNorm(), 1.0f / 2.0f);
    }

    /**
     * @brief Returns a random point inside the specified range in a circle
     *        shell of unit radius, with the specified thickness,
     *        in range [0, 1]. Thickness of 0 will generate points on the
     *        circle edge, while thickness of 1 will generate points within the
     *        entire circle surface. Intermediate values represent the shell,
     *        which is the surface between two concentric circles.
     */
    GE_SCRIPT_EXPORT() Vector2
    getPointInCircleShell(float thickness) const {
      const float minRadius = 1.0f - thickness;

      const Vector2 dir = getUnitVector2D();
      return dir * (minRadius + thickness * Math::pow(getUNorm(), 1.0f / 2.0f));
    }

    /**
     * @brief Returns a random point on a unit arc with the specified length
     *        (angle). Angle of 360 represents a circle.
     */
    GE_SCRIPT_EXPORT() Vector2
    getPointInArc(Degree angle) const {
      float val = getUNorm() * angle.valueRadians();
      return Vector2(Math::cos(val), Math::sin(val));
    }

    /**
     * @brief Returns a random point inside the specified range in an arc shell
     *        of unit radius, with the specified length (angle) and thickness
     *        in range [0, 1]. Angle of 360 represents a circle shell.
     *        Thickness of 0 will generate points on the arc edge, while
     *        thickness of 1 will generate points on the entire arc 'slice'.
     *        Intermediate values represent the shell, which is the surface
     *        between two concentric circles.
     */
    GE_SCRIPT_EXPORT() Vector2
    getPointInArcShell(Degree angle, float thickness) const {
      const float minRadius = 1.0f - thickness;
      const float val = getUNorm() * angle.valueRadians();
      const Vector2 dir(Math::cos(val), Math::sin(val));
      return dir * (minRadius + thickness * std::pow(getUNorm(), 1.0f / 2.0f));
    }

    /**
     * @brief Returns a random set of Barycentric coordinates that may be used
     *        for generating random points on a triangle.
     */
    GE_SCRIPT_EXPORT() Vector3
    getBarycentric() const {
      float u = getUNorm();
      float v = getUNorm();

      if ((u + v) > 1.0f) {
        u = 1.0f - u;
        v = 1.0f - v;
      }

      const float w = 1.0f - u - v;
      return Vector3(u, v, w);
    }

   private:
    mutable uint32 m_seed[4];
  };
}
