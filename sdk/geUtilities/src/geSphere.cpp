/*****************************************************************************/
/**
 * @file    geSphere.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/19
 * @brief   Implements a basic sphere.
 *
 * Implements a basic sphere.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geSphere.h"
#include "geBox.h"
#include "geMatrix4.h"

namespace geEngineSDK {

  Sphere::Sphere(const Vector3* Pts, SIZE_T Count) : m_center(0, 0, 0), m_radius(0) {
    if (Count) {
      const AABox Box(Pts, Count);
      *this = Sphere((Box.m_min + Box.m_max) / 2, 0);

      for (SIZE_T i = 0; i < Count; ++i) {
        const float Dist = Vector3::distSquared(Pts[i], m_center);
        if (Dist > m_radius) {
          m_radius = Dist;
        }
      }

      m_radius = Math::sqrt(m_radius) * 1.001f;
    }
  }

  Sphere
  Sphere::transformBy(const Matrix4& M) const {
    Sphere	Result;
    Result.m_center = M.transformPosition(this->m_center);

    const Vector3 XAxis(M.m[0][0], M.m[0][1], M.m[0][2]);
    const Vector3 YAxis(M.m[1][0], M.m[1][1], M.m[1][2]);
    const Vector3 ZAxis(M.m[2][0], M.m[2][1], M.m[2][2]);

    Result.m_radius = Math::sqrt(Math::max3(XAxis | XAxis,
                                            YAxis | YAxis,
                                            ZAxis | ZAxis)) * m_radius;
    return Result;
  }

  Sphere
  Sphere::transformBy(const Transform& M) const {
    Sphere Result;
    Result.m_center = M.transformPosition(this->m_center);
    Result.m_radius = M.getMaximumAxisScale() * m_radius;
    return Result;
  }

  float
  Sphere::getVolume() const {
    return (4.f / 3.f) * Math::PI * (m_radius * m_radius * m_radius);
  }

  Sphere&
  Sphere::operator+=(const Sphere &Other) {
    if (0.f == m_radius) {
      *this = Other;
    }
    else if (isInside(Other)) {
      *this = Other;
    }
    else if (Other.isInside(*this)) {
      //No change		
    }
    else {
      Sphere NewSphere;

      Vector3 DirToOther = Other.m_center - m_center;
      Vector3 UnitDirToOther = DirToOther;
      UnitDirToOther.normalize();

      float NewRadius = (DirToOther.size() + Other.m_radius + m_radius) * 0.5f;

      //Find end point
      Vector3 End1 = Other.m_center + UnitDirToOther * Other.m_radius;
      Vector3 End2 = m_center - UnitDirToOther * m_radius;
      Vector3 NewCenter = (End1 + End2) * 0.5f;

      NewSphere.m_center = NewCenter;
      NewSphere.m_radius = NewRadius;

      //Make sure both are inside afterwards
      GE_ASSERT(Other.isInside(NewSphere, 1.f));
      GE_ASSERT(isInside(NewSphere, 1.f));

      *this = NewSphere;
    }

    return *this;
  }
}
