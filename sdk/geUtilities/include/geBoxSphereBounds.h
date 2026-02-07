/*****************************************************************************/
/**
 * @file    geBoxSphereBounds.h
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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geVector3.h"
#include "geSphere.h"
#include "geBox.h"

namespace geEngineSDK {
  /**
   * @brief Class for a combined axis aligned bounding box and bounding sphere
   *        with the same origin. (28 bytes).
   */
  class BoxSphereBounds final
  {
   public:
    /**
     * @brief Default constructor.
     */
    BoxSphereBounds() = default;

    /**
     * @brief Creates and initializes a new instance.
     * @param FORCE_INIT::E Enum.
     */
    explicit FORCEINLINE BoxSphereBounds(FORCE_INIT::E)
      : m_origin(FORCE_INIT::kForceInit),
        m_boxExtent(FORCE_INIT::kForceInit),
        m_sphereRadius(0.f) {
      diagnosticCheckNaN();
    }

    /**
     * @brief Creates and initializes a new instance from the specified parameters.
     * @param InOrigin origin of the bounding box and sphere.
     * @param InBoxExtent half size of box.
     * @param InSphereRadius radius of the sphere.
     */
    BoxSphereBounds(const Vector3& InOrigin, const Vector3& InBoxExtent, float InSphereRadius)
      : m_origin(InOrigin),
        m_boxExtent(InBoxExtent),
        m_sphereRadius(InSphereRadius) {
      diagnosticCheckNaN();
    }

    /**
     * @brief Creates and initializes a new instance from the given Box and Sphere.
     * @param Box The bounding box.
     * @param Sphere The bounding sphere.
     */
    BoxSphereBounds(const AABox& Box, const Sphere& sphere) {
      Box.getCenterAndExtents(m_origin, m_boxExtent);
      m_sphereRadius = Math::min(m_boxExtent.size(),
                                 (sphere.m_center - m_origin).size() + sphere.m_radius);
      diagnosticCheckNaN();
    }

    /**
     * @brief Creates and initializes a new instance of the given Box.
     * The sphere radius is taken from the extent of the box.
     * @param Box The bounding box.
     */
    explicit BoxSphereBounds(const AABox& Box) {
      Box.getCenterAndExtents(m_origin, m_boxExtent);
      m_sphereRadius = m_boxExtent.size();
      diagnosticCheckNaN();
    }

    /**
     * @brief Creates and initializes a new instance for the given sphere.
     */
    explicit BoxSphereBounds(const Sphere& sphere)
	    : m_origin(sphere.m_center),
        m_boxExtent(Vector3(sphere.m_radius)),
        m_sphereRadius(sphere.m_radius) {
      diagnosticCheckNaN();
    }

    /**
     * @brief Creates and initializes a new instance from the given set of points.
     * The sphere radius is taken from the extent of the box.
     * @param Points The points to be considered for the bounding box.
     * @param NumPoints Number of points in the Points array.
     */
    BoxSphereBounds(const Vector3* Points, SIZE_T NumPoints);

   public:
    /**
     * @brief Constructs a bounding volume containing both this and B.
     * @param Other The other bounding volume.
     * @return The combined bounding volume.
     */
    FORCEINLINE BoxSphereBounds
    operator+(const BoxSphereBounds& Other) const;

    /**
     * @brief Compare bounding volume this and Other.
     * @param Other The other bounding volume.
     * @return true of they match.
     */
    FORCEINLINE bool
    operator==(const BoxSphereBounds& Other) const;

    /**
     * @brief Compare bounding volume this and Other.
     * @param Other The other bounding volume.
     * @return true of they do not match.
     */
    FORCEINLINE bool
    operator!=(const BoxSphereBounds& Other) const;

   public:
    /**
    * @brief Calculates the squared distance from a point to a bounding box
    * @param Point The point.
    * @return The distance.
    */
    FORCEINLINE float
    computeSquaredDistanceFromBoxToPoint(const Vector3& Point) const {
      Vector3 Mins = m_origin - m_boxExtent;
      Vector3 Maxs = m_origin + m_boxExtent;
      return geEngineSDK::computeSquaredDistanceFromBoxToPoint(Mins, Maxs, Point);
    }

    /**
     * @brief Test whether the spheres from two BoxSphereBounds intersect/overlap.
     * @param A First BoxSphereBounds to test.
     * @param B Second BoxSphereBounds to test.
     * @param Tolerance Error tolerance added to test distance.
     * @return true if spheres intersect, false otherwise.
     */
    FORCEINLINE static bool
    spheresIntersect(const BoxSphereBounds& A,
                     const BoxSphereBounds& B,
                     float Tolerance = Math::KINDA_SMALL_NUMBER) {
      return (A.m_origin - B.m_origin).sizeSquared() <= 
             Math::square(Math::max(0.f, A.m_sphereRadius + B.m_sphereRadius + Tolerance));
    }

    /**
     * @brief Test whether the boxes from two BoxSphereBounds intersect/overlap.
     * @param A First BoxSphereBounds to test.
     * @param B Second BoxSphereBounds to test.
     * @return true if boxes intersect, false otherwise.
     */
    FORCEINLINE static bool
    boxesIntersect(const BoxSphereBounds& A, const BoxSphereBounds& B) {
      return A.getBox().intersect(B.getBox());
    }

    /**
     * @brief Gets the bounding box.
     * @return The bounding box.
     */
    FORCEINLINE AABox
    getBox() const {
      return AABox(m_origin - m_boxExtent, m_origin + m_boxExtent);
    }

    /**
     * @brief Gets the extrema for the bounding box.
     * @param Extrema 1 for positive extrema from the origin, else negative
     * @return The boxes extrema
     */
    Vector3
    getBoxExtrema(uint32 Extrema) const {
      if (Extrema) {
        return m_origin + m_boxExtent;
      }
      return m_origin - m_boxExtent;
    }

    /**
     * @brief Gets the bounding sphere.
     * @return The bounding sphere.
     */
    FORCEINLINE Sphere
    getSphere() const {
      return Sphere(m_origin, m_sphereRadius);
    }

    /**
     * @brief Increase the size of the box and sphere by a given size.
     * @param ExpandAmount The size to increase by.
     * @return A new box with the expanded size.
     */
    FORCEINLINE BoxSphereBounds
    expandBy(float ExpandAmount) const {
      return BoxSphereBounds(m_origin,
                             m_boxExtent + ExpandAmount,
                             m_sphereRadius + ExpandAmount);
    }

    /**
     * @brief Gets a bounding volume transformed by a matrix.
     * @param M The matrix.
     * @return The transformed volume.
     */
    GE_UTILITIES_EXPORT BoxSphereBounds
    transformBy(const Matrix4& M) const;

    /**
     * @brief Gets a bounding volume transformed by a FTransform object.
     * @param M The Transform object.
     * @return The transformed volume.
     */
    GE_UTILITIES_EXPORT BoxSphereBounds
    transformBy(const Transform& M) const;

    /**
     * @brief Constructs a bounding volume containing both A and B.
     * This is a legacy version of the function used to compute primitive
     * bounds, to avoid the need to rebuild lighting after the change.
     */
    friend BoxSphereBounds
    boundsUnion(const BoxSphereBounds& A, const BoxSphereBounds& B) {
      AABox BoundingBox(FORCE_INIT::kForceInit);

      BoundingBox += (A.m_origin - A.m_boxExtent);
      BoundingBox += (A.m_origin + A.m_boxExtent);
      BoundingBox += (B.m_origin - B.m_boxExtent);
      BoundingBox += (B.m_origin + B.m_boxExtent);

      //Build a bounding sphere from the bounding box's origin and the radii of A and B.
      BoxSphereBounds Result(BoundingBox);

      Result.m_sphereRadius = Math::min(Result.m_sphereRadius,
                                        Math::max((A.m_origin - Result.m_origin).size() +
                                                   A.m_sphereRadius,
                                                  (B.m_origin - Result.m_origin).size() +
                                                   B.m_sphereRadius));
      Result.diagnosticCheckNaN();

      return Result;
    }

    GE_UTILITIES_EXPORT void
    diagnosticCheckNaN() const;

    inline bool
    containsNaN() const {
      return m_origin.containsNaN() ||
             m_boxExtent.containsNaN() ||
             !Math::isFinite(m_sphereRadius);
    }

   public:
    /**
     * @brief Holds the origin of the bounding box and sphere.
     */
    Vector3 m_origin;

    /**
     * @brief Holds the extent of the bounding box.
     */
    Vector3 m_boxExtent;

    /**
    * @brief Holds the radius of the bounding sphere. */
    float m_sphereRadius = 0.f;
  };

  /***************************************************************************/
  /**
   * BoxSphereBounds inline functions
   */
  /***************************************************************************/

  FORCEINLINE BoxSphereBounds::BoxSphereBounds(const Vector3* Points, SIZE_T NumPoints) {
    AABox BoundingBox(FORCE_INIT::kForceInit);

    //Find an axis aligned bounding box for the points.
    for (SIZE_T PointIndex = 0; PointIndex < NumPoints; ++PointIndex) {
      BoundingBox += Points[PointIndex];
    }

    BoundingBox.getCenterAndExtents(m_origin, m_boxExtent);

    //Using the center of the bounding box as the origin of the sphere, find
    //the radius of the bounding sphere.
    m_sphereRadius = 0.0f;

    for (SIZE_T PointIndex = 0; PointIndex < NumPoints; ++PointIndex) {
      m_sphereRadius = Math::max(m_sphereRadius, (Points[PointIndex] - m_origin).size());
    }

    diagnosticCheckNaN();
  }

  FORCEINLINE BoxSphereBounds
  BoxSphereBounds::operator+(const BoxSphereBounds& Other) const {
    AABox BoundingBox(FORCE_INIT::kForceInit);

    BoundingBox += (this->m_origin - this->m_boxExtent);
    BoundingBox += (this->m_origin + this->m_boxExtent);
    BoundingBox += (Other.m_origin - Other.m_boxExtent);
    BoundingBox += (Other.m_origin + Other.m_boxExtent);

    //Build a bounding sphere from the bounding box's origin and the radii of A and B.
    BoxSphereBounds Result(BoundingBox);

    Result.m_sphereRadius = Math::min(Result.m_sphereRadius,
                                      Math::max((m_origin - Result.m_origin).size() +
                                                 m_sphereRadius,
                                                (Other.m_origin - Result.m_origin).size() +
                                                 Other.m_sphereRadius));
    Result.diagnosticCheckNaN();
    return Result;
  }

  FORCEINLINE bool
  BoxSphereBounds::operator==(const BoxSphereBounds& Other) const {
    return m_origin == Other.m_origin &&
           m_boxExtent == Other.m_boxExtent &&
           m_sphereRadius == Other.m_sphereRadius;
  }

  FORCEINLINE bool
  BoxSphereBounds::operator!=(const BoxSphereBounds& Other) const {
    return !(*this == Other);
  }
}
