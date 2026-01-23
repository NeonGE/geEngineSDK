/*****************************************************************************/
/**
 * @file    geVector2.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/14
 * @brief   Structure for 2D vectors with half floating point precision.
 *
 * Structure for 2D vectors with half floating point precision.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

#ifndef _INC_VECTOR2HALF_H_
# define _INC_VECTOR2HALF_H_
#endif

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geMath.h"
#include "geFloat16.h"
#include "geVector2.h"

namespace geEngineSDK {
  class Vector2Half
  {
   public:
    /**
     * @brief Default Constructor (no initialization).
     */
    FORCEINLINE Vector2Half() = default;

    /**
     * @brief Constructor.
     * InX half float X value
     * InY half float Y value
     */
    FORCEINLINE Vector2Half(const Float16& InX, const Float16& InY);

    /**
     * @brief Constructor
     * InX float X value
     * InY float Y value
     */
    FORCEINLINE Vector2Half(float InX, float InY);

    /**
     * @brief Constructor
     * Vector2D float vector
     */
    FORCEINLINE Vector2Half(const Vector2& V);

   public:
    /**
     * @brief Assignment operator.
     * @param V The value to assign.
     */
    Vector2Half&
    operator=(const Vector2& V);

    /**
     * @brief Implicit conversion operator for conversion to FVector2D.
     */
    operator Vector2() const;

   public:
    /**
     * @brief Holds the vector's X-component.
     */
    Float16 x;

    /**
     * @brief Holds the vector's Y-component.
     */
    Float16 y;
  };

  /***************************************************************************/
  /**
   * Vector2Half inline functions
   */
  /***************************************************************************/

  FORCEINLINE Vector2Half::Vector2Half(const Float16& InX, const Float16& InY)
    : x(InX),
      y(InY) {}

  FORCEINLINE Vector2Half::Vector2Half(float InX, float InY) : x(InX), y(InY) {}

  FORCEINLINE Vector2Half::Vector2Half(const Vector2& V) : x(V.x), y(V.x) {}

  FORCEINLINE Vector2Half&
  Vector2Half::operator=(const Vector2& V) {
    x = Float16(V.x);
    y = Float16(V.y);
    return *this;
  }

  FORCEINLINE Vector2Half::operator Vector2() const {
    return Vector2(static_cast<float>(x), static_cast<float>(y));
  }
}
