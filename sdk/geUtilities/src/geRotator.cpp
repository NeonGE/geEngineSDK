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
    Quaternion q = toQuaternion();
    return q.inverse().rotator();
  }

  Quaternion
  Rotator::toQuaternion() const {
    diagnosticCheckNaN();

    const Degree dPitch(-pitch);
    const Degree dYaw(yaw);
    const Degree dRoll(-roll);

    const Quaternion qPitch(Vector3::RIGHT,  dPitch);
    const Quaternion qYaw(Vector3::UP,       dYaw);
    const Quaternion qRoll(Vector3::FORWARD, dRoll);

    Quaternion q = qYaw * qPitch * qRoll;

    return q.getNormalized();
  }
}
