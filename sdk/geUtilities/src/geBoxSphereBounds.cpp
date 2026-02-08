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
#include "geTransform.h"

#if USING(GE_REFLECTION)
# include "geRTTRMeta.h"
#endif

namespace geEngineSDK {
  void
  BoxSphereBounds::diagnosticCheckNaN() const {
# if USING(GE_DEBUG_MODE)
    if (m_origin.containsNaN()) {
      GE_LOG(kError, Generic, "Origin contains NaN");
      const_cast<BoxSphereBounds*>(this)->m_origin = Vector3::ZERO;
    }
    if (m_boxExtent.containsNaN()) {
      GE_LOG(kError, Generic, "BoxExtent contains NaN");
      const_cast<BoxSphereBounds*>(this)->m_boxExtent = Vector3::ZERO;
    }
    if (Math::isNaN(m_sphereRadius) || !Math::isFinite(m_sphereRadius)) {
      GE_LOG(kError, Generic, "SphereRadius contains NaN");
      const_cast<BoxSphereBounds*>(this)->m_sphereRadius = 0.f;
    }
# endif
  }  

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

#if USING(GE_REFLECTION)
  RTTR_REGISTRATION
  {
    using namespace rttr;
    registration::class_<BoxSphereBounds>("BoxSphereBounds")
    .constructor<>()(
      metaScriptable(),
      metaTooltip("Default constructor with non-initialized values."),
      metaCategory("[Math]"))
    .constructor<const Vector3&, const Vector3&, float>()(
      metaScriptable(),
      metaTooltip("Constructs and initializes a BoxSphereBounds from the given parameters."),
      metaCategory("[Math]"))
    .constructor<const AABox&, const Sphere&>()(
      metaScriptable(),
      metaTooltip("Constructs and initializes a BoxSphereBounds from the "    \
                  "given Box and Sphere."),
      metaCategory("[Math]"))
    .constructor<const AABox&>()(
      metaScriptable(),
      metaTooltip("Constructs and initializes a BoxSphereBounds from the given Box."),
      metaCategory("[Math]"))
    .constructor<const Sphere&>()(
      metaScriptable(),
      metaTooltip("Constructs and initializes a BoxSphereBounds from the given Sphere."),
      metaCategory("[Math]"))

    //Properties
    .property("origin", &BoxSphereBounds::m_origin)(
      metaScriptable(),
      metaCategory("[Math]"),
      metaTooltip("The origin of the bounding box and sphere."))

    .property("boxExtent", &BoxSphereBounds::m_boxExtent)(
      metaScriptable(),
      metaCategory("[Math]"),
      metaTooltip("The extent of the bounding box."))

    .property("sphereRadius", &BoxSphereBounds::m_sphereRadius)(
      metaScriptable(),
      metaCategory("[Math]"),
      metaTooltip("The radius of the bounding sphere."))

    //Methods
    .method("transformBy",
      select_overload<BoxSphereBounds(const Matrix4&) const>(&BoxSphereBounds::transformBy))(
      metaScriptable(),
      metaTooltip("Transforms this bounding volume by the given matrix and "  \
                  "returns the result."),
      metaCategory("[Math]"))
    .method("transformBy",
      select_overload<BoxSphereBounds(const Transform&) const>(&BoxSphereBounds::transformBy))(
      metaScriptable(),
      metaTooltip("Transforms this bounding volume by the given transform "   \
                  "and returns the result."),
      metaCategory("[Math]"))
    .method("expandBy", &BoxSphereBounds::expandBy)(
      metaScriptable(),
      metaTooltip("Returns a new bounding volume with the box and sphere "    \
                  "expanded by the given amount."),
      metaCategory("[Math]"))
    .method("getBox", &BoxSphereBounds::getBox)(
      metaScriptable(),
      metaTooltip("Returns the bounding box."),
      metaCategory("[Math]"))
    .method("getSphere", &BoxSphereBounds::getSphere)(
      metaScriptable(),
      metaTooltip("Returns the bounding sphere."),
      metaCategory("[Math]"))
    .method("getBoxExtrema", &BoxSphereBounds::getBoxExtrema)(
      metaScriptable(),
      metaTooltip("Returns the bounding box extrema. Pass 1 for positive "    \
                  "extrema from the origin, else negative."),
      metaCategory("[Math]"))
    .method("computeSquaredDistanceFromBoxToPoint",
            &BoxSphereBounds::computeSquaredDistanceFromBoxToPoint)(
      metaScriptable(),
      metaTooltip("Computes the squared distance from the bounding box to a point."),
      metaCategory("[Math]"))
    .method("spheresIntersect", &BoxSphereBounds::spheresIntersect)(
      metaScriptable(),
      metaTooltip("Checks whether the bounding sphere of this volume "        \
                  "intersects with that of another."),
      metaCategory("[Math]"))

    .method("boxesIntersect", &BoxSphereBounds::boxesIntersect)(
      metaScriptable(),
      metaTooltip("Checks whether the bounding box of this volume "          \
                  "intersects with that of another."),
      metaCategory("[Math]"))
    ;
  }
#endif
}
