/*****************************************************************************/
/**
 * @file    geBox2D.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/29
 * @brief   Implements a rectangular 2D Box.
 *
 * Implements a rectangular 2D Box.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geBox2D.h"

namespace geEngineSDK {
  Box2D::Box2D(const Vector2* Points, const SIZE_T Count)
    : m_min(0.f, 0.f),
      m_max(0.f, 0.f),
      m_bIsValid(false) {
    for (SIZE_T PointItr = 0; PointItr < Count; ++PointItr) {
      *this += Points[PointItr];
    }
  }

  Box2D::Box2D(const Vector<Vector2>& Points)
    : m_min(0.f, 0.f),
    m_max(0.f, 0.f),
    m_bIsValid(false) {
    for (const Vector2& EachPoint : Points) {
      *this += EachPoint;
    }
  }
}
