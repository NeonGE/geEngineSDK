/*****************************************************************************/
/**
 * @file    geSphere.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/19
 * @brief   Implements a basic sphere.
 *
 * Implements a basic sphere.
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

namespace geEngineSDK {
  /**
   * @brief Implements a basic sphere.
   */
  class Sphere final
  {
   public:
    /**
     * @brief Default constructor (no initialization).
     */
    Sphere() = default;

    /**
     * @brief Creates and initializes a new sphere with the specified parameters.
     * @param InV Center of sphere.
     * @param InW Radius of sphere.
     */
    Sphere(const Vector3& InV, float InW) : m_center(InV), m_radius(InW) {}

    /**
     * @brief Constructor.
     * @param FORCE_INIT Force Init Enum.
     */
    explicit FORCEINLINE Sphere(FORCE_INIT::E)
      : m_center(FORCE_INIT::kForceInit),
        m_radius(0.0f)
    {}

    /**
     * @brief Constructor.
     * @param Pts Pointer to list of points this sphere must contain.
     * @param Count How many points are in the list.
     */
    GE_UTILITIES_EXPORT Sphere(const Vector3* Pts, SIZE_T Count);

   public:
    /**
     * @brief Check whether two spheres are the same within specified tolerance.
     * @param Sphere The other sphere.
     * @param Tolerance Error Tolerance.
     * @return true if spheres are equal within specified tolerance, otherwise false.
     */
    bool
    equals(const Sphere& sphere, float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      return m_center.equals(sphere.m_center, Tolerance) &&
             Math::abs(m_radius - sphere.m_radius) <= Tolerance;
    }

    /**
     * @brief Check whether sphere is inside of another.
     * @param Other The other sphere.
     * @param Tolerance Error Tolerance.
     * @return true if sphere is inside another, otherwise false.
     */
    bool
    isInside(const Sphere& Other, float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      if (m_radius > Other.m_radius + Tolerance) {
        return false;
      }

      return (m_center - Other.m_center).sizeSquared() <=
              Math::square(Other.m_radius + Tolerance - m_radius);
    }

    /**
     * @brief Checks whether the given location is inside this sphere.
     * @param In The location to test for inside the bounding volume.
     * @return true if location is inside this volume.
     */
    bool
    isInside(const Vector3& In, float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      return (m_center - In).sizeSquared() <= Math::square(m_radius + Tolerance);
    }

    /**
     * @brief Test whether this sphere intersects another.
     * @param  Other The other sphere.
     * @param  Tolerance Error tolerance.
     * @return true if spheres intersect, false otherwise.
     */
    FORCEINLINE bool
    intersects(const Sphere& Other, float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      return (m_center - Other.m_center).sizeSquared() <= 
              Math::square(Math::max(0.f, Other.m_radius + m_radius + Tolerance));
    }

    /**
     * @brief Get result of Transforming sphere by Matrix.
     * @param M Matrix to transform by.
     * @return Result of transformation.
     */
    GE_UTILITIES_EXPORT Sphere
    transformBy(const Matrix4& M) const;

    /**
     * @brief Get result of Transforming sphere with Transform.
     * @param M Transform information.
     * @return Result of transformation.
     */
    GE_UTILITIES_EXPORT Sphere
    transformBy(const Transform& M) const;

    /**
     * @brief Get volume of the current sphere
     * @return Volume (in Engine units).
     */
    GE_UTILITIES_EXPORT float
    getVolume() const;

    /**
     * @brief Adds to this bounding sphere to include a new bounding volume.
     * @param Other the bounding volume to increase the bounding volume to.
     * @return Reference to this bounding volume after resizing to include the
     *         other bounding volume.
     */
    GE_UTILITIES_EXPORT Sphere&
    operator+=(const Sphere& Other);

    Sphere&
    operator+=(const Vector3& point) {
      float dist = (point - m_center).size();
      if (dist > m_radius) {
        float newRadius = (m_radius + dist) * 0.5f;
        m_center += (point - m_center) * (newRadius - m_radius) / dist;
        m_radius = newRadius;
      }

      return *this;
    }

    /**
     * @brief Gets the result of addition to this bounding volume.
     * @param Other The other volume to add to this.
     * @return A new bounding volume.
     */
    Sphere
    operator+(const Sphere& Other) const {
      return Sphere(*this) += Other;
    }

   public:
    /**
     * @brief The sphere's center point.
     */
     Vector3 m_center;

    /**
     * @brief The sphere's radius.
     */
    float m_radius = 0.f;
  };

  /***************************************************************************/
  /**
   * Math inline functions
   */
  /***************************************************************************/

  /**
   * @brief Converts a sphere into a point plus radius squared for the test above
   */
  FORCEINLINE bool
  Math::sphereAABBIntersection(const Sphere& InSphere, const AABox& AABB) {
    float RadiusSquared = Math::square(InSphere.m_radius);
    //If the distance is less than or equal to the radius, they intersect
    return sphereAABBIntersection(InSphere.m_center, RadiusSquared, AABB);
  }
}
