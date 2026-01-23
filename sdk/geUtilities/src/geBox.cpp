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

#define SIMDPP_ARCH_X86_SSE4_1

#if USING(GE_COMPILER_MSVC)
# pragma warning(disable: 4127)
# pragma warning(disable: 4244)
#endif

#if USING(GE_COMPILER_MSVC)
# pragma warning(default: 4127)
# pragma warning(default: 4244)
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
}
