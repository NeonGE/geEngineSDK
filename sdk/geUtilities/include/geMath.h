/*****************************************************************************/
/**
 * @file    geMath.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/04
 * @brief   Utility class providing common scalar math operations.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

#ifndef _INC_MATRIX4_H_
# define _INC_MATRIX4_H_
#endif

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geDegree.h"
#include "geRadian.h"

namespace geEngineSDK {
  /**
   * @brief abs specialization template
   *        (this is done here because of standard compliant)
   */
  template<class T>
  FORCEINLINE T
  abs(const T A) {
    return (A >= (T)0) ? A : -A;
  }

  template<>
  FORCEINLINE float
  abs<float>(const float A) {
    return static_cast<float>(std::fabs(A));
  }

  namespace Implementation {
    /**
     * @brief Helper method for implementing variable-parameter Math::min.
     */
    template<typename T>
    const T&
    min(const T& in) {
      return in;
    }

    /**
     * @brief Helper method for implementing variable-parameter Math::min.
     */
    template<typename A, typename B>
    std::common_type_t<A, B>
    min(const A& a, const B& b) {
      return a < b ? a : b;
    }

    /**
     * @brief Helper method for implementing variable-parameter Math::min.
     */
    template<typename A, typename B, typename ...Args>
    std::common_type_t<A, B, Args...>
    min(const A& a, const B& b, const Args& ...args) {
      return min(min(a, b), min(args...));
    }

    /**
     * @brief Helper method for implementing variable-parameter Math::max.
     */
    template<typename T>
    const T&
    max(const T& in)
    {
      return in;
    }

    /** Helper method for implementing variable-parameter Math::max. */
    template<typename A, typename B>
    std::common_type_t<A, B>
    max(const A& a, const B& b) {
      return a > b ? a : b;
    }

    /** Helper method for implementing variable-parameter Math::max. */
    template<typename A, typename B, typename ...Args>
    std::common_type_t<A, B, Args...>
    max(const A& a, const B& b, const Args& ...args) {
      return max(max(a, b), max(args...));
    }
  }

  /**
   * @brief Utility class providing common scalar math operations.
   */
  class GE_UTILITIES_EXPORT Math
  {
   public:
    GE_NODISCARD static FORCEINLINE int32
    trunc(float F) {
      return static_cast<int32>(F);
    }

    GE_NODISCARD static FORCEINLINE float
    truncFloat(float F) {
      return static_cast<float>(trunc(F));
    }

    GE_NODISCARD static FORCEINLINE int32
    floor(float F) {
      return trunc(std::floor(F));
    }

    GE_NODISCARD static FORCEINLINE float
    floorFloat(float F) {
      return std::floor(F);
    }

    GE_NODISCARD static FORCEINLINE double
    floorDouble(double F) {
      return std::floor(F);
    }

    GE_NODISCARD static FORCEINLINE int32
    round(float F) {
      return trunc(std::round(F));
    }

    GE_NODISCARD static FORCEINLINE float
    roundFloat(float F) {
      return std::round(F);
    }

    GE_NODISCARD static FORCEINLINE double
    roundDouble(double F) {
      // /fixme optimize and consider that it needs to reurn a double
      return std::round(F);
    }
    
    GE_NODISCARD static FORCEINLINE int32
    ceil(float F) {
      return trunc(std::ceil(F));
    }

    GE_NODISCARD static FORCEINLINE float
    ceilFloat(float F) {
      return std::ceil(F);
    }

    GE_NODISCARD static FORCEINLINE double
    ceilDouble(double F) {
      return std::ceil(F);
    }

    GE_NODISCARD static FORCEINLINE float
    fractional(float Value) {
      return Value - truncFloat(Value);
    }

    GE_NODISCARD static FORCEINLINE float
    fmod(float X, float Y) {
      return std::fmod(X, Y);
    }

    GE_NODISCARD static FORCEINLINE float
    pow(float Base, float Exponent) {
      return std::pow(Base, Exponent);
    }

    GE_NODISCARD static FORCEINLINE float
    exp(float Value) {
      return std::exp(Value);
    }

    GE_NODISCARD static FORCEINLINE float
    logE(float Value) {
      return std::log(Value);
    }
    
    GE_NODISCARD static FORCEINLINE float
    log2(float Value) {
      return std::log(Value) / LOG2;
    }

    GE_NODISCARD static FORCEINLINE float
    logX(float Base, float Value) {
      return logE(Value) / logE(Base);
    }

    GE_NODISCARD static FORCEINLINE float
    sqrt(float Value) {
      return std::sqrt(Value);
    }

    GE_NODISCARD static FORCEINLINE Radian
    sqrt(const Radian& Value) {
      return Radian(sqrt(Value.valueRadians()));
    }

    GE_NODISCARD static FORCEINLINE Degree
    sqrt(const Degree& Value) {
      return Degree(sqrt(Value.valueDegrees()));
    }

    GE_NODISCARD static FORCEINLINE float
    invSqrt(float F) {
      return 1.0f / std::sqrt(F);
    }

    //Implement reverse of carmack's fast inv sqrt
    GE_NODISCARD static FORCEINLINE float
    carmackInvSqrt(float F) {
      float Half = 0.5f * F;
      int32 I = *reinterpret_cast<int32*>(&F);
      I = 0x5f3759df - (I >> 1);
      F = *reinterpret_cast<float*>(&I);
      F = F * (1.5f - Half * F * F);
      return F;
    }

    GE_NODISCARD static FORCEINLINE float
    invSqrtEst(float F) {
      return invSqrt(F);
    }

    static FORCEINLINE bool
    isNaN(float A) {
      return ((*reinterpret_cast<uint32*>(&A)) & 0x7F800000) == 0x7F800000 &&
             ((*reinterpret_cast<uint32*>(&A)) & 0x007FFFFF) != 0;
    }

    static FORCEINLINE bool
    isFinite(float A) {
      return ((*reinterpret_cast<uint32*>(&A)) & 0x7F800000) != 0x7F800000;
    }

    static FORCEINLINE bool
    isNegativeFloat(const float& F1) {
      return ((*(uint32*)&F1) >= static_cast<uint32>(0x80000000));
    }

    static FORCEINLINE bool
    isNegativeDouble(const double& A) {
      return ((*(uint64*)&A) >= (uint64)0x8000000000000000);
    }

    GE_NODISCARD static FORCEINLINE float
    cos(float Value) {
      return std::cos(Value);
    }

    GE_NODISCARD static FORCEINLINE float
    sin(float Value) {
      return std::sin(Value);
    }

    GE_NODISCARD static FORCEINLINE float
    tan(float Value) {
      return std::tan(Value);
    }

    GE_NODISCARD static FORCEINLINE float
    cos(const Radian& Value) {
      return std::cos(Value.valueRadians());
    }

    GE_NODISCARD static FORCEINLINE float
    sin(const Radian& Value) {
      return std::sin(Value.valueRadians());
    }

    GE_NODISCARD static FORCEINLINE float
    tan(const Radian& Value) {
      return std::tan(Value.valueRadians());
    }

    GE_NODISCARD static FORCEINLINE float
    cosh(float Value) {
      return std::cosh(Value);
    }

    GE_NODISCARD static FORCEINLINE float
    sinh(float Value) {
      return std::sinh(Value);
    }

    GE_NODISCARD static FORCEINLINE float
      tanh(float Value) {
      return std::tanh(Value);
    }

    GE_NODISCARD static Radian
    acos(float Value);

    GE_NODISCARD static Radian
    asin(float Value);

    GE_NODISCARD static FORCEINLINE Radian
    atan(float Value) {
      return Radian(std::atan(Value));
    }

    GE_NODISCARD static FORCEINLINE Radian
    atan2(float Y, float X) {
      return Radian(std::atan2(Y, X));
    }

    /**
     * @brief Generic Lineal Interpolation function
     * @param x0 Starting value
     * @param x1 Ending value
     * @param Alpha Time coefficient in the range of [0..1]
     * @return Value between x0 and x1 related to alpha
     */
    template<class T, class U>
    GE_NODISCARD static FORCEINLINE T
    lerp(const T& A, const T& B, const U& Alpha) {
      return static_cast<T>(A + Alpha * (B - A));
    }

    /**
     * @brief Determines the position of a value between two other values.
     * @return 0 if @p value is less or equal than @p min,
     *         1 if @p value is equal or greater than @p max,
     *         and value in range (0, 1) otherwise.
     */
    template<typename T>
    GE_NODISCARD static FORCEINLINE float
    invLerp(T val, T _min, T _max) {
      return clamp01((val - _min) / max(_max-_min, 0.0001F));
    }

    /**
     * @brief Performs a cubic interpolation
     * @param  P - end points
     * @param  T - tangent directions at end points
     * @param  Alpha - distance along spline
     * @return Interpolated value
     */
    template< class T, class U >
    GE_NODISCARD static T
    cubicInterp(const T& P0, const T& T0, const T& P1, const T& T1, const U& A) {
      const float A2 = A  * A;
      const float A3 = A2 * A;
      return static_cast<T>(((2 * A3) - (3 * A2) + 1) * P0) +
                            ((A3 - (2 * A2) + A) * T0) +
                            ((A3 - A2) * T1) + (((-2 * A3) + (3 * A2)) * P1);
    }

    template<class U>
    GE_NODISCARD static Rotator
    lerp(const Rotator& A, const Rotator& B, const U& Alpha);

    template<class U>
    GE_NODISCARD static Rotator
    lerpRange(const Rotator& A, const Rotator& B, const U& Alpha);

    template<class U>
    GE_NODISCARD static Quaternion
    lerp(const Quaternion& A, const Quaternion& B, const U& Alpha);

    template<class U>
    GE_NODISCARD static Quaternion
    biLerp(const Quaternion& P00,
           const Quaternion& P10,
           const Quaternion& P01,
           const Quaternion& P11,
           float FracX,
           float FracY);

    /**
     * @brief In the case of quaternions, we use a bezier like approach.
     * T - Actual 'control' orientations.
     */
    template<class U>
    GE_NODISCARD static Quaternion
    cubicInterp(const Quaternion& P0,
                const Quaternion& T0,
                const Quaternion& P1,
                const Quaternion& T1,
                const U& A);

    /**
     * @brief Divide two integers and rounds the result
     */
    template<class T>
    GE_NODISCARD static CONSTEXPR T
    divideAndRoundUp(T Dividend, T Divisor) {
      return (Dividend + Divisor - 1) / Divisor;
    }

    /**
     * @brief Divide two integers and rounds the result
     */
    template <class T>
    GE_NODISCARD static CONSTEXPR T
    divideAndRoundDown(T Dividend, T Divisor) {
      return Dividend / Divisor;
    }

    /**
     * @brief Returns the Square of a value
     */
    template<class T>
    GE_NODISCARD static FORCEINLINE T
    square(const T A) {
      return A*A;
    }

    /**
     * @brief Returns the lowest between two values
     */
    template<class T>
    GE_NODISCARD static FORCEINLINE T
    min(const T A, const T B) {
      return (A <= B) ? A : B;
    }

    /**
     * @brief Returns the highest between two values
     */
    template<class T>
    GE_NODISCARD static FORCEINLINE T
    max(const T A, const T B) {
      return (A >= B) ? A : B;
    }

    /**
     * @brief Returns the lowest between three values
     */
    template<class T>
    GE_NODISCARD static FORCEINLINE T
    min3(const T A, const T B, const T C) {
      return min(min(A, B), C);
    }

    /**
     * @brief Returns the highest between three values
     */
    template<class T>
    GE_NODISCARD static FORCEINLINE T
    max3(const T A, const T B, const T C) {
      return max(max(A, B), C);
    }

    /**
     * @brief Returns the minimum value of the two provided.
     */
    template<typename A, typename B>
    GE_NODISCARD static std::common_type_t<A, B>
    min(const A& a, const B& b) {
      return Implementation::min(a, b);
    }

    /**
     * @brief Returns the minimum value of all the values provided.
     */
    template<typename A, typename B, typename... Args>
    GE_NODISCARD static std::common_type_t<A, B, Args...>
    min(const A& a, const B& b, const Args&... args) {
      return Implementation::min(a, b, args...);
    }

    /**
     * @brief Returns the maximum value of the two provided.
     */
    template<typename A, typename B>
    GE_NODISCARD static std::common_type_t<A, B>
    max(const A& a, const B& b) {
      return Implementation::max(a, b);
    }

    /**
     * @brief Returns the maximum value of all the values provided.
     */
    template<typename A, typename B, typename... Args>
    GE_NODISCARD static std::common_type_t<A, B, Args...>
    max(const A& a, const B& b, const Args&... args) {
      return Implementation::max(a, b, args...);
    }

    /**
     * @brief Clamp a value within an inclusive range.
     */
    template<class T>
    GE_NODISCARD static FORCEINLINE T
    clamp(const T X, const T Min, const T Max) {
      return X < Min ? Min : X < Max ? X : Max;
    }

    /**
     * @brief Clamp a value within an inclusive range [0..1].
     */
    template<typename T>
    GE_NODISCARD static T
    clamp01(T val) {
      return max(min(val, static_cast<T>(1)), static_cast<T>(0));
    }

    /**
     * @brief Returns a floating point remainder for (@p val / @p length).
     */
    GE_NODISCARD static float
    repeat(float val, float length) {
      return val - floor(val / length) * length;
    }

    /**
     * @brief Wraps the value in range [0, length) and reverses the direction
     *        every @p length increment. This results in @p val incrementing
     *        until @p length, then decrementing back to 0, and so on.
     */
    static float
    pingPong(float val, float length) {
      val = repeat(val, length * 2.0f);
      return length - abs(val - length);
    }

    /**
     * @brief Returns the Absolute of a value
     */
    template<class T>
    static FORCEINLINE T
    abs(const T A) {
      return geEngineSDK::abs<T>(A);
    }

    /**
     * @copydoc Math::abs
     * @note    Specialization for Degree data
     */
    static FORCEINLINE Degree
    abs(const Degree& Value) {
      return Degree(std::fabs(Value.valueDegrees()));
    }

    /**
     * @copydoc Math::Abs
     * @note    Specialization for Radian data
     */
    static FORCEINLINE Radian
    abs(const Radian& Value) {
      return Radian(std::fabs(Value.valueRadians()));
    }

    /**
     * @brief Returns the sign of the value (-1, 0, 1)
     */
    template<class T>
    static FORCEINLINE T
    sign(const T A) {
      return (A > static_cast<T>(0)) ? static_cast<T>(1)
          : ((A < static_cast<T>(0)) ? static_cast<T>(-1) : static_cast<T>(0));
    }

    /**
     * @copydoc Math::sign
     * @note    Specialization for Degree values
     */
    static FORCEINLINE Degree
    sign(const Degree& val) {
      return Degree(sign(val.valueDegrees()));
    }

    /**
     * @copydoc Math::sign
     * @note    Specialization for Radian values
     */
    static FORCEINLINE Radian
    sign(const Radian& val) {
      return Radian(sign(val.valueRadians()));
    }

    /**
     * @brief Computes the base 2 logarithm for an integer value that is greater than 0.
     * The result is rounded down to the nearest integer.
     * @param Value The value to compute the log of
     * @return Log2 of Value. 0 if Value is 0.
     */
    static uint32
    floorLog2(uint32 Value);

    static FORCEINLINE uint64
    floorLog2_64(uint64 Value) {
      uint64 pos = 0;
      if (Value >= 1ull << 32) { Value >>= 32; pos += 32; }
      if (Value >= 1ull << 16) { Value >>= 16; pos += 16; }
      if (Value >= 1ull << 8) { Value >>= 8; pos += 8; }
      if (Value >= 1ull << 4) { Value >>= 4; pos += 4; }
      if (Value >= 1ull << 2) { Value >>= 2; pos += 2; }
      if (Value >= 1ull << 1) { pos += 1; }
      return (Value == 0) ? 0 : pos;
    }

    /**
     * @brief Returns the number of zeros before there is a value on the
     *        variable (how many bits are unused)
     */
    static uint32
    countLeadingZeros(uint32 Value);

    /**
    * @copydoc Math::countLeadingZeros(uint32)
    */
    static FORCEINLINE uint64
    countLeadingZeros64(uint64 Value) {
      if (Value == 0) return 64;
      return 63 - floorLog2_64(Value);
    }

    static uint32
    countTrailingZeros(uint32 Value);

    /**
     * @brief Returns the highest natural logarithm of the value
     */
    static FORCEINLINE uint32
    ceilLog2(uint32 Arg) {
      int32 Bitmask = (static_cast<int32>(countLeadingZeros(Arg) << 26)) >> 31;
      return (32 - countLeadingZeros(Arg - 1)) & (~Bitmask);
    }

    static FORCEINLINE uint64
    ceilLog2_64(uint64 Arg) {
      int64 Bitmask = ((int64)(countLeadingZeros64(Arg) << 57)) >> 63;
      return (64 - countLeadingZeros64(Arg - 1)) & (~Bitmask);
    }

    /**
     * @brief Rounds a given value to the immediate superior Power of 2
     */
    static FORCEINLINE uint32
    roundUpToPowerOfTwo(uint32 Arg) {
      return static_cast<uint32>(1 << ceilLog2(Arg));
    }

    /**
     * @brief Spreads bits to every other.
     */
    static FORCEINLINE uint32
    mortonCode2(uint32 x) {
      x &= 0x0000ffff;
      x = (x ^ (x << 8)) & 0x00ff00ff;
      x = (x ^ (x << 4)) & 0x0f0f0f0f;
      x = (x ^ (x << 2)) & 0x33333333;
      x = (x ^ (x << 1)) & 0x55555555;
      return x;
    }

    /**
     * @brief Reverses mortonCode2. Compacts every other bit to the right.
     */
    static FORCEINLINE uint32
    reverseMortonCode2(uint32 x) {
      x &= 0x55555555;
      x = (x ^ (x >> 1)) & 0x33333333;
      x = (x ^ (x >> 2)) & 0x0f0f0f0f;
      x = (x ^ (x >> 4)) & 0x00ff00ff;
      x = (x ^ (x >> 8)) & 0x0000ffff;
      return x;
    }

    /**
     * @brief Spreads bits to every 3rd.
     */
    static FORCEINLINE uint32
    mortonCode3(uint32 x) {
      x &= 0x000003ff;
      x = (x ^ (x << 16)) & 0xff0000ff;
      x = (x ^ (x << 8)) & 0x0300f00f;
      x = (x ^ (x << 4)) & 0x030c30c3;
      x = (x ^ (x << 2)) & 0x09249249;
      return x;
    }

    /**
     * @brief Reverses mortonCode3. Compacts every 3rd bit to the right.
     */
    GE_NODISCARD static FORCEINLINE uint32
    reverseMortonCode3(uint32 x) {
      x &= 0x09249249;
      x = (x ^ (x >> 2)) & 0x030c30c3;
      x = (x ^ (x >> 4)) & 0x0300f00f;
      x = (x ^ (x >> 8)) & 0xff0000ff;
      x = (x ^ (x >> 16)) & 0x000003ff;
      return x;
    }

    /**
     * @brief Returns value based on comparand. The main purpose of this function is to avoid
     * branching based on floating point comparison which can be avoided via compiler
     * intrinsics.
     * Please note that we don't define what happens in the case of NaNs as there might
     * be platform specific differences.
     * @param Comparand     Comparand the results are based on
     * @param ValueGEZero   Return value if Comparand >= 0
     * @param ValueLTZero   Return value if Comparand < 0
     * @return  ValueGEZero if Comparand >= 0, ValueLTZero otherwise
     */
    GE_NODISCARD static CONSTEXPR FORCEINLINE float
    floatSelect(float Comparand, float ValueGEZero, float ValueLTZero) {
      return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
    }

    /**
     * @brief Returns value based on comparand. The main purpose of this function is to avoid
     * branching based on floating point comparison which can be avoided via compiler
     * intrinsics.
     * Please note that we don't define what happens in the case of NaNs as there might
     * be platform specific differences.
     * @param	Comparand     Comparand the results are based on
     * @param	ValueGEZero   Return value if Comparand >= 0
     * @param ValueLTZero   Return value if Comparand < 0
     * @return  ValueGEZero if Comparand >= 0, ValueLTZero otherwise
     */
    GE_NODISCARD static CONSTEXPR FORCEINLINE double
    floatSelect(double Comparand, double ValueGEZero, double ValueLTZero) {
      return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
    }

    /**
     * @brief Checks if the value is between the range. (MaxValue Exclusive)
     */
    template<class U>
    GE_NODISCARD static FORCEINLINE bool
    isWithin(const U& TestValue, const U& MinValue, const U& MaxValue) {
      return ((TestValue >= MinValue) && (TestValue < MaxValue));
    }

    /**
     * @brief Checks if the value is between the range. (MaxValue Inclusive)
     */
    template<class U>
    GE_NODISCARD static FORCEINLINE bool
    isWithinInclusive(const U& TestValue, const U& MinValue, const U& MaxValue) {
      return ((TestValue >= MinValue) && (TestValue <= MaxValue));
    }

    /**
     * @brief Compare 2 floats, using tolerance for inaccuracies.
     * @param A First comparing float
     * @param B Second comparing float
     * @param ErrorTolerance Max allowed difference to be considered Nearly Equal
     * @return true if A and B are Nearly Equal
     */
    GE_NODISCARD static FORCEINLINE bool
    isNearlyEqual(float A, float B, float ErrorTolerance = SMALL_NUMBER) {
      return abs<float>(A - B) < ErrorTolerance;
    }

    /**
     * @brief Compare 2 doubles, using tolerance for inaccuracies.
     * @param A First comparing doubles
     * @param B Second comparing doubles
     * @param ErrorTolerance Max allowed difference to be considered Nearly Equal
     * @return true if A and B are Nearly Equal
     */
    GE_NODISCARD static FORCEINLINE bool
    isNearlyEqual(double A, double B, double ErrorTolerance = SMALL_NUMBER) {
      return abs<double>(A - B) < ErrorTolerance;
    }

    /**
     * @brief	Compare if a value is Nearly Zero, using tolerance for inaccuracies.
     * @param	A Value to test
     * @param	ErrorTolerance Max allowed difference to be considered Zero
     * @return	true if A is Nearly Zero
     */
    GE_NODISCARD static FORCEINLINE bool
    isNearlyZero(float Value, float ErrorTolerance = SMALL_NUMBER) {
      return abs<float>(Value) < ErrorTolerance;
    }

    /**
     * @brief Compare if a value is Nearly Zero, using tolerance for inaccuracies.
     * @param A Value to test
     * @param ErrorTolerance Max allowed difference to be considered Zero
     * @return true if A is Nearly Zero
     */
    GE_NODISCARD static FORCEINLINE bool
    isNearlyZero(double Value, double ErrorTolerance = SMALL_NUMBER) {
      return abs<double>(Value) < ErrorTolerance;
    }

    /**
     * @brief Checks is the specified value a power of two. Only works on integer values.
     */
    GE_NODISCARD static FORCEINLINE bool
    isPowerOfTwo(uint32 Value) {
      return (0 == (Value & (Value - 1)));
    }

    /**
     * @brief Returns the closest Grid multiply to the Location
     */
    GE_NODISCARD static FORCEINLINE float
    gridSnap(float Location, float Grid) {
      if (0.f == Grid)
        return Location;

      return floor((Location + 0.5f*Grid) / Grid)*Grid;
    }

    /**
     * @brief Computes the sine and cosine of a scalar float.
     * @param ScalarSin	Pointer to where the sin result should be stored
     * @param ScalarCos	Pointer to where the cos result should be stored
     * @param Value input angles
     */
    static FORCEINLINE void
    sin_cos(float* ScalarSin, float* ScalarCos, float Value) {
      //Map Value to y in [-pi, pi], x = 2*pi*quotient + remainder.
      float quotient = (INV_PI * 0.5f) * Value;
      if (0.0f <= Value) {
        quotient = static_cast<float>(static_cast<int>(quotient + 0.5f));
      }
      else {
        quotient = static_cast<float>(static_cast<int>(quotient - 0.5f));
      }
      
      float y = Value - TWO_PI * quotient;

      //Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
      float fSign;
      if (HALF_PI < y) {
        y = PI - y;
        fSign = -1.0f;
      }
      else if (-HALF_PI > y) {
        y = -PI - y;
        fSign = -1.0f;
      }
      else {
        fSign = +1.0f;
      }

      float y2 = y * y;

      //11-degree minimax approximation
      *ScalarSin = ((( ((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f)
                   * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

      //10-degree minimax approximation
      float p = (( ((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f)
                * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
      
      *ScalarCos = fSign * p;
    }

    /**
     * @brief Calculates the tangent space vector for a given set of
     *        positions / texture coords.
     */
    GE_NODISCARD static Vector3
    calculateTriTangent(const Vector3& position1,
                        const Vector3& position2,
                        const Vector3& position3,
                        float u1,
                        float v1,
                        float u2,
                        float v2,
                        float u3,
                        float v3);


    /**
     * @brief Sine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    GE_NODISCARD static float
    fastSin0(const Radian& Value) {
      return static_cast<float>(fastASin0(Value.valueRadians()));
    }

    /**
     * @brief Sine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    GE_NODISCARD static float
    fastSin0(float Value);

    /**
     * @brief Sine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastSin0".
     */
    GE_NODISCARD static float
    fastSin1(const Radian& Value) {
      return static_cast<float>(fastASin1(Value.valueRadians()));
    }

    /**
     * @brief Sine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastSin0".
     */
    GE_NODISCARD static float
    fastSin1(float Value);

    /**
     * @brief Cosine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    GE_NODISCARD static float
    fastCos0(const Radian& Value) {
      return static_cast<float>(fastACos0(Value.valueRadians()));
    }

    /**
     * @brief Cosine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    GE_NODISCARD static float
    fastCos0(float Value);

    /**
     * @brief Cosine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastCos0".
     */
    GE_NODISCARD static float
    fastCos1(const Radian& Value) {
      return static_cast<float>(fastACos1(Value.valueRadians()));
    }

    /**
     * @brief Cosine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastCos0".
     */
    GE_NODISCARD static float
    fastCos1(float Value);

    /**
     * @brief Tangent function approximation.
     * @param Value Angle in range [0, pi/4].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    GE_NODISCARD static float
    fastTan0(const Radian& Value) {
      return static_cast<float>(fastATan0(Value.valueRadians()));
    }

    /**
     * @brief Tangent function approximation.
     * @param Value	Angle in range [0, pi/4].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    GE_NODISCARD static float
    fastTan0(float Value);

    /**
     * @brief Tangent function approximation.
     * @param val Angle in range [0, pi/4].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastTan0".
     */
    GE_NODISCARD static float
    fastTan1(const Radian& Value) {
      return static_cast<float>(fastATan1(Value.valueRadians()));
    }

    /**
     * @brief Tangent function approximation.
     * @param Value Angle in range [0, pi/4].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastTan0".
     */
    GE_NODISCARD static float
    fastTan1(float Value);

    /**
     * @brief Inverse sine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    GE_NODISCARD static float
    fastASin0(const Radian& Value) {
      return static_cast<float>(fastASin0(Value.valueRadians()));
    }

    /**
     * @brief Inverse sine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    GE_NODISCARD static float
    fastASin0(float Value);

    /**
     * @brief Inverse sine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastASin0".
     */
    GE_NODISCARD static float
    fastASin1(const Radian& Value) {
      return static_cast<float>(fastASin1(Value.valueRadians()));
    }

    /**
     * @brief Inverse sine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "FastASin0".
     */
    GE_NODISCARD static float
    fastASin1(float Value);

#define FASTASIN_HALF_PI (1.5707963050f)
    /**
     * @brief Computes the ASin of a scalar value.
     * @param Value  input angle
     * @return ASin of Value
     */
    GE_NODISCARD static Radian
    fastAsin(float Value) {
      //Clamp input to [-1,1].
      bool nonnegative = (0.0f <= Value);
      float x = Math::abs(Value);
      float omx = 1.0f - x;
      if (omx < 0.0f) {
        omx = 0.0f;
      }
      
      float root = Math::sqrt(omx);
      //7-degree minimax approximation
      float result = ((((((-0.0012624911f * x + 0.0066700901f) *
                                            x - 0.0170881256f) *
                                            x + 0.0308918810f) *
                                            x - 0.0501743046f) *
                                            x + 0.0889789874f) *
                                            x - 0.2145988016f) *
                                            x + FASTASIN_HALF_PI;
      result *= root;  //acos(|x|)
      return Radian(nonnegative ? FASTASIN_HALF_PI - result :
                                  result - FASTASIN_HALF_PI);
    }
#undef FASTASIN_HALF_PI

    /**
     * @brief Inverse cosine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    GE_NODISCARD static float
    fastACos0(const Radian& Value) {
      return static_cast<float>(fastACos0(Value.valueRadians()));
    }

    /**
     * @brief Inverse cosine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    GE_NODISCARD static float
    fastACos0(float Value);

    /**
     * @brief Inverse cosine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastACos0".
     */
    GE_NODISCARD static float
    fastACos1(const Radian& Value) {
      return static_cast<float>(fastACos1(Value.valueRadians()));
    }

    /**
     * @brief Inverse cosine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastACos0".
     */
    GE_NODISCARD static float
    fastACos1(float Value);

    /**
     * @brief Inverse tangent function approximation.
     * @param Value Angle in range [-1, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    GE_NODISCARD static float
    fastATan0(const Radian& Value) {
      return static_cast<float>(fastATan0(Value.valueRadians()));
    }

    /**
     * @brief Inverse tangent function approximation.
     * @param Value Angle in range [-1, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    GE_NODISCARD static float
    fastATan0(float Value);

    /**
     * @brief Inverse tangent function approximation.
     * @param Value Angle in range [-1, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastATan0".
     */
    GE_NODISCARD static float
    fastATan1(const Radian& Value) {
      return static_cast<float>(fastATan1(Value.valueRadians()));
    }

    /**
     * @brief Inverse tangent function approximation.
     * @param Value Angle in range [-1, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "FastATan0".
     */
    GE_NODISCARD static float
    fastATan1(float Value);

    /**
     * @brief Utility to ensure angle is between +/- 180 degrees by unwinding.
     */
    GE_NODISCARD static float
    unwindDegrees(float A) {
      while (180.f < A) {
        A -= 360.f;
      }

      while (-180.f > A) {
        A += 360.f;
      }

      return A;
    }

    /**
     * @brief Given a heading which may be outside the +/- PI range, 'unwind'
     *        it back into that range.
     */
    GE_NODISCARD static float
    unwindRadians(float A) {
      while (PI < A) {
        A -= TWO_PI;
      }

      while (-PI > A) {
        A += TWO_PI;
      }

      return A;
    }

    /**
     * @brief Solves the linear equation with the parameters A, B.
     *        Returns number of roots found and the roots themselves will be
     *        output in the @p roots array.
     * @param[out] roots Must be at least size of 1.
     * @note  Only returns real roots.
     */
    template<typename T>
    GE_NODISCARD static uint32
    solveLinear(T A, T B, T* roots) {
      if (!isNearlyEqual(A, (T)0)) {
        roots[0] = -B / A;
        return 1;
      }

      roots[0] = 0.0f;
      return 1;
    }

    /**
     * @brief Solves the quadratic equation with the parameters A, B, C.
     *        Returns number of roots found and the roots themselves will be
     *        output in the @p roots array.
     * @param[out] roots Must be at least size of 2.
     * @note Only returns real roots.
     */
    template<typename T>
    GE_NODISCARD static uint32
    solveQuadratic(T A, T B, T C, T* roots) {
      if (!isNearlyEqual(A, (T)0)) {
        T p = B / (2 * A);
        T q = C / A;
        T D = p * p - q;

        if (!isNearlyEqual(D, (T)0)) {
          if (static_cast<T>(0) > D) {
            return 0;
          }

          T sqrtD = sqrt(D);
          roots[0] = sqrtD - p;
          roots[1] = -sqrtD - p;
          return 2;
        }
        else {
          roots[0] = -p;
          roots[1] = -p;
          return 1;
        }
      }

      return solveLinear(B, C, roots);
    }

    /**
     * @brief Solves the cubic equation with the parameters A, B, C, D.
     *        Returns number of roots found and the roots themselves will be
     *        output in the @p roots array.
     * @param[out] roots Must be at least size of 3.
     * @note Only returns real roots.
     */
    template<typename T>
    GE_NODISCARD static uint32
    solveCubic(T A, T B, T C, T D, T* roots) {
      static const T THIRD = (1 / static_cast<T>(3));

      T invA = 1 / A;
      A = B * invA;
      B = C * invA;
      C = D * invA;

      T sqA = square(A);
      T p = THIRD * (-THIRD * sqA + B);
      T q = (static_cast<T>(0.5)) * ((2 / static_cast<T>(27)) * A * sqA - THIRD * A * B + C);

      T cbp = p * p * p;
      D = q * q + cbp;

      uint32 numRoots = 0;
      if (!isNearlyEqual(D, (T)0)) {
        if (0.0 > D) {
          T phi = THIRD * Acos(-q / sqrt(-cbp));
          T t = 2 * sqrt(-p);

          roots[0] =  t * cos(phi);
          roots[1] = -t * cos(phi + PI * THIRD);
          roots[2] = -t * cos(phi - PI * THIRD);

          numRoots = 3;
        }
        else {
          T sqrtD = sqrt(D);
          T u = cbrt(sqrtD + abs(q));

          if (static_cast<T>(0) < q) {
            roots[0] = -u + p / u;
          }
          else {
            roots[0] = u - p / u;
          }
          numRoots = 1;
        }
      }
      else {
        if (!isNearlyEqual(q, (T)0)) {
          T u = cbrt(-q);
          roots[0] = 2 * u;
          roots[1] = -u;
          numRoots = 2;
        }
        else {
          roots[0] = 0.0f;
          numRoots = 1;
        }
      }

      T sub = THIRD * A;
      for (uint32 i = 0; i<numRoots; ++i) {
        roots[i] -= sub;
      }

      return numRoots;
    }

    /**
     * @brief Solves the quartic equation with the parameters A, B, C, D, E. 
     *        Returns number of roots found and the roots themselves will be
     *        output in the @p roots array.
     * @param[out] roots	Must be at least size of 4.
     * @note Only returns real roots.
     */
    template<typename T>
    GE_NODISCARD static uint32
    solveQuartic(T A, T B, T C, T D, T E, T* roots) {
      T invA = 1 / A;
      A = B * invA;
      B = C * invA;
      C = D * invA;
      D = E * invA;

      T sqA = square(A);
      T p = -(3 / static_cast<T>(8))   * sqA + B;
      T q =  (1 / static_cast<T>(8))   * sqA * A - static_cast<T>(0.5) * A * B + C;
      T r = -(3 / static_cast<T>(256)) * sqA * sqA + (1 / static_cast<T>(16)) * sqA * B - 
             (1 / static_cast<T>(4)) * A * C + D;

      uint32 numRoots = 0;
      if (!isNearlyEqual(r, static_cast<T>(0))) {
        T cubicA = 1;
        T cubicB = -(T)0.5 * p;
        T cubicC = -r;
        T cubicD = (T)0.5 * r * p - (1 / (T)8) * q * q;

        solveCubic(cubicA, cubicB, cubicC, cubicD, roots);
        T z = roots[0];

        T u = z * z - r;
        T v = 2 * z - p;

        if (isNearlyEqual(u, static_cast<T>(0))) {
          u = 0;
        }
        else if (0 < u) {
          u = sqrt(u);
        }
        else {
          return 0;
        }

        if (isNearlyEqual(v, static_cast<T>(0))) {
          v = 0;
        }
        else if (0 < v) {
          v = sqrt(v);
        }
        else {
          return 0;
        }

        T quadraticA = 1;
        T quadraticB = q < 0 ? -v : v;
        T quadraticC = z - u;

        numRoots = solveQuadratic(quadraticA, quadraticB, quadraticC, roots);

        quadraticA = 1;
        quadraticB = q < 0 ? v : -v;
        quadraticC = z + u;

        numRoots += solveQuadratic(quadraticA, quadraticB, quadraticC, roots + numRoots);
      }
      else {
        numRoots = solveCubic(q, p, (T)0, (T)1, roots);
        roots[numRoots++] = 0;
      }

      T sub = (1 / static_cast<T>(4)) * A;
      for (uint32 i = 0; i < numRoots; ++i) {
        roots[i] -= sub;
      }

      return numRoots;
    }

    /**
     * @brief Evaluates a cubic Hermite curve at a specific point.
     * @param[in] t         Parameter that at which to evaluate the curve, in range [0, 1].
     * @param[in] pointA    Starting point (at t=0).
     * @param[in] pointB    Ending point (at t=1).
     * @param[in] tangentA  Starting tangent (at t=0).
     * @param[in] tangentB  Ending tangent (at t = 1).
     * @return  Evaluated value at @p t.
     */
    template<class T>
    GE_NODISCARD static T
    cubicHermite(float t,
                 const T& pointA,
                 const T& pointB,
                 const T& tangentA,
                 const T& tangentB) {
      float t2 = t * t;
      float t3 = t2 * t;

      float a = 2 * t3 - 3 * t2 + 1;
      float b = t3 - 2 * t2 + t;
      float c = -2 * t3 + 3 * t2;
      float d = t3 - t2;

      return a * pointA + b * tangentA + c * pointB + d * tangentB;
    }

    /**
     * @brief Evaluates the first derivative of a cubic Hermite curve at a specific point.
     * @param[in] t         Parameter that at which to evaluate the curve, in range [0, 1].
     * @param[in] pointA    Starting point (at t=0).
     * @param[in] pointB    Ending point (at t=1).
     * @param[in] tangentA  Starting tangent (at t=0).
     * @param[in] tangentB  Ending tangent (at t = 1).
     * @return  Evaluated value at @p t.
     */
    template<class T>
    GE_NODISCARD static T
    cubicHermiteD1(float t,
                   const T& pointA,
                   const T& pointB,
                   const T& tangentA,
                   const T& tangentB) {
      float t2 = t * t;

      float a = 6 * t2 - 6 * t;
      float b = 3 * t2 - 4 * t + 1;
      float c = -6 * t2 + 6 * t;
      float d = 3 * t2 - 2 * t;

      return a * pointA + b * tangentA + c * pointB + d * tangentB;
    }

    /**
     * @brief Calculates coefficients needed for evaluating a cubic curve in Hermite form.
     *        Assumes @p t has been normalized is in range [0, 1].
     *        Tangents must be scaled by the length of the curve (length is the
     *        maximum value of @p t before it was normalized).
     * @param[in] pointA    Starting point (at t=0).
     * @param[in] pointB    Ending point (at t=1).
     * @param[in] tangentA  Starting tangent (at t=0).
     * @param[in] tangentB  Ending tangent (at t = 1).
     * @param[out]  coefficients  Four coefficients for the cubic curve,
     *              in order [t^3, t^2, t, 1].
     */
    template<class T>
    static void
    cubicHermiteCoefficients(const T& pointA,
                             const T& pointB,
                             const T& tangentA,
                             const T& tangentB,
                             T(&coefficients)[4]) {
      T diff = pointA - pointB;
      coefficients[0] = 2 * diff + tangentA + tangentB;
      coefficients[1] = -3 * diff - 2 * tangentA - tangentB;
      coefficients[2] = tangentA;
      coefficients[3] = pointA;
    }

    /**
     * @brief Calculates coefficients needed for evaluating a cubic curve in
     *        Hermite form. Assumes @p t is in range [0, @p length].
     *        Tangents must not be scaled by @p length.
     * @param[in] pointA    Starting point (at t=0).
     * @param[in] pointB    Ending point (at t=length).
     * @param[in] tangentA  Starting tangent (at t=0).
     * @param[in] tangentB  Ending tangent (at t=length).
     * @param[in] length    Maximum value the curve will be evaluated at.
     * @param[out]  coefficients  Four coefficients for the cubic curve,
     *              in order [t^3, t^2, t, 1].
     */
    template<class T>
    static void
    cubicHermiteCoefficients(const T& pointA,
                             const T& pointB,
                             const T& tangentA,
                             const T& tangentB,
                             float length,
                             T(&coefficients)[4]) {
      float length2 = length * length;
      float invLength2 = 1.0f / length2;
      float invLength3 = 1.0f / (length2 * length);

      T scaledTangentA = tangentA * length;
      T scaledTangentB = tangentB * length;

      T diff = pointA - pointB;

      coefficients[0] = (2 * diff + scaledTangentA + scaledTangentB) * invLength3;
      coefficients[1] = (-3 * diff - 2 * scaledTangentA - scaledTangentB) * invLength2;
      coefficients[2] = tangentA;
      coefficients[3] = pointA;
    }

    /**
     * @brief Calculates the Romberg Integration.
     * @param[in] a         Lower bound.
     * @param[in] b         Upper bound.
     * @param[in] order     Order of the function.
     * @param[in] integrand Function to integrate.
     * @return  Integrated function.
     */
    template<typename T>
    GE_NODISCARD static T
    rombergIntegration(T a, T b, int32 order, const std::function<T(T)> integrand) {
      Vector<T> h(order + 1);
      Vector<Vector<T>> r(order + 1, Vector<T>(order + 1));

      for (int32 i = 1; i < order + 1; ++i) {
        h[i] = (b - a) / Math::pow(2, i - 1);
      }

      r[1][1] = h[1] / 2 * (integrand(a) + integrand(b));

      for (int32 i = 2; i < order + 1; ++i) {
        T coeff = 0;
        for (int32 k = 1; k <= Math::pow(2, i - 2); ++k) {
          coeff += integrand(a + (2 * k - 1) * h[i]);
        }
        r[i][1] = 0.5 * (r[i - 1][1] + h[i - 1] * coeff);
      }

      for (int32 i = 2; i < order + 1; ++i) {
        for (int j = 2; j <= i; ++j)
          r[i][j] = r[i][j - 1] + (r[i][j - 1] - r[i - 1][j - 1]) / (Math::pow(4, j - 1) - 1);
      }

      return r[order][order];
    }

    /**
     * @brief Calculates the Gaussian Quadrature.
     * @param[in] a Lower bound.
     * @param[in] b Upper bound.
     * @param[in] roots Roots of the function.
     * @param[in] coefficients  Coefficients of the function.
     * @param[in] integrand Function to integrate.
     * @return  Gaussian Quadrature integration.
     */
    template<typename T>
    GE_NODISCARD static T
    gaussianQuadrature(T a,
                       T b,
                       const T* roots,
                       const T* coefficients,
                       size_t count,
                       const std::function<T(T)>& integrand) {
      const T half = static_cast<T>(0.5);
      const T radius = half * (b - a);
      const T center = half * (b + a);
      T res = static_cast<T>(0);

      for (size_t i = 0; i < count; ++i)
      {
        res += coefficients[i] * integrand(radius * roots[i] + center);
      }

      return res * radius;
    }

    /**
     * @brief Checks if two 2D lines intersect
     * @return true if the lines intersect, false otherwise
     */
    GE_NODISCARD static bool
    lineLineIntersection(const Vector2& aa,
                         const Vector2& ab,
                         const Vector2& ba,
                         const Vector2& bb);

    /**
     * @brief Get the point where two 2D lines intersect
     * @return Vector2 describing the point of intersection
     */
    GE_NODISCARD static Vector2
    getLineLineIntersect(const Vector2& aa,
                         const Vector2& ab,
                         const Vector2& ba,
                         const Vector2& bb);

    /**
     * @brief Find the intersection of a line and an offset plane. Assumes that
     *        the line and plane do indeed intersect; you must make sure they're
     *        not parallel before calling.
     * @param Point1 the first point defining the line
     * @param Point2 the second point defining the line
     * @param PlaneOrigin the origin of the plane
     * @param PlaneNormal the normal of the plane
     * @return The point of intersection between the line and the plane.
     */
    GE_NODISCARD static Vector3
    linePlaneIntersection(const Vector3& Point1,
                          const Vector3& Point2,
                          const Vector3& PlaneOrigin,
                          const Vector3& PlaneNormal);

    /**
     * @brief Find the intersection of a line and a plane. Assumes that the
     *        line and plane do indeed intersect; you must make sure they're
     *        not parallel before calling.
     * @param Point1 the first point defining the line
     * @param Point2 the second point defining the line
     * @param plane the Plane
     * @return The point of intersection between the line and the plane.
     */
    GE_NODISCARD static Vector3
    linePlaneIntersection(const Vector3& Point1,
                          const Vector3& Point2,
                          const Plane& plane);

    /**
     * @brief Find the point on the line segment from LineStart to LineEnd
     *        which is closest to Point
     */
    GE_NODISCARD static Vector3
    closestPointOnLine(const Vector3& LineStart,
                       const Vector3& LineEnd,
                       const Vector3& Point);

    /**
     * @brief Determine if a plane and an AABB intersect
     * @param P - the plane to test
     * @param AABB - the axis aligned bounding box to test
     * @return if collision occurs
     */
    GE_NODISCARD static bool
    planeAABBIntersection(const Plane& InP, const AABox& AABB);

    /**
     * @brief Checks if the Point intersects with the Box
     * @param Point the point
     * @param Box the Box to test against
     * @return true if the point intersects with the box false otherwise
     */
    GE_NODISCARD static bool
    pointBoxIntersection(const Vector3& Point, const AABox& Box);

    /**
     * @brief Checks if a line intersects with a box
     * @param Box the Box to test against
     * @param Start the starting position of the line
     * @param End the end position of the line
     * @param Direction Direction of the testing vector
     * @return true if the line intersects with the box false otherwise
     */
    GE_NODISCARD static bool
    lineBoxIntersection(const AABox& Box,
                        const Vector3& Start,
                        const Vector3& End,
                        const Vector3& Direction);

    /**
     * @brief Checks if a line intersects with a box
     * @param Box the Box to test against
     * @param Start the starting position of the line
     * @param End the end position of the line
     * @param Direction Direction of the testing vector
     * @param OneOverDirection Reciprocal direction of the testing
     * @return true if the line intersects with the box false otherwise
     */
    GE_NODISCARD static bool
    lineBoxIntersection(const AABox& Box,
                        const Vector3& Start,
                        const Vector3& End,
                        const Vector3& Direction,
                        const Vector3& OneOverDirection);

    GE_NODISCARD static bool
    lineSphereIntersection(const Vector3& Start,
                           const Vector3& Dir,
                           float Length,
                           const Vector3& Origin,
                           float Radius);

    GE_NODISCARD static bool
    intersectPlanes2(Vector3& I, Vector3& D, const Plane& P1, const Plane& P2);

    GE_NODISCARD static bool
    intersectPlanes3(Vector3& I, const Plane& P1, const Plane& P2, const Plane& P3);

    GE_NODISCARD static float
    getRangePct(Vector2 const& Range, float Value);

    GE_NODISCARD static float
    getRangeValue(Vector2 const& Range, float Pct);

    /**
     * @brief Performs a sphere vs box intersection test using Arvo's algorithm:
     *  for each i in (x, y, z)
     *    if(SphereCenter(i) < BoxMin(i)) d2+=(SphereCenter(i)-BoxMin(i))^2
     *		elif(SphereCenter(i) > BoxMax(i)) d2+=(SphereCenter(i)-BoxMax(i))^2
     * @param Sphere the center of the sphere being tested against the AABB
     * @param RadiusSquared the size of the sphere being tested
     * @param AABB the box being tested against
     * @return Whether the sphere/box intersect or not.
     */
    GE_NODISCARD static bool
    sphereAABBIntersection(const Vector3& SphereCenter,
                           const float RadiusSquared,
                           const AABox& AABB);

    /**
     * @brief Converts a sphere into a point plus radius squared for the test above
     */
    GE_NODISCARD static bool
    sphereAABBIntersection(const Sphere& InSphere, const AABox& AABB);

   public:
    static const float PI;
    static const float INV_PI;
    static const float HALF_PI;
    static const float TWO_PI;

    static const float EULERS_NUMBER;

    static const float SMALL_NUMBER;
    static const float KINDA_SMALL_NUMBER;
    static const float BIG_NUMBER;

    static const float DEG2RAD;
    static const float RAD2DEG;
    static const float LOG2;

    static const float DELTA;

    /**
     * Lengths of normalized vectors (These are half their maximum values
     * to assure that dot products with normalized vectors don't overflow).
     */
    static const float FLOAT_NORMAL_THRESH;

    /**
     * Magic numbers for numerical precision.
     */

    /**
     * Thickness of plane for front/back/inside test
     */
    static const float THRESH_POINT_ON_PLANE;

    /**
     * Thickness of polygon side's side-plane for point-inside/outside/on side
     * test
     */
    static const float THRESH_POINT_ON_SIDE;

    /**
     * Two points are same if within this distance
     */
    static const float THRESH_POINTS_ARE_SAME;

    /**
     * Two points are near if within this distance and can be combined if
     * imprecise math is ok
     */
    static const float THRESH_POINTS_ARE_NEAR;

    /**
     * Two normal points are same if within this distance
     */
    static const float THRESH_NORMALS_ARE_SAME;

    /**
     * Two UV are same if within this threshold (1.0f/1024f)
     */
    static const float THRESH_UVS_ARE_SAME;

    /*************************************************************************/
    //Making this too large results in wrong CSG classification and disaster
    /**
     * Two vectors are near if within this distance and can be combined if
     * imprecise math is ok
     */
    static const float THRESH_VECTORS_ARE_NEAR;
    /*************************************************************************/

    /**
     * A plane splits a polygon in half
     */
    static const float THRESH_SPLIT_POLY_WITH_PLANE;

    /**
     * A plane exactly splits a polygon
     */
    static const float THRESH_SPLIT_POLY_PRECISELY;

    /**
     * Size of a unit normal that is considered "zero", squared
     */
    static const float THRESH_ZERO_NORM_SQUARED;

    /**
     * Two unit vectors are parallel if abs(A dot B) is greater than or equal
     * to this.
     * This is roughly cosine(1.0 degrees).
     */
    static const float THRESH_NORMALS_ARE_PARALLEL;

    /**
     * Two unit vectors are orthogonal (perpendicular) if abs(A dot B) is less
     * than or equal this. This is roughly cosine(89.0 degrees).
     */
    static const float THRESH_NORMALS_ARE_ORTHOGONAL;

    /**
     * Allowed error for a normalized vector (against squared magnitude)
     */
    static const float THRESH_VECTOR_NORMALIZED;
    
    /**
     * Allowed error for a normalized quaternion (against squared magnitude)
     */
    static const float THRESH_QUAT_NORMALIZED;

    /**
     * 
     */
    static const float ZERO_ANIMWEIGHT_THRESH;

  };
}
