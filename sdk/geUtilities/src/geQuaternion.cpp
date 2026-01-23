/*****************************************************************************/
/**
 * @file    geQuaternion.cpp
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
#include "geQuaternion.h"

namespace geEngineSDK {
  const Quaternion Quaternion::IDENTITY = Quaternion(0.f, 0.f, 0.f, 1.f);

  Quaternion::Quaternion(const Matrix4& M) {
    //If Matrix is nullptr, return Identity quaternion. If any of them is 0,
    //you won't be able to construct rotation. If you have two planea at least,
    //we can reconstruct the frame using cross product, but that's a bit
    //expensive op to do here for now, if you convert to matrix from 0 scale
    //and convert back, you'll lose rotation. Don't do that. 
    if (M.getScaledAxis(AXIS::kX).isNearlyZero() ||
      M.getScaledAxis(AXIS::kY).isNearlyZero() ||
      M.getScaledAxis(AXIS::kZ).isNearlyZero()) {
      *this = Quaternion::IDENTITY;
      return;
    }

#if USING(GE_DEBUG_MODE)
    //Make sure the Rotation part of the Matrix is unit length.
    //Changed to this (same as removeScaling) from rotDeterminant as using two
    //different ways of checking unit length matrix caused inconsistency. 

    bool isUnitLenght = (Math::abs(1.f - M.getScaledAxis(AXIS::kX).sizeSquared()) <=
      Math::KINDA_SMALL_NUMBER) &&
      (Math::abs(1.f - M.getScaledAxis(AXIS::kY).sizeSquared()) <=
        Math::KINDA_SMALL_NUMBER) &&
        (Math::abs(1.f - M.getScaledAxis(AXIS::kZ).sizeSquared()) <=
          Math::KINDA_SMALL_NUMBER);

    GE_ASSERT(isUnitLenght);
    if (!isUnitLenght) {
      *this = Quaternion::IDENTITY;
      return;
    }
#endif

    //const MeReal *const t = (MeReal *) tm;
    float	s;

    //Check diagonal (trace)
    const float tr = M.m[0][0] + M.m[1][1] + M.m[2][2];

    if (0.0f < tr) {
      float InvS = Math::invSqrt(tr + 1.f);
      this->w = 0.5f * (1.f / InvS);
      s = 0.5f * InvS;

      this->x = (M.m[1][2] - M.m[2][1]) * s;
      this->y = (M.m[2][0] - M.m[0][2]) * s;
      this->z = (M.m[0][1] - M.m[1][0]) * s;
    }
    else {
      //diagonal is negative
      int32 i = 0;

      if (M.m[1][1] > M.m[0][0]) {
        i = 1;
      }

      if (M.m[2][2] > M.m[i][i]) {
        i = 2;
      }

      static const int32 nxt[3] = { 1, 2, 0 };
      const int32 j = nxt[i];
      const int32 k = nxt[j];

      s = M.m[i][i] - M.m[j][j] - M.m[k][k] + 1.0f;

      float InvS = Math::invSqrt(s);

      float qt[4];
      qt[i] = 0.5f * (1.f / InvS);

      s = 0.5f * InvS;

      qt[3] = (M.m[j][k] - M.m[k][j]) * s;
      qt[j] = (M.m[i][j] + M.m[j][i]) * s;
      qt[k] = (M.m[i][k] + M.m[k][i]) * s;

      this->x = qt[0];
      this->y = qt[1];
      this->z = qt[2];
      this->w = qt[3];

      diagnosticCheckNaN();
    }
  }

  Quaternion::Quaternion(const Rotator& R) {
    *this = R.toQuaternion();
    diagnosticCheckNaN();
  }

  Vector3
  Quaternion::operator*(const Vector3& V) const {
    return rotateVector(V);
  }

  Matrix4
  Quaternion::operator*(const Matrix4& M) const {
    Matrix4 Result;
    Quaternion VT, VR;
    Quaternion Inv = inverse();
    for (int32 I = 0; I<4; ++I) {
      Quaternion VQ(M.m[I][0], M.m[I][1], M.m[I][2], M.m[I][3]);
      vectorQuaternionMultiply(VT, *this, VQ);
      vectorQuaternionMultiply(VR, VT, Inv);
      Result.m[I][0] = VR.x;
      Result.m[I][1] = VR.y;
      Result.m[I][2] = VR.z;
      Result.m[I][3] = VR.w;
    }

    return Result;
  }

  Rotator
  Quaternion::rotator() const {
    diagnosticCheckNaN();
    const float SingularityTest = z * x - w * y;
    const float YawY = 2.f * (w * z + x * y);
    const float YawX = (1.f - 2.f * (Math::square(y) + Math::square(z)));

    //Reference:
    //http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    //http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/

    //this value was found from experience, the above websites recommend different values
    //but that isn't the case for us, so I went through different testing, and finally
    //found the case where both of world lives happily. 
    const float SINGULARITY_THRESHOLD = 0.4999995f;
    Rotator RotatorFromQuat;

    if (-SINGULARITY_THRESHOLD > SingularityTest) {
      RotatorFromQuat.pitch = -90.f;
      RotatorFromQuat.yaw = Math::atan2(YawY, YawX).valueDegrees();
      RotatorFromQuat.roll = Rotator::normalizeAxis(-RotatorFromQuat.yaw -
                                                    (2.f * Math::atan2(x, w).valueDegrees()));
    }
    else if (SINGULARITY_THRESHOLD < SingularityTest) {
      RotatorFromQuat.pitch = 90.f;
      RotatorFromQuat.yaw = Math::atan2(YawY, YawX).valueDegrees();
      RotatorFromQuat.roll = Rotator::normalizeAxis(RotatorFromQuat.yaw -
                                                    (2.f * Math::atan2(x, w).valueDegrees()));
    }
    else {
      RotatorFromQuat.pitch = Math::fastAsin(2.f*(SingularityTest)).valueDegrees();
      RotatorFromQuat.yaw = Math::atan2(YawY, YawX).valueDegrees();
      RotatorFromQuat.roll = Math::atan2(-2.f*(w*x + y*z),
                                         (1.f - 2.f*(x*x + y*y))).valueDegrees();
    }

# if USING(GE_DEBUG_MODE)
    if (RotatorFromQuat.containsNaN()) {
      GE_LOG(kWarning, Generic, "Quaternion::rotator(): Rotator result contains NaN!");
      RotatorFromQuat = Rotator::ZERO;
    }
# endif
    return RotatorFromQuat;
  }

  Quaternion
  Quaternion::makeFromEuler(const Vector3& Euler) {
    return Rotator::makeFromEuler(Euler).toQuaternion();
  }

  void
  Quaternion::toSwingTwist(const Vector3& InTwistAxis,
                           Quaternion& OutSwing,
                           Quaternion& OutTwist) const {
    //Vector part projected onto twist axis
    Vector3 Projection = Vector3::dot(InTwistAxis, Vector3(x, y, z)) * InTwistAxis;

    //Twist quaternion
    OutTwist = Quaternion(Projection.x, Projection.y, Projection.z, w);

    //Singularity close to 180deg
    if(0.0f == OutTwist.sizeSquared()) {
      OutTwist = Quaternion::IDENTITY;
    }
    else {
      OutTwist.normalize();
    }

    //Set swing
    OutSwing = *this * OutTwist.inverse();
  }

  Vector3 Quaternion::euler() const {
    return rotator().euler();
  }

  //Based on:
  //http://lolengine.net/blog/2014/02/24/quaternion-from-two-vectors-final
  //http://www.euclideanspace.com/maths/algebra/vectors/angleBetween/index.htm
  Quaternion
  findBetween_Helper(const Vector3& A, const Vector3& B, float NormAB) {
    float W = NormAB + Vector3::dot(A, B);
    Quaternion Result;

    if (W >= 1e-6f * NormAB) {
      //Axis = Vector::crossProduct(A, B);
      Result = Quaternion(A.y * B.z - A.z * B.y,
                          A.z * B.x - A.x * B.z,
                          A.x * B.y - A.y * B.x,
                          W);
    }
    else {
      //A and B point in opposite directions
      W = 0.f;
      Result = Math::abs(A.x) > Math::abs(A.y) ?
        Quaternion(-A.z, 0.f, A.x, W) :
        Quaternion(0.f, -A.z, A.y, W);
    }

    Result.normalize();
    return Result;
  }

  Quaternion
  Quaternion::findBetweenNormals(const Vector3& A, const Vector3& B) {
    const float NormAB = 1.f;
    return findBetween_Helper(A, B, NormAB);
  }

  Quaternion
  Quaternion::findBetweenVectors(const Vector3& A, const Vector3& B) {
    const float NormAB = Math::sqrt(A.sizeSquared() * B.sizeSquared());
    return findBetween_Helper(A, B, NormAB);
  }

  Quaternion
  Quaternion::log() const {
    Quaternion Result;
    Result.w = 0.f;

    if (1.f > Math::abs(w)) {
      const float Angle = Math::acos(w).valueRadians();
      const float SinAngle = Math::sin(Angle);

      if (Math::abs(SinAngle) >= Math::SMALL_NUMBER) {
        const float Scale = Angle / SinAngle;
        Result.x = Scale*x;
        Result.y = Scale*y;
        Result.z = Scale*z;
        return Result;
      }
    }

    Result.x = x;
    Result.y = y;
    Result.z = z;

    return Result;
  }

  Quaternion
  Quaternion::exp() const {
    const float Angle = Math::sqrt(x*x + y*y + z*z);
    const float SinAngle = Math::sin(Angle);

    Quaternion Result;
    Result.w = Math::cos(Angle);

    if (Math::abs(SinAngle) >= Math::SMALL_NUMBER) {
      const float Scale = SinAngle / Angle;
      Result.x = Scale*x;
      Result.y = Scale*y;
      Result.z = Scale*z;
    }
    else
    {
      Result.x = x;
      Result.y = y;
      Result.z = z;
    }

    return Result;
  }

  Quaternion
  Quaternion::slerp_NotNormalized(const Quaternion& Quat1,
                                  const Quaternion& Quat2,
                                  float Slerp) {
    //Get cosine of angle between Quaternions.
    const float RawCosom = Quat1.x * Quat2.x +
                           Quat1.y * Quat2.y +
                           Quat1.z * Quat2.z +
                           Quat1.w * Quat2.w;
    
    //Unaligned quats - compensate, results in taking shorter route.
    const float Cosom = Math::floatSelect(RawCosom, RawCosom, -RawCosom);
    float Scale0, Scale1;

    if (0.9999f > Cosom) {
      const float Omega = Math::acos(Cosom).valueRadians();
      const float InvSin = 1.f / Math::sin(Omega);
      Scale0 = Math::sin((1.f - Slerp) * Omega) * InvSin;
      Scale1 = Math::sin(Slerp * Omega) * InvSin;
    }
    else {
      //Use linear interpolation.
      Scale0 = 1.0f - Slerp;
      Scale1 = Slerp;
    }

    //In keeping with our flipped Cosom:
    Scale1 = Math::floatSelect(RawCosom, Scale1, -Scale1);

    Quaternion Result;

    Result.x = Scale0 * Quat1.x + Scale1 * Quat2.x;
    Result.y = Scale0 * Quat1.y + Scale1 * Quat2.y;
    Result.z = Scale0 * Quat1.z + Scale1 * Quat2.z;
    Result.w = Scale0 * Quat1.w + Scale1 * Quat2.w;

    return Result;
  }

  Quaternion
  Quaternion::slerpFullPath_NotNormalized(const Quaternion &quat1,
                                          const Quaternion &quat2,
                                          float Alpha) {
    const float CosAngle = Math::clamp(quat1 | quat2, -1.f, 1.f);
    const float Angle = Math::acos(CosAngle).valueRadians();

    if (Math::abs(Angle) < Math::KINDA_SMALL_NUMBER) {
      return quat1;
    }

    const float SinAngle = Math::sin(Angle);
    const float InvSinAngle = 1.f / SinAngle;

    const float Scale0 = Math::sin((1.0f - Alpha)*Angle)*InvSinAngle;
    const float Scale1 = Math::sin(Alpha*Angle)*InvSinAngle;

    return quat1*Scale0 + quat2*Scale1;
  }

  Quaternion
  Quaternion::squad(const Quaternion& quat1,
                    const Quaternion& tang1,
                    const Quaternion& quat2,
                    const Quaternion& tang2,
                    float Alpha) {
    //Always slerp along the short path from quat1 to quat2 to prevent axis flipping.
    //This approach is taken by OGRE engine, amongst others.
    const Quaternion Q1 = Quaternion::slerp_NotNormalized(quat1, quat2, Alpha);
    const Quaternion Q2 = Quaternion::slerpFullPath_NotNormalized(tang1, tang2, Alpha);
    const Quaternion Result = Quaternion::slerpFullPath(Q1, Q2, 2.f * Alpha * (1.f - Alpha));

    return Result;
  }

  Quaternion
  Quaternion::squadFullPath(const Quaternion& quat1,
                            const Quaternion& tang1,
                            const Quaternion& quat2,
                            const Quaternion& tang2,
                            float Alpha) {
    const Quaternion Q1 = Quaternion::slerpFullPath_NotNormalized(quat1, quat2, Alpha);
    const Quaternion Q2 = Quaternion::slerpFullPath_NotNormalized(tang1, tang2, Alpha);
    const Quaternion Result = Quaternion::slerpFullPath(Q1, Q2, 2.f * Alpha * (1.f - Alpha));

    return Result;
  }

  void
  Quaternion::calcTangents(const Quaternion& PrevP,
                           const Quaternion& P,
                           const Quaternion& NextP,
                           float Tension,
                           Quaternion& OutTan) {
    GE_UNREFERENCED_PARAMETER(Tension);

    const Quaternion InvP = P.inverse();
    const Quaternion Part1 = (InvP * PrevP).log();
    const Quaternion Part2 = (InvP * NextP).log();

    const Quaternion PreExp = (Part1 + Part2) * -0.5f;

    OutTan = P * PreExp.exp();
  }

  void
  Quaternion::lookRotation(const Vector3& forwardDir) {
    if (Vector3::ZERO == forwardDir) {
      return;
    }

    Quaternion retValue = IDENTITY;

    Vector3 nrmForwardDir = forwardDir;
    nrmForwardDir.normalize();

    Vector3 currentForwardDir = -retValue.getForwardVector();

    if ((nrmForwardDir + currentForwardDir).sizeSquared() < 0.00005f) {
      //Oops, a 180 degree turn (infinite possible rotation axes)
      //Default to yaw i.e. use current UP
      *this = Quaternion(-z, w, x, -y);
    }
    else {
      //Derive shortest arc to new direction
      Quaternion rotQuat = findBetween(currentForwardDir, nrmForwardDir);
      *this = rotQuat * *this;
    }
  }

  void
  Quaternion::lookRotation(const Vector3& forwardDir, const Vector3& upDir) {
    Vector3 forward = forwardDir;
    Vector3 up = upDir;

    forward.normalize();
    up.normalize();

    if (Math::isNearlyEqual(forward | up, 1.0f)) {
      lookRotation(forward);
      return;
    }

    Vector3 right = forward ^ up;
    Vector3 realUp = right ^ forward;

    right.normalize();
    realUp.normalize();

    w = sqrtf(1.0f + right.x + up.y + forward.z) * 0.5f;
    float w4_recip = 1.0f / (4.0f * w);
    x = (up.z - forward.y) * w4_recip;
    y = (forward.x - right.z) * w4_recip;
    z = (right.y - up.x) * w4_recip;
  }
}
