/*****************************************************************************/
/**
 * @file    geVector4.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/06/28
 * @brief   A 4D homogeneous vector, 4x1 FLOATs, 16-byte aligned.
 *
 * A 4D homogeneous vector, 4x1 FLOATs, 16-byte aligned.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geVector4.h"
#include "geRotator.h"
#include "geQuaternion.h"

#if USING(GE_REFLECTION)
# include "geRTTRMeta.h"
#endif

namespace geEngineSDK {
  const Vector4 Vector4::ZERO = Vector4(0.f, 0.f, 0.f, 0.f);

  Rotator
  Vector4::toOrientationRotator() const {
    Rotator R;

    //yaw around +Y (up), from XZ plane
    R.yaw = Math::atan2(z, x).valueDegrees();

    //pitch around +X, elevation from XZ plane
    R.pitch = Math::atan2(y, Math::sqrt(x * x + z * z)).valueDegrees();

    R.roll = 0.0f;

#if USING(GE_DEBUG_MODE)
    if (R.containsNaN()) {
      GE_LOG(kWarning,
             Generic,
             "Vector4::toOrientationRotator(): Rotator result contains NaN!");
      R = Rotator::ZERO;
    }
#endif
    return R;
  }

  Rotator
  Vector4::rotation() const {
    return toOrientationRotator();
  }

  Quaternion
  Vector4::toOrientationQuat() const {
    //roll=0
    const Radian yawRad = Math::atan2(z, x);
    const Radian pitchRad = Math::atan2(y, Math::sqrt(x * x + z * z));

    Rotator r(pitchRad.valueDegrees(), yawRad.valueDegrees(), 0.0f);
    return r.toQuaternion();
  }

  Vector3
  Rotator::toVector() const {
    float SP, CP, SY, CY;
    Math::sin_cos(&SP, &CP, pitch * Math::DEG2RAD);
    Math::sin_cos(&SY, &CY, yaw * Math::DEG2RAD);

    // forward = +X, up = +Y
    return Vector3(CP * CY, SP, CP * SY);
  }

#if USING(GE_REFLECTION)
  RTTR_REGISTRATION
  {
    using namespace rttr;
    registration::class_<Vector4>("Vector4")
      .constructor<>()(
        metaScriptable(),
        metaTooltip("Default constructor with non-initialized values."),
        metaCategory("[Math]"))

      .constructor<float, float, float, float>()(
        metaScriptable(),
        metaTooltip("Constructs and initializes a Vector4 from the given components."),
        metaCategory("[Math]"))

      //Properties
      .property("x", &Vector4::x)(
        metaScriptable(),
        metaCategory("[Math]"),
        metaTooltip("X component"))

      .property("y", &Vector4::y)(
        metaScriptable(),
        metaCategory("[Math]"),
        metaTooltip("Y component"))

      .property("z", &Vector4::z)(
        metaScriptable(),
        metaCategory("[Math]"),
        metaTooltip("Z component"))

      .property("w", &Vector4::w)(
        metaScriptable(),
        metaCategory("[Math]"),
        metaTooltip("W component"))

      //Constants
      .property_readonly("ZERO", &Vector4::ZERO)(
        metaScriptable(),
        metaReadOnly(),
        metaTooltip("Constant vector (0, 0, 0, 0)."),
        metaCategory("[Math]"))

      //Methods
      .method("size", &Vector4::size)(
        metaScriptable(),
        metaTooltip("Calculates the length of the vector."),
        metaCategory("[Math]"))

      .method("sizeSquared", &Vector4::sizeSquared)(
        metaScriptable(),
        metaTooltip("Calculates the squared length of the vector."),
        metaCategory("[Math]"))

      .method("size3", &Vector4::size3)(
        metaScriptable(),
        metaTooltip("Calculates the length of the 3D vector (ignoring W component)."),
        metaCategory("[Math]"))

      .method("sizeSquared3", &Vector4::sizeSquared3)(
        metaScriptable(),
        metaTooltip("Calculates the squared length of the 3D vector (ignoring W component)."),
        metaCategory("[Math]"))

      .method("toOrientationRotator", &Vector4::toOrientationRotator)(
        metaScriptable(),
        metaTooltip("Converts this vector into an orientation rotator."),
        metaCategory("[Math]"))

      .method("toOrientationQuat", &Vector4::toOrientationQuat)(
        metaScriptable(),
        metaTooltip("Converts this vector into an orientation quaternion."),
        metaCategory("[Math]"))

      .method("rotation", &Vector4::rotation)(
        metaScriptable(),
        metaTooltip("Gets the rotation corresponding to this vector."),
        metaCategory("[Math]"))
      ;
  }
#endif
}
