/*****************************************************************************/
/**
 * @file    geDegree.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/04
 * @brief   Wrapper class which indicates a given angle value is in Degrees.
 *
 * Degree values are interchangeable with Radian values, and conversions will
 * be done automatically between them.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geDegree.h"
#include "geMath.h"

namespace geEngineSDK {
  Degree::Degree(const Radian& r) : m_degree(r.valueDegrees()) {}

  Degree&
  Degree::operator=(const Radian& r) {
    m_degree = r.valueDegrees();
    return *this;
  }

  Degree
  Degree::operator+(const Radian& r) const {
    return Degree(m_degree + r.valueDegrees());
  }

  Degree&
  Degree::operator+=(const Radian& r) {
    m_degree += r.valueDegrees();
    return *this;
  }

  Degree
  Degree::operator-(const Radian& r) const {
    return Degree(m_degree - r.valueDegrees());
  }

  Degree&
  Degree::operator-=(const Radian& r) {
    m_degree -= r.valueDegrees();
    return *this;
  }

  float
  Degree::valueRadians() const {
    return m_degree * Math::DEG2RAD;
  }

  void
  Degree::unwindDegrees() {
    m_degree = Math::unwindDegrees(m_degree);
  }
}
