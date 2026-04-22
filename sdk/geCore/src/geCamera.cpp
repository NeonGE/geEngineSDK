#include "geCamera.h"

namespace geEngineSDK {
  const float Camera::DEFAULT_FOV = 50.0f;
  const float Camera::DEFAULT_NEAR = 1.0f;
  const float Camera::DEFAULT_FAR = 10000.0f;

  Camera::Camera() {
    m_fFov = DEFAULT_FOV;
    m_fNear = DEFAULT_NEAR;
    m_fFar = DEFAULT_FAR;
    m_viewMatrix = Matrix4::IDENTITY;
    m_projMatrix = Matrix4::IDENTITY;
  }

} // namespace geEngineSDK
