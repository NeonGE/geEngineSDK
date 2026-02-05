/*****************************************************************************/
/**
 * @file    geVector2I.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/14
 * @brief   Structure for integer points in 2-d space.
 *
 * Structure for integer points in 2-d space.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

#ifndef _INC_VECTOR2I_H_
# define _INC_VECTOR2I_H_
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
  /**
   * @brief Structure for integer points in 2-d space.
   */
  class Vector2I
  {
   public:
    /**
     * @brief Default constructor (no initialization).
     */
    Vector2I() = default;

    /**
     * @brief Create and initialize a new instance with the specified coordinates.
     * @param InX The x-coordinate.
     * @param InY The y-coordinate.
     */
    Vector2I(int32 InX, int32 InY);

    /**
     * @brief Create and initialize a new instance to zero.
     * @param fie Force init enum
     */
    explicit FORCEINLINE Vector2I(FORCE_INIT::E fie);

    explicit FORCEINLINE Vector2I(int32 val) : x(val), y(val) {}

    /**
     * @brief Get specific component of a point.
     * @param Index Index of vector component.
     * @return const reference to component.
     */
    const int32&
    operator()(uint32 Index) const;

    /**
     * @brief Get specific component of a point.
     * @param PointIndex Index of point component
     * @return reference to component.
     */
    int32&
    operator()(uint32 Index);

    /**
     * @brief Compare two vectors for equality.
     * @param Other The other vector being compared.
     * @return true if the vectors are equal, false otherwise.
     */
    bool
    operator==(const Vector2I& Other) const;

    /**
     * @brief Compare two vectors for inequality.
     * @param Other The other vector being compared.
     * @return true if the vectors are not equal, false otherwise.
     */
    bool
    operator!=(const Vector2I& Other) const;

    /**
     * @brief Add another vector component-wise to this vector.
     * @param Other The vector to add to this vector.
     * @return Reference to this vector after addition.
     */
    Vector2I&
    operator+=(const Vector2I& Other);

    /**
     * @brief Subtract another vector component-wise from this vector.
     * @param Other The vector to subtract from this vector.
     * @return Reference to this vector after subtraction.
     */
    Vector2I&
    operator-=(const Vector2I& Other);

    /**
     * @brief Scale this vector.
     * @param Scale What to multiply the vector by.
     * @return Reference to this vector after multiplication.
     */
    Vector2I&
    operator*=(int32 Scale);

    /**
     * @brief Divide this vector by a scalar.
     * @param Divisor What to divide the vector by.
     * @return Reference to this vector after division.
     */
    Vector2I&
    operator/=(int32 Divisor);

    /**
     * @brief Divide this vector component-wise by another vector.
     * @param Other The vector to divide with.
     * @return Reference to this vector after division.
     */
    Vector2I&
    operator/=(const Vector2I& Other);

    /**
     * @brief Get the result of scaling on this vector.
     * @param Scale What to multiply the vector by.
     * @return A new scaled vector.
     */
    Vector2I
    operator*(int32 Scale) const;

    /**
     * @brief Get the result of division on this vector.
     * @param Divisor What to divide the vector by.
     * @return A new divided vector.
     */
    Vector2I
    operator/(int32 Divisor) const;

    /**
     * @brief Get the result of addition on this vector.
     * @param Other The other vector to add to this.
     * @return A new combined vector.
     */
    Vector2I
    operator+(const Vector2I& Other) const;

    /**
     * @brief Get the result of addition on this vector.
     * @param Scale The value to add to each component of this vector.
     * @return A new vector with each component increased by Scale.
     */
    Vector2I
    operator+(int32 Scale) const;

    /**
     * @brief Get the result of subtraction from this vector.
     * @param Other The other vector to subtract from this.
     * @return A new subtracted vector.
     */
    Vector2I
    operator-(const Vector2I& Other) const;

    /**
     * @brief Get the result of subtraction from this vector.
     * @param Scale The value to subtract from each component of this vector.
     * @return A new vector with each component decreased by Scale.
     */
    Vector2I
    operator-(int32 Scale) const;

    /**
     * @brief Get the result of division on this vector.
     * @param Other The other vector to subtract from this.
     * @return A new subtracted vector.
     */
    Vector2I
    operator/(const Vector2I& Other) const;

    /**
     * @brief Positive operator
     * @return A reference to the real value of the vector
     */
    const Vector2I&
    operator+() const;

    /**
     * @brief Negative operator
     * @return A new vector negative of this one
     */
    Vector2I
    operator-() const;

    /**
    * @brief Dot product operator
    * @return Scalar dot product of this vector
    */
    FORCEINLINE int32
    operator|(const Vector2I& V) const;

    /**
     * @brief Get specific component of the vector.
     * @param Index the index of vector component
     * @return reference to component.
     */
    int32&
    operator[](uint32 Index);

    /**
     * @brief Get specific component of the vector.
     * @param Index the index of vector component
     * @return copy of component value.
     */
    int32
    operator[](uint32 Index) const;

   public:
    /**
     * @brief Exchange the contents of this vector with another.
     */
    FORCEINLINE void
    swap(Vector2I& other);

    /**
     * @brief  Returns the Manhattan distance between this and another point.
     */
    FORCEINLINE uint32
    manhattanDist(const Vector2I& other) const;

    /**
     * @brief Get the component-wise min of two vectors.
     * @see componentMax, getMax
     */
    FORCEINLINE Vector2I
    componentMin(const Vector2I& Other) const;

    /**
     * @brief Get the component-wise max of two vectors.
     * @see componentMin, getMin
     */
    FORCEINLINE Vector2I
    componentMax(const Vector2I& Other) const;

    /**
     * @brief Get the larger of the vector's two components.
     * @return The maximum component of the vector.
     * @see getMin, size, sizeSquared
     */
    int32
    getMax() const;

    /**
     * @brief Get the smaller of the vector's two components.
     * @return The minimum component of the vector.
     * @see getMax, size, sizeSquared
     */
    int32
    getMin() const;

    /**
     * @brief Get the distance of this vector from (0,0).
     * @return The distance of this vector from (0,0).
     * @see getMax, getMin, sizeSquared
     */
    int32
    size() const;

    /**
     * @brief Get the squared distance of this point from (0,0).
     * @return The squared distance of this point from (0,0).
     * @see GetMax, GetMin, Size
     */
    int32
    sizeSquared() const;

    /**
     * @brief Calculates the dot (scalar) product of this vector with another.
     */
    int32
    dot(const Vector2I& vec) const;

   public:
    /**
     * @brief Divide an int vector and round up the result.
     * @param lhs The int vector being divided.
     * @param Divisor What to divide the int vector by.
     * @return A new divided int vector.
     * @see divideAndRoundDown
     */
    static Vector2I
    divideAndRoundUp(Vector2I lhs, int32 Divisor);

    static Vector2I
    divideAndRoundUp(Vector2I lhs, Vector2I Divisor);

    /**
     * @brief Divide an int vector and round down the result.
     * @param lhs The int vector being divided.
     * @param Divisor What to divide the int vector by.
     * @return A new divided int vector.
     * @see divideAndRoundUp
     */
    static Vector2I
    divideAndRoundDown(Vector2I lhs, int32 Divisor);

    /**
     * @brief Get number of components vector has.
     * @return number of components vector has.
     */
    static int32
    num();

    /**
     * @brief Holds the point's x-coordinate.
     */
    int32 x;

    /**
    * @brief Holds the point's y-coordinate.
    */
    int32 y;

    /**
     * @brief An integer point with zeroed values.
     */
    GE_UTILITIES_EXPORT static const Vector2I ZERO;

    GE_REFLECTED_CLASS()
  };

  /***************************************************************************/
  /**
   * Vector2I inline functions
   */
  /***************************************************************************/

  FORCEINLINE Vector2I::Vector2I(int32 InX, int32 InY) : x(InX), y(InY) {}

  FORCEINLINE Vector2I::Vector2I(FORCE_INIT::E) : x(0), y(0) {}

  FORCEINLINE const int32&
  Vector2I::operator()(uint32 Index) const {
    GE_ASSERT(Index < 2);
    return (&x)[Index];
  }

  FORCEINLINE int32&
  Vector2I::operator()(uint32 Index) {
    return (&x)[Index];
  }

  FORCEINLINE int32
  Vector2I::num() {
    return 2;
  }

  FORCEINLINE bool
  Vector2I::operator==(const Vector2I& Other) const {
    return x == Other.x && y == Other.y;
  }

  FORCEINLINE bool
  Vector2I::operator!=(const Vector2I& Other) const {
    return (x != Other.x || y != Other.y);
  }

  FORCEINLINE Vector2I&
    Vector2I::operator+=(const Vector2I& Other) {
    x += Other.x;
    y += Other.y;
    return *this;
  }

  FORCEINLINE Vector2I&
    Vector2I::operator-=(const Vector2I& Other) {
    x -= Other.x;
    y -= Other.y;
    return *this;
  }

  FORCEINLINE Vector2I&
  Vector2I::operator*=(int32 Scale) {
    x *= Scale;
    y *= Scale;
    return *this;
  }

  FORCEINLINE Vector2I&
  Vector2I::operator/=(int32 Divisor) {
    GE_ASSERT(Divisor);
    x /= Divisor;
    y /= Divisor;
    return *this;
  }

  FORCEINLINE Vector2I&
    Vector2I::operator/=(const Vector2I& Other) {
    x /= Other.x;
    y /= Other.y;
    return *this;
  }

  FORCEINLINE Vector2I
  Vector2I::operator*(int32 Scale) const {
    return Vector2I(*this) *= Scale;
  }

  FORCEINLINE Vector2I
  Vector2I::operator/(int32 Divisor) const {
    GE_ASSERT(Divisor);
    return Vector2I(*this) /= Divisor;
  }

  FORCEINLINE const Vector2I&
  Vector2I::operator+() const {
    return *this;
  }

  FORCEINLINE Vector2I
  Vector2I::operator-() const {
    return Vector2I(-x, -y);
  }

  FORCEINLINE int32&
  Vector2I::operator[](uint32 Index) {
    GE_ASSERT(Index < 2);
    return ((0 == Index) ? x : y);
  }

  FORCEINLINE int32
  Vector2I::operator[](uint32 Index) const {
    GE_ASSERT(Index < 2);
    return ((0 == Index) ? x : y);
  }

  FORCEINLINE void
    Vector2I::swap(Vector2I& other) {
    std::swap(x, other.x);
    std::swap(y, other.y);
  }

  FORCEINLINE uint32
  Vector2I::manhattanDist(const Vector2I& other) const {
    return  static_cast<uint32>(Math::abs(float(other.x - x))) +
            static_cast<uint32>(Math::abs(float(other.y - y)));
  }

  FORCEINLINE Vector2I
  Vector2I::componentMin(const Vector2I& Other) const {
    return Vector2I(Math::min(x, Other.x), Math::min(y, Other.y));
  }

  FORCEINLINE Vector2I
  Vector2I::componentMax(const Vector2I& Other) const {
    return Vector2I(Math::max(x, Other.x), Math::max(y, Other.y));
  }

  FORCEINLINE Vector2I
  Vector2I::divideAndRoundUp(Vector2I lhs, int32 Divisor) {
    return Vector2I(Math::divideAndRoundUp(lhs.x, Divisor),
                    Math::divideAndRoundUp(lhs.y, Divisor));
  }

  FORCEINLINE Vector2I
  Vector2I::divideAndRoundUp(Vector2I lhs, Vector2I Divisor) {
    return Vector2I(Math::divideAndRoundUp(lhs.x, Divisor.x),
                    Math::divideAndRoundUp(lhs.y, Divisor.y));
  }

  FORCEINLINE Vector2I
  Vector2I::divideAndRoundDown(Vector2I lhs, int32 Divisor) {
    return Vector2I(Math::divideAndRoundDown(lhs.x, Divisor),
                    Math::divideAndRoundDown(lhs.y, Divisor));
  }

  FORCEINLINE Vector2I
  Vector2I::operator+(const Vector2I& Other) const {
    return Vector2I(*this) += Other;
  }

  FORCEINLINE Vector2I
  Vector2I::operator+(int32 Scale) const {
    return Vector2I(x + Scale, y + Scale);
  }

  FORCEINLINE Vector2I
  Vector2I::operator-(int32 Scale) const {
    return Vector2I(x - Scale, y - Scale);
  }

  FORCEINLINE Vector2I
  Vector2I::operator-(const Vector2I& Other) const {
    return Vector2I(*this) -= Other;
  }

  FORCEINLINE Vector2I
  Vector2I::operator/(const Vector2I& Other) const {
    return Vector2I(*this) /= Other;
  }

  FORCEINLINE int32
  Vector2I::getMax() const {
    return Math::max(x, y);
  }

  FORCEINLINE int32
  Vector2I::getMin() const {
    return Math::min(x, y);
  }

  FORCEINLINE int32
  Vector2I::size() const {
    int64 X64 = static_cast<int64>(x);
    int64 Y64 = static_cast<int64>(y);
    return int32(Math::sqrt(float(X64 * X64 + Y64 * Y64)));
  }

  FORCEINLINE int32
  Vector2I::sizeSquared() const {
    return Math::square(x) + Math::square(y);
  }

  FORCEINLINE int32
  Vector2I::operator|(const Vector2I& V) const {
    return x * V.x + y * V.y;
  }

  FORCEINLINE int32
  Vector2I::dot(const Vector2I& vec) const {
    return *this | vec;
  }
}
