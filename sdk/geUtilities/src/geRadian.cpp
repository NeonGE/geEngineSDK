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

#if USING(GE_REFLECTION)
# include "geRTTRMeta.h"
#endif

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

#if USING(GE_REFLECTION)
  RTTR_REGISTRATION
  {
    using namespace rttr;
  registration::class_<Radian>("geEngineSDK::Radian")
    .constructor<>()(
      metaScriptable(),
      metaTooltip("Default constructor with non-initialized values."),
      metaCategory("[Math]"))
    .constructor<float>()(
      metaScriptable(),
      metaTooltip("Constructs and initializes a Radian from the given value in radians."),
      metaCategory("[Math]"))

    //Properties
    .property_readonly("radian", &Radian::valueRadians)(
      metaScriptable(),
      metaCategory("[Math]"),
      metaTooltip("The angle value in radians."))
    .property_readonly("degree", &Radian::valueDegrees)(
      metaScriptable(),
      metaCategory("[Math]"),
      metaTooltip("The angle value in degrees."))

    //Methods
    .method("unwindRadians", &Radian::unwindRadians)(
      metaScriptable(),
      metaTooltip("Utility to ensure angle is between +/- PI range by unwinding."),
      metaCategory("[Math]"))

    //Operators
    .method("add", rttr::select_overload<Radian(const Radian&) const>(&Radian::operator+))(
      metaScriptable(),
      metaTooltip("Adds two Radian values together."),
      metaCategory("[Math]"))
    .method("add", rttr::select_overload<Radian(const Degree&) const>(&Radian::operator+))(
      metaScriptable(),
      metaTooltip("Adds a Degree value to this Radian value."),
      metaCategory("[Math]"))
    .method("subtract",
      rttr::select_overload<Radian(const Radian&) const>(&Radian::operator-))(
        metaScriptable(),
        metaTooltip("Subtracts one Radian value from another."),
        metaCategory("[Math]"))
    .method("subtract",
      rttr::select_overload<Radian(const Degree&) const>(&Radian::operator-))(
        metaScriptable(),
        metaTooltip("Subtracts a Degree value from this Radian value."),
        metaCategory("[Math]"))
    .method("multiply", rttr::select_overload<Radian(float) const>(&Radian::operator*))(
      metaScriptable(),
      metaTooltip("Multiplies this Radian value by a scalar."),
      metaCategory("[Math]"))

    .method("divide", rttr::select_overload<Radian(float) const>(&Radian::operator/))(
      metaScriptable(),
      metaTooltip("Divides this Radian value by a scalar."),
      metaCategory("[Math]"))
    ;
  }
#endif
}
