/*****************************************************************************/
/**
 * @file    geColor.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/08
 * @brief   Color structures for use on the geEngineSDK
 *
 * Color represented as 4 components, each being a floating point value ranging
 * from 0 to 1.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

#ifndef _INC_COLOR_H_
# define _INC_COLOR_H_
#endif // !_INC_COLOR_H_

/*****************************************************************************/
/**
* Includes
*/
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geMath.h"

namespace geEngineSDK {
  class Float16Color;
  class Vector3;
  class Vector4;

  namespace GAMMA_SPACE {
    enum E {
      kLinear,
      kPow22,
      ksRGB
    };
  }

  /**
   * @brief Color represented as 4 components, each being a floating point
   *        value ranging from 0 to 1.
   *        Color components are Red, Green, Blue and Alpha.
   */
  class GE_UTILITIES_EXPORT LinearColor
  {
   public:
    LinearColor() = default;
    explicit LinearColor(FORCE_INIT::E) : r(0), g(0), b(0), a(0) {}
    LinearColor(float InR, float InG, float InB, float InA = 1.0f) 
      : r(InR),
        g(InG),
        b(InB),
        a(InA) {}

    explicit LinearColor(const Vector3& V);
    explicit LinearColor(const Vector4& V);
    
    /**
     * @brief Color->LinearColor conversion.
     */
    explicit LinearColor(const Color& refColor);

    /**
     * @brief Float16Color->LinearColor conversion.
     */
    explicit LinearColor(const Float16Color& refColor);

    /**
     * @brief Pointer accessor for direct copying.
     */
    float*
    ptr() {
      return &r;
    }

    /**
     * @brief Pointer accessor for direct copying.
     */
    const float*
    ptr() const {
      return &r;
    }

    /**
     * @brief Error-tolerant compare, returns true if they are really close to be the same
     */
    bool
    equals(const LinearColor& ColorB, float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      return  Math::isNearlyEqual(r, ColorB.r, Tolerance) && 
              Math::isNearlyEqual(g, ColorB.g, Tolerance) && 
              Math::isNearlyEqual(b, ColorB.b, Tolerance) && 
              Math::isNearlyEqual(a, ColorB.a, Tolerance);
    }

    bool
    operator==(const LinearColor& rhs) const {
      return  r == rhs.r && 
              g == rhs.g && 
              b == rhs.b &&
              a == rhs.a;
    }

    bool
    operator!=(const LinearColor& rhs) const {
      return  r != rhs.r ||
              g != rhs.g ||
              b != rhs.b ||
              a != rhs.a;
    }

    const float&
    operator[](const int32 Index) const {
      GE_ASSERT(Index >= 0 && Index < 4);
      return (&r)[Index];
    }

    float&
    operator[](const int32 Index) {
      GE_ASSERT(Index >= 0 && Index < 4);
      return (&r)[Index];
    }

    LinearColor
    operator+(const LinearColor& rhs) const {
      return LinearColor(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
    }

    LinearColor&
    operator+=(const LinearColor& rhs) {
      r += rhs.r;
      g += rhs.g;
      b += rhs.b;
      a += rhs.a;
      return *this;
    }

    LinearColor
    operator-(const LinearColor& rhs) const {
      return LinearColor(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
    }

    LinearColor&
    operator-=(const LinearColor& rhs) {
      r -= rhs.r;
      g -= rhs.g;
      b -= rhs.b;
      a -= rhs.a;
      return *this;
    }

    LinearColor
    operator*(const float rhs) const {
      return LinearColor(r * rhs, g * rhs, b * rhs, a * rhs);
    }

    LinearColor&
    operator*=(const float rhs) {
      r *= rhs;
      g *= rhs;
      b *= rhs;
      a *= rhs;
      return *this;
    }

    LinearColor
    operator*(const LinearColor& rhs) const {
      return LinearColor(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);
    }

    LinearColor&
    operator*=(const LinearColor& rhs) {
      r *= rhs.r;
      g *= rhs.g;
      b *= rhs.b;
      a *= rhs.a;
      return *this;
    }

    LinearColor
    operator/(const float rhs) const {
      //To avoids possible zero division (it's also faster)
      const float	InvScalar = 1.0f / rhs;
      return LinearColor(r * InvScalar,
                         g * InvScalar,
                         b * InvScalar,
                         a * InvScalar);
    }

    LinearColor&
    operator/=(const float rhs) {
      const float	InvScalar = 1.0f / rhs;
      r *= InvScalar;
      g *= InvScalar;
      b *= InvScalar;
      a *= InvScalar;
      return *this;
    }

    LinearColor
    operator/(const LinearColor& rhs) const {
      return LinearColor(r / rhs.r, g / rhs.g, b / rhs.b, a / rhs.a);
    }

    LinearColor&
    operator/=(const LinearColor& rhs) {
      r /= rhs.r;
      g /= rhs.g;
      b /= rhs.b;
      a /= rhs.a;
      return *this;
    }

    /**
     * @brief Quantizes the linear color and returns the result as a Color.
     *        This bypasses the sRGB conversion.
     */
    Color
    quantize() const;

    /**
     * @brief Quantizes the linear color and returns the result as a Color
     *        with optional sRGB conversion and quality as goal
     */
    Color
    toColor(const bool bSRGB) const;

    /**
     * @brief Convert from float to RGBE as outlined in Gregory Ward's Real
     *        Pixels article, Graphics Gems II, page 80.
     */
    Color
    toRGBE(void) const;

    /**
     * @brief Converts an Color coming from an observed sRGB output, into a linear color.
     * @param Color The sRGB color that needs to be converted into linear space.
     */
    static LinearColor
    fromSRGBColor(const Color& color);

    /**
    * @brief Converts an Color coming from an observed pow(1/2.2) output, into a linear color.
    * @param color The pow(1/2.2) color that needs to be converted into linear space.
    */
    static LinearColor
    fromPow22Color(const Color& color);

    /**
     * @brief Clamps color value to the range [0..1].
     */
    void
    saturate() {
      r = Math::clamp(r, 0.0f, 1.0f);
      g = Math::clamp(g, 0.0f, 1.0f);
      b = Math::clamp(b, 0.0f, 1.0f);
      a = Math::clamp(a, 0.0f, 1.0f);
    }

    /**
     * @brief Clamps color value to the range [0..1].
     *        Returned the saturated color as a copy.
     */
    LinearColor
    saturateCopy() const {
      return getClamped();
    }

    /**
     * @brief Returns a desaturated color, with 0 meaning no desaturation
     *        and 1 == full desaturation
     * @param Desaturation  Desaturation factor in range [0..1]
     * @return Desaturated color
     */
    LinearColor
    desaturate(float Desaturation) const;

    /**
     * @brief Computes the perceptually weighted luminance value of a color.
     * @note The Luminance Factor used here is (0.3, 0.59, 0.11)
     */
    float
    computeLuminance() const;

    /**
     * @brief Clamps the color to the submitted scale
     */
    LinearColor
    getClamped(float InMin = 0.0f, float InMax = 1.0f) const {
      LinearColor Ret;
      Ret.r = Math::clamp(r, InMin, InMax);
      Ret.g = Math::clamp(g, InMin, InMax);
      Ret.b = Math::clamp(b, InMin, InMax);
      Ret.a = Math::clamp(a, InMin, InMax);
      return Ret;
    }

    LinearColor
    copyWithNewOpacity(float NewOpacicty) const {
      LinearColor NewCopy = *this;
      NewCopy.a = NewOpacicty;
      return NewCopy;
    }

    /**
     * @brief Converts byte hue-saturation-brightness to floating point red-green-blue.
     */
    static LinearColor
    getHSV(uint8 H, uint8 S, uint8 V);

    /**
     * @brief Makes a random but quite nice color.
     */
    static LinearColor
    makeRandomColor();

    /**
     * @brief Converts temperature in Kelvins of a black body radiator to RGB chromaticity.
     */
    static LinearColor
    makeFromColorTemperature(float Temp);

    /**
    * Generates a list of sample points on a Bezier curve defined by 2 points.
    *
    * @param	ControlPoints	Array of 4 Linear Colors (vert1, controlpoint1, controlpoint2, vert2).
    * @param	NumPoints		Number of samples.
    * @param	OutPoints		Receives the output samples.
    * @return					Path length.
    */
    static float
    evaluateBezier(const LinearColor* ControlPoints,
                   int32 NumPoints,
                   Vector<LinearColor>& OutPoints);

    /**
     * @brief Converts a linear space RGB color to an HSV color
     */
    LinearColor
    linearRGBToHSV() const;

    /**
     * @brief Converts an HSV color to a linear space RGB color
     * @note  In this color, the values must be: R = H, G = S, B = V
     */
    LinearColor
    hsvToLinearRGB() const;

    /**
     * @brief Linearly interpolates between two colors by the specified progress amount.
     *        The interpolation is performed in HSV color space taking the shortest path
     *        to the new color's hue. This can give better results than Math::lerp(), but
     *        is much more expensive. The incoming colors are in RGB space, and the output
     *        color will be RGB. The alpha value will also be interpolated.
     *
     * @param From The color and alpha to interpolate from as linear RGBA
     * @param To The color and alpha to interpolate to as linear RGBA
     * @param Progress Scalar interpolation amount (usually between 0.0 and 1.0 inclusive)
     * @return The interpolated color in linear RGB space along with the interpolated alpha
     */
    static LinearColor
    lerpUsingHSV(const LinearColor& From, const LinearColor& To, const float Progress);

    /**
     * @brief Euclidean distance between two Colors
     */
    static FORCEINLINE float
    dist(const LinearColor &V1, const LinearColor &V2) {
      return Math::sqrt(Math::square(V2.r - V1.r) +
                        Math::square(V2.g - V1.g) +
                        Math::square(V2.b - V1.b) +
                        Math::square(V2.a - V1.a));
    }

    /**
     * @brief Returns the maximum value in this color structure
     * @return  The maximum color channel value
     */
    FORCEINLINE float
    getMax() const {
      return Math::max(Math::max3(r, g, b), a);
    }

    /**
     * @brief Returns the minimum value in this color structure
     * @return  The minimum color channel value
     */
    FORCEINLINE float
    getMin() const {
      return Math::min(Math::min3(r, g, b), a);
    }

    /**
     * @brief Useful to detect if a light contribution needs to be rendered
     */
    bool
    isAlmostBlack() const {
      return Math::square(r) < Math::DELTA &&
             Math::square(g) < Math::DELTA &&
             Math::square(b) < Math::DELTA;
    }

    FORCEINLINE float
    getLuminance() const {
      return r * 0.3f + g * 0.59f + b * 0.11f;
    }

   public:
    static const LinearColor Transparent;
    static const LinearColor White;
    static const LinearColor Gray;
    static const LinearColor Black;
    static const LinearColor Red;
    static const LinearColor Green;
    static const LinearColor Blue;
    static const LinearColor Yellow;
    static const LinearColor Cyan;
    static const LinearColor Magenta;
   public:
     /** Static lookup table used for Color -> LinearColor conversion. */
     static float s_pow22OneOver255Table[256];

     /** Static lookup table used for Color -> LinearColor conversion. sRGB */
     static float s_sRGBToLinearTable[256];
   public:
    float r, g, b, a;
  };

  FORCEINLINE LinearColor
  operator*(float Scalar, const LinearColor& color) {
    return color.operator*(Scalar);
  }

  using RGBA = uint32;
  using ARGB = uint32;
  using ABGR = uint32;
  using BGRA = uint32;

  /**
   * @brief Color represented as 4 components, each being a floating point
   *        value ranging from 0 to 1.
   *        Color components are Red, Green, Blue and Alpha.
   */
  class GE_UTILITIES_EXPORT Color
  {
   public:
    Color() = default;

    explicit Color(FORCE_INIT::E) {
      //Put these into the body for proper ordering with
      //INTEL vs non-INTEL_BYTE_ORDER
      r = g = b = a = 0;
    }

    Color(uint8 InR, uint8 InG, uint8 InB, uint8 InA = 255) {
      //Put these into the body for proper ordering with
      //INTEL vs non-INTEL_BYTE_ORDER
      r = InR;
      g = InG;
      b = InB;
      a = InA;
    }

    explicit Color(uint32 InColor) {
      dwColor() = InColor;
    }
   
   private:
     /**
      * @brief Please use .toColor(true) on LinearColor if you wish to convert
      *       from LinearColor to Color, with proper sRGB conversion applied.
      * Note: Do not implement or make public. We don't want people needlessly
      *       and implicitly converting between LinearColor and Color.
      *       It's not a free conversion.
      */
     explicit Color(const LinearColor& LinearColor);
  
   public:
    uint32&
    dwColor() {
      return *(reinterpret_cast<uint32*>(this));
    }

    const uint32&
    dwColor() const {
      return *(reinterpret_cast<uint32*>(const_cast<Color*>(this)));
    }

    bool
    operator==(const Color &C) const {
      return dwColor() == C.dwColor();
    }

    bool
    operator!=(const Color& C) const {
      return dwColor() != C.dwColor();
    }

    void
    operator+=(const Color& C) {
      r = static_cast<uint8>(Math::min<uint32>(r + C.r, 255u));
      g = static_cast<uint8>(Math::min<uint32>(g + C.g, 255u));
      b = static_cast<uint8>(Math::min<uint32>(b + C.b, 255u));
      a = static_cast<uint8>(Math::min<uint32>(a + C.a, 255u));
    }

    /**
     * @brief Convert from RGBE to float as outlined in Gregory Ward's
     *        Real Pixels article, Graphics Gems II, page 80.
     */
    LinearColor
    fromRGBE() const;

    /**
     * @brief Returns a new Color based of this color with the new alpha value.
     * @note  Usage: const Color& MyColor = ColorList::Green.WithAlpha(128);
     */
    Color
    withAlpha(uint8 Alpha) const {
      return Color(r, g, b, Alpha);
    }

    /**
     * @brief Reinterprets the color as a linear color.
     * @return  The linear color representation.
     */
    LinearColor
    reinterpretAsLinear() const {
      return LinearColor(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
    }

    static Color
    makeRandomColor();

    static Color
    makeRedToGreenColorFromScalar(float Scalar);

    static Color
    makeFromColorTemperature(float Temp);

    /**
     * @brief Gets the color in a packed uint32 format packed in the order ARGB.
     */
    FORCEINLINE ARGB
    toPackedARGB() const {
      return (a << 24) | (r << 16) | (g << 8) | (b << 0);
    }

    /**
     * @brief Gets the color in a packed uint32 format packed in the order ABGR.
     */
    FORCEINLINE ABGR
    toPackedABGR() const {
      return (a << 24) | (b << 16) | (g << 8) | (r << 0);
    }

    /**
     * @brief Gets the color in a packed uint32 format packed in the order RGBA.
     */
    FORCEINLINE RGBA
    toPackedRGBA() const {
      return (r << 24) | (g << 16) | (b << 8) | (a << 0);
    }

    /**
     * @brief Gets the color in a packed uint32 format packed in the order BGRA.
     */
    FORCEINLINE BGRA
    toPackedBGRA() const {
      return (b << 24) | (g << 16) | (r << 8) | (a << 0);
    }

   public:
    static const Color Transparent;
    static const Color White;
    static const Color Gray;
    static const Color Black;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Cyan;
    static const Color Magenta;
    static const Color Orange;
    static const Color Purple;
    static const Color Turquoise;
    static const Color Silver;
    static const Color Emerald;

#if USING(GE_ENDIAN_LITTLE)
# if USING(GE_COMPILER_MSVC)
#   pragma warning( push )
#   pragma warning( disable : 4201 )
    //Win32 x86
    union {
      struct
      {
        uint8 b, g, r, a;
      };
      uint32 alignmentDummy;
    };
#   pragma warning( pop )
# else
    //Linux x86, etc
    uint8 b ALIGN_AS(4);
    uint8 g, r, a;
# endif
#else
    union {
      struct
      {
        uint8 a, r, g, b;
      };
      uint32 alignmentDummy;
    };
#endif
  };

  /**
   * @brief Computes a brightness and a fixed point color from a floating
   *        point color.
   */
  extern GE_UTILITIES_EXPORT void
  computeAndFixedColorAndIntensity(const LinearColor& InLinearColor,
                                   Color& OutColor,
                                   float& OutIntensity);
}

namespace std {
  /** Hash value generator for LinearColor. */
  template<>
  struct hash<geEngineSDK::LinearColor>
  {
    size_t operator()(const geEngineSDK::LinearColor& color) const {
      size_t hashVal = 0;
      geEngineSDK::ge_hash_combine(hashVal, color.r);
      geEngineSDK::ge_hash_combine(hashVal, color.g);
      geEngineSDK::ge_hash_combine(hashVal, color.b);
      geEngineSDK::ge_hash_combine(hashVal, color.a);

      return hashVal;
    }
  };
  
  /** Hash value generator for Color. */
  template<>
  struct hash<geEngineSDK::Color>
  {
    size_t operator()(const geEngineSDK::Color& color) const {
      size_t hashVal = 0;
      geEngineSDK::ge_hash_combine(hashVal, color.r);
      geEngineSDK::ge_hash_combine(hashVal, color.g);
      geEngineSDK::ge_hash_combine(hashVal, color.b);
      geEngineSDK::ge_hash_combine(hashVal, color.a);

      return hashVal;
    }
  };
}
