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
    auto& m = M.m;

    const float m00 = m[0][0], m11 = m[1][1], m22 = m[2][2];
    const float trace = m00 + m11 + m22;

    if (trace > 0.0f) {
      const float s = Math::sqrt(trace + 1.0f) * 2.0f; // s = 4w
      x = (m[1][2] - m[2][1]) / s;
      y = (m[2][0] - m[0][2]) / s;
      z = (m[0][1] - m[1][0]) / s;
      w = 0.25f * s;
    }
    else if (m00 > m11 && m00 > m22) {
      const float s = Math::sqrt(1.0f + m00 - m11 - m22) * 2.0f; // s = 4x
      x = 0.25f * s;
      y = (m[0][1] + m[1][0]) / s;
      z = (m[0][2] + m[2][0]) / s;
      w = (m[1][2] - m[2][1]) / s;
    }
    else if (m11 > m22) {
      const float s = Math::sqrt(1.0f + m11 - m00 - m22) * 2.0f; // s = 4y
      x = (m[0][1] + m[1][0]) / s;
      y = 0.25f * s;
      z = (m[1][2] + m[2][1]) / s;
      w = (m[2][0] - m[0][2]) / s;
    }
    else {
      const float s = Math::sqrt(1.0f + m22 - m00 - m11) * 2.0f; // s = 4z
      x = (m[0][2] + m[2][0]) / s;
      y = (m[1][2] + m[2][1]) / s;
      z = 0.25f * s;
      w = (m[0][1] - m[1][0]) / s;
    }

    normalize();
    diagnosticCheckNaN();
  }

  Quaternion::Quaternion(const Rotator& R) {
    *this = R.toQuaternion();
    diagnosticCheckNaN();
  }

  Quaternion
  Quaternion::operator*(const Quaternion& Q) const {
    Quaternion r;
    vectorQuaternionMultiply(r, *this, Q);
    r.diagnosticCheckNaN();
    return r;
  }

  Quaternion
  Quaternion::operator*=(const Quaternion& Q) {
    *this = (*this) * Q;
    return *this;
  }

  Quaternion
  Quaternion::getNormalized(float Tolerance) const {
    const float ss = sizeSquared();
    if (ss <= Tolerance) {
      return Quaternion::IDENTITY;
    }

    const float inv = Math::invSqrt(ss);
    return Quaternion(x * inv, y * inv, z * inv, w * inv);
  }

  void
  Quaternion::normalize(float Tolerance) {
    const float ss = sizeSquared();
    if (ss <= Tolerance) {
      *this = Quaternion::IDENTITY;
      return;
    }

    const float inv = Math::invSqrt(ss);
    x *= inv; y *= inv; z *= inv; w *= inv;
    diagnosticCheckNaN();
  }

  Quaternion
  Quaternion::inverse() const {
    const float ss = sizeSquared();
    if (ss <= Math::SMALL_NUMBER) {
      return Quaternion::IDENTITY;
    }

    const float inv = 1.0f / ss;
    return Quaternion(-x * inv, -y * inv, -z * inv, w * inv);
  }

  Vector3
  Quaternion::operator*(const Vector3& V) const {
    return rotateVector(V);
  }

  Matrix4
  Quaternion::operator*(const Matrix4& M) const {
    //Apply this rotation to the matrix's rotation part:
    //Out = Rot * M  => rotate in "world" sense relative to row-vectors.
    //If you want "local", swap the order at call site.
    const Matrix4 R = toMatrix();
    return R * M;
  }

  Rotator
  Quaternion::rotator() const {
    Quaternion q = getNormalized();

    Vector3 f = q.rotateVector(Vector3::FORWARD).getSafeNormal();
    float yawRad = Math::atan2(f.x, f.z).valueRadians();
    float horiz = Math::sqrt(f.x * f.x + f.z * f.z);
    float pitchRad = Math::atan2(f.y, horiz).valueRadians();

    float yawDeg = Math::RAD2DEG * yawRad;
    float pitchDeg = Math::RAD2DEG * pitchRad;

    Rotator yp(pitchDeg, yawDeg, 0.0f);
    Quaternion qYP = yp.toQuaternion().getNormalized();

    Vector3 upReal = q.rotateVector(Vector3::UP).getSafeNormal();
    Vector3 upRef = qYP.rotateVector(Vector3::UP).getSafeNormal();

    float sinTerm = Vector3::dot(f, upRef ^ upReal);
    float cosTerm = Vector3::dot(upRef, upReal);
    float rollRad = Math::atan2(sinTerm, cosTerm).valueRadians();
    float rollDeg = -Math::RAD2DEG * rollRad;

    return Rotator(pitchDeg, yawDeg, rollDeg);
  }

  Quaternion
  Quaternion::makeFromEuler(const Vector3& EulerDegrees) {
    // EulerRad = (pitch, yaw, roll) in radians
    const Quaternion qPitch(Vector3::UNIT_X, Degree(EulerDegrees.x));
    const Quaternion qYaw  (Vector3::UNIT_Y, Degree(EulerDegrees.y));
    const Quaternion qRoll (Vector3::UNIT_Z, Degree(EulerDegrees.z));

    // Roll -> Pitch -> Yaw
    Quaternion q = (qYaw * qPitch * qRoll);
    q.normalize();
    return q;
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

  Vector3
  Quaternion::euler() const {
    return rotator().euler();
  }

  void
  Quaternion::enforceShortestArcWith(const Quaternion& OtherQuat) {
    if (((*this) | OtherQuat) < 0.0f) {
      x = -x;
      y = -y;
      z = -z;
      w = -w;
    }
  }

  Radian
  Quaternion::angularDistance(const Quaternion& Q) const {
    const float d = Math::clamp(Math::abs((*this) | Q), 0.0f, 1.0f);
    return Math::acos(d) * 2.0f;
  }

  Quaternion
  Quaternion::findBetweenNormals(const Vector3& A, const Vector3& B) {
    return findBetweenVectors(A, B);
  }

  Quaternion
  Quaternion::findBetweenVectors(const Vector3& A, const Vector3& B) {
    Vector3 a = A; a.normalize();
    Vector3 b = B; b.normalize();

    const float dot = Math::clamp(a | b, -1.0f, 1.0f);

    //If vectors are nearly identical
    if (dot > 1.0f - Math::KINDA_SMALL_NUMBER) {
      return Quaternion::IDENTITY;
    }

    //If vectors are opposite
    if (dot < -1.0f + Math::KINDA_SMALL_NUMBER) {
      //Choose an arbitrary orthogonal axis
      Vector3 axis = Vector3::UNIT_X ^ a;
      if (axis.sizeSquared() < Math::KINDA_SMALL_NUMBER) {
        axis = Vector3::UNIT_Y ^ a;
      }
      axis.normalize();
      return Quaternion(axis, Radian(Math::PI));
    }

    Vector3 axis = b ^ a;
    axis.normalize();

    const Radian angle = Math::acos(dot);
    Quaternion q(axis, angle);
    q.normalize();

    return q;
  }

  Quaternion
  Quaternion::log() const {
    Quaternion q = getNormalized();
    const float a = Math::acos(Math::clamp(q.w, -1.0f, 1.0f)).valueRadians();
    const float s = Math::sin(a);

    Quaternion r(FORCE_INIT::kForceInitToZero);
    r.w = 0.0f;

    if (Math::abs(s) > Math::SMALL_NUMBER) {
      const float coeff = a / s;
      r.x = q.x * coeff;
      r.y = q.y * coeff;
      r.z = q.z * coeff;
    }
    return r;
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

  Quaternion
  Quaternion::lookRotation(const Vector3& forwardDir) {
    return lookRotation(forwardDir, Vector3::UNIT_Y);
  }

  Quaternion
  Quaternion::lookRotation(const Vector3& forwardDir, const Vector3& upDir) {
    //Build an orthonormal basis (right, up, forward)
    Vector3 f = forwardDir; f.normalize();
    Vector3 r = upDir ^ f; r.normalize();
    Vector3 u = f ^ r;

    //Row-vector rotation matrix:
    //rows are the basis vectors expressed in world? (depends on your Matrix4 convention)
    Matrix4 m = Matrix4::IDENTITY;
    m.m[0][0] = r.x; m.m[0][1] = r.y; m.m[0][2] = r.z;
    m.m[1][0] = u.x; m.m[1][1] = u.y; m.m[1][2] = u.z;
    m.m[2][0] = f.x; m.m[2][1] = f.y; m.m[2][2] = f.z;

    return Quaternion(m);
  }

  Matrix4
  Quaternion::toMatrix() const {
    //Row-major matrix for row-vectors (v' = v * M).
    Quaternion q = this->getNormalized();

    const float xx = q.x * q.x;
    const float yy = q.y * q.y;
    const float zz = q.z * q.z;

    const float xy = q.x * q.y;
    const float xz = q.x * q.z;
    const float yz = q.y * q.z;

    const float wx = q.w * q.x;
    const float wy = q.w * q.y;
    const float wz = q.w * q.z;

    Matrix4 M = Matrix4::IDENTITY;

    //This is the row-vector version
    M.m[0][0] = 1.0f - 2.0f * (yy + zz);
    M.m[0][1] = 2.0f * (xy + wz);
    M.m[0][2] = 2.0f * (xz - wy);

    M.m[1][0] = 2.0f * (xy - wz);
    M.m[1][1] = 1.0f - 2.0f * (xx + zz);
    M.m[1][2] = 2.0f * (yz + wx);

    M.m[2][0] = 2.0f * (xz + wy);
    M.m[2][1] = 2.0f * (yz - wx);
    M.m[2][2] = 1.0f - 2.0f * (xx + yy);

    return M;
  }
}
