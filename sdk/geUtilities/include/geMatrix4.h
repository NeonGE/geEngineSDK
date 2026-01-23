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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geMath.h"
#include "geVector3.h"
#include "geVector4.h"
#include "geRotator.h"
#include "geQuaternion.h"
#include "gePlane.h"

namespace geEngineSDK {
  class Matrix4
  {
   public:
    /*************************************************************************/
    /**
     * Constructors
     */
    /*************************************************************************/
    FORCEINLINE Matrix4() = default;

    /**
     * @brief Constructor.
     * @param EForceInit Force Init Enum.
     */
    explicit FORCEINLINE Matrix4(FORCE_INIT::E force) {
      memset(this, 0, sizeof(Matrix4));
      if(FORCE_INIT::kForceInit == force) {
        m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
      }
    }

    FORCEINLINE Matrix4(float m00, float m01, float m02, float m03,
                        float m10, float m11, float m12, float m13,
                        float m20, float m21, float m22, float m23,
                        float m30, float m31, float m32, float m33) {
      m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
      m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
      m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
      m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }

    /**
     * @brief Constructor.
     * @param InX X plane
     * @param InY Y plane
     * @param InZ Z plane
     * @param InW W plane
     */
    FORCEINLINE Matrix4(const Plane& InX,
                        const Plane& InY,
                        const Plane& InZ,
                        const Plane& InW);

    /**
     * @brief Constructor.
     * @param InX X vector
     * @param InY Y vector
     * @param InZ Z vector
     * @param InW W vector
     */
    FORCEINLINE Matrix4(const Vector3& InX,
                        const Vector3& InY,
                        const Vector3& InZ,
                        const Vector3& InW);

    /**
     * @brief Swaps the contents of this matrix with another.
     */
    void swap(Matrix4& other) {
      std::swap(m[0][0], other.m[0][0]);
      std::swap(m[0][1], other.m[0][1]);
      std::swap(m[0][2], other.m[0][2]);
      std::swap(m[0][3], other.m[0][3]);
      std::swap(m[1][0], other.m[1][0]);
      std::swap(m[1][1], other.m[1][1]);
      std::swap(m[1][2], other.m[1][2]);
      std::swap(m[1][3], other.m[1][3]);
      std::swap(m[2][0], other.m[2][0]);
      std::swap(m[2][1], other.m[2][1]);
      std::swap(m[2][2], other.m[2][2]);
      std::swap(m[2][3], other.m[2][3]);
      std::swap(m[3][0], other.m[3][0]);
      std::swap(m[3][1], other.m[3][1]);
      std::swap(m[3][2], other.m[3][2]);
      std::swap(m[3][3], other.m[3][3]);
    }

    /**
     * @brief Set this to the identity matrix
     */
    inline void
    setIdentity();

    /**
     * @brief Gets the result of multiplying a Matrix to this.
     * @param Other The matrix to multiply this by.
     * @return  The result of multiplication.
     */
    FORCEINLINE Matrix4
    operator*(const Matrix4& Other) const;

    /**
    * @brief Multiply this by a matrix.
    * @param Other the matrix to multiply by this.
    * @return reference to this after multiply.
    */
    FORCEINLINE void
    operator*=(const Matrix4& Other);

    /**
     * @brief Gets the result of adding a matrix to this.
     * @param Other The Matrix to add.
     * @return  The result of addition.
     */
    FORCEINLINE Matrix4
    operator+(const Matrix4& Other) const;

    /**
     * @brief Adds to this matrix.
     * @param Other The matrix to add to this.
     * @return  Reference to this after addition.
     */
    FORCEINLINE void
    operator+=(const Matrix4& Other);

    /**
     * @brief This isn't applying SCALE, just multiplying float to all members i.e. weighting
     */
    FORCEINLINE Matrix4
    operator*(float Other) const;

    /**
     * @brief Multiply this matrix by a weighting factor.
     * @param other The weight.
     * @return a reference to this after weighting.
     */
    FORCEINLINE void
    operator*=(float Other);

    /**
     * @brief Checks whether two matrix are identical.
     * @param Other The other matrix.
     * @return  true if two matrix are identical, otherwise false.
     */
    inline bool
    operator==(const Matrix4& Other) const;

    /**
     * @brief Checks whether another Matrix is equal to this, within specified tolerance.
     * @param Other The other Matrix.
     * @param Tolerance Error Tolerance.
     * @return  true if two Matrix are equal, within specified tolerance, otherwise false.
     */
    inline bool
    equals(const Matrix4& Other, float Tolerance = Math::KINDA_SMALL_NUMBER) const;

    /**
     * @brief Checks whether another Matrix is not equal to this, within specified tolerance.
     * @param Other The other Matrix.
     * @return  true if two Matrix are not equal, within specified tolerance, otherwise false.
     */
    inline bool
    operator!=(const Matrix4& Other) const;

    /*************************************************************************/
    /**
     * Homogeneous transform.
     */
    /*************************************************************************/

    FORCEINLINE Vector4
    transformVector4(const Vector4& V) const;

    /**
     * @brief Transform a location - will take into account translation part of the Matrix.
     */
    FORCEINLINE Vector4
    transformPosition(const Vector3& V) const;

    /**
     * @brief Inverts the matrix and then transforms V - correctly handles
     *        scaling in this matrix.
     */
    FORCEINLINE Vector3
    inverseTransformPosition(const Vector3& V) const;

    /**
     * @brief Transform a direction vector - will not take into account
     *        translation part of the Matrix.
     * @note  If you want to transform a surface normal (or plane) and correctly account for
     *        non-uniform scaling you should use transformByUsingAdjointT.
     */
    FORCEINLINE Vector4
    transformVector(const Vector3& V) const;

    /**
     * @brief Transform a direction vector by the inverse of this matrix - will
     *        not take into account translation part.
     * @note  If you want to transform a surface normal (or plane) and correctly account for
     *        non-uniform scaling you should use transformByUsingAdjointT with adjoint of
     *        matrix inverse.
     */
    FORCEINLINE Vector3
    inverseTransformVector(const Vector3& V) const;

    /**
     * @brief Transpose.
     */
    FORCEINLINE Matrix4
    getTransposed() const;

    /**
     * @brief Transpose this matrix.
     */
    FORCEINLINE void
    transpose();

    /**
     * @return Determinant of this matrix.
     */
    inline float
    determinant() const;

    /**
     * @return The determinant of rotation 3x3 matrix
     */
    inline float
    rotDeterminant() const;

    /**
     * @brief Fast path, doesn't check for nil matrices in final release builds
     */
    inline Matrix4
    inverseFast() const;

    /**
     * @brief Fast path, and handles nil matrices.
     */
    inline Matrix4
    inverse() const;

    inline Matrix4
    transposeAdjoint() const;

    /**
     * @brief Remove any scaling from this matrix (ie magnitude of each row is 1)
     *        with error Tolerance
     */
    inline void
    removeScaling(float Tolerance = Math::SMALL_NUMBER);

    /**
     * @brief Returns matrix after RemoveScaling with error Tolerance
     */
    inline Matrix4
    getMatrixWithoutScale(float Tolerance = Math::SMALL_NUMBER) const;

    /**
     * @brief Remove any scaling from this matrix (ie magnitude of each row is 1)
     *        and return the 3D scale vector that was initially present with error Tolerance
     */
    inline Vector3
    extractScaling(float Tolerance = Math::SMALL_NUMBER);

    /**
     * @brief return a 3D scale vector calculated from this matrix (where each
     *        component is the magnitude of a row vector) with error Tolerance.
     */
    inline Vector3
    getScaleVector(float Tolerance = Math::SMALL_NUMBER) const;

    /**
     * @brief Remove any translation from this matrix
     */
    inline Matrix4
    removeTranslation() const;

    /**
     * @brief Returns a matrix with an additional translation concatenated.
     */
    inline Matrix4
    concatTranslation(const Vector3& Translation) const;

    /**
     * @brief Returns true if any element of this matrix is NaN
     */
    inline bool
    containsNaN() const;

    /**
     * @brief Scale the translation part of the matrix by the supplied vector.
     */
    inline void
    scaleTranslation(const Vector3& Scale3D);

    /**
     * @return The maximum magnitude of any row of the matrix.
     */
    inline float
    getMaximumAxisScale() const;

    /**
     * @brief Apply Scale to this matrix
     */
    inline Matrix4
    applyScale(float Scale);

    /**
     * @return The origin of the co-ordinate system
     */
    inline Vector3
    getOrigin() const;

    /**
     * @brief get axis of this matrix scaled by the scale of the matrix
     * @param i index into the axis of the matrix
     * @return vector of the axis
     */
    inline Vector3
    getScaledAxis(AXIS::E Axis) const;

    /**
     * @brief Get axes of this matrix scaled by the scale of the matrix
     * @param X axes returned to this param
     * @param Y axes returned to this param
     * @param Z axes returned to this param
     */
    inline void
    getScaledAxes(Vector3 &X, Vector3 &Y, Vector3 &Z) const;

    /**
     * @brief Get unit length axis of this matrix
     * @param i index into the axis of the matrix
     * @return vector of the axis
     */
    inline Vector3
    getUnitAxis(AXIS::E Axis) const;

    /**
     * @brief Get unit length axes of this matrix
     * @param X axes returned to this param
     * @param Y axes returned to this param
     * @param Z axes returned to this param
     */
    inline void
    getUnitAxes(Vector3 &X, Vector3 &Y, Vector3 &Z) const;

    /**
     * @brief Set an axis of this matrix
     * @param i index into the axis of the matrix
     * @param Axis vector of the axis
     */
    inline void
    setAxis(int32 i, const Vector3& Axis);

    /**
     * @brief Set the origin of the coordinate system to the given vector
     */
    inline void
    setOrigin(const Vector3& NewOrigin);

    /**
     * @brief Update the axes of the matrix if any value is NULL do not update that axis
     * @param Axis0 set matrix row 0
     * @param Axis1 set matrix row 1
     * @param Axis2 set matrix row 2
     * @param Origin set matrix row 3
     */
    inline void
    setAxes(Vector3* Axis0 = nullptr,
            Vector3* Axis1 = nullptr,
            Vector3* Axis2 = nullptr,
            Vector3* Origin = nullptr);

    /**
     * @brief get a column of this matrix
     * @param i index into the column of the matrix
     * @return vector of the column
     */
    inline Vector3
    getColumn(int32 i) const;

    /**
     * @return Rotator representation of this matrix
     */
    GE_UTILITIES_EXPORT Rotator
    rotator() const;

    /**
     * @brief Transform a rotation matrix into a quaternion.
     * @warning rotation part will need to be unit length for this to be right!
     */
    GE_UTILITIES_EXPORT Quaternion
    toQuaternion() const;

    /*************************************************************************/
    /**
     * Frustum plane extraction.
     */
    /*************************************************************************/

    /**
     * @param OutPlane the near plane of the Frustum of this matrix
     */
    FORCEINLINE bool
    getFrustumNearPlane(Plane& OutPlane) const;

    /**
     * @param OutPlane the far plane of the Frustum of this matrix
     */
    FORCEINLINE bool
    getFrustumFarPlane(Plane& OutPlane) const;

    /**
     * @param OutPlane the left plane of the Frustum of this matrix
     */
    FORCEINLINE bool
    getFrustumLeftPlane(Plane& OutPlane) const;

    /**
     * @param OutPlane the right plane of the Frustum of this matrix
     */
    FORCEINLINE bool
    getFrustumRightPlane(Plane& OutPlane) const;

    /**
     * @param OutPlane the top plane of the Frustum of this matrix
     */
    FORCEINLINE bool
    getFrustumTopPlane(Plane& OutPlane) const;

    /**
     * @param OutPlane the bottom plane of the Frustum of this matrix
     */
    FORCEINLINE bool
    getFrustumBottomPlane(Plane& OutPlane) const;

    /**
     * @brief Utility for mirroring this transform across a certain plane, and
     *        flipping one of the axis as well.
     */
    inline void
    mirror(AXIS::E MirrorAxis, AXIS::E FlipAxis);

    /**
     * Convert this Atom to the 3x4 transpose of the transformation matrix.
     */
    void
    to3x4MatrixTranspose(float* Out) const {
      const float* Src = &_m[0];
      float* Dest = Out;

      Dest[0] = Src[0];   // [0][0]
      Dest[1] = Src[4];   // [1][0]
      Dest[2] = Src[8];   // [2][0]
      Dest[3] = Src[12];  // [3][0]

      Dest[4] = Src[1];   // [0][1]
      Dest[5] = Src[5];   // [1][1]
      Dest[6] = Src[9];   // [2][1]
      Dest[7] = Src[13];  // [3][1]

      Dest[8] = Src[2];   // [0][2]
      Dest[9] = Src[6];   // [1][2]
      Dest[10] = Src[10]; // [2][2]
      Dest[11] = Src[14]; // [3][2]
    }

   public:
    union {
      float ALIGN_AS(16) m[4][4];
      float ALIGN_AS(16)_m[16];
    };

    /**
     * @brief Constants for Matrix with Zero values and Identity values
     */
    static GE_UTILITIES_EXPORT const Matrix4 ALIGN_AS(16) ZERO;
    static GE_UTILITIES_EXPORT const Matrix4 ALIGN_AS(16) IDENTITY;
  };

  /**
   * @brief A storage class for compile-time fixed size matrices.
   */
  template<uint32 NumRows, uint32 NumColumns>
  class Matrix
  {
   public:
    /*************************************************************************/
    /**
     * Variables
     */
    /*************************************************************************/
    float ALIGN_AS(16) m[NumRows][NumColumns];

    /**
     * @brief Empty constructor
     */
    Matrix();

    /**
     * @brief Construct this object from a 4x4 Matrix
     * @param InMatrix Matrix reference
     */
    Matrix(const Matrix4& InMatrix);
  };

  template<uint32 NumRows, uint32 NumColumns>
  FORCEINLINE Matrix<NumRows, NumColumns>::Matrix() {}

  template<uint32 NumRows, uint32 NumColumns>
  FORCEINLINE Matrix<NumRows, NumColumns>::Matrix(const Matrix4& InMatrix)
  {
    for (uint32 RowIndex = 0; (RowIndex < NumRows) && (RowIndex < 4); ++RowIndex) {
      for (uint32 ColumnIndex = 0;
           (ColumnIndex < NumColumns) && (ColumnIndex < 4);
           ++ColumnIndex) {
        m[RowIndex][ColumnIndex] = InMatrix.m[RowIndex][ColumnIndex];
      }
    }
  }


  class BasisVectorMatrix : public Matrix4
  {
   public:
    //Create Basis matrix from 3 axis vectors and the origin
    BasisVectorMatrix(const Vector3& XAxis,
                      const Vector3& YAxis,
                      const Vector3& ZAxis,
                      const Vector3& Origin);
  };


  class LookAtMatrix : public Matrix4
  {
  public:
    /**
     * @brief Creates a view matrix using an eye position, a look at position, and up vector.
     *        This does the same thing as D3DXMatrixLookAtLH.
     */
    LookAtMatrix(const Vector3& EyePosition,
                 const Vector3& LookAtPosition,
                 const Vector3& UpVector);
  };

  //Very high quality 4x4 matrix inverse
  static inline void
  inverse4x4(double* dst, const float* src) {
    const auto s0 =  static_cast<double>(src[0]);
    const auto s1 =  static_cast<double>(src[1]);
    const auto s2 =  static_cast<double>(src[2]);
    const auto s3 =  static_cast<double>(src[3]);
    const auto s4 =  static_cast<double>(src[4]);
    const auto s5 =  static_cast<double>(src[5]);
    const auto s6 =  static_cast<double>(src[6]);
    const auto s7 =  static_cast<double>(src[7]);
    const auto s8 =  static_cast<double>(src[8]);
    const auto s9 =  static_cast<double>(src[9]);
    const auto s10 = static_cast<double>(src[10]);
    const auto s11 = static_cast<double>(src[11]);
    const auto s12 = static_cast<double>(src[12]);
    const auto s13 = static_cast<double>(src[13]);
    const auto s14 = static_cast<double>(src[14]);
    const auto s15 = static_cast<double>(src[15]);

    double inv[16];
    inv[0] = s5 * s10 * s15 -
             s5 * s11 * s14 -
             s9 * s6 * s15 +
             s9 * s7 * s14 +
             s13 * s6 * s11 -
             s13 * s7 * s10;
    inv[1] = -s1 * s10 * s15 +
             s1 * s11 * s14 +
             s9 * s2 * s15 -
             s9 * s3 * s14 -
             s13 * s2 * s11 +
             s13 * s3 * s10;
    inv[2] = s1 * s6  * s15 -
             s1 * s7  * s14 -
             s5 * s2 * s15 +
             s5 * s3 * s14 +
             s13 * s2 * s7 -
             s13 * s3 * s6;
    inv[3] = -s1 * s6  * s11 +
             s1 * s7  * s10 +
             s5 * s2 * s11 -
             s5 * s3 * s10 -
             s9  * s2 * s7 +
             s9  * s3 * s6;
    inv[4] = -s4 * s10 * s15 +
             s4 * s11 * s14 +
             s8 * s6 * s15 -
             s8 * s7 * s14 -
             s12 * s6 * s11 +
             s12 * s7 * s10;
    inv[5] = s0 * s10 * s15 -
             s0 * s11 * s14 -
             s8 * s2 * s15 +
             s8 * s3 * s14 +
             s12 * s2 * s11 -
             s12 * s3 * s10;
    inv[6] = -s0 * s6  * s15 +
             s0 * s7  * s14 +
             s4 * s2 * s15 -
             s4 * s3 * s14 -
             s12 * s2 * s7 +
             s12 * s3 * s6;
    inv[7] = s0 * s6  * s11 -
             s0 * s7  * s10 -
             s4 * s2 * s11 +
             s4 * s3 * s10 +
             s8  * s2 * s7 -
             s8  * s3 * s6;
    inv[8] = s4 * s9  * s15 -
             s4 * s11 * s13 -
             s8 * s5 * s15 +
             s8 * s7 * s13 +
             s12 * s5 * s11 -
             s12 * s7 * s9;
    inv[9] = -s0 * s9  * s15 +
             s0 * s11 * s13 +
             s8 * s1 * s15 -
             s8 * s3 * s13 -
             s12 * s1 * s11 +
             s12 * s3 * s9;
    inv[10] = s0 * s5  * s15 -
              s0 * s7  * s13 -
              s4 * s1 * s15 +
              s4 * s3 * s13 +
              s12 * s1 * s7 -
              s12 * s3 * s5;
    inv[11] = -s0 * s5  * s11 +
              s0 * s7  * s9 +
              s4 * s1 * s11 -
              s4 * s3 * s9 -
              s8  * s1 * s7 +
              s8  * s3 * s5;
    inv[12] = -s4 * s9  * s14 +
              s4 * s10 * s13 +
              s8 * s5 * s14 -
              s8 * s6 * s13 -
              s12 * s5 * s10 +
              s12 * s6 * s9;
    inv[13] = s0 * s9  * s14 -
              s0 * s10 * s13 -
              s8 * s1 * s14 +
              s8 * s2 * s13 +
              s12 * s1 * s10 -
              s12 * s2 * s9;
    inv[14] = -s0 * s5  * s14 +
              s0 * s6  * s13 +
              s4 * s1 * s14 -
              s4 * s2 * s13 -
              s12 * s1 * s6 +
              s12 * s2 * s5;
    inv[15] = s0 * s5  * s10 -
              s0 * s6  * s9 -
              s4 * s1 * s10 +
              s4 * s2 * s9 +
              s8  * s1 * s6 -
              s8  * s2 * s5;

    double det = s0 * inv[0] + s1 * inv[4] + s2 * inv[8] + s3 * inv[12];
    if (0.0 != det) {
      det = 1.0 / det;
    }
    for (int i = 0; i < 16; ++i) {
      dst[i] = inv[i] * det;
    }
  }
}

namespace geEngineSDK {
  /***************************************************************************/
  /**
   * Constructors
   */
  /***************************************************************************/

  FORCEINLINE Matrix4::Matrix4(const Plane& InX,
                               const Plane& InY,
                               const Plane& InZ,
                               const Plane& InW) {
    m[0][0] = InX.x; m[0][1] = InX.y;  m[0][2] = InX.z;  m[0][3] = InX.w;
    m[1][0] = InY.x; m[1][1] = InY.y;  m[1][2] = InY.z;  m[1][3] = InY.w;
    m[2][0] = InZ.x; m[2][1] = InZ.y;  m[2][2] = InZ.z;  m[2][3] = InZ.w;
    m[3][0] = InW.x; m[3][1] = InW.y;  m[3][2] = InW.z;  m[3][3] = InW.w;
  }

  FORCEINLINE Matrix4::Matrix4(const Vector3& InX,
                               const Vector3& InY,
                               const Vector3& InZ,
                               const Vector3& InW) {
    m[0][0] = InX.x; m[0][1] = InX.y;  m[0][2] = InX.z;  m[0][3] = 0.0f;
    m[1][0] = InY.x; m[1][1] = InY.y;  m[1][2] = InY.z;  m[1][3] = 0.0f;
    m[2][0] = InZ.x; m[2][1] = InZ.y;  m[2][2] = InZ.z;  m[2][3] = 0.0f;
    m[3][0] = InW.x; m[3][1] = InW.y;  m[3][2] = InW.z;  m[3][3] = 1.0f;
  }


  inline void
  Matrix4::setIdentity() {
    m[0][0] = 1; m[0][1] = 0;  m[0][2] = 0;  m[0][3] = 0;
    m[1][0] = 0; m[1][1] = 1;  m[1][2] = 0;  m[1][3] = 0;
    m[2][0] = 0; m[2][1] = 0;  m[2][2] = 1;  m[2][3] = 0;
    m[3][0] = 0; m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
  }

  FORCEINLINE Matrix4
  Matrix4::operator*(const Matrix4& Other) const {
    Matrix4 Result;
    Result.m[0][0] = m[0][0] * Other.m[0][0] + m[0][1] * Other.m[1][0] +
                     m[0][2] * Other.m[2][0] + m[0][3] * Other.m[3][0];
    Result.m[0][1] = m[0][0] * Other.m[0][1] + m[0][1] * Other.m[1][1] +
                     m[0][2] * Other.m[2][1] + m[0][3] * Other.m[3][1];
    Result.m[0][2] = m[0][0] * Other.m[0][2] + m[0][1] * Other.m[1][2] +
                     m[0][2] * Other.m[2][2] + m[0][3] * Other.m[3][2];
    Result.m[0][3] = m[0][0] * Other.m[0][3] + m[0][1] * Other.m[1][3] +
                     m[0][2] * Other.m[2][3] + m[0][3] * Other.m[3][3];

    Result.m[1][0] = m[1][0] * Other.m[0][0] + m[1][1] * Other.m[1][0] +
                     m[1][2] * Other.m[2][0] + m[1][3] * Other.m[3][0];
    Result.m[1][1] = m[1][0] * Other.m[0][1] + m[1][1] * Other.m[1][1] +
                     m[1][2] * Other.m[2][1] + m[1][3] * Other.m[3][1];
    Result.m[1][2] = m[1][0] * Other.m[0][2] + m[1][1] * Other.m[1][2] +
                     m[1][2] * Other.m[2][2] + m[1][3] * Other.m[3][2];
    Result.m[1][3] = m[1][0] * Other.m[0][3] + m[1][1] * Other.m[1][3] +
                     m[1][2] * Other.m[2][3] + m[1][3] * Other.m[3][3];

    Result.m[2][0] = m[2][0] * Other.m[0][0] + m[2][1] * Other.m[1][0] +
                     m[2][2] * Other.m[2][0] + m[2][3] * Other.m[3][0];
    Result.m[2][1] = m[2][0] * Other.m[0][1] + m[2][1] * Other.m[1][1] +
                     m[2][2] * Other.m[2][1] + m[2][3] * Other.m[3][1];
    Result.m[2][2] = m[2][0] * Other.m[0][2] + m[2][1] * Other.m[1][2] +
                     m[2][2] * Other.m[2][2] + m[2][3] * Other.m[3][2];
    Result.m[2][3] = m[2][0] * Other.m[0][3] + m[2][1] * Other.m[1][3] +
                     m[2][2] * Other.m[2][3] + m[2][3] * Other.m[3][3];

    Result.m[3][0] = m[3][0] * Other.m[0][0] + m[3][1] * Other.m[1][0] +
                     m[3][2] * Other.m[2][0] + m[3][3] * Other.m[3][0];
    Result.m[3][1] = m[3][0] * Other.m[0][1] + m[3][1] * Other.m[1][1] +
                     m[3][2] * Other.m[2][1] + m[3][3] * Other.m[3][1];
    Result.m[3][2] = m[3][0] * Other.m[0][2] + m[3][1] * Other.m[1][2] +
                     m[3][2] * Other.m[2][2] + m[3][3] * Other.m[3][2];
    Result.m[3][3] = m[3][0] * Other.m[0][3] + m[3][1] * Other.m[1][3] +
                     m[3][2] * Other.m[2][3] + m[3][3] * Other.m[3][3];
    return Result;
  }

  FORCEINLINE void
  Matrix4::operator*=(const Matrix4& Other) {
    *this = *this * Other;
  }

  FORCEINLINE Matrix4
  Matrix4::operator+(const Matrix4& Other) const {
    Matrix4 ResultMat;

    for (int32 X = 0; X < 4; ++X) {
      for (int32 Y = 0; Y < 4; ++Y) {
        ResultMat.m[X][Y] = m[X][Y] + Other.m[X][Y];
      }
    }

    return ResultMat;
  }

  FORCEINLINE void
  Matrix4::operator+=(const Matrix4& Other) {
    *this = *this + Other;
  }

  FORCEINLINE Matrix4
  Matrix4::operator*(float Other) const {
    Matrix4 ResultMat;

    for (int32 X = 0; X < 4; ++X) {
      for (int32 Y = 0; Y < 4; ++Y) {
        ResultMat.m[X][Y] = m[X][Y] * Other;
      }
    }

    return ResultMat;
  }

  FORCEINLINE void
  Matrix4::operator*=(float Other) {
    *this = *this * Other;
  }

  /***************************************************************************/
  /**
   * Comparison operators.
   */
  /***************************************************************************/

  inline bool
  Matrix4::operator==(const Matrix4& Other) const {
    for (int32 X = 0; X < 4; ++X) {
      for (int32 Y = 0; Y < 4; ++Y) {
        if (m[X][Y] != Other.m[X][Y]) {
          return false;
        }
      }
    }

    return true;
  }

  //Error-tolerant comparison.
  inline bool
  Matrix4::equals(const Matrix4& Other, float Tolerance) const {
    for (int32 X = 0; X < 4; ++X)
    {
      for (int32 Y = 0; Y < 4; ++Y) {
        if (Math::abs(m[X][Y] - Other.m[X][Y]) > Tolerance) {
          return false;
        }
      }
    }

    return true;
  }

  inline bool
  Matrix4::operator!=(const Matrix4& Other) const {

    return !(*this == Other);
  }

  /***************************************************************************/
  /**
   * Homogeneous transform.
   */
  /***************************************************************************/

  FORCEINLINE Vector4
  Matrix4::transformVector4(const Vector4 &P) const {
    Vector4 Result;
    
    Result.x = P.x * m[0][0] + P.y * m[1][0] + P.z * m[2][0] + P.w * m[3][0];
    Result.y = P.x * m[0][1] + P.y * m[1][1] + P.z * m[2][1] + P.w * m[3][1];
    Result.z = P.x * m[0][2] + P.y * m[1][2] + P.z * m[2][2] + P.w * m[3][2];
    Result.w = P.x * m[0][3] + P.y * m[1][3] + P.z * m[2][3] + P.w * m[3][3];

    return Result;
  }


  /***************************************************************************/
  /**
   * Transform position
   */
  /***************************************************************************/

  FORCEINLINE Vector4
  Matrix4::transformPosition(const Vector3 &V) const {
    return transformVector4(Vector4(V.x, V.y, V.z, 1.0f));
  }

  /**
   * Inverts the matrix and then transforms V - correctly handles scaling in this matrix.
   */
  FORCEINLINE Vector3
  Matrix4::inverseTransformPosition(const Vector3 &V) const {
    Matrix4 InvSelf = this->inverseFast();
    return InvSelf.transformPosition(V);
  }

  /***************************************************************************/
  /**
   * Transform vector
   */
  /***************************************************************************/

  /**
   *	Transform a direction vector - wont take into account translation part of the Matrix4.
   *	If you want to transform a surface normal (or plane) and correctly account for
   *  non-uniform scaling you should use transformByUsingAdjointT.
   */
  FORCEINLINE Vector4
  Matrix4::transformVector(const Vector3& V) const {
    return transformVector4(Vector4(V.x, V.y, V.z, 0.0f));
  }

  /**
   * Faster version of inverseTransformVector that assumes no scaling.
   * WARNING: Will NOT work correctly if there is scaling in the matrix.
   */
  FORCEINLINE Vector3 Matrix4::inverseTransformVector(const Vector3 &V) const {
    Matrix4 InvSelf = this->inverseFast();
    return InvSelf.transformVector(V);
  }

  FORCEINLINE Matrix4
  Matrix4::getTransposed() const {
    Matrix4	Result;

    Result.m[0][0] = m[0][0];
    Result.m[0][1] = m[1][0];
    Result.m[0][2] = m[2][0];
    Result.m[0][3] = m[3][0];

    Result.m[1][0] = m[0][1];
    Result.m[1][1] = m[1][1];
    Result.m[1][2] = m[2][1];
    Result.m[1][3] = m[3][1];

    Result.m[2][0] = m[0][2];
    Result.m[2][1] = m[1][2];
    Result.m[2][2] = m[2][2];
    Result.m[2][3] = m[3][2];

    Result.m[3][0] = m[0][3];
    Result.m[3][1] = m[1][3];
    Result.m[3][2] = m[2][3];
    Result.m[3][3] = m[3][3];

    return Result;
  }

  FORCEINLINE void
  Matrix4::transpose() {
    Matrix4 Result = getTransposed();
    *this = Result;
  }

  inline float
  Matrix4::determinant() const {
    return	m[0][0] * (
      m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
      m[2][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) +
      m[3][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2])
      ) -
      m[1][0] * (
        m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
        m[2][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
        m[3][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2])
        ) +
      m[2][0] * (
        m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) -
        m[1][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
        m[3][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2])
        ) -
      m[3][0] * (
        m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) -
        m[1][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]) +
        m[2][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2])
        );
  }

  /**
   * Calculate determinant of rotation 3x3 matrix
   */
  inline float
  Matrix4::rotDeterminant() const {
    return
      m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
      m[1][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]) +
      m[2][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]);
  }

  /***************************************************************************/
  /**
   * Inverse.
   */
  /***************************************************************************/

  /**
   * Fast path, doesn't check for nil matrices in final release builds
   */
  inline Matrix4
  Matrix4::inverseFast() const {
    //If we're in non final release, then make sure we're not creating NaNs
# if USING(GE_DEBUG_MODE)
    //Check for zero scale matrix to invert
    if (getScaledAxis(AXIS::kX).isNearlyZero(Math::SMALL_NUMBER) &&
        getScaledAxis(AXIS::kY).isNearlyZero(Math::SMALL_NUMBER) &&
        getScaledAxis(AXIS::kZ).isNearlyZero(Math::SMALL_NUMBER)) {
      GE_LOG(kError, Generic, "Matrix4::inverseFast(), trying to invert a "
             "NIL matrix this results in NaNs! Use inverse() instead.");
    }
# endif
    Matrix4 Res;
    Matrix4 Temp;

    Temp.m[0][0] = m[2][2] * m[3][3] - m[2][3] * m[3][2];
    Temp.m[0][1] = m[1][2] * m[3][3] - m[1][3] * m[3][2];
    Temp.m[0][2] = m[1][2] * m[2][3] - m[1][3] * m[2][2];

    Temp.m[1][0] = m[2][2] * m[3][3] - m[2][3] * m[3][2];
    Temp.m[1][1] = m[0][2] * m[3][3] - m[0][3] * m[3][2];
    Temp.m[1][2] = m[0][2] * m[2][3] - m[0][3] * m[2][2];

    Temp.m[2][0] = m[1][2] * m[3][3] - m[1][3] * m[3][2];
    Temp.m[2][1] = m[0][2] * m[3][3] - m[0][3] * m[3][2];
    Temp.m[2][2] = m[0][2] * m[1][3] - m[0][3] * m[1][2];

    Temp.m[3][0] = m[1][2] * m[2][3] - m[1][3] * m[2][2];
    Temp.m[3][1] = m[0][2] * m[2][3] - m[0][3] * m[2][2];
    Temp.m[3][2] = m[0][2] * m[1][3] - m[0][3] * m[1][2];

    float Det0 = m[1][1] * Temp.m[0][0] - m[2][1] * Temp.m[0][1] + m[3][1] * Temp.m[0][2];
    float Det1 = m[0][1] * Temp.m[1][0] - m[2][1] * Temp.m[1][1] + m[3][1] * Temp.m[1][2];
    float Det2 = m[0][1] * Temp.m[2][0] - m[1][1] * Temp.m[2][1] + m[3][1] * Temp.m[2][2];
    float Det3 = m[0][1] * Temp.m[3][0] - m[1][1] * Temp.m[3][1] + m[2][1] * Temp.m[3][2];

    float Determinant = m[0][0] * Det0 - m[1][0] * Det1 + m[2][0] * Det2 - m[3][0] * Det3;
    const float	RDet = 1.0f / Determinant;

    Res.m[0][0] =  RDet * Det0;
    Res.m[0][1] = -RDet * Det1;
    Res.m[0][2] =  RDet * Det2;
    Res.m[0][3] = -RDet * Det3;
    Res.m[1][0] = -RDet * (m[1][0] * Temp.m[0][0] -
                           m[2][0] * Temp.m[0][1] +
                           m[3][0] * Temp.m[0][2]);
    Res.m[1][1] =  RDet * (m[0][0] * Temp.m[1][0] -
                           m[2][0] * Temp.m[1][1] +
                           m[3][0] * Temp.m[1][2]);
    Res.m[1][2] = -RDet * (m[0][0] * Temp.m[2][0] -
                           m[1][0] * Temp.m[2][1] +
                           m[3][0] * Temp.m[2][2]);
    Res.m[1][3] =  RDet * (m[0][0] * Temp.m[3][0] -
                           m[1][0] * Temp.m[3][1] +
                           m[2][0] * Temp.m[3][2]);
    Res.m[2][0] =  RDet * (m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) -
                           m[2][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) +
                           m[3][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]));
    Res.m[2][1] = -RDet * (m[0][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) -
                           m[2][0] * (m[0][1] * m[3][3] - m[0][3] * m[3][1]) +
                           m[3][0] * (m[0][1] * m[2][3] - m[0][3] * m[2][1]));
    Res.m[2][2] =  RDet * (m[0][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) -
                           m[1][0] * (m[0][1] * m[3][3] - m[0][3] * m[3][1]) +
                           m[3][0] * (m[0][1] * m[1][3] - m[0][3] * m[1][1]));
    Res.m[2][3] = -RDet * (m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) -
                           m[1][0] * (m[0][1] * m[2][3] - m[0][3] * m[2][1]) +
                           m[2][0] * (m[0][1] * m[1][3] - m[0][3] * m[1][1]));
    Res.m[3][0] = -RDet * (m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) -
                           m[2][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) +
                           m[3][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]));
    Res.m[3][1] =  RDet * (m[0][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) -
                           m[2][0] * (m[0][1] * m[3][2] - m[0][2] * m[3][1]) +
                           m[3][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]));
    Res.m[3][2] = -RDet * (m[0][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) -
                           m[1][0] * (m[0][1] * m[3][2] - m[0][2] * m[3][1]) +
                           m[3][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]));
    Res.m[3][3] =  RDet * (m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
                           m[1][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]) +
                           m[2][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]));
    return Res;
  }

  // Inverse.
  inline Matrix4
  Matrix4::inverse() const {
    Matrix4 Res;

    // Check for zero scale matrix to invert
    if (getScaledAxis(AXIS::kX).isNearlyZero(Math::SMALL_NUMBER) &&
        getScaledAxis(AXIS::kY).isNearlyZero(Math::SMALL_NUMBER) &&
        getScaledAxis(AXIS::kZ).isNearlyZero(Math::SMALL_NUMBER)) {
      //Just set to zero - avoids unsafe inverse of zero and duplicates what
      //QNANs were resulting in before (scaling away all children)
      Res = Matrix4::IDENTITY;
    }
    else {
      const float	Det = determinant();
      if (0.0f == Det) {
        Res = Matrix4::IDENTITY;
      }
      else {
        Matrix4 Temp;

        Temp.m[0][0] = m[2][2] * m[3][3] - m[2][3] * m[3][2];
        Temp.m[0][1] = m[1][2] * m[3][3] - m[1][3] * m[3][2];
        Temp.m[0][2] = m[1][2] * m[2][3] - m[1][3] * m[2][2];

        Temp.m[1][0] = m[2][2] * m[3][3] - m[2][3] * m[3][2];
        Temp.m[1][1] = m[0][2] * m[3][3] - m[0][3] * m[3][2];
        Temp.m[1][2] = m[0][2] * m[2][3] - m[0][3] * m[2][2];

        Temp.m[2][0] = m[1][2] * m[3][3] - m[1][3] * m[3][2];
        Temp.m[2][1] = m[0][2] * m[3][3] - m[0][3] * m[3][2];
        Temp.m[2][2] = m[0][2] * m[1][3] - m[0][3] * m[1][2];

        Temp.m[3][0] = m[1][2] * m[2][3] - m[1][3] * m[2][2];
        Temp.m[3][1] = m[0][2] * m[2][3] - m[0][3] * m[2][2];
        Temp.m[3][2] = m[0][2] * m[1][3] - m[0][3] * m[1][2];

        float Det0 = m[1][1] * Temp.m[0][0] - m[2][1] * Temp.m[0][1] + m[3][1] * Temp.m[0][2];
        float Det1 = m[0][1] * Temp.m[1][0] - m[2][1] * Temp.m[1][1] + m[3][1] * Temp.m[1][2];
        float Det2 = m[0][1] * Temp.m[2][0] - m[1][1] * Temp.m[2][1] + m[3][1] * Temp.m[2][2];
        float Det3 = m[0][1] * Temp.m[3][0] - m[1][1] * Temp.m[3][1] + m[2][1] * Temp.m[3][2];

        float Determinant = m[0][0] * Det0 - m[1][0] * Det1 + m[2][0] * Det2 - m[3][0] * Det3;
        const float	RDet = 1.0f / Determinant;

        Res.m[0][0] =  RDet * Det0;
        Res.m[0][1] = -RDet * Det1;
        Res.m[0][2] =  RDet * Det2;
        Res.m[0][3] = -RDet * Det3;
        Res.m[1][0] = -RDet * (m[1][0] * Temp.m[0][0] -
                               m[2][0] * Temp.m[0][1] +
                               m[3][0] * Temp.m[0][2]);
        Res.m[1][1] =  RDet * (m[0][0] * Temp.m[1][0] -
                               m[2][0] * Temp.m[1][1] +
                               m[3][0] * Temp.m[1][2]);
        Res.m[1][2] = -RDet * (m[0][0] * Temp.m[2][0] -
                               m[1][0] * Temp.m[2][1] +
                               m[3][0] * Temp.m[2][2]);
        Res.m[1][3] =  RDet * (m[0][0] * Temp.m[3][0] -
                               m[1][0] * Temp.m[3][1] +
                               m[2][0] * Temp.m[3][2]);
        Res.m[2][0] =  RDet * (m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) -
                               m[2][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) +
                               m[3][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]));
        Res.m[2][1] = -RDet * (m[0][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) -
                               m[2][0] * (m[0][1] * m[3][3] - m[0][3] * m[3][1]) +
                               m[3][0] * (m[0][1] * m[2][3] - m[0][3] * m[2][1]));
        Res.m[2][2] =  RDet * (m[0][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) -
                               m[1][0] * (m[0][1] * m[3][3] - m[0][3] * m[3][1]) +
                               m[3][0] * (m[0][1] * m[1][3] - m[0][3] * m[1][1]));
        Res.m[2][3] = -RDet * (m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) -
                               m[1][0] * (m[0][1] * m[2][3] - m[0][3] * m[2][1]) +
                               m[2][0] * (m[0][1] * m[1][3] - m[0][3] * m[1][1]));
        Res.m[3][0] = -RDet * (m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) -
                               m[2][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) +
                               m[3][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]));
        Res.m[3][1] =  RDet * (m[0][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) -
                               m[2][0] * (m[0][1] * m[3][2] - m[0][2] * m[3][1]) +
                               m[3][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]));
        Res.m[3][2] = -RDet * (m[0][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) -
                               m[1][0] * (m[0][1] * m[3][2] - m[0][2] * m[3][1]) +
                               m[3][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]));
        Res.m[3][3] =  RDet * (m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
                               m[1][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]) +
                               m[2][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]));
      }
    }

    return Res;
  }

  inline Matrix4
  Matrix4::transposeAdjoint() const {
    Matrix4 TA;

    TA.m[0][0] = m[1][1] * m[2][2] - m[1][2] * m[2][1];
    TA.m[0][1] = m[1][2] * m[2][0] - m[1][0] * m[2][2];
    TA.m[0][2] = m[1][0] * m[2][1] - m[1][1] * m[2][0];
    TA.m[0][3] = 0.f;

    TA.m[1][0] = m[2][1] * m[0][2] - m[2][2] * m[0][1];
    TA.m[1][1] = m[2][2] * m[0][0] - m[2][0] * m[0][2];
    TA.m[1][2] = m[2][0] * m[0][1] - m[2][1] * m[0][0];
    TA.m[1][3] = 0.f;

    TA.m[2][0] = m[0][1] * m[1][2] - m[0][2] * m[1][1];
    TA.m[2][1] = m[0][2] * m[1][0] - m[0][0] * m[1][2];
    TA.m[2][2] = m[0][0] * m[1][1] - m[0][1] * m[1][0];
    TA.m[2][3] = 0.f;

    TA.m[3][0] = 0.f;
    TA.m[3][1] = 0.f;
    TA.m[3][2] = 0.f;
    TA.m[3][3] = 1.f;

    return TA;
  }

  //NOTE: There is some compiler optimization issues with WIN64 that cause
  //FORCEINLINE to cause a crash
  //Remove any scaling from this matrix (i.e. magnitude of each row is 1)
  inline void
  Matrix4::removeScaling(float Tolerance) {
    //For each row, find magnitude, and if its non-zero re-scale so its unit length.
    const float SquareSum0 = (m[0][0] * m[0][0]) + (m[0][1] * m[0][1]) + (m[0][2] * m[0][2]);
    const float SquareSum1 = (m[1][0] * m[1][0]) + (m[1][1] * m[1][1]) + (m[1][2] * m[1][2]);
    const float SquareSum2 = (m[2][0] * m[2][0]) + (m[2][1] * m[2][1]) + (m[2][2] * m[2][2]);
    const float Scale0 = Math::floatSelect(SquareSum0 - Tolerance,
                                           Math::invSqrt(SquareSum0),
                                           1.0f);
    const float Scale1 = Math::floatSelect(SquareSum1 - Tolerance,
                                           Math::invSqrt(SquareSum1),
                                           1.0f);
    const float Scale2 = Math::floatSelect(SquareSum2 - Tolerance,
                                           Math::invSqrt(SquareSum2),
                                           1.0f);
    m[0][0] *= Scale0;
    m[0][1] *= Scale0;
    m[0][2] *= Scale0;
    m[1][0] *= Scale1;
    m[1][1] *= Scale1;
    m[1][2] *= Scale1;
    m[2][0] *= Scale2;
    m[2][1] *= Scale2;
    m[2][2] *= Scale2;
  }

  //Returns matrix without scale information
  inline Matrix4
  Matrix4::getMatrixWithoutScale(float Tolerance) const {
    Matrix4 Result = *this;
    Result.removeScaling(Tolerance);
    return Result;
  }

  /**
   * @brief Remove any scaling from this matrix (i.e. magnitude of each row is 1)
   *        and return the 3D scale vector that was initially present.
   */
  inline Vector3
  Matrix4::extractScaling(float Tolerance) {
    Vector3 Scale3D(0, 0, 0);

    //For each row, find magnitude, and if its non-zero re-scale so its unit length.
    const float SquareSum0 = (m[0][0] * m[0][0]) + (m[0][1] * m[0][1]) + (m[0][2] * m[0][2]);
    const float SquareSum1 = (m[1][0] * m[1][0]) + (m[1][1] * m[1][1]) + (m[1][2] * m[1][2]);
    const float SquareSum2 = (m[2][0] * m[2][0]) + (m[2][1] * m[2][1]) + (m[2][2] * m[2][2]);

    if (SquareSum0 > Tolerance) {
      float Scale0 = Math::sqrt(SquareSum0);
      Scale3D[0] = Scale0;
      float InvScale0 = 1.f / Scale0;
      m[0][0] *= InvScale0;
      m[0][1] *= InvScale0;
      m[0][2] *= InvScale0;
    }
    else {
      Scale3D[0] = 0;
    }

    if (SquareSum1 > Tolerance) {
      float Scale1 = Math::sqrt(SquareSum1);
      Scale3D[1] = Scale1;
      float InvScale1 = 1.f / Scale1;
      m[1][0] *= InvScale1;
      m[1][1] *= InvScale1;
      m[1][2] *= InvScale1;
    }
    else {
      Scale3D[1] = 0;
    }

    if (SquareSum2 > Tolerance) {
      float Scale2 = Math::sqrt(SquareSum2);
      Scale3D[2] = Scale2;
      float InvScale2 = 1.f / Scale2;
      m[2][0] *= InvScale2;
      m[2][1] *= InvScale2;
      m[2][2] *= InvScale2;
    }
    else {
      Scale3D[2] = 0;
    }

    return Scale3D;
  }

  /**
   * @return A 3D scale vector calculated from this matrix (where each
   *        component is the magnitude of a row vector).
   */
  inline Vector3
  Matrix4::getScaleVector(float Tolerance) const {
    Vector3 Scale3D(1, 1, 1);

    //For each row, find magnitude, and if its non-zero re-scale so its unit length.
    for (int32 i = 0; i<3; ++i) {
      const float SquareSum = (m[i][0] * m[i][0]) + (m[i][1] * m[i][1]) + (m[i][2] * m[i][2]);
      if (SquareSum > Tolerance) {
        Scale3D[i] = Math::sqrt(SquareSum);
      }
      else {
        Scale3D[i] = 0.f;
      }
    }

    return Scale3D;
  }
  
  //Remove any translation from this matrix
  inline Matrix4
  Matrix4::removeTranslation() const {
    Matrix4 Result = *this;
    Result.m[3][0] = 0.0f;
    Result.m[3][1] = 0.0f;
    Result.m[3][2] = 0.0f;
    return Result;
  }

  FORCEINLINE Matrix4
  Matrix4::concatTranslation(const Vector3& Translation) const {
    Matrix4 Result;

    float* Dest = &Result.m[0][0];
    const float* Src = &m[0][0];
    const float* Trans = &Translation.x;

    Dest[0] = Src[0];
    Dest[1] = Src[1];
    Dest[2] = Src[2];
    Dest[3] = Src[3];
    Dest[4] = Src[4];
    Dest[5] = Src[5];
    Dest[6] = Src[6];
    Dest[7] = Src[7];
    Dest[8] = Src[8];
    Dest[9] = Src[9];
    Dest[10] = Src[10];
    Dest[11] = Src[11];
    Dest[12] = Src[12] + Trans[0];
    Dest[13] = Src[13] + Trans[1];
    Dest[14] = Src[14] + Trans[2];
    Dest[15] = Src[15];

    return Result;
  }

  /**
   * Returns true if any element of this matrix is not finite
   */
  inline bool
  Matrix4::containsNaN() const {
    for (auto i : m) {
      for (int32 j = 0; j < 4; ++j) {
        if (!Math::isFinite(i[j])) {
          return true;
        }
      }
    }
    return false;
  }

  /**
   * @return the maximum magnitude of any row of the matrix.
   */
  inline float
  Matrix4::getMaximumAxisScale() const {
    const float MaxRowScaleSquared = Math::max3(getScaledAxis(AXIS::kX).sizeSquared(),
                                                getScaledAxis(AXIS::kY).sizeSquared(),
                                                getScaledAxis(AXIS::kZ).sizeSquared());
    return Math::sqrt(MaxRowScaleSquared);
  }

  inline void
  Matrix4::scaleTranslation(const Vector3& InScale3D) {
    m[3][0] *= InScale3D.x;
    m[3][1] *= InScale3D.y;
    m[3][2] *= InScale3D.z;
  }

  //GetOrigin
  inline Vector3
  Matrix4::getOrigin() const {
    return Vector3(m[3][0], m[3][1], m[3][2]);
  }

  inline Vector3
  Matrix4::getScaledAxis(AXIS::E InAxis) const {
    switch (InAxis)
    {
      case AXIS::kX:
        return Vector3(m[0][0], m[0][1], m[0][2]);

      case AXIS::kY:
        return Vector3(m[1][0], m[1][1], m[1][2]);

      case AXIS::kZ:
        return Vector3(m[2][0], m[2][1], m[2][2]);

      default:
        GE_ASSERT(false);
    }

    return Vector3::ZERO;
  }

  inline void
  Matrix4::getScaledAxes(Vector3& X, Vector3& Y, Vector3& Z) const {
    X.x = m[0][0]; X.y = m[0][1]; X.z = m[0][2];
    Y.x = m[1][0]; Y.y = m[1][1]; Y.z = m[1][2];
    Z.x = m[2][0]; Z.y = m[2][1]; Z.z = m[2][2];
  }

  inline Vector3
  Matrix4::getUnitAxis(AXIS::E InAxis) const {
    return getScaledAxis(InAxis).getSafeNormal();
  }

  inline void
  Matrix4::getUnitAxes(Vector3& X, Vector3& Y, Vector3& Z) const
  {
    getScaledAxes(X, Y, Z);
    X.normalize();
    Y.normalize();
    Z.normalize();
  }

  inline void
  Matrix4::setAxis(int32 i, const Vector3& Axis) {
    GE_ASSERT(i >= 0 && i <= 2);
    m[i][0] = Axis.x;
    m[i][1] = Axis.y;
    m[i][2] = Axis.z;
  }

  inline void
  Matrix4::setOrigin(const Vector3& NewOrigin) {
    m[3][0] = NewOrigin.x;
    m[3][1] = NewOrigin.y;
    m[3][2] = NewOrigin.z;
  }

  inline void
  Matrix4::setAxes(Vector3* Axis0, Vector3* Axis1, Vector3* Axis2, Vector3* Origin) {
    if (nullptr != Axis0) {
      m[0][0] = Axis0->x;
      m[0][1] = Axis0->y;
      m[0][2] = Axis0->z;
    }
    if (nullptr != Axis1) {
      m[1][0] = Axis1->x;
      m[1][1] = Axis1->y;
      m[1][2] = Axis1->z;
    }
    if (nullptr != Axis2) {
      m[2][0] = Axis2->x;
      m[2][1] = Axis2->y;
      m[2][2] = Axis2->z;
    }
    if (nullptr != Origin) {
      m[3][0] = Origin->x;
      m[3][1] = Origin->y;
      m[3][2] = Origin->z;
    }
  }

  inline Vector3
  Matrix4::getColumn(int32 i) const {
    GE_ASSERT(i >= 0 && i < 4);
    return Vector3(m[0][i], m[1][i], m[2][i]);
  }

  FORCEINLINE bool
  makeFrustumPlane(float A, float B, float C, float D, Plane& OutPlane) {
    const float	LengthSquared = A * A + B * B + C * C;
    if (Math::DELTA*Math::DELTA < LengthSquared) {
      const float	InvLength = Math::invSqrt(LengthSquared);
      OutPlane = Plane(-A * InvLength, -B * InvLength, -C * InvLength, D * InvLength);
      return true;
    }
    return false;
  }

  //Frustum plane extraction.
  FORCEINLINE bool
  Matrix4::getFrustumNearPlane(Plane& OutPlane) const {
    return makeFrustumPlane(m[0][2], m[1][2], m[2][2], m[3][2], OutPlane);
  }

  FORCEINLINE bool
  Matrix4::getFrustumFarPlane(Plane& OutPlane) const {
    return makeFrustumPlane(m[0][3] - m[0][2],
                            m[1][3] - m[1][2],
                            m[2][3] - m[2][2],
                            m[3][3] - m[3][2],
                            OutPlane);
  }

  FORCEINLINE bool
  Matrix4::getFrustumLeftPlane(Plane& OutPlane) const {
    return makeFrustumPlane(m[0][3] + m[0][0],
                            m[1][3] + m[1][0],
                            m[2][3] + m[2][0],
                            m[3][3] + m[3][0],
                            OutPlane);
  }

  FORCEINLINE bool
  Matrix4::getFrustumRightPlane(Plane& OutPlane) const {
    return makeFrustumPlane(m[0][3] - m[0][0],
                            m[1][3] - m[1][0],
                            m[2][3] - m[2][0],
                            m[3][3] - m[3][0],
                            OutPlane);
  }

  FORCEINLINE bool
  Matrix4::getFrustumTopPlane(Plane& OutPlane) const {
    return makeFrustumPlane(m[0][3] - m[0][1],
                            m[1][3] - m[1][1],
                            m[2][3] - m[2][1],
                            m[3][3] - m[3][1],
                            OutPlane);
  }

  FORCEINLINE bool
  Matrix4::getFrustumBottomPlane(Plane& OutPlane) const {
    return makeFrustumPlane(m[0][3] + m[0][1],
                            m[1][3] + m[1][1],
                            m[2][3] + m[2][1],
                            m[3][3] + m[3][1],
                            OutPlane);
  }

  /**
   * Utility for mirroring this transform across a certain plane,
   * and flipping one of the axis as well.
   */
  inline void
  Matrix4::mirror(AXIS::E MirrorAxis, AXIS::E FlipAxis) {
    if (AXIS::kX == MirrorAxis) {
      m[0][0] *= -1.f;
      m[1][0] *= -1.f;
      m[2][0] *= -1.f;
      m[3][0] *= -1.f;
    }
    else if (AXIS::kY == MirrorAxis) {
      m[0][1] *= -1.f;
      m[1][1] *= -1.f;
      m[2][1] *= -1.f;
      m[3][1] *= -1.f;
    }
    else if (AXIS::kZ == MirrorAxis) {
      m[0][2] *= -1.f;
      m[1][2] *= -1.f;
      m[2][2] *= -1.f;
      m[3][2] *= -1.f;
    }

    if (AXIS::kX == FlipAxis) {
      m[0][0] *= -1.f;
      m[0][1] *= -1.f;
      m[0][2] *= -1.f;
    }
    else if (AXIS::kY == FlipAxis) {
      m[1][0] *= -1.f;
      m[1][1] *= -1.f;
      m[1][2] *= -1.f;
    }
    else if (AXIS::kZ == FlipAxis) {
      m[2][0] *= -1.f;
      m[2][1] *= -1.f;
      m[2][2] *= -1.f;
    }
  }

  /**
   * Apply Scale to this matrix
   */
  inline Matrix4
  Matrix4::applyScale(float Scale) {
    Matrix4 ScaleMatrix(
      Plane(Scale, 0.0f, 0.0f, 0.0f),
      Plane(0.0f, Scale, 0.0f, 0.0f),
      Plane(0.0f, 0.0f, Scale, 0.0f),
      Plane(0.0f, 0.0f, 0.0f, 1.0f)
    );
    return ScaleMatrix*(*this);
  }

  /**
   * FPlane inline functions.
   */

  inline Plane
  Plane::transformBy(const Matrix4& M) const {
    const Matrix4 tmpTA = M.transposeAdjoint();
    const float DetM = M.determinant();
    return this->transformByUsingAdjointT(M, DetM, tmpTA);
  }

  inline Plane
  Plane::transformByUsingAdjointT(const Matrix4& M, float DetM, const Matrix4& TA) const {
    Vector3 newNorm = TA.transformVector(*this).getSafeNormal();
    if (0.f > DetM) {
      newNorm *= -1.0f;
    }

    return Plane(M.transformPosition(*this * w), newNorm);
  }

  FORCEINLINE BasisVectorMatrix::BasisVectorMatrix(const Vector3& XAxis,
                                                   const Vector3& YAxis,
                                                   const Vector3& ZAxis,
                                                   const Vector3& Origin) {
    for (uint32 RowIndex = 0; RowIndex < 3; ++RowIndex) {
      m[RowIndex][0] = (&XAxis.x)[RowIndex];
      m[RowIndex][1] = (&YAxis.x)[RowIndex];
      m[RowIndex][2] = (&ZAxis.x)[RowIndex];
      m[RowIndex][3] = 0.0f;
    }

    m[3][0] = Origin | XAxis;
    m[3][1] = Origin | YAxis;
    m[3][2] = Origin | ZAxis;
    m[3][3] = 1.0f;
  }

  FORCEINLINE LookAtMatrix::LookAtMatrix(const Vector3& EyePosition,
                                         const Vector3& LookAtPosition,
                                         const Vector3& UpVector) {
    const Vector3 ZAxis = (LookAtPosition - EyePosition).getSafeNormal();
    const Vector3 XAxis = (UpVector ^ ZAxis).getSafeNormal();
    const Vector3 YAxis = ZAxis ^ XAxis;

    for (uint32 RowIndex = 0; RowIndex < 3; ++RowIndex) {
      m[RowIndex][0] = (&XAxis.x)[RowIndex];
      m[RowIndex][1] = (&YAxis.x)[RowIndex];
      m[RowIndex][2] = (&ZAxis.x)[RowIndex];
      m[RowIndex][3] = 0.0f;
    }

    Vector3 negEyePos = -EyePosition;

    m[3][0] = negEyePos | XAxis;
    m[3][1] = negEyePos | YAxis;
    m[3][2] = negEyePos | ZAxis;
    m[3][3] = 1.0f;
  }
}

namespace geEngineSDK {
  class PerspectiveMatrix : public Matrix4
  {
   public:
#define Z_PRECISION	0.0f

    /**
     * @brief Constructor
     * @param HalfFOVX Half FOV in the X axis
     * @param HalfFOVY Half FOV in the Y axis
     * @param MultFOVX multiplier on the X axis
     * @param MultFOVY multiplier on the y axis
     * @param MinZ distance to the near Z plane
     * @param MaxZ distance to the far Z plane
     */
    PerspectiveMatrix(float HalfFOVX,
                      float HalfFOVY,
                      float MultFOVX,
                      float MultFOVY,
                      float MinZ,
                      float MaxZ);

    /**
     * @brief Constructor
     * @param HalfFOV half Field of View in the Y direction
     * @param Width view space width
     * @param Height view space height
     * @param MinZ distance to the near Z plane
     * @param MaxZ distance to the far Z plane
     * @note that the FOV you pass in is actually half the FOV, unlike most
     *       perspective matrix functions (D3DXMatrixPerspectiveFovLH).
     */
    PerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ, float MaxZ);

    /**
     * @brief Constructor
     * @param HalfFOV half Field of View in the Y direction
     * @param Width view space width
     * @param Height view space height
     * @param MinZ distance to the near Z plane
     * @note that the FOV you pass in is actually half the FOV, unlike most
     *        perspective matrix functions (D3DXMatrixPerspectiveFovLH).
     */
    PerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ);
  };

  class ReversedZPerspectiveMatrix : public Matrix4
  {
   public:
    ReversedZPerspectiveMatrix(float HalfFOVX,
                               float HalfFOVY,
                               float MultFOVX,
                               float MultFOVY,
                               float MinZ,
                               float MaxZ);

    ReversedZPerspectiveMatrix(float HalfFOV,
                               float Width,
                               float Height,
                               float MinZ,
                               float MaxZ);

    ReversedZPerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ);
  };

# if USING(GE_COMPILER_MSVC)
#   pragma warning (push)
    //Disable possible division by 0 warning
#   pragma warning (disable : 4723)
# endif

  FORCEINLINE PerspectiveMatrix::PerspectiveMatrix(float HalfFOVX,
                                                   float HalfFOVY,
                                                   float MultFOVX,
                                                   float MultFOVY,
                                                   float MinZ,
                                                   float MaxZ)
    : Matrix4(Plane(MultFOVX / Math::tan(HalfFOVX), 0.0f, 0.0f, 0.0f),
              Plane(0.0f, MultFOVY / Math::tan(HalfFOVY), 0.0f, 0.0f),
              Plane(0.0f,
                    0.0f,
                    ((MinZ == MaxZ) ? (1.0f - Z_PRECISION) : MaxZ / (MaxZ - MinZ)),
                    1.0f),
              Plane(0.0f,
                    0.0f,
                    -MinZ * ((MinZ == MaxZ) ? (1.0f - Z_PRECISION) : MaxZ / (MaxZ - MinZ)),
                    0.0f))
  {}

  FORCEINLINE PerspectiveMatrix::PerspectiveMatrix(float HalfFOV,
                                                   float Width,
                                                   float Height,
                                                   float MinZ,
                                                   float MaxZ)
    : Matrix4(Plane(1.0f / Math::tan(HalfFOV), 0.0f, 0.0f, 0.0f),
              Plane(0.0f, Width / Math::tan(HalfFOV) / Height, 0.0f, 0.0f),
              Plane(0.0f,
                    0.0f,
                    ((MinZ == MaxZ) ? (1.0f - Z_PRECISION) : MaxZ / (MaxZ - MinZ)),
                    1.0f),
              Plane(0.0f,
                    0.0f,
                    -MinZ * ((MinZ == MaxZ) ? (1.0f - Z_PRECISION) : MaxZ / (MaxZ - MinZ)),
                    0.0f))
  {}

  FORCEINLINE PerspectiveMatrix::PerspectiveMatrix(float HalfFOV,
                                                   float Width,
                                                   float Height,
                                                   float MinZ)
    : Matrix4(Plane(1.0f / Math::tan(HalfFOV), 0.0f, 0.0f, 0.0f),
              Plane(0.0f, Width / Math::tan(HalfFOV) / Height, 0.0f, 0.0f),
              Plane(0.0f, 0.0f, (1.0f - Z_PRECISION), 1.0f),
              Plane(0.0f, 0.0f, -MinZ * (1.0f - Z_PRECISION), 0.0f))
  {}

  FORCEINLINE ReversedZPerspectiveMatrix::ReversedZPerspectiveMatrix(float HalfFOVX,
                                                                     float HalfFOVY,
                                                                     float MultFOVX,
                                                                     float MultFOVY,
                                                                     float MinZ,
                                                                     float MaxZ)
    : Matrix4(Plane(MultFOVX / Math::tan(HalfFOVX), 0.0f, 0.0f, 0.0f),
              Plane(0.0f, MultFOVY / Math::tan(HalfFOVY), 0.0f, 0.0f),
              Plane(0.0f, 0.0f, ((MinZ == MaxZ) ? 0.0f : MinZ / (MinZ - MaxZ)), 1.0f),
              Plane(0.0f, 0.0f, ((MinZ == MaxZ) ? MinZ : -MaxZ * MinZ / (MinZ - MaxZ)), 0.0f))
  {}

  FORCEINLINE ReversedZPerspectiveMatrix::ReversedZPerspectiveMatrix(float HalfFOV,
                                                                     float Width,
                                                                     float Height,
                                                                     float MinZ,
                                                                     float MaxZ)
    : Matrix4(Plane(1.0f / Math::tan(HalfFOV), 0.0f, 0.0f, 0.0f),
              Plane(0.0f, Width / Math::tan(HalfFOV) / Height, 0.0f, 0.0f),
              Plane(0.0f, 0.0f, ((MinZ == MaxZ) ? 0.0f : MinZ / (MinZ - MaxZ)), 1.0f),
              Plane(0.0f, 0.0f, ((MinZ == MaxZ) ? MinZ : -MaxZ * MinZ / (MinZ - MaxZ)), 0.0f))
  {}

  FORCEINLINE ReversedZPerspectiveMatrix::ReversedZPerspectiveMatrix(float HalfFOV,
                                                                     float Width,
                                                                     float Height,
                                                                     float MinZ)
    : Matrix4(Plane(1.0f / Math::tan(HalfFOV), 0.0f, 0.0f, 0.0f),
              Plane(0.0f, Width / Math::tan(HalfFOV) / Height, 0.0f, 0.0f),
              Plane(0.0f, 0.0f, 0.0f, 1.0f),
              Plane(0.0f, 0.0f, MinZ, 0.0f))
  {}

# if USING(GE_COMPILER_MSVC)
#   pragma warning (pop)
# endif
}

namespace geEngineSDK {
  class OrthoMatrix : public Matrix4
  {
   public:

    /**
     * @brief Constructor
     * @param Width view space width
     * @param Height view space height
     * @param ZScale scale in the Z axis
     * @param ZOffset offset in the Z axis
     */
    OrthoMatrix(float Width, float Height, float ZScale, float ZOffset);

    /**
     * @brief Constructor for a orthographic perspective matrix (left-hand)
     * @param Left - Coordinate for the left vertical clipping plane
     * @param Right - Coordinate for the right vertical clipping plane
     * @param Bottom - Coordinate for the bottom horizontal clipping plane
     * @param Top - Coordinate for the top horizontal clipping plane
     * @param ZNear - The near clipping distance
     * @param ZFar - The far clipping distance
     */
    OrthoMatrix(float Left, float Right, float Bottom, float Top, float ZNear, float ZFar);
  };

  class ReversedZOrthoMatrix : public Matrix4
  {
   public:
    ReversedZOrthoMatrix(float Width, float Height, float ZScale, float ZOffset);
  };

  FORCEINLINE OrthoMatrix::OrthoMatrix(float Width, float Height, float ZScale, float ZOffset)
    : Matrix4(Plane((Width) ? (1.0f / Width) : 1.0f, 0.0f, 0.0f, 0.0f),
              Plane(0.0f, (Height) ? (1.0f / Height) : 1.f, 0.0f, 0.0f),
              Plane(0.0f, 0.0f, ZScale, 0.0f),
              Plane(0.0f, 0.0f, ZOffset * ZScale, 1.0f)) {}

  FORCEINLINE OrthoMatrix::OrthoMatrix(float Left,
                                       float Right,
                                       float Bottom,
                                       float Top,
                                       float ZNear,
                                       float ZFar) {
    //Compute the reciprocal width and height and the depth range
    const float RWidth = 1.0f / (Right - Left);
    const float RHeight = 1.0f / (Top - Bottom);
    const float RDepth = 1.0f / (ZFar - ZNear);

    //Set up the orthographic matrix
    m[0][0] = 2.0f * RWidth;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;

    m[1][0] = 0.0f;
    m[1][1] = 2.0f * RHeight;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;

    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = RDepth;
    m[2][3] = 0.0f;

    m[3][0] = -(Right + Left) * RWidth;
    m[3][1] = -(Top + Bottom) * RHeight;
    m[3][2] = -ZNear * RDepth;
    m[3][3] = 1.0f;
  }

  FORCEINLINE ReversedZOrthoMatrix::ReversedZOrthoMatrix(float Width,
                                                         float Height,
                                                         float ZScale,
                                                         float ZOffset)
    : Matrix4(Plane((Width) ? (1.0f / Width) : 1.0f, 0.0f, 0.0f, 0.0f),
              Plane(0.0f, (Height) ? (1.0f / Height) : 1.f, 0.0f, 0.0f),
              Plane(0.0f, 0.0f, -ZScale, 0.0f),
              Plane(0.0f, 0.0f, 1.0f - ZOffset * ZScale, 1.0f)) {}
}

namespace geEngineSDK {
  /**
   * @brief Mirrors a point about an arbitrary plane
   */
  class MirrorMatrix : public Matrix4
  {
   public:
    /**
     * @brief Constructor. Updated for the fact that our Plane uses Ax+By+Cz=D.
     * @param Plane source plane for mirroring (assumed normalized)
     */
    MirrorMatrix(const Plane& plane);
  };

  FORCEINLINE MirrorMatrix::MirrorMatrix(const Plane& plane) : Matrix4(
      Plane(-2.f*plane.x*plane.x + 1.f, -2.f*plane.y*plane.x, -2.f*plane.z*plane.x, 0.f),
      Plane(-2.f*plane.x*plane.y, -2.f*plane.y*plane.y + 1.f, -2.f*plane.z*plane.y, 0.f),
      Plane(-2.f*plane.x*plane.z, -2.f*plane.y*plane.z, -2.f*plane.z*plane.z + 1.f, 0.f),
      Plane( 2.f*plane.x*plane.w,  2.f*plane.y*plane.w,  2.f*plane.z*plane.w, 1.f))
  {
    GE_ASSERT(Math::abs(1.f - plane.sizeSquared()) < 
              Math::KINDA_SMALL_NUMBER && "not normalized");
  }
}

namespace geEngineSDK {
  /**
   * @brief Realigns the near plane for an existing projection matrix with an
   *        arbitrary clip plane
   */
  class ClipProjectionMatrix : public Matrix4
  {
   public:
    /**
     * @brief Constructor
     * @param SrcProjMat - source projection matrix to pre-multiply with the clip matrix
     * @param plane - clipping plane used to build the clip matrix
     *        (assumed to be in camera space)
     */
    ClipProjectionMatrix(const Matrix4& srcProjMat, const Plane& plane);

  private:
    /**
     * @brief Return sign of a number
     */
    FORCEINLINE float
    sgn(float a);
  };

  FORCEINLINE ClipProjectionMatrix::ClipProjectionMatrix(const Matrix4& srcProjMat,
                                                         const Plane& plane)
    : Matrix4(srcProjMat) {
    //Calculate the clip-space corner point opposite the clipping plane as
    //(sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and transform it into camera
    //space by multiplying it by the inverse of the projection matrix
    Plane CornerPlane(sgn(plane.x) / srcProjMat.m[0][0],
                      sgn(plane.y) / srcProjMat.m[1][1],
                      1.0f,
                      -(1.0f - srcProjMat.m[2][2]) / srcProjMat.m[3][2]);

    //Calculate the scaled plane vector
    Plane ProjPlane(plane * (1.0f / (plane | CornerPlane)));

    //Use the projected space clip plane in z column 
    //Note: (account for our negated w coefficient)
    m[0][2] =  ProjPlane.x;
    m[1][2] =  ProjPlane.y;
    m[2][2] =  ProjPlane.z;
    m[3][2] = -ProjPlane.w;
  }

  FORCEINLINE float
  ClipProjectionMatrix::sgn(float a) {
    if (a > 0.0f) { return (1.0f); }
    if (a < 0.0f) { return (-1.0f); }
    return (0.0f);
  }
}

namespace geEngineSDK {
  /**
   * @brief Scale matrix.
   */
  class ScaleMatrix : public Matrix4
  {
   public:
    /**
     * @param Scale uniform scale to apply to matrix.
     */
    ScaleMatrix(float Scale);

    /**
     * @param Scale Non-uniform scale to apply to matrix.
     */
    ScaleMatrix(const Vector3& Scale);

    /**
     * @brief Matrix factory. Return a Matrix4 so we don't have type conversion
     *        issues in expressions.
     */
    static Matrix4
    make(float Scale) {
      return ScaleMatrix(Scale);
    }

    /**
     * @brief Matrix factory. Return a Matrix4 so we don't have type conversion
     *        issues in expressions.
     */
    static Matrix4
    make(const Vector3& Scale) {
      return ScaleMatrix(Scale);
    }
  };

  FORCEINLINE ScaleMatrix::ScaleMatrix(float Scale)
    : Matrix4(Plane(Scale, 0.0f, 0.0f, 0.0f),
              Plane(0.0f, Scale, 0.0f, 0.0f),
              Plane(0.0f, 0.0f, Scale, 0.0f),
              Plane(0.0f, 0.0f, 0.0f, 1.0f))
  {}

  FORCEINLINE ScaleMatrix::ScaleMatrix(const Vector3& Scale)
    : Matrix4(Plane(Scale.x, 0.0f, 0.0f, 0.0f),
              Plane(0.0f, Scale.y, 0.0f, 0.0f),
              Plane(0.0f, 0.0f, Scale.z, 0.0f),
              Plane(0.0f, 0.0f, 0.0f, 1.0f))
  {}
}

namespace geEngineSDK {
  class TranslationMatrix : public Matrix4
  {
   public:

    /**
     * @brief Constructor translation matrix based on given vector
     */
    TranslationMatrix(const Vector3& Delta);

    /**
     * @brief Matrix factory. Return a Matrix4 so we don't have type conversion
     *        issues in expressions.
     */
    static Matrix4
    make(Vector3 const& Delta) {
      return TranslationMatrix(Delta);
    }
  };

  FORCEINLINE TranslationMatrix::TranslationMatrix(const Vector3& Delta)
    : Matrix4(Plane(1.0f, 0.0f, 0.0f, 0.0f),
              Plane(0.0f, 1.0f, 0.0f, 0.0f),
              Plane(0.0f, 0.0f, 1.0f, 0.0f),
              Plane(Delta.x, Delta.y, Delta.z, 1.0f))
  {}
}

namespace geEngineSDK {
  /**
   * @brief Combined rotation and translation matrix
   */
  class RotationTranslationMatrix : public Matrix4
  {
   public:
    /**
     * @brief Constructor.
     * @param Rot rotation
     * @param Origin translation to apply
     */
    RotationTranslationMatrix(const Rotator& Rot, const Vector3& Origin);

    /**
     * @brief Matrix factory. Return a Matrix4 so we don't have type conversion
     *        issues in expressions.
     */
    static Matrix4
    make(const Rotator& Rot, const Vector3& Origin) {
      return RotationTranslationMatrix(Rot, Origin);
    }
  };

  FORCEINLINE RotationTranslationMatrix::RotationTranslationMatrix(const Rotator& Rot,
                                                                   const Vector3& Origin) {
    float SP, SY, SR;
    float CP, CY, CR;
    Math::sin_cos(&SP, &CP, Rot.pitch * Math::DEG2RAD);
    Math::sin_cos(&SY, &CY, Rot.yaw   * Math::DEG2RAD);
    Math::sin_cos(&SR, &CR, Rot.roll  * Math::DEG2RAD);

    m[0][0] = CY * CR + SY * SP * SR;
    m[0][1] = SR * CP;
    m[0][2] = -SY * CR + CY * SP * SR;
    m[0][3] = 0.f;

    m[1][0] = -CY * SR + SY * SP * CR;
    m[1][1] = CR * CP;
    m[1][2] = SR * SY + CY * SP * CR;
    m[1][3] = 0.f;

    m[2][0] = SY * CP;
    m[2][1] = -SP;
    m[2][2] = CY * CP;
    m[2][3] = 0.f;

    m[3][0] = Origin.x;
    m[3][1] = Origin.y;
    m[3][2] = Origin.z;
    m[3][3] = 1.f;
  }
}

namespace geEngineSDK {
  /**
   * @brief Rotation matrix no translation
   */
  class RotationMatrix : public RotationTranslationMatrix
  {
   public:
    /**
     * @brief Constructor.
     * @param Rot rotation
     */
    RotationMatrix(const Rotator& Rot) : RotationTranslationMatrix(Rot, Vector3::ZERO) {}

    /**
     * @brief Matrix factory. Return a Matrix4 so we don't have type conversion
     *        issues in expressions.
     */
    static Matrix4
    make(Rotator const& Rot) {
      return RotationMatrix(Rot);
    }

    /**
     * @brief Matrix factory. Return an Matrix4 so we don't have type conversion
     *        issues in expressions.
     */
    static GE_UTILITIES_EXPORT Matrix4
    make(Quaternion const& Rot);

    /**
     * @brief Builds a rotation matrix given only a XAxis. Y and Z are unspecified
     *        but will be orthonormal. XAxis need not be normalized.
     */
    static GE_UTILITIES_EXPORT Matrix4
    makeFromX(Vector3 const& XAxis);

    /**
     * @brief Builds a rotation matrix given only a YAxis. X and Z are unspecified
     *        but will be orthonormal. YAxis need not be normalized.
     */
    static GE_UTILITIES_EXPORT Matrix4
    makeFromY(Vector3 const& YAxis);

    /**
     * @brief Builds a rotation matrix given only a ZAxis. X and Y are unspecified
     *        but will be orthonormal. ZAxis need not be normalized.
     */
    static GE_UTILITIES_EXPORT Matrix4
    makeFromZ(Vector3 const& ZAxis);

    /**
     * @brief Builds a matrix with given X and Y axes. X will remain fixed,
     *        Y may be changed minimally to enforce orthogonality.
     *        Z will be computed. Inputs need not be normalized.
     */
    static GE_UTILITIES_EXPORT Matrix4
    makeFromXY(Vector3 const& XAxis, Vector3 const& YAxis);

    /**
     * @brief Builds a matrix with given X and Z axes. X will remain fixed,
     *        Z may be changed minimally to enforce orthogonality.
     *        Y will be computed. Inputs need not be normalized.
     */
    static GE_UTILITIES_EXPORT Matrix4
    makeFromXZ(Vector3 const& XAxis, Vector3 const& ZAxis);

    /**
     * @brief Builds a matrix with given Y and X axes. Y will remain fixed,
     *        X may be changed minimally to enforce orthogonality.
     *        Z will be computed. Inputs need not be normalized.
     */
    static GE_UTILITIES_EXPORT Matrix4
    makeFromYX(Vector3 const& YAxis, Vector3 const& XAxis);

    /**
     * @brief Builds a matrix with given Y and Z axes. Y will remain fixed,
     *        Z may be changed minimally to enforce orthogonality.
     *        X will be computed. Inputs need not be normalized.
     */
    static GE_UTILITIES_EXPORT Matrix4
    makeFromYZ(Vector3 const& YAxis, Vector3 const& ZAxis);

    /**
     * @brief Builds a matrix with given Z and X axes. Z will remain fixed,
     *        X may be changed minimally to enforce orthogonality.
     *        Y will be computed. Inputs need not be normalized.
     */
    static GE_UTILITIES_EXPORT Matrix4
    makeFromZX(Vector3 const& ZAxis, Vector3 const& XAxis);

    /**
     * @brief Builds a matrix with given Z and Y axes. Z will remain fixed,
     *        Y may be changed minimally to enforce orthogonality.
     *        X will be computed. Inputs need not be normalized.
     */
    static GE_UTILITIES_EXPORT Matrix4
    makeFromZY(Vector3 const& ZAxis, Vector3 const& YAxis);
  };
}

namespace geEngineSDK {
  /**
   * @brief Combined Scale rotation and translation matrix
   */
  class ScaleRotationTranslationMatrix : public Matrix4
  {
   public:
    /**
     * @brief Constructor.
     * @param Scale scale to apply to matrix
     * @param Rot rotation
     * @param Origin translation to apply
     */
     ScaleRotationTranslationMatrix(const Vector3& Scale,
                                    const Rotator& Rot,
                                    const Vector3& Origin);
  };

  namespace {
    void
    getSinCos(float& S, float& C, float Degrees) {
      if (0.f == Degrees) {
        S = 0.f;
        C = 1.f;
      }
      else if (90.f == Degrees) {
        S = 1.f;
        C = 0.f;
      }
      else if (180.f == Degrees) {
        S = 0.f;
        C = -1.f;
      }
      else if (270.f == Degrees) {
        S = -1.f;
        C = 0.f;
      }
      else {
        Math::sin_cos(&S, &C, Degrees * Math::DEG2RAD);
      }
    }
  }

  FORCEINLINE ScaleRotationTranslationMatrix::
    ScaleRotationTranslationMatrix(const Vector3& Scale,
                                   const Rotator& Rot,
                                   const Vector3& Origin) {
    float SP, SY, SR;
    float CP, CY, CR;
    getSinCos(SP, CP, Rot.pitch);
    getSinCos(SY, CY, Rot.yaw);
    getSinCos(SR, CR, Rot.roll);

    m[0][0] = (CP * CY) * Scale.x;
    m[0][1] = (CP * SY) * Scale.x;
    m[0][2] = (SP)* Scale.x;
    m[0][3] = 0.f;

    m[1][0] = (SR * SP * CY - CR * SY) * Scale.y;
    m[1][1] = (SR * SP * SY + CR * CY) * Scale.y;
    m[1][2] = (-SR * CP) * Scale.y;
    m[1][3] = 0.f;

    m[2][0] = (-(CR * SP * CY + SR * SY)) * Scale.z;
    m[2][1] = (CY * SR - CR * SP * SY) * Scale.z;
    m[2][2] = (CR * CP) * Scale.z;
    m[2][3] = 0.f;

    m[3][0] = Origin.x;
    m[3][1] = Origin.y;
    m[3][2] = Origin.z;
    m[3][3] = 1.f;
  }
}

namespace geEngineSDK {
  /**
   * @brief Rotates about an Origin point.
   */
  class RotationAboutPointMatrix : public RotationTranslationMatrix
  {
   public:
    /**
     * @brief Constructor.
     * @param Rot rotation
     * @param Origin about which to rotate.
     */
    RotationAboutPointMatrix(const Rotator& Rot, const Vector3& Origin);

    /**
     * @brief Matrix factory. Return a Matrix4 so we don't have type conversion
     *        issues in expressions.
     */
    static Matrix4
    make(const Rotator& Rot, const Vector3& Origin) {
      return RotationAboutPointMatrix(Rot, Origin);
    }

    /**
     * @brief Matrix factory. Return a Matrix4 so we don't have type conversion
     *        issues in expressions.
     */
    static GE_UTILITIES_EXPORT Matrix4
    make(const Quaternion& Rot, const Vector3& Origin);
  };

  FORCEINLINE RotationAboutPointMatrix::RotationAboutPointMatrix(const Rotator& Rot,
                                                                 const Vector3& Origin)
    : RotationTranslationMatrix(Rot, Origin) {
    //RotationTranslationMatrix generates R * T.
    //We need -T * R * T, so prepend that translation:
    Vector3 XAxis(m[0][0], m[1][0], m[2][0]);
    Vector3 YAxis(m[0][1], m[1][1], m[2][1]);
    Vector3 ZAxis(m[0][2], m[1][2], m[2][2]);

    m[3][0] -= XAxis | Origin;
    m[3][1] -= YAxis | Origin;
    m[3][2] -= ZAxis | Origin;
  }
}

namespace geEngineSDK {
  /**
   * @brief Inverse Rotation matrix
   */
  class InverseRotationMatrix : public Matrix4
  {
   public:
    /**
     * @brief Constructor.
     * @param Rot rotation
     */
    InverseRotationMatrix(const Rotator& Rot);
  };

  FORCEINLINE InverseRotationMatrix::InverseRotationMatrix(const Rotator& Rot) : Matrix4(
      Matrix4( //Yaw
        Plane(+Math::cos(Rot.yaw * Math::DEG2RAD),
              -Math::sin(Rot.yaw * Math::DEG2RAD),
              0.0f,
              0.0f),
        Plane(+Math::sin(Rot.yaw * Math::DEG2RAD),
              +Math::cos(Rot.yaw * Math::DEG2RAD),
              0.0f,
              0.0f),
        Plane(0.0f, 0.0f, 1.0f, 0.0f),
        Plane(0.0f, 0.0f, 0.0f, 1.0f)) *
      Matrix4( //Pitch
        Plane(+Math::cos(Rot.pitch * Math::DEG2RAD),
              0.0f,
              -Math::sin(Rot.pitch * Math::DEG2RAD),
              0.0f),
        Plane(0.0f, 1.0f, 0.0f, 0.0f),
        Plane(+Math::sin(Rot.pitch * Math::DEG2RAD),
              0.0f,
              +Math::cos(Rot.pitch * Math::DEG2RAD),
              0.0f),
        Plane(0.0f, 0.0f, 0.0f, 1.0f)) *
      Matrix4( //Roll
        Plane(1.0f, 0.0f, 0.0f, 0.0f),
        Plane(0.0f,
              +Math::cos(Rot.roll * Math::DEG2RAD),
              +Math::sin(Rot.roll * Math::DEG2RAD),
              0.0f),
        Plane(0.0f,
              -Math::sin(Rot.roll * Math::DEG2RAD),
              +Math::cos(Rot.roll * Math::DEG2RAD),
              0.0f),
        Plane(0.0f, 0.0f, 0.0f, 1.0f))) {}
}

namespace geEngineSDK {
  /**
   * @brief Rotation and translation matrix using quaternion rotation
   */
  class QuatRotationTranslationMatrix : public Matrix4
  {
   public:

    /**
     * @brief Constructor
     * @param Q rotation
     * @param Origin translation to apply
     */
    GE_UTILITIES_EXPORT QuatRotationTranslationMatrix(const Quaternion& Q,
                                                    const Vector3& Origin);

    /**
     * @brief Matrix factory. Return an FMatrix so we don't have type
     *        conversion issues in expressions.
    */
    static GE_UTILITIES_EXPORT Matrix4
    make(const Quaternion& Q, const Vector3& Origin) {
      return QuatRotationTranslationMatrix(Q, Origin);
    }
  };

  /**
   * @brief Rotation matrix using quaternion rotation
   */
  class QuatRotationMatrix : public QuatRotationTranslationMatrix
  {
   public:
    /**
     * @brief Constructor
     * @param Q rotation
     */
    QuatRotationMatrix(const Quaternion& Q)
      : QuatRotationTranslationMatrix(Q, Vector3::ZERO) {}

    /**
     * @brief Matrix factory. Return an Matrix4 so we don't have type
     *        conversion issues in expressions.
     */
    static Matrix4
    make(const Quaternion& Q) {
      return QuatRotationMatrix(Q);
    }
  };
}

namespace std {
  /**
  * @brief	Hash value generator for Matrix4.
  */
  template<>
  struct hash<geEngineSDK::Matrix4>
  {
    size_t
    operator()(const geEngineSDK::Matrix4& matrix) const {
      size_t hash = 0;

      for (float i : matrix._m) {
        geEngineSDK::ge_hash_combine(hash, i);
      }
      return hash;
    }
  };
}
