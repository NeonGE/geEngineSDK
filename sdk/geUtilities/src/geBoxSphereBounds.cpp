/*****************************************************************************/
/**
 * @file    geBoxSphereBounds.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/29
 * @brief   Class for a combined axis aligned bounding box and bounding sphere.
 *
 * Class for a combined axis aligned bounding box and bounding sphere with the
 * same origin. (28 bytes).
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geBoxSphereBounds.h"
#include "geMatrix4.h"

namespace geEngineSDK {
  BoxSphereBounds
    BoxSphereBounds::transformBy(const Matrix4& M) const {
# if USING(GE_DEBUG_MODE)
    if (M.containsNaN()) {
      GE_LOG(kError, Generic, "Input Matrix contains NaN/Inf!");
      (const_cast<Matrix4*>(&M))->setIdentity();
    }
# endif
    BoxSphereBounds Result;

    const Vector4 vOrigin(m_origin, 0.0f);
    const Vector4 vExtent(m_boxExtent, 0.0f);

    const Vector4 m0(M.m[0][0], M.m[0][1], M.m[0][2], M.m[0][3]);
    const Vector4 m1(M.m[1][0], M.m[1][1], M.m[1][2], M.m[1][3]);
    const Vector4 m2(M.m[2][0], M.m[2][1], M.m[2][2], M.m[2][3]);
    const Vector4 m3(M.m[3][0], M.m[3][1], M.m[3][2], M.m[3][3]);

    Vector4 NewOrigin = Vector4(vOrigin.x, vOrigin.x, vOrigin.x, vOrigin.x) * m0;
    NewOrigin += (Vector4(vOrigin.y, vOrigin.y, vOrigin.y, vOrigin.y) * m1);
    NewOrigin += (Vector4(vOrigin.z, vOrigin.z, vOrigin.z, vOrigin.z) * m2);
    NewOrigin += m3;

    Vector4 NewExt = (Vector4(vExtent.x, vExtent.x, vExtent.x, vExtent.x) * m0).vectorAbs();
    NewExt += (Vector4(vExtent.y, vExtent.y, vExtent.y, vExtent.y) * m1).vectorAbs();
    NewExt += (Vector4(vExtent.z, vExtent.z, vExtent.z, vExtent.z) * m2).vectorAbs();

    Result.m_boxExtent = NewExt;
    Result.m_origin = NewOrigin;

    Vector4 MRad = m0 * m0;
    MRad += m1 * m1;
    MRad += m2 * m2;
    Result.m_sphereRadius = Math::sqrt(Math::max3(MRad.x, MRad.y, MRad.z)) * m_sphereRadius;

    Result.diagnosticCheckNaN();
    return Result;
  }

  BoxSphereBounds
  BoxSphereBounds::transformBy(const Transform& M) const {
    const Matrix4 Mat = M.toMatrixWithScale();
    BoxSphereBounds Result = transformBy(Mat);
    return Result;
  }
}
