/*****************************************************************************/
/**
 * @file    geBox.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/19
 * @brief   Implements an axis-aligned box.
 *
 * Boxes describe an axis-aligned extent in three dimensions. They are used for
 * many different things in the Engine and in games, such as bounding volumes,
 * collision detection and visibility calculation.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geBox.h"
#include "geVector4.h"
#include "geMatrix4.h"
#include "geTransform.h"

#if USING(GE_REFLECTION)
# include "geRTTRMeta.h"
#endif

namespace geEngineSDK {
  AABox::AABox(const Vector3* Points, SIZE_T Count)
    : m_min(0, 0, 0),
      m_max(0, 0, 0),
      m_isValid(0) {
    for (SIZE_T i = 0; i < Count; ++i) {
      *this += Points[i];
    }
  }

  AABox::AABox(const Vector<Vector3>& Points)
    : m_min(0, 0, 0),
      m_max(0, 0, 0),
      m_isValid(0) {
    for (const auto& Point : Points) {
      *this += Point;
    }
  }

  AABox
  AABox::transformBy(const Matrix4& M) const {
    //If we are not valid, return another invalid box.
    if (!m_isValid) {
      return AABox(FORCE_INIT::kForceInit);
    }

    AABox NewBox;

    const Vector4 VecMin(m_min.x, m_min.y, m_min.z, 0.0f);
    const Vector4 VecMax(m_max.x, m_max.y, m_max.z, 0.0f);

    const Vector4 Half(0.5f, 0.5f, 0.5f, 0.0f);
    const Vector4 Origin = (VecMax + VecMin) * Half;  // Center of the box
    const Vector4 Extent = (VecMax - VecMin) * Half;  // Half-extents of the box

    // Transform the origin (center) of the box
    // Assuming transformPosition applies the full matrix including translation
    Vector4 NewOrigin = M.transformPosition(Origin);

    // Transform the extents of the box
    Vector4 NewExtent = (M.transformVector(Vector4(Extent.x, 0, 0, 0)).vectorAbs() +
      M.transformVector(Vector4(0, Extent.y, 0, 0)).vectorAbs() +
      M.transformVector(Vector4(0, 0, Extent.z, 0)).vectorAbs());

    // Compute the new AABB
    const Vector4 NewVecMin = NewOrigin - NewExtent;
    const Vector4 NewVecMax = NewOrigin + NewExtent;

    NewBox.m_min = NewVecMin;
    NewBox.m_max = NewVecMax;
    NewBox.m_isValid = 1;

    return NewBox;
  }

  AABox
  AABox::transformBy(const Transform& M) const {
    return transformBy(M.toMatrixWithScale());
  }

  AABox
  AABox::inverseTransformBy(const Transform& M) const {
    const Vector3 Vertices[8] =
    {
      Vector3(m_min),
      Vector3(m_min.x, m_min.y, m_max.z),
      Vector3(m_min.x, m_max.y, m_min.z),
      Vector3(m_max.x, m_min.y, m_min.z),
      Vector3(m_max.x, m_max.y, m_min.z),
      Vector3(m_max.x, m_min.y, m_max.z),
      Vector3(m_min.x, m_max.y, m_max.z),
      Vector3(m_max)
    };

    AABox NewBox(FORCE_INIT::kForceInit);

    for (const auto& Vertice : Vertices) {
      Vector4 ProjectedVertex = M.inverseTransformPosition(Vertice);
      NewBox += ProjectedVertex;
    }

    return NewBox;
  }

  AABox
  AABox::transformProjectBy(const Matrix4& ProjM) const {
    const Vector3 Vertices[8] =
    {
      Vector3(m_min),
      Vector3(m_min.x, m_min.y, m_max.z),
      Vector3(m_min.x, m_max.y, m_min.z),
      Vector3(m_max.x, m_min.y, m_min.z),
      Vector3(m_max.x, m_max.y, m_min.z),
      Vector3(m_max.x, m_min.y, m_max.z),
      Vector3(m_min.x, m_max.y, m_max.z),
      Vector3(m_max)
    };

    AABox NewBox(FORCE_INIT::kForceInit);

    for (auto& Vertice : Vertices) {
      Vector4 ProjectedVertex = ProjM.transformPosition(Vertice);
      NewBox += (static_cast<Vector3>(ProjectedVertex)) / ProjectedVertex.w;
    }

    return NewBox;
  }

  AABox
  AABox::overlap(const AABox& Other) const {
    if (intersect(Other) == false) {
      static AABox EmptyBox(FORCE_INIT::kForceInit);
      return EmptyBox;
    }

    //Otherwise they overlap, so find overlapping box
    Vector3 MinVector, MaxVector;

    MinVector.x = Math::max(m_min.x, Other.m_min.x);
    MaxVector.x = Math::min(m_max.x, Other.m_max.x);

    MinVector.y = Math::max(m_min.y, Other.m_min.y);
    MaxVector.y = Math::min(m_max.y, Other.m_max.y);

    MinVector.z = Math::max(m_min.z, Other.m_min.z);
    MaxVector.z = Math::min(m_max.z, Other.m_max.z);

    return AABox(MinVector, MaxVector);
  }

#if USING(GE_REFLECTION)
  RTTR_REGISTRATION
  {
    using namespace rttr;
    registration::class_<AABox>("AABox")
    .constructor<>()(
      metaScriptable(),
      metaTooltip("Default constructor with non-initialized values."),
      metaCategory("[Math]"))
    .constructor<const Vector3&, const Vector3&>()(
      metaScriptable(),
      metaTooltip("Constructs and initializes an AABox from the given minimum and maximum points."),
      metaCategory("[Math]"))

    //Properties
    .property("min", &AABox::m_min)(
      metaScriptable(),
      metaCategory("[Math]"),
      metaTooltip("Minimum point of the box."))
    .property("max", &AABox::m_max)(
      metaScriptable(),
      metaCategory("[Math]"),
      metaTooltip("Maximum point of the box."))

    //Methods
    .method("computeSquaredDistanceToPoint", &AABox::computeSquaredDistanceToPoint)(
      metaScriptable(),
      metaTooltip("Computes the squared distance from a point to the box."),
      metaCategory("[Math]"))

    .method("expandBy",
      rttr::select_overload<AABox(float)const>(&AABox::expandBy))(
      metaScriptable(),
      metaTooltip("Expands the box by a given distance in all directions."),
      metaCategory("[Math]"))
    .method("expandBy",
      rttr::select_overload<AABox(const Vector3&)const>(&AABox::expandBy))(
      metaScriptable(),
      metaTooltip("Expands the box by a given vector, where each component "  \
                  "specifies the expansion distance in that direction."),
      metaCategory("[Math]"))
    .method("expandBy",
      rttr::select_overload<AABox(const Vector3&, const Vector3&)const>(&AABox::expandBy))(
      metaScriptable(),
      metaTooltip("Expands the box by given minimum and maximum vectors."),
      metaCategory("[Math]"))
    .method("shiftBy", &AABox::shiftBy)(
      metaScriptable(),
      metaTooltip("Shifts the box by the given vector."),
      metaCategory("[Math]"))
    .method("moveTo", &AABox::moveTo)(
      metaScriptable(),
      metaTooltip("Moves the box to the specified location, keeping its size."),
      metaCategory("[Math]"))
    .method("getCenter", &AABox::getCenter)(
      metaScriptable(),
      metaTooltip("Gets the center point of the box."),
      metaCategory("[Math]"))
    .method("getClosestPointTo", &AABox::getClosestPointTo)(
      metaScriptable(),
      metaTooltip("Gets the closest point on the box to the specified point."),
      metaCategory("[Math]"))
    .method("getExtent", &AABox::getExtent)(
      metaScriptable(),
      metaTooltip("Gets the extent (size) of the box."),
      metaCategory("[Math]"))
    .method("getSize", &AABox::getSize)(
      metaScriptable(),
      metaTooltip("Gets the size of the box."),
      metaCategory("[Math]"))
    .method("getVolume", &AABox::getVolume)(
      metaScriptable(),
      metaTooltip("Gets the volume of the box."),
      metaCategory("[Math]"))
    .method("intersect", &AABox::intersect)(
      metaScriptable(),
      metaTooltip("Checks if this box intersects with another box."),
      metaCategory("[Math]"))
    .method("intersectXY", &AABox::intersectXY)(
      metaScriptable(),
      metaTooltip("Checks if this box intersects with another box in the XY plane."),
      metaCategory("[Math]"))
    .method("overlap", &AABox::overlap)(
      metaScriptable(),
      metaTooltip("Calculates the overlapping box between this box and another box."),
      metaCategory("[Math]"))
    .method("isInside",
      rttr::select_overload<bool(const Vector3&) const>(&AABox::isInside))(
      metaScriptable(),
      metaTooltip("Checks if a point is inside the box."),
      metaCategory("[Math]"))
    .method("isInsideOrOn", &AABox::isInsideOrOn)(
      metaScriptable(),
      metaTooltip("Checks if a point is inside or on the boundary of the box."),
      metaCategory("[Math]"))
    .method("isInside",
      rttr::select_overload<bool(const AABox&) const>(&AABox::isInside))(
      metaScriptable(),
      metaTooltip("Checks if another box is completely inside this box."),
      metaCategory("[Math]"))
    .method("isInsideXY",
      rttr::select_overload<bool(const Vector3&) const>(&AABox::isInsideXY))(
      metaScriptable(),
      metaTooltip("Checks if a point is inside the box in the XY plane."),
      metaCategory("[Math]"))
    .method("isInsideXY",
      rttr::select_overload<bool(const AABox&) const>(&AABox::isInsideXY))(
      metaScriptable(),
      metaTooltip("Checks if another box is completely inside this box in the XY plane."),
      metaCategory("[Math]"))
    .method("transformBy",
      rttr::select_overload<AABox(const Matrix4&) const>(&AABox::transformBy))(
      metaScriptable(),
      metaTooltip("Transforms the box by the given matrix."),
      metaCategory("[Math]"))
    .method("transformBy",
      rttr::select_overload<AABox(const Transform&) const>(&AABox::transformBy))(
      metaScriptable(),
      metaTooltip("Transforms the box by the given Transform object."),
      metaCategory("[Math]"))
    .method("inverseTransformBy", &AABox::inverseTransformBy)(
      metaScriptable(),
      metaTooltip("Inverse transforms the box by the given Transform object."),
      metaCategory("[Math]"))
    .method("transformProjectBy", &AABox::transformProjectBy)(
      metaScriptable(),
      metaTooltip("Transforms the box by the given projection matrix."),
      metaCategory("[Math]"))
    ;
  }
#endif
}
