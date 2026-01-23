/*****************************************************************************/
/**
 * @file    geMatrix4.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/10/16
 * @brief   Class representing a 4x4 Matrix of floating point values.
 *
 * This class represents a 4x4 Matrix described as column vector.
 * Matrix-matrix multiplication happens with a pre-multiple of the transpose.
 * In other words, Res = Mat1.operator*(Mat2) means Res = Mat2^T * Mat1, as
 * opposed to Res = Mat1 * Mat2.
 * Matrix elements are accessed with m[RowIndex][ColumnIndex].
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geMatrix4.h"

namespace geEngineSDK {
  const Matrix4 Matrix4::ZERO(0.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 0.0f);

  const Matrix4 Matrix4::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f,
                                  0.0f, 1.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f,
                                  0.0f, 0.0f, 0.0f, 1.0f);


  Rotator Matrix4::rotator() const {
    const Vector3 XAxis = getScaledAxis(AXIS::kX);
    const Vector3 YAxis = getScaledAxis(AXIS::kY);
    const Vector3 ZAxis = getScaledAxis(AXIS::kZ);

    Rotator tmpRotator = Rotator(Math::atan2(XAxis.z, Math::sqrt(
                                              Math::square(XAxis.x) +
                                              Math::square(XAxis.y))).valueDegrees(),
                                 Math::atan2(XAxis.y, XAxis.x).valueDegrees(),
                                 0);

    const Vector3 SYAxis = RotationMatrix(tmpRotator).getScaledAxis(AXIS::kY);
    tmpRotator.roll = Math::atan2(ZAxis | SYAxis, YAxis | SYAxis).valueDegrees();

    tmpRotator.diagnosticCheckNaN();
    return tmpRotator;
  }

  Quaternion Matrix4::toQuaternion() const {
    Quaternion Result(*this);
    return Result;
  }

  QuatRotationTranslationMatrix::QuatRotationTranslationMatrix(const Quaternion& Q,
                                                               const Vector3& Origin) {
    //Make sure Quaternion is normalized (Only done in debug)
    GE_ASSERT(Q.isNormalized());

    const float x2 = Q.x + Q.x;  const float y2 = Q.y + Q.y;  const float z2 = Q.z + Q.z;
    const float xx = Q.x * x2;   const float xy = Q.x * y2;   const float xz = Q.x * z2;
    const float yy = Q.y * y2;   const float yz = Q.y * z2;   const float zz = Q.z * z2;
    const float wx = Q.w * x2;   const float wy = Q.w * y2;   const float wz = Q.w * z2;

    m[0][0] = 1.0f - (yy + zz);
    m[1][0] = xy - wz;
    m[2][0] = xz + wy;
    m[3][0] = Origin.x;

    m[0][1] = xy + wz;
    m[1][1] = 1.0f - (xx + zz);
    m[2][1] = yz - wx;
    m[3][1] = Origin.y;

    m[0][2] = xz - wy;
    m[1][2] = yz + wx;
    m[2][2] = 1.0f - (xx + yy);
    m[3][2] = Origin.z;

    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
  }

  Matrix4
  RotationAboutPointMatrix::make(const Quaternion& Rot, const Vector3& Origin) {
    return RotationAboutPointMatrix(Rot.rotator(), Origin);
  }

  Matrix4
  RotationMatrix::make(Quaternion const& Rot) {
    return QuatRotationTranslationMatrix(Rot, Vector3::ZERO);
  }

  Matrix4
  RotationMatrix::makeFromX(Vector3 const& XAxis) {
    Vector3 const NewX = XAxis.getSafeNormal();

    //Try to use up if possible
    Vector3 const UpVector = (Math::abs(NewX.z) < (1.f - Math::KINDA_SMALL_NUMBER)) ?
                              Vector3(0, 0, 1.f) :
                              Vector3(1.f, 0, 0);

    const Vector3 NewY = (UpVector ^ NewX).getSafeNormal();
    const Vector3 NewZ = NewX ^ NewY;

    return Matrix4(NewX, NewY, NewZ, Vector3::ZERO);
  }

  Matrix4
  RotationMatrix::makeFromY(Vector3 const& YAxis) {
    Vector3 const NewY = YAxis.getSafeNormal();

    //Try to use up if possible
    Vector3 const UpVector = (Math::abs(NewY.z) < (1.f - Math::KINDA_SMALL_NUMBER)) ?
                              Vector3(0, 0, 1.f) :
                              Vector3(1.f, 0, 0);

    const Vector3 NewZ = (UpVector ^ NewY).getSafeNormal();
    const Vector3 NewX = NewY ^ NewZ;

    return Matrix4(NewX, NewY, NewZ, Vector3::ZERO);
  }

  Matrix4
  RotationMatrix::makeFromZ(Vector3 const& ZAxis) {
    Vector3 const NewZ = ZAxis.getSafeNormal();

    //Try to use up if possible
    Vector3 const UpVector = (Math::abs(NewZ.z) < (1.f - Math::KINDA_SMALL_NUMBER)) ?
                              Vector3(0, 0, 1.f) :
                              Vector3(1.f, 0, 0);

    const Vector3 NewX = (UpVector ^ NewZ).getSafeNormal();
    const Vector3 NewY = NewZ ^ NewX;

    return Matrix4(NewX, NewY, NewZ, Vector3::ZERO);
  }

  Matrix4
  RotationMatrix::makeFromXY(Vector3 const& XAxis, Vector3 const& YAxis) {
    Vector3 NewX = XAxis.getSafeNormal();
    Vector3 Norm = YAxis.getSafeNormal();

    //if they're almost same, we need to find arbitrary vector
    if (Math::isNearlyEqual(Math::abs(NewX | Norm), 1.f)) {
      //make sure we don't ever pick the same as NewX
      Norm = (Math::abs(NewX.z) < (1.f - Math::KINDA_SMALL_NUMBER)) ?
              Vector3(0, 0, 1.f) :
              Vector3(1.f, 0, 0);
    }

    const Vector3 NewZ = (NewX ^ Norm).getSafeNormal();
    const Vector3 NewY = NewZ ^ NewX;

    return Matrix4(NewX, NewY, NewZ, Vector3::ZERO);
  }

  Matrix4
  RotationMatrix::makeFromXZ(Vector3 const& XAxis, Vector3 const& ZAxis) {
    Vector3 const NewX = XAxis.getSafeNormal();
    Vector3 Norm = ZAxis.getSafeNormal();

    //if they're almost same, we need to find arbitrary vector
    if (Math::isNearlyEqual(Math::abs(NewX | Norm), 1.f)) {
      //make sure we don't ever pick the same as NewX
      Norm = (Math::abs(NewX.z) < (1.f - Math::KINDA_SMALL_NUMBER)) ?
              Vector3(0, 0, 1.f) :
              Vector3(1.f, 0, 0);
    }

    const Vector3 NewY = (Norm ^ NewX).getSafeNormal();
    const Vector3 NewZ = NewX ^ NewY;

    return Matrix4(NewX, NewY, NewZ, Vector3::ZERO);
  }

  Matrix4
  RotationMatrix::makeFromYX(Vector3 const& YAxis, Vector3 const& XAxis) {
    Vector3 const NewY = YAxis.getSafeNormal();
    Vector3 Norm = XAxis.getSafeNormal();

    //if they're almost same, we need to find arbitrary vector
    if (Math::isNearlyEqual(Math::abs(NewY | Norm), 1.f)) {
      // make sure we don't ever pick the same as NewX
      Norm = (Math::abs(NewY.z) < (1.f - Math::KINDA_SMALL_NUMBER)) ?
              Vector3(0, 0, 1.f) :
              Vector3(1.f, 0, 0);
    }

    const Vector3 NewZ = (Norm ^ NewY).getSafeNormal();
    const Vector3 NewX = NewY ^ NewZ;

    return Matrix4(NewX, NewY, NewZ, Vector3::ZERO);
  }

  Matrix4
  RotationMatrix::makeFromYZ(Vector3 const& YAxis, Vector3 const& ZAxis) {
    Vector3 const NewY = YAxis.getSafeNormal();
    Vector3 Norm = ZAxis.getSafeNormal();

    //if they're almost same, we need to find arbitrary vector
    if (Math::isNearlyEqual(Math::abs(NewY | Norm), 1.f)) {
      //make sure we don't ever pick the same as NewX
      Norm = (Math::abs(NewY.z) < (1.f - Math::KINDA_SMALL_NUMBER)) ?
              Vector3(0, 0, 1.f) :
              Vector3(1.f, 0, 0);
    }

    const Vector3 NewX = (NewY ^ Norm).getSafeNormal();
    const Vector3 NewZ = NewX ^ NewY;

    return Matrix4(NewX, NewY, NewZ, Vector3::ZERO);
  }

  Matrix4
  RotationMatrix::makeFromZX(Vector3 const& ZAxis, Vector3 const& XAxis) {
    Vector3 const NewZ = ZAxis.getSafeNormal();
    Vector3 Norm = XAxis.getSafeNormal();

    //if they're almost same, we need to find arbitrary vector
    if (Math::isNearlyEqual(Math::abs(NewZ | Norm), 1.f)) {
      //make sure we don't ever pick the same as NewX
      Norm = (Math::abs(NewZ.z) < (1.f - Math::KINDA_SMALL_NUMBER)) ?
              Vector3(0, 0, 1.f) :
              Vector3(1.f, 0, 0);
    }

    const Vector3 NewY = (NewZ ^ Norm).getSafeNormal();
    const Vector3 NewX = NewY ^ NewZ;

    return Matrix4(NewX, NewY, NewZ, Vector3::ZERO);
  }

  Matrix4
  RotationMatrix::makeFromZY(Vector3 const& ZAxis, Vector3 const& YAxis) {
    Vector3 const NewZ = ZAxis.getSafeNormal();
    Vector3 Norm = YAxis.getSafeNormal();

    //if they're almost same, we need to find arbitrary vector
    if (Math::isNearlyEqual(Math::abs(NewZ | Norm), 1.f)) {
      // make sure we don't ever pick the same as NewX
      Norm = (Math::abs(NewZ.z) < (1.f - Math::KINDA_SMALL_NUMBER)) ?
        Vector3(0, 0, 1.f) :
        Vector3(1.f, 0, 0);
    }

    const Vector3 NewX = (Norm ^ NewZ).getSafeNormal();
    const Vector3 NewY = NewZ ^ NewX;

    return Matrix4(NewX, NewY, NewZ, Vector3::ZERO);
  }
}
