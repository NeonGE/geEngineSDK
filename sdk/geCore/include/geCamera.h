/*****************************************************************************/
/**
 * @file    geCamera.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2022/03/27
 * @brief   Create a utilities base for creating math cameras.
 *
 * Create a utilities base for creating math cameras.
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
#include "gePrerequisitesCore.h"
#include "geDegree.h"
#include "geRadian.h"
#include "geVector3.h"
#include "geQuaternion.h"
#include "geMatrix4.h"

namespace geEngineSDK {
  class GE_CORE_EXPORT Camera
  {
   public:
    Camera();
    virtual ~Camera() = default;

    void
    setOrthographic(float width, float height, float fNear, float fFar) {
      m_bIsOrtho = true;
      m_fOrthoWidth = width;
      m_fOrthoHeight = height;
      m_fNear = fNear;
      m_fFar = fFar;
      m_bIsDirty = true;
    }

    void
    setPerspective(Degree degFov, float fWidth, float fHeight, float fNear, float fFar) {
      m_bIsOrtho = false;
      m_fFov = degFov.valueDegrees();
      m_fScreenWidth = fWidth;
      m_fScreenHeight = fHeight;
      m_fNear = fNear;
      m_fFar = fFar;
      m_bIsDirty = true;
    }

    void
    setPosition(const Vector3& position) {
      m_position = position;
      m_bIsDirty = true;
    }

    void
    setLookAt(const Vector3& lookAt) {
      m_lookAt = lookAt;
      m_bIsDirty = true;
    }

    void
    setUp(const Vector3& up) {
      m_up = up;
      m_bIsDirty = true;
    }

    void
    updateCamera() {
      if (m_bIsDirty) {
        m_viewMatrix = LookAtMatrix(m_position, m_lookAt, m_up);

        if (m_bIsOrtho) {
          m_projMatrix = OrthoMatrix(m_fOrthoWidth, m_fOrthoHeight, m_fNear, m_fFar);
        }
        else {
          m_projMatrix = PerspectiveMatrix(m_fFov.valueRadians(),
                                           m_fScreenWidth,
                                           m_fScreenHeight,
                                           m_fNear,
                                           m_fFar);
        }

        m_bIsDirty = false;
      }
    }

    const Matrix4&
    getViewMatrix() {
      if (m_bIsDirty) {
        updateCamera();
      }

      return m_viewMatrix;
    }

    const Matrix4&
    getProjMatrix() {
      if (m_bIsDirty) {
        updateCamera();
      }

      return m_projMatrix;
    }

    void
    setScreenSize(float width, float height) {
      m_fScreenWidth = width;
      m_fScreenHeight = height;
      m_bIsDirty = true;
    }

    void
    setFov(Degree fov) {
      m_fFov = fov;
      m_bIsDirty = true;
    }

    void
    setNear(float fNear) {
      m_fNear = fNear;
      m_bIsDirty = true;
    }

    void
    setFar(float fFar) {
      m_fFar = fFar;
      m_bIsDirty = true;
    }

    void
    moveCamera(const Vector3& direction) {
      m_position += direction;
      m_bIsDirty = true;
    }

    void
    panCamera(float dx, float dy) {
      Vector3 front = (m_lookAt - m_position).getSafeNormal();
      Vector3 right = (Vector3::UP ^ front).getSafeNormal();
      Vector3 up = front ^ right;

      m_position += right * dx;
      m_position += up * dy;

      m_lookAt += right * dx;
      m_lookAt += up * dy;

      m_bIsDirty = true;
    }

    void
    rotateCamera(const Radian& yaw, const Radian& pitch) {
      Quaternion yawRot(Vector3::UP, yaw);
      Quaternion pitchRot(Vector3::RIGHT, pitch);

      Vector3 forward = m_lookAt - m_position;
      forward = yawRot * forward;
      forward = pitchRot * forward;

      m_lookAt = m_position + forward;

      m_bIsDirty = true;
    }

    void
    zoomCamera(float amount) {
      Vector3 forward = m_lookAt - m_position;
      forward.normalize();

      m_position += forward * amount;
      //m_lookAt += forward * amount;

      m_bIsDirty = true;
    }

    void
    setDistance(float distance) {
      Vector3 forward = m_lookAt - m_position;
      forward.normalize();

      m_position = m_lookAt - forward * distance;

      m_bIsDirty = true;
    }

    float
      getDistance() const {
      return (m_lookAt - m_position).size();
    }

    void
    orbitCamera(const Radian& yaw, const Radian& pitch, const Vector3& center) {    
      Vector3 forward = m_position - center;

      Vector3 forwardNormal = forward.getSafeNormal();
      float dot = forwardNormal | Vector3::UP;
      Radian currentPitch = Math::acos(dot);

      float newPitch = currentPitch.valueRadians() + pitch.valueRadians();
      newPitch = Math::clamp(newPitch, Degree(0).valueRadians(), Degree(180).valueRadians());

      Quaternion yawRot(Vector3::UP, yaw);
      Quaternion pitchRot(getRight(), newPitch - currentPitch);

      forward = yawRot * forward;
      auto backUpForward = forward;
      forward = pitchRot * forward;

      dot = forward.getUnsafeNormal() | Vector3::UP;
      if (Math::abs(dot) > 0.999f) {
        forward = backUpForward;
      }
      
      m_position = center + forward;
      m_lookAt = center;

      m_bIsDirty = true;
    }

    inline Vector3
    getRight() const {
      Vector3 front = (m_lookAt - m_position).getSafeNormal();
      if (Math::abs(front | Vector3::UP) > 0.999f) {
        return (front ^ Vector3::RIGHT).getSafeNormal();
      }
      return (Vector3::UP ^ front).getSafeNormal();
    }

    inline Vector3
    getForward() const {
      return (m_lookAt - m_position).getSafeNormal();
    }

    inline Vector3
    getUp() const {
      Vector3 front = (m_lookAt - m_position).getSafeNormal();
      Vector3 right = (Vector3::UP ^ front).getSafeNormal();
      return front ^ right;
    }

    bool
    operator==(const Camera& other) const {
      return m_bIsDirty == other.m_bIsDirty &&
             m_bIsOrtho == other.m_bIsOrtho &&
             m_fOrthoWidth == other.m_fOrthoWidth &&
             m_fOrthoHeight == other.m_fOrthoHeight &&
             m_fNear == other.m_fNear &&
             m_fFar == other.m_fFar &&
             m_fFov == other.m_fFov &&
             m_fScreenWidth == other.m_fScreenWidth &&
             m_fScreenHeight == other.m_fScreenHeight &&
             m_position == other.m_position &&
             m_lookAt == other.m_lookAt &&
             m_up == other.m_up;
    }

    bool
    operator!=(const Camera& other) const {
      return !(*this == other);
    }

   public:
    bool m_bIsDirty = true;
    bool m_bIsOrtho = false;
    float m_fOrthoWidth = 1.0f;
    float m_fOrthoHeight = 1.0f;

    float m_fNear = DEFAULT_NEAR;
    float m_fFar = DEFAULT_FAR;
    Degree m_fFov = Degree(DEFAULT_FOV);
    float m_fScreenWidth = 0.0f;
    float m_fScreenHeight = 0.0f;

    Vector3 m_position = Vector3::ZERO;
    Vector3 m_lookAt = Vector3::FORWARD;
    Vector3 m_up = Vector3::UP;

    Matrix4 m_viewMatrix;
    Matrix4 m_projMatrix;

   public:
    static const float DEFAULT_FOV;
    static const float DEFAULT_NEAR;
    static const float DEFAULT_FAR;
  };

} // namespace geEngineSDK
