/*****************************************************************************/
/**
 * @file    geBox2DI.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/12
 * @brief   Implements a rectangular integer 2D Box.
 *
 * Implements a rectangular integer 2D Box.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geBox2DI.h"

namespace geEngineSDK {
  Box2DI::Box2DI(const Vector2I* Points, const SIZE_T Count)
    : m_min(0, 0),
      m_max(0, 0),
      m_bIsValid(false) {
    for (SIZE_T PointItr = 0; PointItr < Count; ++PointItr) {
      *this += Points[PointItr];
    }
  }

  Box2DI::Box2DI(const Vector<Vector2I>& Points)
    : m_min(0, 0),
    m_max(0, 0),
    m_bIsValid(false) {
    for (const Vector2I& EachPoint : Points) {
      *this += EachPoint;
    }
  }
}
