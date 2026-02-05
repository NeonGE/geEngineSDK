/*****************************************************************************/
/**
 * @file    geVector4.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/06/28
 * @brief   A 4D homogeneous vector, 4x1 FLOATs, 16-byte aligned.
 *
 * A 4D homogeneous vector, 4x1 FLOATs, 16-byte aligned.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

#ifndef _INC_VECTOR4_H_
# define _INC_VECTOR4_H_
#endif

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geDebug.h"
#include "geMath.h"
#include "geColor.h"
#include "geVector2.h"
#include "geVector3.h"

#if USING(GE_REFLECTION)
# include <rttr/type>
#endif

namespace geEngineSDK {
  /**
  * 
  */
  class ALIGN_AS(16) Vector4
  {
   public:
    /**
     * @brief Constructor.
     * @param InVector 3D Vector to set first three components.
     * @param InW W Coordinate.
     */
    Vector4(const Vector3& InVector, float InW = 1.0f);

    /**
     * @brief Creates and initializes a new vector from a color value.
     * @param InColour Color used to set vector.
     */
    Vector4(const LinearColor& InColor);

    /**
     * @brief Creates and initializes a new vector from the specified components.
     * @param InX X Coordinate.
     * @param InY Y Coordinate.
     * @param InZ Z Coordinate.
     * @param InW W Coordinate.
     */
    explicit Vector4(float InX = 0.0f, float InY = 0.0f, float InZ = 0.0f, float InW = 1.0f);

    /**
     * @brief Creates and initializes a new vector from the specified 2D vectors.
     * @param InXY A 2D vector holding the X- and Y-components.
     * @param InZW A 2D vector holding the Z- and W-components.
     */
    explicit Vector4(Vector2 InXY, Vector2 InZW);

    /**
     * @brief Creates and initializes a new vector to zero.
     * @param FORCE_INIT Force Init Enum.
     */
    explicit Vector4(FORCE_INIT::E);

   public:
    /**
     * @brief Access a specific component of the vector.
     * @param ComponentIndex The index of the component.
     * @return Reference to the desired component.
     */
    FORCEINLINE float&
    operator[](uint32 ComponentIndex);

    /**
     * @brief Access a specific component of the vector.
     * @param ComponentIndex The index of the component.
     * @return Copy of the desired component.
     */
    FORCEINLINE float
    operator[](uint32 ComponentIndex) const;

    /*************************************************************************/
    /**
     * Unary operators.
     */
    /*************************************************************************/

    /**
     * @brief Gets a negated copy of the vector.
     * @return A negated copy of the vector.
     */
    FORCEINLINE Vector4
    operator-() const;

    /**
     * @brief Gets the result of adding a vector to this.
     * @param V The vector to add.
     * @return The result of vector addition.
     */
    FORCEINLINE Vector4
    operator+(const Vector4& V) const;

    /**
     * @brief Adds another vector to this one.
     * @param V The other vector to add.
     * @return Copy of the vector after addition.
     */
    FORCEINLINE Vector4
    operator+=(const Vector4& V);

    /**
     * @brief Gets the result of subtracting a vector from this.
     * @param V The vector to subtract.
     * @return The result of vector subtraction.
     */
    FORCEINLINE Vector4
    operator-(const Vector4& V) const;

    /**
     * @brief Gets the result of scaling this vector.
     * @param Scale The scaling factor.
     * @return The result of vector scaling.
     */
    FORCEINLINE Vector4
    operator*(float Scale) const;

    /**
     * @brief Gets the result of dividing this vector.
     * @param Scale What to divide by.
     * @return The result of division.
     */
    Vector4
    operator/(float Scale) const;

    /**
     * @brief Gets the result of dividing this vector.
     * @param V What to divide by.
     * @return The result of division.
     */
    Vector4
    operator/(const Vector4& V) const;

    /**
     * @brief Gets the result of multiplying a vector with this.
     * @param V The vector to multiply.
     * @return The result of vector multiplication.
     */
    Vector4
    operator*(const Vector4& V) const;

    /**
     * @brief Gets the result of multiplying a vector with another Vector (component wise).
     * @param V The vector to multiply.
     * @return The result of vector multiplication.
     */
    Vector4
    operator*=(const Vector4& V);

    /**
     * @brief Gets the result of dividing a vector with another Vector (component wise).
     * @param V The vector to divide with.
     * @return The result of vector multiplication.
     */
    Vector4
    operator/=(const Vector4& V);

    /**
     * @brief Gets the result of scaling this vector.
     * @param Scale The scaling factor.
     * @return The result of vector scaling.
     */
    Vector4
    operator*=(float S);

    /**
     * @brief Calculates 3D Dot product of two 4D vectors.
     * @param V1 The first vector.
     * @param V2 The second vector.
     * @return The 3D Dot product.
     */
    friend FORCEINLINE float
    dot3(const Vector4& V1, const Vector4& V2) {
      return V1.x*V2.x + V1.y*V2.y + V1.z*V2.z;
    }

    /**
     * @brief Calculates 4D Dot product.
     * @param V1 The first vector.
     * @param V2 The second vector.
     * @return The 4D Dot Product.
     */
    friend FORCEINLINE float
    dot4(const Vector4& V1, const Vector4& V2) {
      return V1.x*V2.x + V1.y*V2.y + V1.z*V2.z + V1.w*V2.w;
    }

    /**
     * @brief Scales a vector.
     * @param Scale The scaling factor.
     * @param V The vector to scale.
     * @return The result of scaling.
     */
    friend FORCEINLINE Vector4
    operator*(float Scale, const Vector4& V) {
      return V.operator*(Scale);
    }

    /**
     * @brief Checks for equality against another vector.
     * @param V The other vector.
     * @return true if the two vectors are the same, otherwise false.
     */
    bool
    operator==(const Vector4& V) const;

    /**
     * @brief Checks for inequality against another vector.
     * @param V The other vector.
     * @return true if the two vectors are different, otherwise false.
     */
    bool
    operator!=(const Vector4& V) const;

    /**
     * @brief Calculate Cross product between this and another vector.
     * @param V The other vector.
     * @return The Cross product.
     */
    Vector4
    operator^(const Vector4& V) const;

   public:
    /*************************************************************************/
    /**
     * Simple functions.
     */
    /*************************************************************************/

    /**
     * @brief Gets a specific component of the vector.
     * @param Index The index of the component.
     * @return Reference to the component.
     */
    float&
    component(uint32 Index);

    /**
     * @brief Error tolerant comparison.
     * @param V Vector to compare against.
     * @param Tolerance Error Tolerance.
     * @return true if the two vectors are equal within specified tolerance, otherwise false.
     */
    bool
    equals(const Vector4& V, float Tolerance = Math::KINDA_SMALL_NUMBER) const;

    Vector4
    vectorAbs() const;

    Vector4
    vectorMin(const Vector4& V) const;

    Vector4
    vectorMax(const Vector4& V) const;

    /**
     * @brief Check if the vector is of unit length, with specified tolerance.
     * @param LengthSquaredTolerance Tolerance against squared length.
     * @return true if the vector is a unit vector within the specified tolerance.
     */
    bool
    isUnit3(float LengthSquaredTolerance = Math::KINDA_SMALL_NUMBER) const;

    /**
     * @brief Returns a normalized copy of the vector if safe to normalize.
     * @param Tolerance Minimum squared length of vector for normalization.
     * @return A normalized copy of the vector or a zero vector.
     */
    FORCEINLINE Vector4
    getSafeNormal(float Tolerance = Math::SMALL_NUMBER) const;

    /**
     * @brief Calculates normalized version of vector without checking if it is non-zero.
     * @return Normalized version of vector.
     */
    FORCEINLINE Vector4
    getUnsafeNormal3() const;

    /**
     * @brief Return the Rotator orientation corresponding to the direction in
     *        which the vector points.
     * Sets Yaw and Pitch to the proper numbers, and sets roll to zero because
     * the roll can't be determined from a vector.
     * @return Rotator from the Vector's direction.
     */
    GE_UTILITIES_EXPORT Rotator
    toOrientationRotator() const;

    /**
     * @brief Return the Quaternion orientation corresponding to the direction
     *        in which the vector points.
     * @return Quaternion from the Vector's direction.
     */
    GE_UTILITIES_EXPORT Quaternion
    toOrientationQuat() const;

    /**
     * @brief Return the Rotator orientation corresponding to the direction in
     *        which the vector points.
     * Sets Yaw and Pitch to the proper numbers, and sets roll to zero because
     * the roll can't be determined from a vector.
     * Identical to 'toOrientationRotator()'.
     * @return Rotator from the Vector's direction.
     * @see toOrientationRotator()
     */
    GE_UTILITIES_EXPORT Rotator
    rotation() const;

    /**
     * @brief Set all of the vectors coordinates.
     * @param InX New X Coordinate.
     * @param InY New Y Coordinate.
     * @param InZ New Z Coordinate.
     * @param InW New W Coordinate.
     */
    FORCEINLINE void
    set(float InX, float InY, float InZ, float InW);

    /**
     * @brief Get the length of this vector not taking W component into account.
     * @return The length of this vector.
     */
    float
    size3() const;

    /**
     * @brief Get the squared length of this vector not taking W component into account.
     * @return The squared length of this vector.
     */
    float
    sizeSquared3() const;

    /**
     * @brief Get the length (magnitude) of this vector, taking the W component into account
     * @return The length of this vector
     */
    float
    size() const;

    /**
     * @brief Get the squared length of this vector, taking the W component into account
     * @return The squared length of this vector
     */
    float
    sizeSquared() const;

    /**
     * @brief Utility to check if there are any non-finite values (NaN or Inf) in this vector.
     */
    bool
    containsNaN() const;

    /**
     * @brief Utility to check if all of the components of this vector are
     *        nearly zero given the tolerance.
     */
    bool
    isNearlyZero3(float Tolerance = Math::KINDA_SMALL_NUMBER) const;

    /**
     * @brief Reflect vector.
     */
    Vector4
    reflect3(const Vector4& Normal) const;

    /**
     * @brief Find good arbitrary axis vectors to represent U and V axes of a
     *        plane, given just the normal.
     */
    void
    findBestAxisVectors3(Vector4& Axis1, Vector4& Axis2) const;

# if USING(GE_DEBUG_MODE)
    FORCEINLINE void
    diagnosticCheckNaN() {
      if (containsNaN()) {
        GE_LOG(kWarning, Generic, "Vector4 contains NaN:");
        *this = Vector4(Vector3::ZERO);
      }
    }
# else
    FORCEINLINE void
    diagnosticCheckNaN() {}
# endif

   public:
    static GE_UTILITIES_EXPORT const Vector4 ZERO;

    /**
     * @brief The vector's X-component.
     */
    float x;

    /**
     * @brief The vector's Y-component.
     */
    float y;

    /**
     * @brief The vector's Z-component.
     */
    float z;

    /**
     * @brief The vector's W-component.
     */
    float w;
  };

  /***************************************************************************/
  /**
   * Vector34 inline functions.
   */
  /***************************************************************************/

  FORCEINLINE Vector4::Vector4(const Vector3& InVector, float InW)
    : x(InVector.x),
      y(InVector.y),
      z(InVector.z),
      w(InW) {
    diagnosticCheckNaN();
  }

  FORCEINLINE Vector4::Vector4(const LinearColor& InColor)
    : x(InColor.r),
      y(InColor.g),
      z(InColor.b),
      w(InColor.a) {
    diagnosticCheckNaN();
  }

  FORCEINLINE Vector4::Vector4(float InX, float InY, float InZ, float InW)
    : x(InX),
      y(InY),
      z(InZ),
      w(InW) {
    diagnosticCheckNaN();
  }

  FORCEINLINE Vector4::Vector4(FORCE_INIT::E) : x(0.f), y(0.f), z(0.f), w(0.f) {
    diagnosticCheckNaN();
  }

  FORCEINLINE Vector4::Vector4(Vector2 InXY, Vector2 InZW)
    : x(InXY.x), y(InXY.y), z(InZW.x), w(InZW.y) {
    diagnosticCheckNaN();
  }

  FORCEINLINE float&
  Vector4::operator[](uint32 ComponentIndex) {
    return (&x)[ComponentIndex];
  }

  FORCEINLINE float
  Vector4::operator[](uint32 ComponentIndex) const {
    return (&x)[ComponentIndex];
  }

  FORCEINLINE void
  Vector4::set(float InX, float InY, float InZ, float InW) {
    x = InX;
    y = InY;
    z = InZ;
    w = InW;
    diagnosticCheckNaN();
  }

  FORCEINLINE Vector4
  Vector4::operator-() const {
    return Vector4(-x, -y, -z, -w);
  }

  FORCEINLINE Vector4
  Vector4::operator+(const Vector4& V) const {
    return Vector4(x + V.x, y + V.y, z + V.z, w + V.w);
  }

  FORCEINLINE Vector4
  Vector4::operator+=(const Vector4& V) {
    x += V.x; y += V.y; z += V.z; w += V.w;
    diagnosticCheckNaN();
    return *this;
  }

  FORCEINLINE Vector4
  Vector4::operator-(const Vector4& V) const {
    return Vector4(x - V.x, y - V.y, z - V.z, w - V.w);
  }

  FORCEINLINE Vector4
  Vector4::operator*(float Scale) const {
    return Vector4(x * Scale, y * Scale, z * Scale, w * Scale);
  }

  FORCEINLINE Vector4
  Vector4::operator/(float Scale) const {
    const float RScale = 1.f / Scale;
    return Vector4(x * RScale, y * RScale, z * RScale, w * RScale);
  }

  FORCEINLINE Vector4
  Vector4::operator*(const Vector4& V) const {
    return Vector4(x * V.x, y * V.y, z * V.z, w * V.w);
  }

  FORCEINLINE Vector4
  Vector4::operator^(const Vector4& V) const {
    return Vector4(y * V.z - z * V.y,
                   z * V.x - x * V.z,
                   x * V.y - y * V.x,
                   0.0f);
  }

  FORCEINLINE float&
  Vector4::component(uint32 Index) {
    return (&x)[Index];
  }

  FORCEINLINE bool
  Vector4::operator==(const Vector4& V) const {
    return ((x == V.x) && (y == V.y) && (z == V.z) && (w == V.w));
  }

  FORCEINLINE bool
  Vector4::operator!=(const Vector4& V) const {
    return ((x != V.x) || (y != V.y) || (z != V.z) || (w != V.w));
  }

  FORCEINLINE bool
  Vector4::equals(const Vector4& V, float Tolerance) const {
    return Math::abs(x - V.x) <= Tolerance &&
           Math::abs(y - V.y) <= Tolerance &&
           Math::abs(z - V.z) <= Tolerance &&
           Math::abs(w - V.w) <= Tolerance;
  }

  FORCEINLINE Vector4
  Vector4::vectorAbs() const {
    return Vector4(Math::abs(x), Math::abs(y), Math::abs(z), Math::abs(w));
  }

  FORCEINLINE Vector4
  Vector4::vectorMin(const Vector4& V) const {
    Vector4 Result;
    Result.x = Math::min(x, V.x);
    Result.y = Math::min(y, V.y);
    Result.z = Math::min(z, V.z);
    Result.w = Math::min(w, V.w);
    return Result;
  }

  FORCEINLINE Vector4
  Vector4::vectorMax(const Vector4& V) const {
    Vector4 Result;
    Result.x = Math::max(x, V.x);
    Result.y = Math::max(y, V.y);
    Result.z = Math::max(z, V.z);
    Result.w = Math::max(w, V.w);
    return Result;
  }

  FORCEINLINE Vector4
  Vector4::getSafeNormal(float Tolerance) const {
    const float SquareSum = x*x + y*y + z*z;
    if (SquareSum > Tolerance) {
      const float Scale = Math::invSqrt(SquareSum);
      return Vector4(x*Scale, y*Scale, z*Scale, 0.0f);
    }
    return Vector4(0.f);
  }

  FORCEINLINE Vector4
  Vector4::getUnsafeNormal3() const {
    const float Scale = Math::invSqrt(x*x + y*y + z*z);
    return Vector4(x*Scale, y*Scale, z*Scale, 0.0f);
  }

  FORCEINLINE float
  Vector4::size3() const {
    return Math::sqrt(x*x + y*y + z*z);
  }

  FORCEINLINE float
  Vector4::sizeSquared3() const {
    return x*x + y*y + z*z;
  }

  FORCEINLINE float
  Vector4::size() const {
    return Math::sqrt(x*x + y*y + z*z + w*w);
  }

  FORCEINLINE float
  Vector4::sizeSquared() const {
    return x*x + y*y + z*z + w*w;
  }

  FORCEINLINE bool
  Vector4::isUnit3(float LengthSquaredTolerance) const {
    return Math::abs(1.0f - sizeSquared3()) < LengthSquaredTolerance;
  }

  FORCEINLINE bool
  Vector4::containsNaN() const {
    return (!Math::isFinite(x) ||
            !Math::isFinite(y) ||
            !Math::isFinite(z) ||
            !Math::isFinite(w));
  }

  FORCEINLINE bool
  Vector4::isNearlyZero3(float Tolerance) const {
    return Math::abs(x) <= Tolerance &&
           Math::abs(y) <= Tolerance &&
           Math::abs(z) <= Tolerance;
  }

  FORCEINLINE Vector4
  Vector4::reflect3(const Vector4& Normal) const {
    return 2.0f * dot3(*this, Normal) * Normal - *this;
  }

  FORCEINLINE void
  Vector4::findBestAxisVectors3(Vector4& Axis1, Vector4& Axis2) const {
    const float NX = Math::abs(x);
    const float NY = Math::abs(y);
    const float NZ = Math::abs(z);

    // Find best basis vectors.
    if (NZ > NX && NZ > NY) {
      Axis1 = Vector4(1, 0, 0);
    }
    else {
      Axis1 = Vector4(0, 0, 1);
    }

    Axis1 = (Axis1 - *this * dot3(Axis1, *this)).getSafeNormal();
    Axis2 = Axis1 ^ *this;
  }

  FORCEINLINE Vector4
  Vector4::operator*=(const Vector4& V) {
    x *= V.x; y *= V.y; z *= V.z; w *= V.w;
    diagnosticCheckNaN();
    return *this;
  }

  FORCEINLINE Vector4
  Vector4::operator/=(const Vector4& V) {
    x /= V.x; y /= V.y; z /= V.z; w /= V.w;
    diagnosticCheckNaN();
    return *this;
  }

  FORCEINLINE Vector4
  Vector4::operator*=(float S) {
    x *= S; y *= S; z *= S; w *= S;
    diagnosticCheckNaN();
    return *this;
  }

  FORCEINLINE Vector4
  Vector4::operator/(const Vector4& V) const {
    return Vector4(x / V.x, y / V.y, z / V.z, w / V.w);
  }

  /***************************************************************************/
  /**
   * Vector3 inline functions
   */
  /***************************************************************************/

  FORCEINLINE Vector3::Vector3(const Vector4& V) : x(V.x), y(V.y), z(V.z) {}
}
