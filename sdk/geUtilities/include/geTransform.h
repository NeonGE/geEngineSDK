/*****************************************************************************/
/**
 * @file    geTransform.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/04/28
 * @brief   Transform composed of Scale, Rotation, and Translation.
 *
 * Transform composed of Scale, Rotation (as a quaternion), and Translation.
 *
 * Transforms can be used to convert from one space to another, for example by
 * transforming positions and directions from local space to world space.
 *
 * Transformation of position vectors is applied in the order:
 * Scale -> Rotate -> Translate.
 * Transformation of direction vectors is applied in the order:
 * Scale -> Rotate.
 *
 * Order matters when composing transforms: C = A * B will yield a transform
 * C that logically first applies A then B to any subsequent transformation.
 * Note that this is the opposite order of quaternion multiplication.
 *
 * Example: LocalToWorld = (DeltaRotation * LocalToWorld) will change rotation
 * in local space by DeltaRotation.
 * Example: LocalToWorld = (LocalToWorld * DeltaRotation) will change rotation
 * in world space by DeltaRotation.
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
#include "gePrerequisitesUtilities.h"

#include "geMatrix4.h"
#include "geVector3.h"
#include "geRotator.h"
#include "geQuaternion.h"

namespace geEngineSDK {
  class GE_UTILITIES_EXPORT Transform
  {
   public:
    /**
     * @brief Default constructor.
     */
    FORCEINLINE Transform()
      : m_rotation(0.f, 0.f, 0.f, 1.f),
        m_translation(0.f),
        m_scale3D(Vector3::UNIT)
    {}

    /**
     * @brief Constructor with an initial translation
     * @param InTranslation The value to use for the translation component
     */
    FORCEINLINE explicit Transform(const Vector3& InTranslation)
      : m_rotation(Quaternion::IDENTITY),
        m_translation(InTranslation),
        m_scale3D(Vector3::UNIT)
    {}

    /**
     * @brief Constructor with leaving uninitialized memory
     */
    FORCEINLINE explicit Transform(FORCE_INIT::E) {
      //Note: This can be used to track down initialization issues with bone
      //transform arrays; but it will cause issues with transient fields such
      //as RootMotionDelta that get initialized to 0 by default
    }

    /**
     * @brief Constructor with an initial rotation
     * @param InRotation The value to use for rotation component
     */
    FORCEINLINE explicit Transform(const Quaternion& InRotation)
      : m_rotation(InRotation),
        m_translation(Vector3::ZERO),
        m_scale3D(Vector3::UNIT)
    {}

    /**
     * @brief Constructor with an initial rotation
     * @param InRotation The value to use for rotation component
     *        (after being converted to a quaternion)
     */
    FORCEINLINE explicit Transform(const Rotator& InRotation)
      : m_rotation(InRotation),
        m_translation(Vector3::ZERO),
        m_scale3D(Vector3::UNIT)
    {}

    /**
     * @brief Constructor with all components initialized
     * @param InRotation The value to use for rotation component
     * @param InTranslation The value to use for the translation component
     * @param InScale3D The value to use for the scale component
     */
    FORCEINLINE Transform(const Quaternion& InRotation,
                          const Vector3& InTranslation,
                          const Vector3& InScale3D = Vector3::UNIT)
      : m_rotation(InRotation),
        m_translation(InTranslation),
        m_scale3D(InScale3D)
    {}

    /**
     * @brief Constructor with all components initialized, taking a Rotator as
     *        the rotation component
     * @param InRotation The value to use for rotation component
     *        (after being converted to a quaternion)
     * @param InTranslation The value to use for the translation component
     * @param InScale3D The value to use for the scale component
     */
    FORCEINLINE Transform(const Rotator& InRotation,
                          const Vector3& InTranslation,
                          const Vector3& InScale3D = Vector3::UNIT)
      : m_rotation(InRotation),
        m_translation(InTranslation),
        m_scale3D(InScale3D)
    {}

    /**
     * @brief Copy-constructor
     * @param InTransform The source transform from which all components will
     *        be copied
     */
    FORCEINLINE Transform(const Transform& InTransform) :
      m_rotation(InTransform.m_rotation),
      m_translation(InTransform.m_translation),
      m_scale3D(InTransform.m_scale3D)
    {}

    /**
     * @brief Constructor for converting a Matrix (including scale)
     *        into a Transform.
     */
    FORCEINLINE explicit Transform(const Matrix4& InMatrix) {
      setFromMatrix(InMatrix);
    }

    /**
     * @brief Constructor that takes basis axes and translation
     */
    FORCEINLINE Transform(const Vector3& InX,
                          const Vector3& InY,
                          const Vector3& InZ,
                          const Vector3& InTranslation) {
      setFromMatrix(Matrix4(InX, InY, InZ, InTranslation));
    }

    /**
     * @brief Copy another Transform into this one
     */
    FORCEINLINE Transform&
    operator=(const Transform& Other) {
      this->m_rotation = Other.m_rotation;
      this->m_translation = Other.m_translation;
      this->m_scale3D = Other.m_scale3D;
      return *this;
    }

    /**
     * @brief Convert this Transform to a transformation matrix with scaling.
     */
    FORCEINLINE Matrix4
    toMatrixWithScale() const {
      Matrix4 OutMatrix;

      GE_ASSERT(isRotationNormalized());

      const float x2 = m_rotation.x + m_rotation.x;
      const float y2 = m_rotation.y + m_rotation.y;
      const float z2 = m_rotation.z + m_rotation.z;

      const float xx2 = m_rotation.x * x2;
      const float yy2 = m_rotation.y * y2;
      const float zz2 = m_rotation.z * z2;
      const float xy2 = m_rotation.x * y2;
      const float xz2 = m_rotation.x * z2;
      const float yz2 = m_rotation.y * z2;
      const float wx2 = m_rotation.w * x2;
      const float wy2 = m_rotation.w * y2;
      const float wz2 = m_rotation.w * z2;

      OutMatrix.m[0][0] = (1.0f - (yy2 + zz2)) * m_scale3D.x;
      OutMatrix.m[1][1] = (1.0f - (xx2 + zz2)) * m_scale3D.y;
      OutMatrix.m[2][2] = (1.0f - (xx2 + yy2)) * m_scale3D.z;

      OutMatrix.m[0][1] = (xy2 + wz2) * m_scale3D.x;
      OutMatrix.m[1][0] = (xy2 - wz2) * m_scale3D.y;

      OutMatrix.m[0][2] = (xz2 - wy2) * m_scale3D.x;
      OutMatrix.m[2][0] = (xz2 + wy2) * m_scale3D.z;

      OutMatrix.m[1][2] = (yz2 + wx2) * m_scale3D.y;
      OutMatrix.m[2][1] = (yz2 - wx2) * m_scale3D.z;

      OutMatrix.m[3][0] = m_translation.x;
      OutMatrix.m[3][1] = m_translation.y;
      OutMatrix.m[3][2] = m_translation.z;

      OutMatrix.m[0][3] = 0.0f;
      OutMatrix.m[1][3] = 0.0f;
      OutMatrix.m[2][3] = 0.0f;
      OutMatrix.m[3][3] = 1.0f;

      return OutMatrix;
    }

    /**
     * @brief Convert this Transform to matrix with scaling and compute the
     *        inverse of that.
     */
    FORCEINLINE Matrix4
    toInverseMatrixWithScale() const {
      //TODO: optimize
      return toMatrixWithScale().inverse();
    }

    /**
     * @brief Convert this Transform to inverse.
     */
    FORCEINLINE Transform
    inverse() const {
      Quaternion   InvRotation = m_rotation.inverse();
      //This used to cause NaN if Scale contained 0 
      Vector3 InvScale3D = getSafeScaleReciprocal(m_scale3D);
      Vector3 InvTranslation = InvRotation * (InvScale3D * -m_translation);
      return Transform(InvRotation, InvTranslation, InvScale3D);
    }

    /**
     * @brief Convert this Transform to a transformation matrix, ignoring its
     *        scaling
     */
    FORCEINLINE Matrix4
    toMatrixNoScale() const {
      Matrix4 OutMatrix;

      GE_ASSERT(isRotationNormalized());

      OutMatrix.m[3][0] = m_translation.x;
      OutMatrix.m[3][1] = m_translation.y;
      OutMatrix.m[3][2] = m_translation.z;

      const float x2 = m_rotation.x + m_rotation.x;
      const float y2 = m_rotation.y + m_rotation.y;
      const float z2 = m_rotation.z + m_rotation.z;
      
      {
        const float xx2 = m_rotation.x * x2;
        const float yy2 = m_rotation.y * y2;
        const float zz2 = m_rotation.z * z2;

        OutMatrix.m[0][0] = (1.0f - (yy2 + zz2));
        OutMatrix.m[1][1] = (1.0f - (xx2 + zz2));
        OutMatrix.m[2][2] = (1.0f - (xx2 + yy2));
      }
      {
        const float yz2 = m_rotation.y * z2;
        const float wx2 = m_rotation.w * x2;

        OutMatrix.m[2][1] = (yz2 - wx2);
        OutMatrix.m[1][2] = (yz2 + wx2);
      }
      {
        const float xy2 = m_rotation.x * y2;
        const float wz2 = m_rotation.w * z2;

        OutMatrix.m[1][0] = (xy2 - wz2);
        OutMatrix.m[0][1] = (xy2 + wz2);
      }
      {
        const float xz2 = m_rotation.x * z2;
        const float wy2 = m_rotation.w * y2;

        OutMatrix.m[2][0] = (xz2 + wy2);
        OutMatrix.m[0][2] = (xz2 - wy2);
      }

      OutMatrix.m[0][3] = 0.0f;
      OutMatrix.m[1][3] = 0.0f;
      OutMatrix.m[2][3] = 0.0f;
      OutMatrix.m[3][3] = 1.0f;

      return OutMatrix;
    }

    /**
     * @brief Set this transform to the weighted blend of the supplied two
     *        transforms.
     */
    FORCEINLINE void
    blend(const Transform& Atom1, const Transform& Atom2, float Alpha) {
      GE_ASSERT(Atom1.isRotationNormalized());
      GE_ASSERT(Atom2.isRotationNormalized());

      if (Alpha <= Math::ZERO_ANIMWEIGHT_THRESH) {
        //if blend is all the way for child1, then just copy its bone atoms
        (*this) = Atom1;
      }
      else if (Alpha >= 1.f - Math::ZERO_ANIMWEIGHT_THRESH) {
        //if blend is all the way for child2, then just copy its bone atoms
        (*this) = Atom2;
      }
      else {
        //Simple linear interpolation for translation and scale.
        m_translation = Math::lerp(Atom1.m_translation, Atom2.m_translation, Alpha);
        m_scale3D = Math::lerp(Atom1.m_scale3D, Atom2.m_scale3D, Alpha);
        m_rotation = Quaternion::fastLerp(Atom1.m_rotation, Atom2.m_rotation, Alpha);

        //Re normalize
        m_rotation.normalize();
      }
    }

    /**
     * @brief Set this Transform to the weighted blend of it and the supplied
     *        Transform.
     */
    FORCEINLINE void
    blendWith(const Transform& OtherAtom, float Alpha) {
      GE_ASSERT(isRotationNormalized());
      GE_ASSERT(OtherAtom.isRotationNormalized());

      if (Alpha > Math::ZERO_ANIMWEIGHT_THRESH) {
        if (Alpha >= 1.f - Math::ZERO_ANIMWEIGHT_THRESH) {
          //if blend is all the way for child2, then just copy its bone atoms
          (*this) = OtherAtom;
        }
        else {
          //Simple linear interpolation for translation and scale.
          m_translation = Math::lerp(m_translation, OtherAtom.m_translation, Alpha);
          m_scale3D = Math::lerp(m_scale3D, OtherAtom.m_scale3D, Alpha);
          m_rotation = Quaternion::fastLerp(m_rotation, OtherAtom.m_rotation, Alpha);

          //Re normalize
          m_rotation.normalize();
        }
      }
    }

    /**
     * @brief Quaternion addition is wrong here. This is just a special case
     *        for linear interpolation. Use only within blends!!
     *        Rotation part is NOT normalized!!
     */
    FORCEINLINE Transform
    operator+(const Transform& Atom) const {
      return Transform(m_rotation + Atom.m_rotation,
                       m_translation + Atom.m_translation,
                       m_scale3D + Atom.m_scale3D);
    }

    FORCEINLINE Transform&
    operator+=(const Transform& Atom) {
      m_translation += Atom.m_translation;

      m_rotation.x += Atom.m_rotation.x;
      m_rotation.y += Atom.m_rotation.y;
      m_rotation.z += Atom.m_rotation.z;
      m_rotation.w += Atom.m_rotation.w;

      m_scale3D += Atom.m_scale3D;

      return *this;
    }

    FORCEINLINE Transform
    operator*(float Mult) const {
      return Transform(m_rotation * Mult,
                       m_translation * Mult,
                       m_scale3D * Mult);
    }

    FORCEINLINE Transform&
    operator*=(float Mult) {
      m_translation *= Mult;
      m_rotation.x *= Mult;
      m_rotation.y *= Mult;
      m_rotation.z *= Mult;
      m_rotation.w *= Mult;
      m_scale3D *= Mult;

      return *this;
    }

    /**
     * @brief Return a transform that is the result of this multiplied by
     *        another transform. Order matters when composing transforms:
     *        C = A * B will yield a transform C that logically first applies
     *        A then B to any subsequent transformation.
     * @param  Other other transform by which to multiply.
     * @return new transform: this * Other
     */
    FORCEINLINE Transform
    operator*(const Transform& Other) const;

    /**
     * @brief Sets this transform to the result of this multiplied by another
     *        transform. Order matters when composing transforms : C = A * B
     *        will yield a transform C that logically first applies A then B to
     *        any subsequent transformation.
     * @param  Other other transform by which to multiply.
     */
    FORCEINLINE void
    operator*=(const Transform& Other);

    /**
     * @brief Return a transform that is the result of this multiplied by
     *        another transform (made only from a rotation).
     *        Order matters when composing transforms : C = A * B will yield a
     *        transform C that logically first applies A then B to any
     *        subsequent transformation.
     * @param  Other other quaternion rotation by which to multiply.
     * @return new transform: this * Transform(Other)
     */
    FORCEINLINE Transform
    operator*(const Quaternion& Other) const;

    /**
     * @brief Sets this transform to the result of this multiplied by another
     *        transform (made only from a rotation).
     *        Order matters when composing transforms : C = A * B will yield a
     *        transform C that logically first applies A then B to any
     *        subsequent transformation.
     * @param  Other other quaternion rotation by which to multiply.
     */
    FORCEINLINE void
    operator*=(const Quaternion& Other);

    FORCEINLINE static bool
    anyHasNegativeScale(const Vector3& InScale3D,
                        const  Vector3& InOtherScale3D);

    FORCEINLINE void
    scaleTranslation(const Vector3& InScale3D);

    FORCEINLINE void
    scaleTranslation(const float& Scale);

    FORCEINLINE void
    removeScaling(float Tolerance = Math::SMALL_NUMBER);

    FORCEINLINE float
    getMaximumAxisScale() const;

    FORCEINLINE float
    getMinimumAxisScale() const;

    /*************************************************************************/
    /**
     * Inverse does not work well with VQS format(in particular non-uniform),
     * so removing it, but made two below functions to be used instead.
     */
    /*************************************************************************/

    /*************************************************************************/
    /**
     * The below 2 functions are the ones to get delta transform and return
     * Transform format that can be concatenated.
     * Inverse itself can't concatenate with VQS format(since VQS always
     * transform from S->Q->T, where inverse happens from T(-1)->Q(-1)->S(-1))
     * So these 2 provides ways to fix this.
     * getRelativeTransform returns this * Other(-1) and parameter is
     * Other(not Other(-1))
     * getRelativeTransformReverse returns this(-1) * Other, and parameter is
     * Other.
     */
    /*************************************************************************/
    Transform
    getRelativeTransform(const Transform& Other) const;

    Transform
    getRelativeTransformReverse(const Transform& Other) const;

    /**
     * @brief Set current transform and the relative to ParentTransform.
     *        Equates to This = This->getRelativeTransform(Parent), but saves
     *        the intermediate Transform storage and copy.
     */
    void
    setToRelativeTransform(const Transform& ParentTransform);

    FORCEINLINE Vector4
    transformVector4(const Vector4& V) const;

    FORCEINLINE Vector4
    transformVector4NoScale(const Vector4& V) const;

    FORCEINLINE Vector3
    transformPosition(const Vector3& V) const;

    FORCEINLINE Vector3
    transformPositionNoScale(const Vector3& V) const;

    /**
     * @brief Inverts the transform and then transforms V - correctly handles
     *        scaling in this transform.
     */
    FORCEINLINE Vector3
    inverseTransformPosition(const Vector3 &V) const;

    FORCEINLINE Vector3
    inverseTransformPositionNoScale(const Vector3 &V) const;

    FORCEINLINE Vector3
    transformVector(const Vector3& V) const;

    FORCEINLINE Vector3
    transformVectorNoScale(const Vector3& V) const;

    /**
     * @brief Transform a direction vector by the inverse of this transform
     *        - will not take into account translation part.
     *        If you want to transform a surface normal (or plane) and
     *        correctly account for non-uniform scaling you should use
     *        transformByUsingAdjointT with adjoint of matrix inverse.
     */
    FORCEINLINE Vector3
    inverseTransformVector(const Vector3 &V) const;

    FORCEINLINE Vector3
    inverseTransformVectorNoScale(const Vector3 &V) const;

    /**
     * @brief Transform a rotation.
     *        For example if this is a LocalToWorld transform,
     *        transformRotation(Q) would transform Q from local to world space.
     */
    FORCEINLINE Quaternion
    transformRotation(const Quaternion& Q) const;

    /**
     * @brief Inverse transform a rotation.
     *        For example if this is a LocalToWorld transform,
     *        inverseTransformRotation(Q) would transform Q from world to local
     *        space.
     */
    FORCEINLINE Quaternion
    inverseTransformRotation(const Quaternion& Q) const;

    FORCEINLINE Transform
    getScaled(float Scale) const;

    FORCEINLINE Transform
    getScaled(const Vector3& Scale) const;

    FORCEINLINE Vector3
    getScaledAxis(AXIS::E InAxis) const;

    FORCEINLINE Vector3
    getUnitAxis(AXIS::E InAxis) const;

    FORCEINLINE void
    mirror(AXIS::E MirrorAxis, AXIS::E FlipAxis);

    FORCEINLINE static Vector3
    getSafeScaleReciprocal(const Vector3& InScale,
                           float Tolerance = Math::SMALL_NUMBER);

    //Temp function for easy conversion
    FORCEINLINE Vector3
    getLocation() const {
      return getTranslation();
    }

    FORCEINLINE Rotator
    rotator() const {
      return m_rotation.rotator();
    }

    /**
     * @brief Calculate the determinant
     */
    FORCEINLINE float
    getDeterminant() const {
      return m_scale3D.x * m_scale3D.y * m_scale3D.z;
    }

    /**
     * @brief Set the translation of this transformation
     */
    FORCEINLINE void
    setLocation(const Vector3& Origin) {
      m_translation = Origin;
    }

    /**
     * @brief Checks the components for non-finite values (NaN or Inf).
     * @return Returns true if any component (rotation, translation, or scale) is not finite.
     */
    bool
    containsNaN() const {
      return (m_translation.containsNaN() ||
              m_rotation.containsNaN() ||
              m_scale3D.containsNaN());
    }

    inline bool
    isValid() const {
      if (containsNaN()) {
        return false;
      }

      if (!m_rotation.isNormalized()) {
        return false;
      }

      return true;
    }

    /**
     * Binary comparison operators.
     */
    bool
    operator==(const Transform& Other) const {
    return m_rotation == Other.m_rotation &&
           m_translation == Other.m_translation &&
           m_scale3D==Other.m_scale3D;
    }

    bool
    operator!=(const Transform& Other) const {
      return m_rotation != Other.m_rotation ||
             m_translation != Other.m_translation ||
             m_scale3D != Other.m_scale3D;
    }

   private:
    FORCEINLINE bool
    private_RotationEquals(const Quaternion& InRotation,
                           const float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      return m_rotation.equals(InRotation, Tolerance);
    }

    FORCEINLINE bool
    private_TranslationEquals(const Vector3& InTranslation,
                              const float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      return m_translation.equals(InTranslation, Tolerance);
    }

    FORCEINLINE bool
    private_Scale3DEquals(const Vector3& InScale3D,
                          const float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      return m_scale3D.equals(InScale3D, Tolerance);
    }

   public:
    /**
     * @brief Test if A's rotation equals B's rotation, within a tolerance.
     *        Preferred over "A.getRotation().equals(B.getRotation())" because
     *        it is faster on some platforms.
     */
    FORCEINLINE static bool
    areRotationsEqual(const Transform& A,
                      const Transform& B,
                      float Tolerance = Math::KINDA_SMALL_NUMBER) {
      return A.private_RotationEquals(B.m_rotation, Tolerance);
    }

    /**
     * @brief Test if A's translation equals B's rotation, within a tolerance.
     *        Preferred over "A.getTranslation().equals(B.getTranslation())"
     *        because it is faster on some platforms.
     */
    FORCEINLINE static bool
    areTranslationsEqual(const Transform& A,
                         const Transform& B,
                         float Tolerance = Math::KINDA_SMALL_NUMBER) {
      return A.private_TranslationEquals(B.m_translation, Tolerance);
    }

    /**
     * @brief Test if A's scale equals B's rotation, within a tolerance.
     *        Preferred over "A.getScale3D().equals(B.getScale3D())"
     *        because it is faster on some platforms.
     */
    FORCEINLINE static bool
    areScale3DsEqual(const Transform& A,
                     const Transform& B,
                     float Tolerance = Math::KINDA_SMALL_NUMBER) {
      return A.private_Scale3DEquals(B.m_scale3D, Tolerance);
    }

    /**
     * @brief Test if this Transform's rotation equals another's rotation,
     *        within a tolerance. Preferred over
     *        "getRotation().equals(Other.getRotation())" because it is faster
     *        on some platforms.
     */
    FORCEINLINE bool
    rotationEquals(const Transform& Other,
                   float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      return areRotationsEqual(*this, Other, Tolerance);
    }

    /**
     * @brief Test if this Transform's translation equals another's rotation,
     *        within a tolerance. Preferred over
     *        "getTranslation().equals(Other.getTranslation())" because it is
     *        faster on some platforms.
     */
    FORCEINLINE bool
    translationEquals(const Transform& Other,
                      float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      return areTranslationsEqual(*this, Other, Tolerance);
    }

    /**
     * @brief Test if this Transform's scale equals another's rotation,
     *        within a tolerance. Preferred over
     *        "getScale3D().equals(Other.getScale3D())" because it is
     *        faster on some platforms.
     */
    FORCEINLINE bool
    scale3DEquals(const Transform& Other,
                  float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      return areScale3DsEqual(*this, Other, Tolerance);
    }

    /**
     * @brief Test if all components of the transforms are equal, within a
     *        tolerance.
     */
    inline bool
    equals(const Transform& Other,
           float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      return private_TranslationEquals(Other.m_translation, Tolerance) &&
             private_RotationEquals(Other.m_rotation, Tolerance) &&
             private_Scale3DEquals(Other.m_scale3D, Tolerance);
    }

    /**
     * @brief Test if rotation and translation components of the transforms are
     *        equal, within a tolerance.
     */
    inline bool
    equalsNoScale(const Transform& Other,
                  float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      return private_TranslationEquals(Other.m_translation, Tolerance) &&
             private_RotationEquals(Other.m_rotation, Tolerance);
    }

    /**
     * @brief Create a new transform: OutTransform = A * B.
     *        Order matters when composing transforms : A * B will yield a
     *        transform that logically first applies A then B to any subsequent
     *        transformation.
     * @param OutTransform pointer to transform that will store the result of
     *        A * B.
     * @param A Transform A.
     * @param B Transform B.
     */
    FORCEINLINE static void
    multiply(Transform* OutTransform, const Transform* A, const Transform* B);

    /**
     * @brief Sets the components
     * @param InRotation The new value for the m_rotation component
     * @param InTranslation The new value for the m_translation component
     * @param InScale3D The new value for the m_scale3D component
     */
    FORCEINLINE void
    setComponents(const Quaternion& InRotation,
                  const Vector3& InTranslation,
                  const Vector3& InScale3D) {
      m_rotation = InRotation;
      m_translation = InTranslation;
      m_scale3D = InScale3D;
    }

    /**
     * @brief Sets the components to the identity transform:
     *        Rotation = (0,0,0,1)
     *        Translation = (0,0,0)
     *        Scale3D = (1,1,1)
     */
    FORCEINLINE void
    setIdentity() {
      m_rotation = Quaternion::IDENTITY;
      m_translation = Vector3::ZERO;
      m_scale3D = Vector3::UNIT;
    }

    /**
     * @brief Scales the Scale3D component by a new factor
     * @param Scale3DMultiplier The value to multiply Scale3D with
     */
    FORCEINLINE void
    multiplyScale3D(const Vector3& Scale3DMultiplier) {
      m_scale3D *= Scale3DMultiplier;
    }

    /**
     * @brief Sets the translation component
     * @param NewTranslation The new value for the translation component
     */
    FORCEINLINE void
    setTranslation(const Vector3& NewTranslation) {
      m_translation = NewTranslation;
    }

    /**
     * @brief Copy translation from another Transform.
     */
    FORCEINLINE void
    copyTranslation(const Transform& Other) {
      m_translation = Other.m_translation;
    }

    /**
     * @brief Concatenates another rotation to this transformation
     * @param DeltaRotation The rotation to concatenate in the following
     *        fashion: Rotation = Rotation * DeltaRotation
     */
    FORCEINLINE void
    concatenateRotation(const Quaternion& DeltaRotation) {
      m_rotation = m_rotation * DeltaRotation;
    }

    /**
     * @brief Adjusts the translation component of this transformation
     * @param DeltaTranslation The translation to add in the following
     *        fashion: Translation += DeltaTranslation
     */
    FORCEINLINE void
    addToTranslation(const Vector3& DeltaTranslation) {
      m_translation += DeltaTranslation;
    }

    /**
     * @brief Add the translations from two Transforms and return the result.
     * @return A.Translation + B.Translation
     */
    FORCEINLINE static Vector3
    addTranslations(const Transform& A, const Transform& B) {
      return A.m_translation + B.m_translation;
    }

    /**
     * @brief Subtract translations from two Transforms and return the
     *        difference.
     * @return A.Translation - B.Translation.
     */
    FORCEINLINE static Vector3
    subtractTranslations(const Transform& A, const Transform& B) {
      return A.m_translation - B.m_translation;
    }

    /**
     * @brief Sets the rotation component
     * @param NewRotation The new value for the rotation component
     */
    FORCEINLINE void
    setRotation(const Quaternion& NewRotation) {
      m_rotation = NewRotation;
    }

    /**
     * @brief Copy rotation from another Transform.
     */
    FORCEINLINE void
    copyRotation(const Transform& Other) {
      m_rotation = Other.m_rotation;
    }

    /**
     * @brief Sets the Scale3D component
     * @param NewScale3D The new value for the Scale3D component
     */
    FORCEINLINE void
    setScale3D(const Vector3& NewScale3D) {
      m_scale3D = NewScale3D;
    }

    /**
     * @brief Copy scale from another Transform.
     */
    FORCEINLINE void
    copyScale3D(const Transform& Other) {
      m_scale3D = Other.m_scale3D;
    }

    /**
     * @brief Sets both the translation and Scale3D components at the same time
     * @param NewTranslation The new value for the translation component
     * @param NewScale3D The new value for the Scale3D component
     */
    FORCEINLINE void
    setTranslationAndScale3D(const Vector3& NewTranslation,
                             const Vector3& NewScale3D) {
      m_translation = NewTranslation;
      m_scale3D = NewScale3D;
    }

    /**
     * @note: Added template type function for Accumulate
     * The template type isn't much useful yet, but it is with the plan to move
     * forward to unify blending features with just type of additive or full
     * pose. Eventually it would be nice to just call blend and it all works
     * depending on full pose or additive, but right now that is a lot more
     * refactoring. For now this types only defines the different functionality
     * of accumulate
     */

    /**
     * @brief Accumulates another transform with this one
     *        Rotation is accumulated multiplicatively
     *        (Rotation = SourceAtom.Rotation * Rotation)
     *        Translation is accumulated additively 
     *        (Translation += SourceAtom.Translation)
     *        Scale3D is accumulated multiplicatively
     *        (Scale3D *= SourceAtom.Scale3D)
     * @param SourceAtom The other transform to accumulate into this one
     */
    FORCEINLINE void
    accumulate(const Transform& SourceAtom) {
      //Add ref pose relative animation to base animation, only if rotation is
      //significant.
      if (Math::square(SourceAtom.m_rotation.w) < 1.f - Math::DELTA*Math::DELTA) {
        m_rotation = SourceAtom.m_rotation * m_rotation;
      }

      m_translation += SourceAtom.m_translation;
      m_scale3D *= SourceAtom.m_scale3D;

      GE_ASSERT(isRotationNormalized());
    }

    /**
     * @brief Accumulates another transform with this, with a blending weight
     *        Let SourceAtom = Atom * BlendWeight
     *        Rotation is accumulated multiplicatively
     *        (Rotation = SourceAtom.Rotation * Rotation).
     *        Translation is accumulated additively
     *        (Translation += SourceAtom.Translation)
     *        Scale3D is accumulated multiplicatively
     *        (Scale3D *= SourceAtom.Scale3D)
     * @note: Rotation will not be normalized! Will have to be done manually.
     * @param Atom The other transform to accumulate into this one
     * @param BlendWeight The weight to multiply Atom by before it is
     *        accumulated.
     */
    FORCEINLINE void
    accumulate(const Transform& Atom, float BlendWeight) {
      Transform SourceAtom(Atom * BlendWeight);

      //Add ref pose relative animation to base animation, only if rotation is
      //significant.
      if (Math::square(SourceAtom.m_rotation.w) < 1.f - Math::DELTA*Math::DELTA) {
        m_rotation = SourceAtom.m_rotation * m_rotation;
      }

      m_translation += SourceAtom.m_translation;
      m_scale3D *= SourceAtom.m_scale3D;
    }

    /**
     * @brief Accumulates another transform with this one, with an optional
     *        blending weight
     *        Rotation is accumulated additively, in the shortest direction
     *        (Rotation = Rotation +/- DeltaAtom.Rotation * Weight)
     *        Translation is accumulated additively
     *        (Translation += DeltaAtom.Translation * Weight)
     *        Scale3D is accumulated additively
     *        (Scale3D += DeltaAtom.Scale3D * Weight)
     * @param DeltaAtom The other transform to accumulate into this one
     * @param Weight The weight to multiply DeltaAtom by before it is
     *        accumulated.
     */
    FORCEINLINE void
    accumulateWithShortestRotation(const Transform& DeltaAtom, float BlendWeight) {
      Transform Atom(DeltaAtom * BlendWeight);

      //To ensure the 'shortest route', we make sure the dot product between
      //the accumulator and the incoming child atom is positive.
      if ((Atom.m_rotation | m_rotation) < 0.f) {
        m_rotation.x -= Atom.m_rotation.x;
        m_rotation.y -= Atom.m_rotation.y;
        m_rotation.z -= Atom.m_rotation.z;
        m_rotation.w -= Atom.m_rotation.w;
      }
      else {
        m_rotation.x += Atom.m_rotation.x;
        m_rotation.y += Atom.m_rotation.y;
        m_rotation.z += Atom.m_rotation.z;
        m_rotation.w += Atom.m_rotation.w;
      }

      m_translation += Atom.m_translation;
      m_scale3D += Atom.m_scale3D;
    }

    /**
     * @brief Accumulates another transform with this, with a blending weight
     *        Let SourceAtom = Atom * BlendWeight
     *        Rotation is accumulated multiplicatively
     *        (Rotation = SourceAtom.Rotation * Rotation).
     *        Translation is accumulated additively
     *        (Translation += SourceAtom.Translation)
     *        Scale3D is accumulated assuming incoming scale is additive scale
     *        (Scale3D *= (1 + SourceAtom.Scale3D))
     *
     * When we create additive, we create additive scale based on
     * [TargetScale/SourceScale -1] because that way when you apply weight of
     * 0.3, you don't shrink. We only saves the % of grow/shrink when we apply
     * that back to it, we add back the 1, so that it goes back to it.
     * This solves issue where you blend two additives with 0.3, you don't come
     * back to 0.6 scale, but 1 scale at the end because
     * [1 + [1-1]*0.3 + [1-1]*0.3] becomes 1, so you don't shrink by applying
     * additive scale.
     *
     * @note: Rotation will not be normalized! Will have to be done manually.
     *
     * @param Atom The other transform to accumulate into this one
     * @param BlendWeight The weight to multiply Atom by before it is
     *        accumulated.
     */
    FORCEINLINE void
    accumulateWithAdditiveScale(const Transform& Atom, float BlendWeight) {
      const Vector3 DefaultScale(Vector3::UNIT);

      Transform SourceAtom(Atom * BlendWeight);

      //Add ref pose relative animation to base animation, only if rotation is
      //significant.
      if (Math::square(SourceAtom.m_rotation.w) < 1.f - Math::DELTA*Math::DELTA) {
        m_rotation = SourceAtom.m_rotation * m_rotation;
      }

      m_translation += SourceAtom.m_translation;
      m_scale3D *= (DefaultScale + SourceAtom.m_scale3D);
    }

    /**
     * @brief Set the translation and Scale3D components of this transform to a
     *        linearly interpolated combination of two other transforms
     *
     * Translation = Math::lerp(SourceAtom1.Translation,
     *                          SourceAtom2.Translation, Alpha)
     * Scale3D = Math::lerp(SourceAtom1.Scale3D, SourceAtom2.Scale3D, Alpha)
     *
     * @param SourceAtom1 The starting point source atom
     *        (used 100% if Alpha is 0)
     * @param SourceAtom2 The ending point source atom
     *        (used 100% if Alpha is 1)
     * @param Alpha The blending weight between SourceAtom1 and SourceAtom2
     */
    FORCEINLINE void
    lerpTranslationScale3D(const Transform& SourceAtom1,
                           const Transform& SourceAtom2,
                           float Alpha) {
      m_translation = Math::lerp(SourceAtom1.m_translation,
                                 SourceAtom2.m_translation,
                                 Alpha);
      m_scale3D = Math::lerp(SourceAtom1.m_scale3D, SourceAtom2.m_scale3D, Alpha);
    }

    /**
     * @brief Normalize the rotation component of this transformation
     */
    FORCEINLINE void
    normalizeRotation() {
      m_rotation.normalize();
    }

    /**
     * @brief Checks whether the rotation component is normalized or not
     * @return true if the rotation component is normalized, false otherwise.
     */
    FORCEINLINE bool
    isRotationNormalized() const {
      return m_rotation.isNormalized();
    }

    /**
     * @brief Blends the Identity transform with a weighted source transform
     *        and accumulates that into a destination transform
     *
     *        SourceAtom = blend(Identity, SourceAtom, BlendWeight)
     *        FinalAtom.Rotation = SourceAtom.Rotation * FinalAtom.Rotation
     *        FinalAtom.Translation += SourceAtom.Translation
     *        FinalAtom.Scale3D *= SourceAtom.Scale3D
     *
     * @param FinalAtom [in/out] The atom to accumulate the blended source
     *        atom into
     * @param SourceAtom The target transformation (used when BlendWeight = 1);
     *        this is modified during the process
     * @param BlendWeight The blend weight between Identity and SourceAtom
     */
    FORCEINLINE static void
    blendFromIdentityAndAccumulate(Transform& FinalAtom,
                                   Transform& SourceAtom,
                                   float BlendWeight) {
      const Transform AdditiveIdentity(Quaternion::IDENTITY, Vector3::ZERO, Vector3::ZERO);
      const Vector3 DefaultScale(Vector3::UNIT);

      //Scale delta by weight
      if (BlendWeight < (1.f - Math::ZERO_ANIMWEIGHT_THRESH)) {
        SourceAtom.blend(AdditiveIdentity, SourceAtom, BlendWeight);
      }

      //Add ref pose relative animation to base animation, only if rotation is
      //significant.
      if (Math::square(SourceAtom.m_rotation.w) < 1.f - Math::DELTA*Math::DELTA) {
        FinalAtom.m_rotation = SourceAtom.m_rotation * FinalAtom.m_rotation;
      }

      FinalAtom.m_translation += SourceAtom.m_translation;
      FinalAtom.m_scale3D *= (DefaultScale + SourceAtom.m_scale3D);

      GE_ASSERT(FinalAtom.isRotationNormalized());
    }

    /**
     * @brief Returns the rotation component
     * @return The rotation component
     */
    FORCEINLINE Quaternion
    getRotation() const {
      return m_rotation;
    }

    /**
     * @brief Returns the translation component
     * @return The translation component
     */
    FORCEINLINE Vector3
    getTranslation() const {
      return m_translation;
    }

    /**
     * @brief Returns the Scale3D component
     * @return The Scale3D component
     */
    FORCEINLINE Vector3
    getScale3D() const {
      return m_scale3D;
    }

    /**
     * @brief Sets the Rotation and Scale3D of this transformation from another
     *        transform
     * @param SrcBA The transform to copy rotation and Scale3D from
     */
    FORCEINLINE void
    copyRotationPart(const Transform& SrcBA) {
      m_rotation = SrcBA.m_rotation;
      m_scale3D = SrcBA.m_scale3D;
    }

    /**
     * @brief Sets the Translation and Scale3D of this transformation from
     *        another transform
     * @param SrcBA The transform to copy translation and Scale3D from
     */
    FORCEINLINE void
    copyTranslationAndScale3D(const Transform& SrcBA) {
      m_translation = SrcBA.m_translation;
      m_scale3D = SrcBA.m_scale3D;
    }

    void
    setFromMatrix(const Matrix4& InMatrix) {
      Matrix4 M = InMatrix;

      //Get the 3D scale from the matrix
      m_scale3D = M.extractScaling();

      //If there is negative scaling going on, we handle that here
      if (InMatrix.determinant() < 0.f) {
        //Assume it is along X and modify transform accordingly. 
        //It doesn't actually matter which axis we choose, the 'appearance'
        //will be the same
        m_scale3D.x *= -1.f;
        M.setAxis(0, -M.getScaledAxis(AXIS::kX));
      }

      m_rotation = Quaternion(M);
      m_translation = InMatrix.getOrigin();

      //Normalize rotation
      m_rotation.normalize();
    }

    /**
     * @brief Converts the provided world position to a space relative to the
     *        provided parent, and sets it as the current transform's position.
     */
    FORCEINLINE void
    setWorldTranslation(const Vector3& position, const Transform& parent);

    /**
     * @brief Converts the provided world rotation to a space relative to the
     *        provided parent, and sets it as the current transform's rotation.
     */
    FORCEINLINE void
    setWorldRotation(const Quaternion& rotation, const Transform& parent);

    /**
     * @brief Converts the provided world scale to a space relative to the
     *        provided parent, and sets it as the current transform's scale.
     */
    FORCEINLINE void
    setWorldScale(const Vector3& scale, const Transform& parent);

    /**
     * @brief Makes the current transform relative to the provided transform.
     *        In another words, converts from a world coordinate system to one
     *        local to the provided transform.
     */
    FORCEINLINE void
    makeLocal(const Transform& parent);

    /**
     * @brief Makes the current transform absolute. In another words, converts
     *        from a local coordinate system relative to the provided transform
     *        to a world coordinate system.
     */
    FORCEINLINE void
    makeWorld(const Transform& parent);

    /**
     * @brief Orients the object so it is looking at the provided @p location
     *        (world space) where @p up is used for determining the location
     *        of the object's Y axis.
     */
    FORCEINLINE void
    lookAt(const Vector3& location, const Vector3& up = Vector3::UP);

   private:
    /**
     * @brief Create a new transform: OutTransform = A * B using the matrix
     *        while keeping the scale that's given by A and B
     * @note  Please note that this operation is a lot more expensive than
     *        normal Multiply
     * Order matters when composing transforms : A * B will yield a transform
     * that logically first applies A then B to any subsequent transformation.
     * @param OutTransform pointer to transform that will store the result of
     *        A * B.
     * @param A Transform A.
     * @param B Transform B.
     */
    FORCEINLINE static void
    multiplyUsingMatrixWithScale(Transform* OutTransform,
                                 const Transform* A,
                                 const Transform* B);
    /**
     * @brief Create a new transform from multiplications of given to matrices
     *        (AMatrix*BMatrix) using desired scale
     * This is used by multiplyUsingMatrixWithScale and
     * getRelativeTransformUsingMatrixWithScale
     * @note  This is only used to handle negative scale
     * @param AMatrix first Matrix of operation
     * @param BMatrix second Matrix of operation
     * @param DesiredScale - there is no check on if the magnitude is correct
     *        here. It assumes that is correct.
     * @param OutTransform the constructed transform
     */
    FORCEINLINE static void
    constructTransformFromMatrixWithDesiredScale(const Matrix4& AMatrix,
                                                 const Matrix4& BMatrix,
                                                 const Vector3& DesiredScale,
                                                 Transform& OutTransform);

    /**
     * @brief Create a new transform: OutTransform = Base * Relative(-1) using
     *        the matrix while keeping the scale that's given by Base and
     *        Relative.
     * @note  Please note that this operation is a lot more expensive than
     *        normal getRelativeTransform.
     * @param OutTransform pointer to transform that will store the result of
     *        Base * Relative(-1).
     * @param BAse Transform Base.
     * @param Relative Transform Relative.
     */
    static void
    getRelativeTransformUsingMatrixWithScale(Transform* OutTransform,
                                             const Transform* Base,
                                             const Transform* Relative);

   public:
    /**
     * @brief The identity transformation
     *        Rotation = Quaternion::Identity
     *        Translation = Vector3::ZERO
     *        Scale3D = Vector3::UNIT
     */
    static const Transform IDENTITY;

   protected:
    /**
     * @brief Rotation of this transformation, as a quaternion.
     */
    Quaternion m_rotation = Quaternion::IDENTITY;

    /**
     * @briefTranslation of this transformation, as a vector.
     */
    Vector3	m_translation = Vector3::ZERO;

    /**
     * @brief3D scale (always applied in local space) as a vector.
     */
    Vector3	m_scale3D = Vector3::UNIT;
  };

  FORCEINLINE bool
  Transform::anyHasNegativeScale(const Vector3& InScale3D,
                                 const Vector3& InOtherScale3D) {
    return (InScale3D.x < 0.f || InScale3D.y < 0.f || InScale3D.z < 0.f ||
            InOtherScale3D.x < 0.f || InOtherScale3D.y < 0.f || InOtherScale3D.z < 0.f);
  }

  /**
   * @brief Scale the translation part of the Transform by the supplied vector.
   */
  FORCEINLINE void
  Transform::scaleTranslation(const Vector3& InScale3D) {
    m_translation *= InScale3D;
  }
  
  FORCEINLINE void
  Transform::scaleTranslation(const float& Scale) {
    m_translation *= Scale;
  }

  /**
   * @brief This function is from matrix, and all it does is to normalize
   *        rotation portion
   */
  FORCEINLINE void
  Transform::removeScaling(float /*Tolerance*/) {
    m_scale3D = Vector3::UP;
    m_rotation.normalize();
  }

  FORCEINLINE void
  Transform::multiplyUsingMatrixWithScale(Transform* OutTransform,
                                          const Transform* A,
                                          const Transform* B) {
    //The goal of using M is to get the correct orientation but for
    //translation, we still need scale
    constructTransformFromMatrixWithDesiredScale(A->toMatrixWithScale(),
                                                 B->toMatrixWithScale(),
                                                 A->m_scale3D * B->m_scale3D,
                                                 *OutTransform);
  }

  FORCEINLINE void
  Transform::constructTransformFromMatrixWithDesiredScale(const Matrix4& AMatrix,
                                                          const Matrix4& BMatrix,
                                                          const Vector3& DesiredScale,
                                                          Transform& OutTransform) {
    //The goal of using M is to get the correct orientation but for
    //translation, we still need scale
    Matrix4 M = AMatrix * BMatrix;
    M.removeScaling();

    //Apply negative scale back to axes
    Vector3 SignedScale = DesiredScale.getSignVector();

    M.setAxis(0, SignedScale.x * M.getScaledAxis(AXIS::kX));
    M.setAxis(1, SignedScale.y * M.getScaledAxis(AXIS::kY));
    M.setAxis(2, SignedScale.z * M.getScaledAxis(AXIS::kZ));

    //@note: If you have negative with 0 scale, this will return rotation that
    //is identity since matrix loses that axes
    Quaternion Rotation = Quaternion(M);
    Rotation.normalize();

    //Set values back to output
    OutTransform.m_scale3D = DesiredScale;
    OutTransform.m_rotation = Rotation;

    //Technically I could calculate this using Transform but then it does more
    //quaternion multiplication instead of using Scale in matrix multiplication
    //it's a question of between removeScaling vs using Transform to move
    //translation
    OutTransform.m_translation = M.getOrigin();
  }

  /**
   * @brief Returns Multiplied Transform of 2 Transforms
   */
  FORCEINLINE void
  Transform::multiply(Transform* OutTransform,
                      const Transform* A,
                      const Transform* B) {
    GE_ASSERT(A->isRotationNormalized());
    GE_ASSERT(B->isRotationNormalized());

    //When Q = quaternion, S = single scalar scale, and T = translation
    //QST(A) = Q(A), S(A), T(A), and QST(B) = Q(B), S(B), T(B)

    //QST (AxB)

    //QST(A) = Q(A)*S(A)*P*-Q(A) + T(A)
    //QST(AxB) = Q(B)*S(B)*QST(A)*-Q(B) + T(B)
    //QST(AxB) = Q(B)*S(B)*[Q(A)*S(A)*P*-Q(A) + T(A)]*-Q(B) + T(B)
    //QST(AxB) = Q(B)*S(B)*Q(A)*S(A)*P*-Q(A)*-Q(B) + Q(B)*S(B)*T(A)*-Q(B) + T(B)
    //QST(AxB) = [Q(B)*Q(A)]*[S(B)*S(A)]*P*-[Q(B)*Q(A)] + Q(B)*S(B)*T(A)*-Q(B) + T(B)

    //Q(AxB) = Q(B)*Q(A)
    //S(AxB) = S(A)*S(B)
    //T(AxB) = Q(B)*S(B)*T(A)*-Q(B) + T(B)

    if (anyHasNegativeScale(A->m_scale3D, B->m_scale3D)) {
      //@note, if you have 0 scale with negative, you're going to lose rotation
      //as it can't convert back to quaternion
      multiplyUsingMatrixWithScale(OutTransform, A, B);
    }
    else {
      OutTransform->m_rotation = B->m_rotation * A->m_rotation;
      OutTransform->m_scale3D = A->m_scale3D * B->m_scale3D;
      OutTransform->m_translation = B->m_rotation * (B->m_scale3D*A->m_translation) +
                                    B->m_translation;
    }
  }

  /**
   * @brief Apply Scale to this transform
   */
  FORCEINLINE Transform
  Transform::getScaled(float InScale) const {
    Transform A(*this);
    A.m_scale3D *= InScale;
    return A;
  }

  /**
   * @brief Apply Scale to this transform
   */
  FORCEINLINE Transform
  Transform::getScaled(const Vector3& InScale) const {
    Transform A(*this);
    A.m_scale3D *= InScale;
    return A;
  }

  /**
   * @brief Transform homogeneous Vector4, ignoring the scaling part of this
   *        transform
   */
  FORCEINLINE Vector4
  Transform::transformVector4NoScale(const Vector4& V) const {
    //If not, this won't work
    GE_ASSERT(0.f == V.w || 1.f == V.w);

    //Transform using QST is following
    //QST(P) = Q*S*P*-Q + T
    //Where Q = quaternion, S = scale, T = translation
    Vector4 myTransform = Vector4(m_rotation.rotateVector(Vector3(V)), 0.f);
    if (1.f == V.w) {
      myTransform += Vector4(m_translation, 1.f);
    }

    return myTransform;
  }

  /**
   * @brief Transform Vector4
   */
  FORCEINLINE Vector4
  Transform::transformVector4(const Vector4& V) const {
    //If not, this won't work
    GE_ASSERT(0.f == V.w || 1.f == V.w);

    //Transform using QST is following
    //QST(P) = Q*S*P*-Q + T
    //Where Q = quaternion, S = scale, T = translation

    Vector4 myTransform = Vector4(m_rotation.rotateVector(m_scale3D * Vector3(V)), 0.f);
    if (1.f == V.w) {
      myTransform += Vector4(m_translation, 1.f);
    }

    return myTransform;
  }

  FORCEINLINE Vector3
  Transform::transformPosition(const Vector3& V) const {
    return m_rotation.rotateVector(m_scale3D * V) + m_translation;
  }

  FORCEINLINE Vector3
  Transform::transformPositionNoScale(const Vector3& V) const {
    return m_rotation.rotateVector(V) + m_translation;
  }

  FORCEINLINE Vector3 Transform::transformVector(const Vector3& V) const {
    return m_rotation.rotateVector(m_scale3D * V);
  }

  FORCEINLINE Vector3
  Transform::transformVectorNoScale(const Vector3& V) const {
    return m_rotation.rotateVector(V);
  }

  //Do backward operation when inverse, translation -> rotation -> scale
  FORCEINLINE Vector3
  Transform::inverseTransformPosition(const Vector3 &V) const {
    return (m_rotation.unrotateVector(V - m_translation)) * getSafeScaleReciprocal(m_scale3D);
  }

  //Do backward operation when inverse, translation -> rotation
  FORCEINLINE Vector3
  Transform::inverseTransformPositionNoScale(const Vector3 &V) const {
    return (m_rotation.unrotateVector(V - m_translation));
  }

  //Do backward operation when inverse, translation -> rotation -> scale
  FORCEINLINE Vector3
  Transform::inverseTransformVector(const Vector3 &V) const {
    return (m_rotation.unrotateVector(V)) * getSafeScaleReciprocal(m_scale3D);
  }

  //Do backward operation when inverse, translation -> rotation
  FORCEINLINE Vector3
  Transform::inverseTransformVectorNoScale(const Vector3 &V) const {
    return (m_rotation.unrotateVector(V));
  }

  FORCEINLINE Quaternion
  Transform::transformRotation(const Quaternion& Q) const {
    return getRotation() * Q;
  }

  FORCEINLINE Quaternion
  Transform::inverseTransformRotation(const Quaternion& Q) const {
    return getRotation().inverse() * Q;
  }

  FORCEINLINE Transform
  Transform::operator*(const Transform& Other) const {
    Transform Output;
    multiply(&Output, this, &Other);
    return Output;
  }

  FORCEINLINE void
  Transform::operator*=(const Transform& Other) {
    multiply(this, this, &Other);
  }

  FORCEINLINE Transform
  Transform::operator*(const Quaternion& Other) const {
    Transform Output, OtherTransform(Other, Vector3::ZERO, Vector3::UNIT);
    multiply(&Output, this, &OtherTransform);
    return Output;
  }

  FORCEINLINE void
  Transform::operator*=(const Quaternion& Other) {
    Transform OtherTransform(Other, Vector3::ZERO, Vector3::UNIT);
    multiply(this, this, &OtherTransform);
  }

  //x = 0, y = 1, z = 2
  FORCEINLINE Vector3
  Transform::getScaledAxis(AXIS::E InAxis) const {
    if (InAxis == AXIS::kX) {
      return transformVector(Vector3(1.f, 0.f, 0.f));
    }
    else if (InAxis == AXIS::kY) {
      return transformVector(Vector3(0.f, 1.f, 0.f));
    }

    return transformVector(Vector3(0.f, 0.f, 1.f));
  }

  //x = 0, y = 1, z = 2
  FORCEINLINE Vector3
  Transform::getUnitAxis(AXIS::E InAxis) const {
    if (InAxis == AXIS::kX) {
      return transformVectorNoScale(Vector3(1.f, 0.f, 0.f));
    }
    else if (InAxis == AXIS::kY) {
      return transformVectorNoScale(Vector3(0.f, 1.f, 0.f));
    }

    return transformVectorNoScale(Vector3(0.f, 0.f, 1.f));
  }

  FORCEINLINE void
  Transform::mirror(AXIS::E MirrorAxis, AXIS::E FlipAxis) {
    //We do convert to Matrix for mirroring.
    Matrix4 M = toMatrixWithScale();
    M.mirror(MirrorAxis, FlipAxis);
    setFromMatrix(M);
  }

  /**
   * @brief Same version of Matrix4::getMaximumAxisScale function
   * @return the maximum magnitude of all components of the 3D scale.
   */
  inline float
  Transform::getMaximumAxisScale() const {
    return m_scale3D.getAbsMax();
  }

  /**
   * @return the minimum magnitude of all components of the 3D scale.
   */
  inline float
  Transform::getMinimumAxisScale() const {
    return m_scale3D.getAbsMin();
  }

  //Mathematically if you have 0 scale, it should be infinite, however, in
  //practice if you have 0 scale, and relative transform doesn't make much
  //sense anymore because you should be instead of showing gigantic infinite
  //mesh also returning BIG_NUMBER causes sequential NaN issues by multiplying 
  //so we hardcode as 0
  FORCEINLINE Vector3
  Transform::getSafeScaleReciprocal(const Vector3& InScale, float Tolerance) {
    Vector3 SafeReciprocalScale;
    if (Math::abs(InScale.x) <= Tolerance) {
      SafeReciprocalScale.x = 0.f;
    }
    else {
      SafeReciprocalScale.x = 1 / InScale.x;
    }

    if (Math::abs(InScale.y) <= Tolerance) {
      SafeReciprocalScale.y = 0.f;
    }
    else {
      SafeReciprocalScale.y = 1 / InScale.y;
    }

    if (Math::abs(InScale.z) <= Tolerance) {
      SafeReciprocalScale.z = 0.f;
    }
    else {
      SafeReciprocalScale.z = 1 / InScale.z;
    }

    return SafeReciprocalScale;
  }

  FORCEINLINE void
  Transform::setWorldTranslation(const Vector3& position, const Transform& parent) {
    Vector3 invScale = parent.getScale3D();
    if (invScale.x != 0) { invScale.x = 1.0f / invScale.x; }
    if (invScale.y != 0) { invScale.y = 1.0f / invScale.y; }
    if (invScale.z != 0) { invScale.z = 1.0f / invScale.z; }

    Quaternion invRotation = parent.getRotation().inverse();

    m_translation = invRotation.rotateVector(position - parent.getTranslation()) *  invScale;
  }

  FORCEINLINE void
  Transform::setWorldRotation(const Quaternion& rotation, const Transform& parent) {
    Quaternion invRotation = parent.getRotation().inverse();
    m_rotation = invRotation * rotation;
  }

  FORCEINLINE void
  Transform::setWorldScale(const Vector3& scale, const Transform& parent) {
    Matrix4 parentMatrix = parent.toInverseMatrixWithScale();
    Matrix4 scaleMat = QuatRotationMatrix::make(Quaternion::IDENTITY);
    for (uint32 row = 0; row < 3; ++row) {
      for (uint32 col = 0; col < 3; ++col) {
        scaleMat.m[row][col] = scale[row] * scaleMat.m[row][col];
      }
    }

    scaleMat = parentMatrix * scaleMat;
    m_scale3D.x = scaleMat.m[0][0];
    m_scale3D.y = scaleMat.m[1][1];
    m_scale3D.z = scaleMat.m[1][1];
  }

  FORCEINLINE void
  Transform::makeLocal(const Transform& parent) {
    setWorldTranslation(m_translation, parent);
    setWorldRotation(m_rotation, parent);
    setWorldScale(m_scale3D, parent);
  }

  FORCEINLINE void
  Transform::makeWorld(const Transform& parent) {
    //Update orientation
    const Quaternion& parentOrientation = parent.getRotation();
    m_rotation = parentOrientation * m_rotation;

    //Update scale
    const Vector3& parentScale = parent.getScale3D();

    //Scale own position by parent scale, just combine as equivalent axes,
    //no shearing
    m_scale3D = parentScale * m_scale3D;

    //Change position vector based on parent's orientation & scale
    m_translation = parentOrientation.rotateVector(parentScale * m_translation);

    //Add altered position vector to parents
    m_translation += parent.getTranslation();
  }

  FORCEINLINE void
  Transform::lookAt(const Vector3& location, const Vector3& up) {
    Vector3 forward = location - getTranslation();
    Quaternion rotation = getRotation();
    rotation.lookRotation(forward, up);
    setRotation(rotation);
  }
}
