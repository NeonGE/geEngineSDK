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

namespace geEngineSDK {
  const Vector4 Vector4::ZERO = Vector4(0.f, 0.f, 0.f, 0.f);

  Rotator
  Vector4::toOrientationRotator() const {
    Rotator R;

    //Find yaw.
    R.yaw = Math::atan2(y, x).valueDegrees();

    //Find pitch.
    R.pitch = Math::atan2(z, Math::sqrt(x*x + y*y)).valueDegrees();

    //Find roll.
    R.roll = 0;

# if USING(GE_DEBUG_MODE)
    if (R.containsNaN()) {
      GE_LOG(kWarning, Generic, "Vector4::toRotation(): Rotator result "
             "contains NaN!");
      R = Rotator::ZERO;
    }
# endif
    return R;
  }

  Rotator
  Vector4::rotation() const {
    return toOrientationRotator();
  }

  Quaternion
  Vector4::toOrientationQuat() const {
    //Essentially an optimized Vector->Rotator->Quat made possible by knowing Roll == 0,
    //and avoiding radians->degrees->radians. This is done to avoid adding any roll
    //(which our API states as a constraint).
    const float YawRad = Math::atan2(y, x).valueRadians();
    const float PitchRad = Math::atan2(z, Math::sqrt(x*x + y*y)).valueRadians();

    const float DIVIDE_BY_2 = 0.5f;
    float SP, SY;
    float CP, CY;

    Math::sin_cos(&SP, &CP, PitchRad * DIVIDE_BY_2);
    Math::sin_cos(&SY, &CY, YawRad * DIVIDE_BY_2);

    Quaternion RotationQuat;
    RotationQuat.x = SP*SY;
    RotationQuat.y = -SP*CY;
    RotationQuat.z = CP*SY;
    RotationQuat.w = CP*CY;
    return RotationQuat;
  }

  Vector3
  Rotator::toVector() const {
    float CP, SP, CY, SY;
    Math::sin_cos(&SP, &CP, Math::DEG2RAD * pitch);
    Math::sin_cos(&SY, &CY, Math::DEG2RAD * yaw);

    return Vector3(CP*CY, CP*SY, SP);
  }
}
