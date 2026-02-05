/*****************************************************************************/
/**
 * @file    geVector2.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/14
 * @brief   A vector in 2-D space composed of components (X, Y).
 *
 * A vector in 2-D space composed of components (X, Y) with floating point
 * precision.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

#ifndef _INC_VECTOR2_H_
# define _INC_VECTOR2_H_
#endif

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geMath.h"

#if USING(GE_REFLECTION)
# include <rttr/type>
#endif

namespace geEngineSDK {
  class Vector3;

  class Vector2
  {
   public:

    /**
     * @brief Default constructor (no initialization).
     */
    FORCEINLINE Vector2() = default;

    /**
     * @brief Constructor using initial values for each component.
     * @param InX X coordinate.
     * @param InY Y coordinate.
     */
    FORCEINLINE Vector2(float InX, float InY);

    /**
     * @brief Constructs a vector from a Vector2I.
     * @param InPos Integer point used to set this vector.
     */
    GE_UTILITIES_EXPORT explicit Vector2(const Vector2I& InPos);

    /**
     * @brief Constructor which initializes all components to zero.
     * @param FORCE_INIT enum
     */
    explicit FORCEINLINE Vector2(FORCE_INIT::E);

    /**
     * @brief Constructs a vector from an Vector3.
     * Copies the X and Y components from the Vector3.
     * @param V Vector to copy from.
     */
    GE_UTILITIES_EXPORT explicit Vector2(const Vector3& V);

   public:
    /**
     * @brief Gets the result of adding two vectors together.
     * @param V The other vector to add to this.
     * @return The result of adding the vectors together.
     */
    FORCEINLINE Vector2
    operator+(const Vector2& V) const;

    /**
     * @brief Gets the result of subtracting a vector from this one.
     * @param V The other vector to subtract from this.
     * @return The result of the subtraction.
     */
    FORCEINLINE Vector2
    operator-(const Vector2& V) const;

    /**
     * @brief Gets the result of scaling the vector (multiplying each component by a value).
     * @param Scale How much to scale the vector by.
     * @return The result of scaling this vector.
     */
    FORCEINLINE Vector2
    operator*(float Scale) const;

    /**
     * @brief Gets the result of dividing each component of the vector by a value.
     * @param Scale How much to divide the vector by.
     * @return The result of division on this vector.
     */
    Vector2
    operator/(float Scale) const;

    /**
     * @brief Gets the result of this vector + float A.
     * @param A Float to add to each component.
     * @return The result of this vector + float A.
     */
    FORCEINLINE Vector2
    operator+(float A) const;

    /**
     * @brief Gets the result of subtracting from each component of the vector.
     * @param A Float to subtract from each component
     * @return The result of this vector - float A.
     */
    FORCEINLINE Vector2
    operator-(float A) const;

    /**
     * @brief Gets the result of component-wise multiplication of this vector by another.
     * @param V The other vector to multiply this by.
     * @return The result of the multiplication.
     */
    FORCEINLINE Vector2
    operator*(const Vector2& V) const;

    /**
     * @brief Gets the result of component-wise division of this vector by another.
     * @param V The other vector to divide this by.
     * @return The result of the division.
     */
    Vector2
    operator/(const Vector2& V) const;

    /**
     * @brief Calculates dot product of this vector and another.
     * @param V The other vector.
     * @return The dot product.
     */
    FORCEINLINE float
    operator|(const Vector2& V) const;

    /**
     * @brief Calculates cross product of this vector and another.
     * @param V The other vector.
     * @return The cross product.
     */
    FORCEINLINE float
    operator^(const Vector2& V) const;

   public:
    /**
     * @brief Compares this vector against another for equality.
     * @param V The vector to compare against.
     * @return true if the two vectors are equal, otherwise false.
     */
    bool
    operator==(const Vector2& V) const;

    /**
     * @brief Compares this vector against another for inequality.
     * @param V The vector to compare against.
     * @return true if the two vectors are not equal, otherwise false.
     */
    bool
    operator!=(const Vector2& V) const;

    /**
     * @brief Checks whether both components of this vector are less than another.
     * @param Other The vector to compare against.
     * @return true if this is the smaller vector, otherwise false.
     */
    bool
    operator<(const Vector2& Other) const;

    /**
     * @brief Checks whether both components of this vector are greater than another.
     * @param Other The vector to compare against.
     * @return true if this is the larger vector, otherwise false.
     */
    bool
    operator>(const Vector2& Other) const;

    /**
     * @brief Checks whether both components of this vector are less than or equal to another.
     * @param Other The vector to compare against.
     * @return true if this vector is less than or equal to the other vector, otherwise false.
     */
    bool
    operator<=(const Vector2& Other) const;

    /**
     * @brief Checks whether both components of this vector are greater than or
     *        equal to another.
     * @param Other The vector to compare against.
     * @return  true if this vector is greater than or equal to the other vector
                otherwise false.
     */
    bool
    operator>=(const Vector2& Other) const;

    /**
     * @brief Gets a negated copy of the vector.
     * @return A negated copy of the vector.
     */
    FORCEINLINE Vector2
    operator-() const;

    /**
     * @brief Adds another vector to this.
     * @param V The other vector to add.
     * @return Copy of the vector after addition.
     */
    FORCEINLINE Vector2
    operator+=(const Vector2& V);

    /**
     * @brief Subtracts another vector from this.
     * @param V The other vector to subtract.
     * @return Copy of the vector after subtraction.
     */
    FORCEINLINE Vector2
    operator-=(const Vector2& V);

    /**
     * @brief Scales this vector.
     * @param Scale The scale to multiply vector by.
     * @return Copy of the vector after scaling.
     */
    FORCEINLINE Vector2
    operator*=(float Scale);

    /**
     * @brief Divides this vector.
     * @param V What to divide vector by.
     * @return Copy of the vector after division.
     */
    Vector2
    operator/=(float V);

    /**
     * @brief Multiplies this vector with another vector, using component-wise multiplication.
     * @param V The vector to multiply with.
     * @return Copy of the vector after multiplication.
     */
    Vector2
    operator*=(const Vector2& V);

    /**
     * @brief Divides this vector by another vector, using component-wise division.
     * @param V The vector to divide by.
     * @return Copy of the vector after division.
     */
    Vector2
    operator/=(const Vector2& V);

    /**
     * @brief Gets specific component of the vector.
     * @param Index the index of vector component
     * @return reference to component.
     */
    float&
    operator[](uint32 Index);

    /**
     * @brief Gets specific component of the vector.
     * @param Index the index of vector component
     * @return copy of component value.
     */
    float
    operator[](uint32 Index) const;

    /**
     * @brief Gets a specific component of the vector.
     * @param Index The index of the component required.
     * @return Reference to the specified component.
     */
    float&
    component(uint32 Index);

    /**
     * @brief Gets a specific component of the vector.
     * @param Index The index of the component required.
     * @return Copy of the specified component.
     */
    float
    component(uint32 Index) const;

   public:
     /**
      * @brief Exchange the contents of this vector with another.
      */
     FORCEINLINE void
     swap(Vector2& other);

    /**
     * @brief Calculates the dot product of two vectors.
     * @param A The first vector.
     * @param B The second vector.
     * @return The dot product.
     */
    static FORCEINLINE float
    dot(const Vector2& A, const Vector2& B);

    /**
     * @brief Squared distance between two 2D points.
     * @param V1 The first point.
     * @param V2 The second point.
     * @return The squared distance between two 2D points.
     */
    static FORCEINLINE float
    distSquared(const Vector2& V1, const Vector2& V2);

    /**
     * @brief Distance between two 2D points.
     * @param V1 The first point.
     * @param V2 The second point.
     * @return The distance between two 2D points.
     */
    static FORCEINLINE float
    distance(const Vector2& V1, const Vector2& V2);

    /**
     * @brief Generates a vector perpendicular to this vector.
     */
    FORCEINLINE Vector2
    perpendicular() const;

    /**
     * @brief Calculate the cross product of two vectors.
     * @param A The first vector.
     * @param B The second vector.
     * @return The cross product.
     */
    static FORCEINLINE float
    crossProduct(const Vector2& A, const Vector2& B);

    /**
     * @brief Performs Gram-Schmidt orthonormalization on both vectors
     * @param u The first vector.
     * @param v The second vector.
     */
    static FORCEINLINE void
    orthonormalize(Vector2& u, Vector2& v);

    /**
     * @brief Checks for equality with error-tolerant comparison.
     * @param V The vector to compare.
     * @param Tolerance Error tolerance.
     * @return true if the vectors are equal within specified tolerance, otherwise false.
     */
    bool
    equals(const Vector2& V, float Tolerance = Math::KINDA_SMALL_NUMBER) const;

    /**
     * @brief Set the values of the vector directly.
     * @param InX New X coordinate.
     * @param InY New Y coordinate.
     */
    void
    set(float InX, float InY);

    /**
     * @brief Sets this vector's components to the minimum of its own and the
     *        ones of the passed in vector.
     */
    void
    floor(const Vector2& V);

    /**
     * @brief Sets this vector's components to the maximum of its own and the
     *        ones of the passed in vector.
     */
    void
    ceil(const Vector2& V);

    /**
     * @brief Get the maximum value of the vector's components.
     * @return The maximum value of the vector's components.
     */
    float
    getMax() const;

    /**
     * @brief Get the maximum absolute value of the vector's components.
     * @return The maximum absolute value of the vector's components.
     */
    float
    getAbsMax() const;

    /**
     * @brief Get the minimum value of the vector's components.
     * @return The minimum value of the vector's components.
     */
    float
    getMin() const;

    /**
     * @brief Get the length (magnitude) of this vector.
     * @return The length of this vector.
     */
    float
    size() const;

    /**
     * @brief Get the squared length of this vector.
     * @return The squared length of this vector.
     */
    float
    sizeSquared() const;

    /**
     * @brief Rotates around axis (0,0,1)
     * @param AngleDeg Angle to rotate (in degrees)
     * @return Rotated Vector
     */
    Vector2
    getRotated(const float AngleDeg) const;

    /**
     * @brief Gets a normalized copy of the vector, checking it is safe to do
     *        so based on the length. Returns zero vector if vector length is
     *        too small to safely normalize.
     * @param Tolerance Minimum squared length of vector for normalization.
     * @return A normalized copy of the vector if safe, (0,0) otherwise.
     */
    Vector2
    getSafeNormal(float Tolerance = Math::SMALL_NUMBER) const;

    /**
     * @brief Normalize this vector in-place if it is large enough, set it to (0,0) otherwise.
     * @param Tolerance Minimum squared length of vector for normalization.
     * @see getSafeNormal()
     */
    void
    normalize(float Tolerance = Math::SMALL_NUMBER);

    /**
     * @brief Checks whether vector is near to zero within a specified tolerance.
     * @param Tolerance Error tolerance.
     * @return true if vector is in tolerance to zero, otherwise false.
     */
    bool
    isNearlyZero(float Tolerance = Math::KINDA_SMALL_NUMBER) const;

    /**
     * @brief Utility to convert this vector into a unit direction vector and
     *        its original length.
     * @param OutDir Reference passed in to store unit direction vector.
     * @param OutLength Reference passed in to store length of the vector.
     */
    void
    toDirectionAndLength(Vector2 &OutDir, float &OutLength) const;

    /**
     * @brief Checks whether all components of the vector are exactly zero.
     * @return true if vector is exactly zero, otherwise false.
     */
    bool
    isZero() const;

    /**
     * @brief Get this vector as an Int Vector.
     * @return New Int Vector from this vector.
     */
    GE_UTILITIES_EXPORT Vector2I
    toVector2I() const;

    /**
     * @brief Creates a copy of this vector with both axes clamped to the given range.
     * @return New vector with clamped axes.
     */
    Vector2
    clampAxes(float MinAxisVal, float MaxAxisVal) const;

    /**
     * @brief Get a copy of the vector as sign only.
     * Each component is set to +1 or -1, with the sign of zero treated as +1.
     * @param A copy of the vector with each component set to +1 or -1
     */
    FORCEINLINE Vector2
    getSignVector() const;

    /**
     * @brief Get a copy of this vector with absolute value of each component.
     * @return A copy of this vector with absolute value of each component.
     */
    FORCEINLINE Vector2
    getAbs() const;

    /**
     * @brief Utility to check if there are any non-finite values (NaN or Inf) in this vector.
     * @return true if there are any non-finite values in this vector, false otherwise.
     */
    FORCEINLINE bool
    containsNaN() const {
      return (!Math::isFinite(x) || !Math::isFinite(y));
    }

    /**
     * @brief Converts spherical coordinates on the unit sphere into a
     *        Cartesian unit length vector.
     */
    GE_UTILITIES_EXPORT Vector3
    sphericalToUnitCartesian() const;

   public:
     /**
      * @brief Vector's X component.
      */
    float x;

    /**
     * @brief Vector's Y component.
     */
    float y;

    /**
     * @brief Global 2D zero vector constant (0,0)
     */
    static GE_UTILITIES_EXPORT const Vector2 ZERO;

    /**
     * @brief Global 2D unit vector constant (1,1)
     */
    static GE_UTILITIES_EXPORT const Vector2 UNIT;

    /**
     * @brief Global 2D unit vector X constant (1,0)
     */
    static GE_UTILITIES_EXPORT const Vector2 UNIT_X;

    /**
     * @brief Global 2D unit vector Y constant (0,1)
     */
    static GE_UTILITIES_EXPORT const Vector2 UNIT_Y;

    GE_REFLECTED_CLASS()
  };

  /***************************************************************************/
  /**
   * Vector2 inline functions
   */
  /***************************************************************************/

  FORCEINLINE Vector2
  operator*(float Scale, const Vector2& V) {
    return V.operator*(Scale);
  }

  FORCEINLINE Vector2::Vector2(float InX, float InY) : x(InX), y(InY) {}

  FORCEINLINE Vector2::Vector2(FORCE_INIT::E) : x(0), y(0) {}

  FORCEINLINE Vector2
  Vector2::operator+(const Vector2& V) const {
    return Vector2(x + V.x, y + V.y);
  }

  FORCEINLINE Vector2
  Vector2::operator-(const Vector2& V) const {
    return Vector2(x - V.x, y - V.y);
  }

  FORCEINLINE Vector2
  Vector2::operator*(float Scale) const {
    return Vector2(x * Scale, y * Scale);
  }

  FORCEINLINE Vector2
  Vector2::operator/(float Scale) const {
    const float RScale = 1.f / Scale;
    return Vector2(x * RScale, y * RScale);
  }

  FORCEINLINE Vector2
  Vector2::operator+(float A) const {
    return Vector2(x + A, y + A);
  }

  FORCEINLINE Vector2
  Vector2::operator-(float A) const {
    return Vector2(x - A, y - A);
  }

  FORCEINLINE Vector2
  Vector2::operator*(const Vector2& V) const {
    return Vector2(x * V.x, y * V.y);
  }

  FORCEINLINE Vector2
  Vector2::operator/(const Vector2& V) const {
    return Vector2(x / V.x, y / V.y);
  }

  FORCEINLINE float
  Vector2::operator|(const Vector2& V) const {
    return x*V.x + y*V.y;
  }

  FORCEINLINE float
  Vector2::operator^(const Vector2& V) const {
    return x*V.y - y*V.x;
  }

  FORCEINLINE void
  Vector2::swap(Vector2& other) {
    std::swap(x, other.x);
    std::swap(y, other.y);
  }

  FORCEINLINE float
  Vector2::dot(const Vector2& A, const Vector2& B) {
    return A | B;
  }

  FORCEINLINE float
  Vector2::distSquared(const Vector2 &V1, const Vector2 &V2) {
    return Math::square(V2.x - V1.x) + Math::square(V2.y - V1.y);
  }

  FORCEINLINE float
  Vector2::distance(const Vector2& V1, const Vector2& V2) {
    return Math::sqrt(Vector2::distSquared(V1, V2));
  }

  FORCEINLINE Vector2
  Vector2::perpendicular() const {
    return Vector2(-y, x);
  }

  FORCEINLINE float
  Vector2::crossProduct(const Vector2& A, const Vector2& B) {
    return A ^ B;
  }

  FORCEINLINE void
  Vector2::orthonormalize(Vector2& u, Vector2& v) {
    u.normalize();
    float UdV = u|v;
    v -= u * UdV;
    v.normalize();
  }

  FORCEINLINE bool
  Vector2::operator==(const Vector2& V) const {
    return x == V.x && y == V.y;
  }

  FORCEINLINE bool
  Vector2::operator!=(const Vector2& V) const {
    return x != V.x || y != V.y;
  }

  FORCEINLINE bool
  Vector2::operator<(const Vector2& Other) const {
    return x < Other.x && y < Other.y;
  }

  FORCEINLINE bool
  Vector2::operator>(const Vector2& Other) const {
    return x > Other.x && y > Other.y;
  }

  FORCEINLINE bool
  Vector2::operator<=(const Vector2& Other) const {
    return x <= Other.x && y <= Other.y;
  }

  FORCEINLINE bool
  Vector2::operator>=(const Vector2& Other) const {
    return x >= Other.x && y >= Other.y;
  }
  
  FORCEINLINE bool
  Vector2::equals(const Vector2& V, float Tolerance) const {
    return Math::abs(x - V.x) <= Tolerance && Math::abs(y - V.y) <= Tolerance;
  }

  FORCEINLINE Vector2
  Vector2::operator-() const {
    return Vector2(-x, -y);
  }

  FORCEINLINE Vector2
  Vector2::operator+=(const Vector2& V) {
    x += V.x;
    y += V.y;
    return *this;
  }

  FORCEINLINE Vector2 Vector2::operator-=(const Vector2& V) {
    x -= V.x;
    y -= V.y;
    return *this;
  }

  FORCEINLINE Vector2
  Vector2::operator*=(float Scale) {
    x *= Scale;
    y *= Scale;
    return *this;
  }


  FORCEINLINE Vector2
  Vector2::operator/=(float V) {
    const float RV = 1.f / V;
    x *= RV;
    y *= RV;
    return *this;
  }

  FORCEINLINE Vector2
  Vector2::operator*=(const Vector2& V) {
    x *= V.x;
    y *= V.y;
    return *this;
  }

  FORCEINLINE Vector2
  Vector2::operator/=(const Vector2& V) {
    x /= V.x;
    y /= V.y;
    return *this;
  }

  FORCEINLINE float&
  Vector2::operator[](uint32 Index) {
    GE_ASSERT(Index<2);
    return ((Index == 0) ? x : y);
  }

  FORCEINLINE float
  Vector2::operator[](uint32 Index) const {
    GE_ASSERT(Index<2);
    return ((Index == 0) ? x : y);
  }

  FORCEINLINE void
  Vector2::set(float InX, float InY) {
    x = InX;
    y = InY;
  }

  FORCEINLINE void
  Vector2::floor(const Vector2& V) {
    if (V.x < x) x = V.x;
    if (V.y < y) y = V.y;
  }

  FORCEINLINE void
  Vector2::ceil(const Vector2& V) {
    if (V.x > x) x = V.x;
    if (V.y > y) y = V.y;
  }

  FORCEINLINE float
  Vector2::getMax() const {
    return Math::max(x, y);
  }

  FORCEINLINE float
  Vector2::getAbsMax() const {
    return Math::max(Math::abs(x), Math::abs(y));
  }

  FORCEINLINE float
  Vector2::getMin() const {
    return Math::min(x, y);
  }

  FORCEINLINE float
  Vector2::size() const {
    return Math::sqrt(x*x + y*y);
  }

  FORCEINLINE float
  Vector2::sizeSquared() const {
    return x*x + y*y;
  }

  FORCEINLINE Vector2
  Vector2::getRotated(const float AngleDeg) const {
    //Based on Vector::rotateAngleAxis with Axis(0,0,1)
    float radAngle = AngleDeg * Math::DEG2RAD;

    float S = Math::sin(radAngle);
    float C = Math::cos(radAngle);
    //const float OMC = 1.0f - C;

    return Vector2(C * x - S * y, S * x + C * y);
  }

  FORCEINLINE Vector2
  Vector2::getSafeNormal(float Tolerance) const {
    const float SquareSum = x*x + y*y;
    if (SquareSum > Tolerance) {
      const float Scale = Math::invSqrt(SquareSum);
      return Vector2(x*Scale, y*Scale);
    }
    return Vector2(0.f, 0.f);
  }

  FORCEINLINE void
  Vector2::normalize(float Tolerance) {
    const float SquareSum = x*x + y*y;
    if (SquareSum > Tolerance) {
      const float Scale = Math::invSqrt(SquareSum);
      x *= Scale;
      y *= Scale;
      return;
    }
    x = 0.0f;
    y = 0.0f;
  }

  FORCEINLINE void
  Vector2::toDirectionAndLength(Vector2 &OutDir, float &OutLength) const {
    OutLength = size();
    if (Math::SMALL_NUMBER < OutLength) {
      float OneOverLength = 1.0f / OutLength;
      OutDir = Vector2(x*OneOverLength, y*OneOverLength);
    }
    else {
      OutDir = Vector2::ZERO;
    }
  }

  FORCEINLINE bool
  Vector2::isNearlyZero(float Tolerance) const {
    return  Math::abs(x) <= Tolerance &&
            Math::abs(y) <= Tolerance;
  }

  FORCEINLINE bool
  Vector2::isZero() const {
    return 0.f == x && 0.f == y;
  }

  FORCEINLINE float&
  Vector2::component(uint32 Index) {
    return (&x)[Index];
  }

  FORCEINLINE float
  Vector2::component(uint32 Index) const {
    return (&x)[Index];
  }

  FORCEINLINE Vector2
  Vector2::clampAxes(float MinAxisVal, float MaxAxisVal) const {
    return Vector2(Math::clamp(x, MinAxisVal, MaxAxisVal),
                   Math::clamp(y, MinAxisVal, MaxAxisVal));
  }

  FORCEINLINE Vector2
  Vector2::getSignVector() const {
    return Vector2(Math::floatSelect(x, 1.f, -1.f),
                   Math::floatSelect(y, 1.f, -1.f));
  }

  FORCEINLINE Vector2
  Vector2::getAbs() const {
    return Vector2(Math::abs(x), Math::abs(y));
  }

  /***************************************************************************/
  /**
   * Math inline functions
   */
  /***************************************************************************/

  FORCEINLINE float
  Math::getRangePct(Vector2 const& Range, float Value) {
    return (Range.x != Range.y) ? (Value - Range.x) / (Range.y - Range.x) : Range.x;
  }

  FORCEINLINE float
  Math::getRangeValue(Vector2 const& Range, float Pct) {
    return lerp<float>(Range.x, Range.y, Pct);
  }
}
