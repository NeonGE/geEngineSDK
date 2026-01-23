/*****************************************************************************/
/**
 * @file    geQuaternion.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/06/28
 * @brief   Quaternion that represent a rotation about an axis in 3-D space.
 *
 * Quaternion that represent a rotation about an axis in 3-D space.
 * The x, y, z, w components also double as the Axis / Angle format.
 *
 * Order matters when composing quaternions: C = A * B will yield a quaternion
 * C that logically first applies B then A to any subsequent transformation
 * (right first, then left).
 * Note that this is the opposite order of Transform multiplication.
 *
 * Example: LocalToWorld = (LocalToWorld * DeltaRotation) will change rotation
 *          in local space by DeltaRotation.
 * Example: LocalToWorld = (DeltaRotation * LocalToWorld) will change rotation
 *          in world space by DeltaRotation.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

#ifndef _INC_QUATERNION_H_
# define _INC_QUATERNION_H_
#endif

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geMath.h"
#include "geVector3.h"
#include "geRotator.h"
#include "geMatrix4.h"
#include "geDebug.h"

namespace geEngineSDK {

  class Quaternion
  {
   public:
    /**
     * @brief Default constructor (no initialization).
     */
    FORCEINLINE Quaternion() = default;

    /**
     * @brief Creates and initializes a new quaternion, with the w component
     *        either 0 or 1.
     * @param FORCE_INIT::E: if equal to kForceInitToZero then w is 0,
     *        otherwise w = 1 (creating an identity transform)
     */
    explicit FORCEINLINE Quaternion(FORCE_INIT::E e);

    /**
     * @brief Constructor.
     * @param InX x component of the quaternion
     * @param InY y component of the quaternion
     * @param InZ z component of the quaternion
     * @param InW w component of the quaternion
     */
    FORCEINLINE Quaternion(float InX, float InY, float InZ, float InW);

    /**
     * @brief Creates and initializes a new quaternion from the given matrix.
     * @param M The rotation matrix to initialize from.
     */
    explicit GE_UTILITIES_EXPORT Quaternion(const Matrix4& M);

    /**
     * @brief Creates and initializes a new quaternion from the given rotator.
     * @param R The rotator to initialize from.
     */
    explicit GE_UTILITIES_EXPORT Quaternion(const Rotator& R);

    /**
     * @brief Creates and initializes a new quaternion from the a rotation
     *        around the given axis.
     * @param Axis assumed to be a normalized vector
     * @param Angle angle to rotate above the given axis (in radians)
     */
    Quaternion(const Vector3& Axis, float AngleRad);

   public:
    /**
     * @brief Gets the result of adding a Quaternion to this.
     *        This is a component-wise addition; composing quaternions should
     *        be done via multiplication.
     * @param Q The Quaternion to add.
     * @return The result of addition.
     */
    FORCEINLINE Quaternion
    operator+(const Quaternion& Q) const;

    /**
     * @brief Adds to this quaternion.
     *        This is a component-wise addition; composing quaternions should
     *        be done via multiplication.
     * @param Other The quaternion to add to this.
     * @return Result after addition.
     */
    FORCEINLINE Quaternion
    operator+=(const Quaternion& Q);

    /**
     * @brief Gets the result of subtracting a Quaternion to this.
     *        This is a component-wise subtraction; composing quaternions
     *        should be done via multiplication.
     * @param Q The Quaternion to subtract.
     * @return The result of subtraction.
     */
    FORCEINLINE Quaternion
    operator-(const Quaternion& Q) const;

    /**
     * @brief Checks whether another Quaternion is equal to this, within specified tolerance.
     * @param Q The other Quaternion.
     * @param Tolerance Error tolerance for comparison with other Quaternion.
     * @return true if two Quaternions are equal, within specified tolerance, otherwise false.
     */
    FORCEINLINE bool
    equals(const Quaternion& Q, float Tolerance = Math::KINDA_SMALL_NUMBER) const;

    /**
     * @brief Checks whether this Quaternion is an Identity Quaternion.
     *        Assumes Quaternion tested is normalized.
     * @param Tolerance Error tolerance for comparison with Identity Quaternion.
     * @return true if Quaternion is a normalized Identity Quaternion.
     */
    FORCEINLINE bool
    isIdentity(float Tolerance = Math::SMALL_NUMBER) const;

    /**
     * @brief Subtracts another quaternion from this.
     *        This is a component-wise subtraction; composing quaternions
     *        should be done via multiplication.
     * @param Q The other quaternion.
     * @return reference to this after subtraction.
     */
    FORCEINLINE Quaternion
    operator-=(const Quaternion& Q);

    /**
     * @brief Gets the result of multiplying this by another quaternion (this * Q).
     *        Order matters when composing quaternions: C = A * B will yield a
     *        quaternion C that logically first applies B then A to any subsequent
     *        transformation (right first, then left).
     * @param Q The Quaternion to multiply this by.
     * @return The result of multiplication (this * Q).
     */
    FORCEINLINE Quaternion
    operator*(const Quaternion& Q) const;

    /**
     * @brief Multiply this by a quaternion (this = this * Q).
     *        Order matters when composing quaternions: C = A * B will yield a
     *        quaternion C that logically first applies B then A to any
     *        subsequent transformation (right first, then left).
     * @param Q the quaternion to multiply this by.
     * @return The result of multiplication (this * Q).
     */
    FORCEINLINE Quaternion
    operator*=(const Quaternion& Q);

    /**
     * @brief Rotate a vector by this quaternion.
     * @param V the vector to be rotated
     * @return vector after rotation
     * @see RotateVector
     */
    GE_UTILITIES_EXPORT Vector3
    operator*(const Vector3& V) const;

    /**
     * @brief Multiply this by a matrix.
     * @param M Matrix to multiply by.
     * @return Matrix result after multiplication.
     */
    GE_UTILITIES_EXPORT Matrix4
    operator*(const Matrix4& M) const;

    /**
     * @brief Multiply this quaternion by a scaling factor.
     * @param Scale The scaling factor.
     * @return a reference to this after scaling.
     */
    FORCEINLINE Quaternion
    operator*=(const float Scale);

    /**
     * @brief Get the result of scaling this quaternion.
     * @param Scale The scaling factor.
     * @return The result of scaling.
     */
    FORCEINLINE Quaternion
    operator*(const float Scale) const;

    /**
     * @brief Divide this quaternion by scale.
     * @param Scale What to divide by.
     * @return a reference to this after scaling.
     */
    FORCEINLINE Quaternion
    operator/=(const float Scale);

    /**
     * @brief Divide this quaternion by scale.
     * @param Scale What to divide by.
     * @return new Quaternion of this after division by scale.
     */
    FORCEINLINE Quaternion
    operator/(const float Scale) const;

    /**
     * @brief Checks whether two quaternions are identical.
     *        This is an exact comparison per-component; see equals() for a 
     *        comparison that allows for a small error tolerance and flipped
     *        axes of rotation.
     * @param Q The other quaternion.
     * @return true if two quaternion are identical, otherwise false.
     * @see Equals
     */
    bool
    operator==(const Quaternion& Q) const;

    /**
     * @brief Checks whether two quaternions are not identical.
     * @param Q The other quaternion.
     * @return true if two quaternion are not identical, otherwise false.
     */
    bool
    operator!=(const Quaternion& Q) const;

    /**
     * @brief Calculates dot product of two quaternions.
     * @param Q The other quaternions.
     * @return The dot product.
     */
    float
    operator|(const Quaternion& Q) const;

   public:
    /**
     * @brief Convert a vector of floating-point Euler angles (in degrees) into a Quaternion.
     * @param Euler the Euler angles
     * @return constructed Quaternion
     */
    static GE_UTILITIES_EXPORT Quaternion
    makeFromEuler(const Vector3& Euler);

    /**
     * @brief Convert a Quaternion into floating-point Euler angles (in degrees).
     */
    GE_UTILITIES_EXPORT Vector3
    euler() const;

    /**
     * @brief Normalize this quaternion if it is large enough.
     * If it is too small, returns an identity quaternion.
     * @param Tolerance Minimum squared length of quaternion for normalization.
     */
    FORCEINLINE void
    normalize(float Tolerance = Math::SMALL_NUMBER);

    /**
     * @brief Get a normalized copy of this quaternion.
     * If it is too small, returns an identity quaternion.
     * @param Tolerance Minimum squared length of quaternion for normalization.
     */
    FORCEINLINE Quaternion
    getNormalized(float Tolerance = Math::SMALL_NUMBER) const;

    /**
     * @brief Return true if this quaternion is normalized
     */
    bool
    isNormalized() const;

    /**
     * @brief Get the length of this quaternion.
     * @return The length of this quaternion.
     */
    FORCEINLINE float
    size() const;

    /**
     * @brief Get the length squared of this quaternion.
     * @return The length of this quaternion.
     */
    FORCEINLINE float
    sizeSquared() const;

    /**
     * @brief get the axis and angle of rotation of this quaternion
     * @param Axis{out] vector of axis of the quaternion
     * @param Angle{out] angle of the quaternion
     * @warning : assumes normalized quaternions.
     */
    void
    toAxisAndAngle(Vector3& Axis, float& Angle) const;

    /**
     * @brief Get the swing and twist decomposition for a specified axis
     * @param InTwistAxis Axis to use for decomposition
     * @param OutSwing swing component quaternion
     * @param OutTwist Twist component quaternion
     * @warning assumes normalised quaternion and twist axis
     */
    GE_UTILITIES_EXPORT void
    toSwingTwist(const Vector3& InTwistAxis,
                 Quaternion& OutSwing,
                 Quaternion& OutTwist) const;

    /**
     * @brief Rotate a vector by this quaternion.
     * @param V the vector to be rotated
     * @return vector after rotation
     */
    Vector3
    rotateVector(Vector3 V) const;

    /**
     * @brief Rotate a vector by the inverse of this quaternion.
     * @param V the vector to be rotated
     * @return vector after rotation by the inverse of this quaternion.
     */
    Vector3
    unrotateVector(Vector3 V) const;

    /**
     * @return quaternion with W=0 and V=theta*v.
     */
    GE_UTILITIES_EXPORT Quaternion
    log() const;

    /**
     * @note exp should really only be used after log.
     * Assumes a quaternion with w=0 and v=theta*v (where |v| = 1).
     * exp(q) = (sin(theta)*v, cos(theta))
     */
    GE_UTILITIES_EXPORT Quaternion
    exp() const;

    /**
     * @return inverse of this quaternion
     */
    FORCEINLINE Quaternion
    inverse() const;

    /**
     * @brief Enforce that the delta between this Quaternion and another one represents
     * the shortest possible rotation angle
     */
    void
    enforceShortestArcWith(const Quaternion& OtherQuat);

    /**
     * @brief Get the forward direction (x axis) after it has been rotated by this Quaternion.
     */
    FORCEINLINE Vector3
    getAxisX() const;

    /**
     * @brief Get the right direction (y axis) after it has been rotated by this Quaternion.
     */
    FORCEINLINE Vector3
    getAxisY() const;

    /**
     * @brief Get the up direction (z axis) after it has been rotated by this Quaternion.
     */
    FORCEINLINE Vector3
    getAxisZ() const;

    /**
     * @brief Get the forward direction (x axis) after it has been rotated by this Quaternion.
     */
    FORCEINLINE Vector3
    getForwardVector() const;

    /**
     * @brief Get the right direction (Y axis) after it has been rotated by this Quaternion.
     */
    FORCEINLINE Vector3
    getRightVector() const;

    /**
     * @brief Get the up direction (Z axis) after it has been rotated by this Quaternion.
     */
    FORCEINLINE Vector3
    getUpVector() const;

    /**
     * @brief Convert a rotation into a unit vector facing in its direction.
     *        Equivalent to getForwardVector().
     */
    FORCEINLINE Vector3
    toVector() const;

    /**
     * @brief Get the Rotator representation of this Quaternion.
     */
    GE_UTILITIES_EXPORT Rotator
    rotator() const;

    /**
     * @brief Get the axis of rotation of the Quaternion.
     * This is the axis around which rotation occurs to transform the canonical
     * coordinate system to the target orientation.
     * For the identity Quaternion which has no such rotation, Vector3(1,0,0) is returned.
     */
    FORCEINLINE Vector3
    getRotationAxis() const;

    /**
     * @brief Find the angular distance between two rotation quaternions (in radians)
     */
    FORCEINLINE float
    angularDistance(const Quaternion& Q) const;

    /**
     * @brief Utility to check if there are any non-finite values (NaN or Inf) in this Quat.
     * @return true if there are any non-finite values in this Quaternion, otherwise false.
     */
    bool
    containsNaN() const;

    /**
    * @brief Orients the quaternion so its negative z axis points to the
    *        provided direction.
    * @param[in] forwardDir  Direction to orient towards.
    */
    GE_UTILITIES_EXPORT void
    lookRotation(const Vector3& forwardDir);

    /**
    * @brief Orients the quaternion so its negative z axis points to the
    *        provided direction.
    * @param[in] forwardDir  Direction to orient towards.
    * @param[in] upDir       Constrains y axis orientation to a plane this
    *            vector lies on. This rule might be broken if forward and up
    *            direction are nearly parallel.
    */
    GE_UTILITIES_EXPORT void
    lookRotation(const Vector3& forwardDir, const Vector3& upDir);

    static FORCEINLINE void
    vectorQuaternionMultiply(Quaternion& Result,
                             const Quaternion& Q1,
                             const Quaternion& Q2) {
      //Store intermediate results in result
      Result.x = Q1.w * Q2.x + Q1.x * Q2.w + Q1.y * Q2.z - Q1.z * Q2.y;
      Result.y = Q1.w * Q2.y - Q1.x * Q2.z + Q1.y * Q2.w + Q1.z * Q2.x;
      Result.z = Q1.w * Q2.z + Q1.x * Q2.y - Q1.y * Q2.x + Q1.z * Q2.w;
      Result.w = Q1.w * Q2.w - Q1.x * Q2.x - Q1.y * Q2.y - Q1.z * Q2.z;
    }

   public:

#if USING(GE_DEBUG_MODE)
    FORCEINLINE void
    diagnosticCheckNaN() const {
      if (containsNaN()) {
        GE_LOG(kWarning, Generic, "Quaternion contains NaN");
        *const_cast<Quaternion*>(this) = Quaternion::IDENTITY;
      }
    }

    FORCEINLINE void
    diagnosticCheckNaN(const String& Message) const {
      if (containsNaN()) {
        GE_LOG(kWarning, Generic, "{0}: Quaternion contains NaN", Message);
        *const_cast<Quaternion*>(this) = Quaternion::IDENTITY;
      }
    }
#else
    FORCEINLINE void diagnosticCheckNaN() const {}
    FORCEINLINE void diagnosticCheckNaN(const String&) const {}
#endif

   public:
    /**
     * @brief Generates the 'smallest' (geodesic) rotation between two vectors
     *        of arbitrary length.
     */
    static FORCEINLINE Quaternion
    findBetween(const Vector3& vector1, const Vector3& vector2) {
      return findBetweenVectors(vector1, vector2);
    }

    /**
     * @brief Generates the 'smallest' (geodesic) rotation between two normals
     *        (assumed to be unit length).
     */
    static GE_UTILITIES_EXPORT Quaternion
    findBetweenNormals(const Vector3& Normal1, const Vector3& Normal2);

    /**
     * @brief Generates the 'smallest' (geodesic) rotation between two vectors
     *        of arbitrary length.
     */
    static GE_UTILITIES_EXPORT Quaternion
    findBetweenVectors(const Vector3& vector1, const Vector3& vector2);

    /**
     * @brief Error measure (angle) between two quaternions, ranged [0..1].
     *        Returns the hypersphere-angle between two quaternions; alignment
     *        shouldn't matter, though
     * @note normalized input is expected.
     */
    static FORCEINLINE float
    error(const Quaternion& Q1, const Quaternion& Q2);

    /**
     * @brief Quaternion::error with auto-normalization.
     */
    static FORCEINLINE float
    errorAutoNormalize(const Quaternion& A, const Quaternion& B);

    /**
     * @brief Fast Linear Quaternion Interpolation.
     * Result is NOT normalized.
     */
    static FORCEINLINE Quaternion
    fastLerp(const Quaternion& A, const Quaternion& B, const float Alpha);

    /**
     * @brief Bi-Linear Quaternion interpolation.
     * Result is NOT normalized.
     */
    static FORCEINLINE Quaternion
    fastBilerp(const Quaternion& P00,
               const Quaternion& P10,
               const Quaternion& P01,
               const Quaternion& P11,
               float FracX,
               float FracY);

    /**
     * @brief Spherical interpolation. Will correct alignment. Result is NOT normalized.
     */
    static GE_UTILITIES_EXPORT Quaternion
    slerp_NotNormalized(const Quaternion &Quat1, const Quaternion &Quat2, float Slerp);

    /**
     * @brief Spherical interpolation. Will correct alignment. Result is normalized.
     */
    static FORCEINLINE Quaternion
    slerp(const Quaternion &Quat1, const Quaternion &Quat2, float Slerp) {
      return slerp_NotNormalized(Quat1, Quat2, Slerp).getNormalized();
    }

    /**
     * @brief Simpler Slerp that doesn't do any checks for 'shortest distance'
     *        etc. We need this for the cubic interpolation stuff so that the
     *        multiple Slerps dont go in different directions.
     * Result is NOT normalized.
     */
    static GE_UTILITIES_EXPORT Quaternion
    slerpFullPath_NotNormalized(const Quaternion &quat1,
                                const Quaternion &quat2,
                                float Alpha);

    /**
     * @brief Simpler Slerp that doesn't do any checks for 'shortest distance'
     *        etc. We need this for the cubic interpolation stuff so that the
     *        multipleslerps don't go in different directions.
     * Result is normalized.
     */
    static FORCEINLINE Quaternion
    slerpFullPath(const Quaternion &quat1, const Quaternion &quat2, float Alpha) {
      return slerpFullPath_NotNormalized(quat1, quat2, Alpha).getNormalized();
    }

    /**
     * @brief Given start and end quaternions of quat1 and quat2, and tangents
     *        at those points tang1 and tang2, calculate the point at Alpha
     *        (between 0 and 1) between them. Result is normalized.
     * This will correct alignment by ensuring that the shortest path is taken.
     */
    static GE_UTILITIES_EXPORT Quaternion
    squad(const Quaternion& quat1,
          const Quaternion& tang1,
          const Quaternion& quat2,
          const Quaternion& tang2,
          float Alpha);

    /**
     * @brief Simpler Squad that doesn't do any checks for 'shortest distance'
     *        etc. Given start and end quaternions of quat1 and quat2, and
     *        tangents at those points tang1 and tang2, calculate the point at
     *        Alpha (between 0 and 1) between them. Result is normalized.
     */
    static GE_UTILITIES_EXPORT Quaternion
    squadFullPath(const Quaternion& quat1,
                  const Quaternion& tang1,
                  const Quaternion& quat2,
                  const Quaternion& tang2,
                  float Alpha);

    /**
    * @brief Calculate tangents between given points
    * @param PrevP quaternion at P-1
    * @param P quaternion to return the tangent
    * @param NextP quaternion P+1
    * @param Tension @todo document
    * @param OutTan Out control point
    */
    static GE_UTILITIES_EXPORT void
    calcTangents(const Quaternion& PrevP,
                 const Quaternion& P,
                 const Quaternion& NextP,
                 float Tension,
                 Quaternion& OutTan);

   public:
    /**
     * @brief Identity quaternion.
     */
    static GE_UTILITIES_EXPORT const Quaternion IDENTITY;

   public:
    /**
     * @brief The quaternion's X-component.
     */
    float x;

    /**
     * @brief The quaternion's Y-component.
     */
    float y;

    /**
     * @brief The quaternion's Z-component.
     */
    float z;

    /**
     * @brief The quaternion's W-component.
     */
    float w;
  };

  /***************************************************************************/
  /**
   * Quaternion inline functions.
   */
  /***************************************************************************/

  FORCEINLINE Quaternion::Quaternion(FORCE_INIT::E e)
    : x(0),
      y(0),
      z(0),
      w(FORCE_INIT::kForceInitToZero == e ? 0.0f : 1.0f)
  {}

  FORCEINLINE Quaternion::Quaternion(float InX, float InY, float InZ, float InW)
    : x(InX),
      y(InY),
      z(InZ),
      w(InW) {
    diagnosticCheckNaN();
  }

  FORCEINLINE Quaternion::Quaternion(const Vector3& Axis, float AngleRad) {
    const float half_a = 0.5f * AngleRad;
    float s, c;
    Math::sin_cos(&s, &c, half_a);

    x = s * Axis.x;
    y = s * Axis.y;
    z = s * Axis.z;
    w = c;

    diagnosticCheckNaN();
  }

  FORCEINLINE Quaternion
  Quaternion::operator+(const Quaternion& Q) const {
    return Quaternion(x + Q.x, y + Q.y, z + Q.z, w + Q.w);
  }

  FORCEINLINE Quaternion
  Quaternion::operator+=(const Quaternion& Q) {
    x += Q.x;
    y += Q.y;
    z += Q.z;
    w += Q.w;

    diagnosticCheckNaN();
    return *this;
  }

  FORCEINLINE Quaternion
  Quaternion::operator-(const Quaternion& Q) const {
    return Quaternion(x - Q.x, y - Q.y, z - Q.z, w - Q.w);
  }

  FORCEINLINE bool Quaternion::equals(const Quaternion& Q, float Tolerance) const {
    return (Math::abs(x - Q.x) <= Tolerance &&
            Math::abs(y - Q.y) <= Tolerance &&
            Math::abs(z - Q.z) <= Tolerance &&
            Math::abs(w - Q.w) <= Tolerance) ||
           (Math::abs(x + Q.x) <= Tolerance &&
            Math::abs(y + Q.y) <= Tolerance &&
            Math::abs(z + Q.z) <= Tolerance &&
            Math::abs(w + Q.w) <= Tolerance);
  }

  FORCEINLINE bool
  Quaternion::isIdentity(float Tolerance) const {
    return equals(Quaternion::IDENTITY, Tolerance);
  }

  Quaternion
  Quaternion::operator-=(const Quaternion& Q) {
    x -= Q.x;
    y -= Q.y;
    z -= Q.z;
    w -= Q.w;

    diagnosticCheckNaN();

    return *this;
  }

  FORCEINLINE Quaternion
  Quaternion::operator*(const Quaternion& Q) const {
    Quaternion Result;
    vectorQuaternionMultiply(Result, *this, Q);
    Result.diagnosticCheckNaN();

    return Result;
  }

  FORCEINLINE Quaternion
  Quaternion::operator*=(const Quaternion& Q) {
    Quaternion Result;
    vectorQuaternionMultiply(Result, *this, Q);
    *this = Result;
    diagnosticCheckNaN();
    return *this;
  }

  FORCEINLINE Quaternion
  Quaternion::operator*=(const float Scale) {
    x *= Scale;
    y *= Scale;
    z *= Scale;
    w *= Scale;
    diagnosticCheckNaN();
    return *this;
  }

  FORCEINLINE Quaternion
  Quaternion::operator*(const float Scale) const {
    return Quaternion(Scale * x, Scale * y, Scale * z, Scale * w);
  }

  FORCEINLINE Quaternion
  Quaternion::operator/=(const float Scale) {
    const float Recip = 1.0f / Scale;
    x *= Recip;
    y *= Recip;
    z *= Recip;
    w *= Recip;

    diagnosticCheckNaN();

    return *this;
  }

  FORCEINLINE Quaternion
  Quaternion::operator/(const float Scale) const {
    const float Recip = 1.0f / Scale;
    return Quaternion(x * Recip, y * Recip, z * Recip, w * Recip);
  }

  FORCEINLINE bool
  Quaternion::operator==(const Quaternion& Q) const {
    return x == Q.x && y == Q.y && z == Q.z && w == Q.w;
  }

  FORCEINLINE bool
  Quaternion::operator!=(const Quaternion& Q) const {
    return x != Q.x || y != Q.y || z != Q.z || w != Q.w;
  }

  FORCEINLINE float
  Quaternion::operator|(const Quaternion& Q) const {
    return x * Q.x + y * Q.y + z * Q.z + w * Q.w;
  }

  FORCEINLINE void
  Quaternion::normalize(float Tolerance) {
    const float SquareSum = x * x + y * y + z * z + w * w;

    if (SquareSum >= Tolerance) {
      const float Scale = Math::invSqrt(SquareSum);
      x *= Scale;
      y *= Scale;
      z *= Scale;
      w *= Scale;
    }
    else
    {
      *this = Quaternion::IDENTITY;
    }
  }

  FORCEINLINE Quaternion
  Quaternion::getNormalized(float Tolerance) const {
    Quaternion Result(*this);
    Result.normalize(Tolerance);
    return Result;
  }

  FORCEINLINE bool
  Quaternion::isNormalized() const {
    return (Math::abs(1.f - sizeSquared()) < Math::THRESH_QUAT_NORMALIZED);
  }

  FORCEINLINE float
  Quaternion::size() const {
    return Math::sqrt(x * x + y * y + z * z + w * w);
  }

  FORCEINLINE float
  Quaternion::sizeSquared() const {
    return (x * x + y * y + z * z + w * w);
  }

  FORCEINLINE void
  Quaternion::toAxisAndAngle(Vector3& Axis, float& Angle) const {
    Angle = 2.f * Math::acos(w).valueRadians();
    Axis = getRotationAxis();
  }

  FORCEINLINE Vector3
  Quaternion::getRotationAxis() const {
    //Ensure we never try to sqrt a neg number
    const float S = Math::sqrt(Math::max(1.f - (w * w), 0.f));

    if (0.0001f <= S) {
      return Vector3(x / S, y / S, z / S);
    }

    return Vector3(1.f, 0.f, 0.f);
  }

  float
  Quaternion::angularDistance(const Quaternion& Q) const {
    float InnerProd = x*Q.x + y*Q.y + z*Q.z + w*Q.w;
    return Math::acos((2 * InnerProd * InnerProd) - 1.f).valueRadians();
  }

  FORCEINLINE Vector3
  Quaternion::rotateVector(Vector3 V) const {
    //http://people.csail.mit.edu/bkph/articles/Quaternions.pdf
    //V' = V + 2w(Q x V) + (2Q x (Q x V))
    //refactor:
    //V' = V + w(2(Q x V)) + (Q x (2(Q x V)))
    //T = 2(Q x V);
    //V' = V + w*(T) + (Q x T)

    const Vector3 Q(x, y, z);
    const Vector3 T = 2.f * Vector3::crossProduct(Q, V);
    const Vector3 Result = V + (w * T) + Vector3::crossProduct(Q, T);
    return Result;
  }

  FORCEINLINE Vector3
  Quaternion::unrotateVector(Vector3 V) const {
    const Vector3 Q(-x, -y, -z);  //Inverse
    const Vector3 T = 2.f * Vector3::crossProduct(Q, V);
    const Vector3 Result = V + (w * T) + Vector3::crossProduct(Q, T);
    return Result;
  }

  FORCEINLINE Quaternion
  Quaternion::inverse() const {
    GE_ASSERT(isNormalized());
    return Quaternion(-x, -y, -z, w);
  }

  FORCEINLINE void
  Quaternion::enforceShortestArcWith(const Quaternion& OtherQuat) {
    const float DotResult = (OtherQuat | *this);
    const float Bias = Math::floatSelect(DotResult, 1.0f, -1.0f);
    x *= Bias;
    y *= Bias;
    z *= Bias;
    w *= Bias;
  }

  FORCEINLINE Vector3
  Quaternion::getAxisX() const {
    return rotateVector(Vector3(1.f, 0.f, 0.f));
  }

  FORCEINLINE Vector3
  Quaternion::getAxisY() const {
    return rotateVector(Vector3(0.f, 1.f, 0.f));
  }

  FORCEINLINE Vector3
  Quaternion::getAxisZ() const {
    return rotateVector(Vector3(0.f, 0.f, 1.f));
  }

  FORCEINLINE Vector3
  Quaternion::getForwardVector() const {
    return getAxisX();
  }

  FORCEINLINE Vector3
  Quaternion::getRightVector() const {
    return getAxisY();
  }

  FORCEINLINE Vector3
  Quaternion::getUpVector() const {
    return getAxisZ();
  }

  FORCEINLINE Vector3
  Quaternion::toVector() const {
    return getAxisX();
  }

  FORCEINLINE float
  Quaternion::error(const Quaternion& Q1, const Quaternion& Q2) {
    const float cosom = Math::abs(Q1.x * Q2.x + Q1.y * Q2.y + Q1.z * Q2.z + Q1.w * Q2.w);
    return (Math::abs(cosom) < 0.9999999f) ? 
      Math::acos(cosom).valueRadians()*(Math::INV_PI) : 0.0f;
  }

  FORCEINLINE float
  Quaternion::errorAutoNormalize(const Quaternion& A, const Quaternion& B) {
    Quaternion Q1 = A;
    Q1.normalize();

    Quaternion Q2 = B;
    Q2.normalize();

    return Quaternion::error(Q1, Q2);
  }

  /**
   * Fast Linear Quaternion Interpolation.
   * Result is NOT normalized.
   */
  FORCEINLINE Quaternion
  Quaternion::fastLerp(const Quaternion& A, const Quaternion& B, const float Alpha) {
    //To ensure the 'shortest route', we make sure the dot product between the
    //both rotations is positive.
    const float DotResult = (A | B);
    const float Bias = Math::floatSelect(DotResult, 1.0f, -1.0f);
    return (B * Alpha) + (A * (Bias * (1.f - Alpha)));
  }

  FORCEINLINE Quaternion
  Quaternion::fastBilerp(const Quaternion& P00,
                         const Quaternion& P10,
                         const Quaternion& P01,
                         const Quaternion& P11,
                         float FracX,
                         float FracY) {
    return Quaternion::fastLerp(Quaternion::fastLerp(P00, P10, FracX),
                                Quaternion::fastLerp(P01, P11, FracX),
                                FracY);
  }

  FORCEINLINE bool
  Quaternion::containsNaN() const {
    return (!Math::isFinite(x) ||
            !Math::isFinite(y) ||
            !Math::isFinite(z) ||
            !Math::isFinite(w));
  }

  /***************************************************************************/
  /**
   * Math inline functions.
   */
  /***************************************************************************/

  template<class U>
  Quaternion Math::lerp(const Quaternion& A, const Quaternion& B, const U& Alpha) {
    return Quaternion::slerp(A, B, Alpha);
  }

  template<class U>
  Quaternion Math::biLerp(const Quaternion& P00,
                          const Quaternion& P10,
                          const Quaternion& P01,
                          const Quaternion& P11,
                          float FracX,
                          float FracY) {
    Quaternion Result;

    Result = lerp(Quaternion::slerp_NotNormalized(P00, P10, FracX),
                  Quaternion::slerp_NotNormalized(P01, P11, FracX),
                  FracY);
    return Result;
  }

  template<class U>
  Quaternion Math::cubicInterp(const Quaternion& P0, 
                               const Quaternion& T0,
                               const Quaternion& P1,
                               const Quaternion& T1,
                               const U& A) {
    return Quaternion::squad(P0, T0, P1, T1, A);
  }
}
