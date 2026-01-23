/*****************************************************************************/
/**
 * @file    geRotator.cpp
 * @date    2017/06/28
 * @brief   Implements a container for rotation information.
 *
 * All rotation values are stored in degrees.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geRotator.h"
#include "geVector3.h"
#include "geQuaternion.h"
#include "geMatrix4.h"

namespace geEngineSDK {
  const Rotator Rotator::ZERO = Rotator(0.f, 0.f, 0.f);

  Rotator::Rotator(const Quaternion& Quat) {
    *this = Quat.rotator();
    diagnosticCheckNaN();
  }

  Vector3
  Rotator::euler() const {
    return Vector3(pitch, yaw, roll);
  }

  Rotator
  Rotator::makeFromEuler(const Vector3& Euler) {
    return Rotator(Euler.x, Euler.y, Euler.z);
  }

  Vector3
  Rotator::unrotateVector(const Vector3& V) const {
    return RotationMatrix(*this).getTransposed().transformVector(V);
  }

  Vector3
  Rotator::rotateVector(const Vector3& V) const {
    return RotationMatrix(*this).transformVector(V);
  }

  void
  Rotator::getWindingAndRemainder(Rotator& Winding, Rotator& Remainder) const {
    //YAW
    Remainder.yaw = normalizeAxis(yaw);
    Winding.yaw = yaw - Remainder.yaw;

    //PITCH
    Remainder.pitch = normalizeAxis(pitch);
    Winding.pitch = pitch - Remainder.pitch;

    //ROLL
    Remainder.roll = normalizeAxis(roll);
    Winding.roll = roll - Remainder.roll;
  }

  Rotator
  Rotator::getInverse() const {
    return toQuaternion().inverse().rotator();
  }

  Quaternion
  Rotator::toQuaternion() const {
    diagnosticCheckNaN();
    const float DIVIDE_BY_2 = Math::DEG2RAD / 2.0f;
    float SP, SY, SR;
    float CP, CY, CR;

    Math::sin_cos(&SP, &CP, pitch * DIVIDE_BY_2);
    Math::sin_cos(&SY, &CY, yaw * DIVIDE_BY_2);
    Math::sin_cos(&SR, &CR, roll * DIVIDE_BY_2);

    Quaternion RotationQuat;
    RotationQuat.w = CR * CP * CY + SR * SP * SY;  
    RotationQuat.x = CR * SP * CY + SR * CP * SY;
    RotationQuat.y = CR * CP * SY - SR * SP * CY;
    RotationQuat.z = SR * CP * CY - CR * SP * SY;

    //Very large inputs can cause NaN's. Want to catch this here
    GE_ASSERT(!RotationQuat.containsNaN() &&
              "Invalid input to Rotator::toQuaternion - generated NaN output");

    return RotationQuat;
  }
}
