/*****************************************************************************/
/**
 * @file    geTransform.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/04/28
 * @brief   Contains information about 3D object's position, rotation and scale
 *
 * Contains information about 3D object's position, rotation and scale, and
 * provides methods to manipulate it. 
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geTransform.h"

namespace geEngineSDK {
  //Transform identity
  //@note: Do not reference Vector3::ZERO or Vector3::UNIT
  //because they're not initialized yet, it will come as 0 vector
  const Transform Transform::IDENTITY(Quaternion(0.f, 0.f, 0.f, 1.f),
                                      Vector3(0.f),
                                      Vector3(1.f));

  Transform
  Transform::getRelativeTransformReverse(const Transform& Other) const {
    //A (-1) * B = VQS(B)(VQS (A)(-1))
    // 
    //Scale = S(B)/S(A)
    //Rotation = Q(B) * Q(A)(-1)
    //Translation = T(B)-S(B)/S(A) *[Q(B)*Q(A)(-1)*T(A)*Q(A)*Q(B)(-1)]
    //where A = this, and B = Other
    Transform Result;

    Vector3 SafeRecipScale3D = getSafeScaleReciprocal(m_scale3D);
    Result.m_scale3D = Other.m_scale3D * SafeRecipScale3D;
    Result.m_rotation = Other.m_rotation * m_rotation.inverse();
    Result.m_translation = Other.m_translation - Result.m_scale3D *
                           (Result.m_rotation * m_translation);

    return Result;
  }

  /**
   * @brief Set current transform and the relative to ParentTransform.
   * Equates to This = This->getRelativeTransform(Parent), but saves the
   * intermediate Transform storage and copy.
   */
  void
  Transform::setToRelativeTransform(const Transform& ParentTransform) {
    //A * B(-1) = VQS(B)(-1) (VQS (A))
    //
    //Scale = S(A)/S(B)
    //Rotation = Q(B)(-1) * Q(A)
    //Translation = 1/S(B) *[Q(B)(-1)*(T(A)-T(B))*Q(B)]
    //where A = this, B = Other
    const Vector3 SafeRecipScale3D = getSafeScaleReciprocal(ParentTransform.m_scale3D,
                                                            Math::SMALL_NUMBER);
    const Quaternion InverseRot = ParentTransform.m_rotation.inverse();

    m_scale3D *= SafeRecipScale3D;
    m_translation = (InverseRot * (m_translation - ParentTransform.m_translation)) *
                    SafeRecipScale3D;
    m_rotation = InverseRot * m_rotation;
  }

  void
  Transform::getRelativeTransformUsingMatrixWithScale(Transform* OutTransform,
                                                      const Transform* Base,
                                                      const Transform* Relative) {
    //The goal of using M is to get the correct orientation but for
    //translation, we still need scale
    Matrix4 AM = Base->toMatrixWithScale();
    Matrix4 BM = Relative->toMatrixWithScale();
    //Get combined scale
    Vector3 SafeRecipScale3D = getSafeScaleReciprocal(Relative->m_scale3D,
                                                      Math::SMALL_NUMBER);
    Vector3 DesiredScale3D = Base->m_scale3D * SafeRecipScale3D;
    constructTransformFromMatrixWithDesiredScale(AM,
                                                 BM.inverse(),
                                                 DesiredScale3D,
                                                 *OutTransform);
  }

  Transform
  Transform::getRelativeTransform(const Transform& Other) const {
    //A * B(-1) = VQS(B)(-1) (VQS (A))
    //
    //Scale = S(A)/S(B)
    //Rotation = Q(B)(-1) * Q(A)
    //Translation = 1/S(B) *[Q(B)(-1)*(T(A)-T(B))*Q(B)]
    //where A = this, B = Other
    Transform Result;

    if (anyHasNegativeScale(m_scale3D, Other.getScale3D())) {
      //@note, if you have 0 scale with negative, you're going to lose rotation
      //as it can't convert back to quaternion
      getRelativeTransformUsingMatrixWithScale(&Result, this, &Other);
    }
    else {
      Vector3 SafeRecipScale3D = getSafeScaleReciprocal(Other.m_scale3D,
                                                        Math::SMALL_NUMBER);
      Result.m_scale3D = m_scale3D * SafeRecipScale3D;

      if (!Other.m_rotation.isNormalized()) {
        return Transform::IDENTITY;
      }

      Quaternion Inverse = Other.m_rotation.inverse();
      Result.m_rotation = Inverse * m_rotation;

      Result.m_translation = (Inverse * (m_translation - Other.m_translation)) *
                             (SafeRecipScale3D);
    }

    return Result;
  }
}
