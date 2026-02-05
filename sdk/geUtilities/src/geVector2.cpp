/*****************************************************************************/
/**
 * @file    geVector2.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geVector2.h"
#include "geVector2I.h"
#include "geVector3.h"

#if USING(GE_REFLECTION)
# include "geRTTRMeta.h"
#endif

namespace geEngineSDK {
  const Vector2 Vector2::ZERO = Vector2(0.f, 0.f);
  const Vector2 Vector2::UNIT = Vector2(1.f, 1.f);
  const Vector2 Vector2::UNIT_X = Vector2(1.f, 0.f);
  const Vector2 Vector2::UNIT_Y = Vector2(0.f, 1.f);

  Vector2::Vector2(const Vector3& V) {
    x = V.x;
    y = V.y;
  }

  Vector2::Vector2(const Vector2I& InPos) {
    x = static_cast<float>(InPos.x);
    y = static_cast<float>(InPos.y);
  }

  Vector2I
  Vector2::toVector2I() const {
    return Vector2I(Math::round(x), Math::round(y));
  }

  Vector3
  Vector2::sphericalToUnitCartesian() const {
    const float SinTheta = Math::sin(x);
    return Vector3(Math::cos(y) * SinTheta,
      Math::sin(y) * SinTheta,
      Math::cos(x));
  }

#if USING(GE_REFLECTION)
  RTTR_REGISTRATION
  {
    using namespace rttr;
    registration::class_<Vector2>("Vector2")
      .constructor<>()(
        metaScriptable(),
        metaTooltip("Default constructor with non-initialized values."),
        metaCategory("[Math]"))

      .constructor<float, float>()(
        metaScriptable(),
        metaTooltip("Constructs and initializes a Vector2 from the given components."),
        metaCategory("[Math]"))

      //Properties
      .property("x", &Vector2::x)(
        metaScriptable(),
        metaCategory("[Math]"),
        metaTooltip("X component"))

      .property("y", &Vector2::y)(
        metaScriptable(),
        metaCategory("[Math]"),
        metaTooltip("Y component"))

      //Constants
      .property_readonly("ZERO", &Vector2::ZERO)(
        metaScriptable(),
        metaReadOnly(),
        metaTooltip("Constant vector (0, 0)."),
        metaCategory("[Math]"))

      .property_readonly("UNIT", &Vector2::UNIT)(
        metaScriptable(),
        metaReadOnly(),
        metaTooltip("Constant vector (1, 1)."),
        metaCategory("[Math]"))

      .property_readonly("UNIT_X", &Vector2::UNIT_X)(
        metaScriptable(),
        metaReadOnly(),
        metaTooltip("Constant vector (1, 0)."),
        metaCategory("[Math]"))

      .property_readonly("UNIT_Y", &Vector2::UNIT_Y)(
        metaScriptable(),
        metaReadOnly(),
        metaTooltip("Constant vector (0, 1)."),
        metaCategory("[Math]"))

      //Methods
      .method("size", &Vector2::size)(
        metaScriptable(),
        metaTooltip("Calculates the size (magnitude) of this vector."),
        metaCategory("[Math]"))
      .method("sizeSquared", &Vector2::sizeSquared)(
        metaScriptable(),
        metaTooltip("Calculates the squared size of this vector."),
        metaCategory("[Math]"))

      .method("dot", &Vector2::operator|)(
        metaScriptable(),
        metaTooltip("Calculates the dot product between this and another vector."),
        metaCategory("[Math]"))
      .method("cross", &Vector2::operator^)(
        metaScriptable(),
        metaTooltip("Calculates the cross product of two vectors."),
        metaCategory("[Math]"))

      .method("component",
        rttr::select_overload<float(const uint32) const>(&Vector2::component))(
          metaScriptable(),
          metaTooltip("Returns the specified component of the vector (0 = X, 1 = Y)."),
          metaCategory("[Math]"))

      .method("getMax", &Vector2::getMax)(
        metaScriptable(),
        metaTooltip("Returns the largest component of the vector."),
        metaCategory("[Math]"))
      .method("getAbsMax", &Vector2::getAbsMax)(
        metaScriptable(),
        metaTooltip("Returns the largest absolute component of the vector."),
        metaCategory("[Math]"))
      .method("getMin", &Vector2::getMin)(
        metaScriptable(),
        metaTooltip("Returns the smallest component of the vector."),
        metaCategory("[Math]"))
      .method("getRotated", &Vector2::getRotated)(
        metaScriptable(),
        metaTooltip("Returns a copy of this vector rotated by AngleDeg degrees "\
                    "around the Z axis."),
        metaCategory("[Math]"))
      .method("isZero", &Vector2::isZero)(
        metaScriptable(),
        metaTooltip("Checks whether this vector is exactly zero."),
        metaCategory("[Math]"))
      .method("sphericalToUnitCartesian", &Vector2::sphericalToUnitCartesian)(
        metaScriptable(),
        metaTooltip("Converts this vector, interpreted as spherical coordinates "\
                    "(theta=X, phi=Y), to a unit Cartesian Vector3."),
        metaCategory("[Math]"))

      .method("normalize", &Vector2::normalize)(
        metaScriptable(),
        metaTooltip("Normalizes this vector in place if it is large enough." \
                    "Sets to zero vector otherwise."),
        metaCategory("[Math]"))
      .method("getNormalized", &Vector2::getSafeNormal)(
        metaScriptable(),
        metaTooltip("Returns a normalized copy of this vector if it is large enough." \
                    "Returns zero vector otherwise."),
        metaCategory("[Math]"))

        //Vector arithmetic operators
      .method("add",
        rttr::select_overload<Vector2(const Vector2&) const>(&Vector2::operator+))(
        metaScriptable(),
        metaTooltip("Adds this vector and another."),
        metaCategory("[Math]"))

      .method("sub",
        rttr::select_overload<Vector2(const Vector2&) const>(&Vector2::operator-))(
        metaScriptable(),
        metaTooltip("Subtracts another vector from this vector."),
        metaCategory("[Math]"))

      .method("equals",
          rttr::select_overload<bool(const Vector2&) const>(&Vector2::operator==))(
        metaScriptable(),
        metaTooltip("Checks whether this vector is equal to another."),
        metaCategory("[Math]"))
    ;
  }
#endif // USING(GE_REFLECTION)
}
