/*****************************************************************************/
/**
 * @file    geRadian.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/04
 * @brief   Wrapper class which indicates a given angle value is in Radians.
 *
 * Radian values are interchangeable with Degree values, and conversions will
 * be done automatically between them.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geRadian.h"
#include "geMath.h"

namespace geEngineSDK {
  Radian::Radian(const Degree& d) : m_radian(d.valueRadians()) {}

  Radian&
  Radian::operator=(const Degree& d) {
    m_radian = d.valueRadians();
    return *this;
  }

  Radian
  Radian::operator+(const Degree& d) const {
    return Radian(m_radian + d.valueRadians());
  }

  Radian&
  Radian::operator+=(const Degree& d) {
    m_radian += d.valueRadians();
    return *this;
  }

  Radian
  Radian::operator-(const Degree& d) const {
    return Radian(m_radian - d.valueRadians());
  }

  Radian&
  Radian::operator-=(const Degree& d) {
    m_radian -= d.valueRadians();
    return *this;
  }

  float
  Radian::valueDegrees() const {
    return m_radian * Math::RAD2DEG;
  }

  void
  Radian::unwindRadians() {
    m_radian = Math::unwindRadians(m_radian);
  }
}
