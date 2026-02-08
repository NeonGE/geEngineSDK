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

#if USING(GE_REFLECTION)
# include "geRTTRMeta.h"
#endif

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

#if USING(GE_REFLECTION)
  RTTR_REGISTRATION
  {
    rttr::registration::class_<Degree>("geEngineSDK::Degree")
    .constructor<>()(
      metaScriptable(),
      metaTooltip("Default constructor with non-initialized values."),
      metaCategory("[Math]"))
    .constructor<float>()(
      metaScriptable(),
      metaTooltip("Constructs and initializes a Degree from the given value in degrees."),
      metaCategory("[Math]"))

    //Properties
    .property_readonly("degree", &Degree::valueDegrees)(
      metaScriptable(),
      metaCategory("[Math]"),
      metaTooltip("The angle value in degrees."))

    .property_readonly("radian", &Degree::valueRadians)(
      metaScriptable(),
      metaCategory("[Math]"),
      metaTooltip("The angle value in radians."))

    //Methods
    .method("unwindDegrees", &Degree::unwindDegrees)(
      metaScriptable(),
      metaTooltip("Utility to ensure angle is between +/- 180 degrees by unwinding."),
      metaCategory("[Math]"))

    .method("toRadian", &Degree::valueRadians)(
      metaScriptable(),
      metaTooltip("Converts this Degree value to radians."),
      metaCategory("[Math]"))

    //Operators
    .method("add", rttr::select_overload<Degree(const Degree&) const>(&Degree::operator+))(
      metaScriptable(),
      metaTooltip("Adds two Degree values together."),
      metaCategory("[Math]"))

    .method("add", rttr::select_overload<Degree(const Radian&) const>(&Degree::operator+))(
      metaScriptable(),
      metaTooltip("Adds a Radian value to this Degree value."),
      metaCategory("[Math]"))

    .method("subtract",
      rttr::select_overload<Degree(const Degree&) const>(&Degree::operator-))(
      metaScriptable(),
      metaTooltip("Subtracts one Degree value from another."),
      metaCategory("[Math]"))

    .method("subtract",
      rttr::select_overload<Degree(const Radian&) const>(&Degree::operator-))(
      metaScriptable(),
      metaTooltip("Subtracts a Radian value from this Degree value."),
      metaCategory("[Math]"))

    .method("multiply", rttr::select_overload<Degree(float) const>(&Degree::operator*))(
      metaScriptable(),
      metaTooltip("Multiplies this Degree value by a scalar."),
      metaCategory("[Math]"))

    .method("multiply",
      rttr::select_overload<Degree(const Degree&) const>(&Degree::operator*))(
      metaScriptable(),
      metaTooltip("Multiplies this Degree value by another Degree value."),
      metaCategory("[Math]"))

    .method("divide", rttr::select_overload<Degree(float) const>(&Degree::operator/))(
      metaScriptable(),
      metaTooltip("Divides this Degree value by a scalar."),
      metaCategory("[Math]"))

    .method("equals",
      rttr::select_overload<bool(const Degree&) const>(&Degree::operator==))(
      metaScriptable(),
      metaTooltip("Checks whether this Degree value is equal to another."),
      metaCategory("[Math]"))

    ;
  }
#endif
}
