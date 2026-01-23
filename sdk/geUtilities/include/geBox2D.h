/*****************************************************************************/
/**
 * @file    geBox2D.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/29
 * @brief   Implements a rectangular 2D Box.
 *
 * Implements a rectangular 2D Box.
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
#include "geVector2.h"

namespace geEngineSDK {
  /**
   * @brief Implements a rectangular 2D Box.
   */
  class Box2D final
  {
   public:
    /**
     * @brief Default constructor (no initialization).
     */
    Box2D() = default;

    /**
     * @brief Creates and initializes a new box.
     * Box extents are initialized to zero and the box is marked as invalid.
     * @param FORCE_INIT::E Enum.
     */
    explicit Box2D(FORCE_INIT::E) {
      init();
    }

    /**
     * @brief Creates and initializes a new box from the specified parameters.
     * @param InMin The box's minimum point.
     * @param InMax The box's maximum point.
     */
    Box2D(const Vector2& InMin, const Vector2& InMax)
      : m_min(InMin),
        m_max(InMax),
        m_bIsValid(true)
    {}

    /**
     * @brief Creates and initializes a new box from the given set of points.
     * @param Points Array of Points to create for the bounding volume.
     * @param Count The number of points.
     */
    GE_UTILITIES_EXPORT Box2D(const Vector2* Points, const SIZE_T Count);

    /**
     * @brief Creates and initializes a new box from an array of points.
     * @param Points Array of Points to create for the bounding volume.
     */
    explicit GE_UTILITIES_EXPORT Box2D(const Vector<Vector2>& Points);

   public:
    /**
     * @brief Compares two boxes for equality.
     * @param Other The other box to compare with.
     * @return true if the boxes are equal, false otherwise.
     */
    bool
    operator==(const Box2D& Other) const {
      return (m_min == Other.m_min) && (m_max == Other.m_max);
    }

    /**
     * @brief Adds to this bounding box to include a given point.
     * @param Other The point to increase the bounding volume to.
     * @return Reference to this bounding box after resizing to include the other point.
     */
    FORCEINLINE Box2D&
    operator+=(const Vector2 &Other);

    /**
     * @brief Gets the result of addition to this bounding volume.
     * @param Other The other point to add to this.
     * @return A new bounding volume.
     */
    Box2D
    operator+(const Vector2& Other) const {
      return Box2D(*this) += Other;
    }

    /**
     * @brief Adds to this bounding box to include a new bounding volume.
     * @param Other The bounding volume to increase the bounding volume to.
     * @return Reference to this bounding volume after resizing to include the
     *         other bounding volume.
     */
    FORCEINLINE Box2D&
    operator+=(const Box2D& Other);

    /**
     * @brief Gets the result of addition to this bounding volume.
     * @param Other The other volume to add to this.
     * @return A new bounding volume.
     */
    Box2D
    operator+(const Box2D& Other) const {
      return Box2D(*this) += Other;
    }

    /**
     * @brief Gets reference to the min or max of this bounding volume.
     * @param Index The index into points of the bounding volume.
     * @return A reference to a point of the bounding volume.
     */
    Vector2&
    operator[](SIZE_T Index) {
      GE_ASSERT(Index < 2);
      if (0 == Index) {
        return m_min;
      }
      return m_max;
    }

   public:
    /**
     * @brief Calculates the distance of a point to this box.
     * @param Point The point.
     * @return The distance.
     */
    FORCEINLINE float
    computeSquaredDistanceToPoint(const Vector2& Point) const {
      //Accumulates the distance as we iterate axis
      float DistSquared = 0.f;

      if (Point.x < m_min.x) {
        DistSquared += Math::square(Point.x - m_min.x);
      }
      else if (Point.x > m_max.x) {
        DistSquared += Math::square(Point.x - m_max.x);
      }

      if (Point.y < m_min.y) {
        DistSquared += Math::square(Point.y - m_min.y);
      }
      else if (Point.y > m_max.y) {
        DistSquared += Math::square(Point.y - m_max.y);
      }

      return DistSquared;
    }

    /**
     * @brief Increase the bounding box volume.
     * @param W The size to increase volume by.
     * @return A new bounding box increased in size.
     */
    Box2D
    expandBy(const float W) const {
      return Box2D(m_min - Vector2(W, W), m_max + Vector2(W, W));
    }

    /**
     * @brief Gets the box area.
     * @return Box area.
     * @see getCenter, getCenterAndExtents, getExtent, getSize
     */
    float
    getArea() const {
      return (m_max.x - m_min.x) * (m_max.y - m_min.y);
    }

    /**
     * @brief Gets the box's center point.
     * @return Th center point.
     * @see getArea, getCenterAndExtents, getExtent, getSize
     */
    Vector2
    getCenter() const {
      return Vector2((m_min + m_max) * 0.5f);
    }

    /**
     * @brief Get the center and extents
     * @param center[out] reference to center point
     * @param Extents[out] reference to the extent around the center
     * @see getArea, getCenter, getExtent, getSize
     */
    void
    getCenterAndExtents(Vector2& center, Vector2& Extents) const {
      Extents = getExtent();
      center = m_min + Extents;
    }

    /**
     * @brief Calculates the closest point on or inside the box to a given point in space.
     * @param Point The point in space.
     * @return The closest point on or inside the box.
     */
    FORCEINLINE Vector2
    getClosestPointTo(const Vector2& Point) const;

    /**
     * @brief Gets the box extents around the center.
     * @return Box extents.
     * @see getArea, getCenter, getCenterAndExtents, getSize
     */
    Vector2
    getExtent() const {
      return 0.5f * (m_max - m_min);
    }


    /**
     * @brief Gets the box size.
     * @return Box size.
     * @see GetArea, GetCenter, GetCenterAndExtents, GetExtent
     */
    Vector2
    getSize() const {
      return (m_max - m_min);
    }

    /**
     * @brief Set the initial values of the bounding box to Zero.
     */
    void
    init() {
      m_min = m_max = Vector2::ZERO;
      m_bIsValid = false;
    }

    /**
     * @brief Checks whether the given box intersects this box.
     * @param other bounding box to test intersection
     * @return true if boxes intersect, false otherwise.
     */
    FORCEINLINE bool
    intersect(const Box2D& other) const;

    /**
     * @brief Checks whether the given point is inside this box.
     * @param Point The point to test.
     * @return true if the point is inside this box, otherwise false.
     */
    bool
    isInside(const Vector2 & TestPoint) const {
      return ((TestPoint.x > m_min.x) && (TestPoint.x < m_max.x) &&
              (TestPoint.y > m_min.y) && (TestPoint.y < m_max.y));
    }

    /**
     * @brief Checks whether the given box is fully encapsulated by this box.
     * @param Other The box to test for encapsulation within the bounding volume.
     * @return true if box is inside this volume, false otherwise.
     */
    bool
    isInside(const Box2D& Other) const {
      return (isInside(Other.m_min) && isInside(Other.m_max));
    }

    /**
     * @brief Shift bounding box position.
     * @param The offset vector to shift by.
     * @return A new shifted bounding box.
     */
    Box2D
    shiftBy(const Vector2& Offset) const {
      return Box2D(m_min + Offset, m_max + Offset);
    }

   public:
    /**
     * @brief Holds the box's minimum point.
     */
    Vector2 m_min;

    /**
     * @brief Holds the box's maximum point.
     */
    Vector2 m_max;

    /**
     * @brief Holds a flag indicating whether this box is valid.
     */
    bool m_bIsValid = false;
  };

  /***************************************************************************/
  /**
   * Box2D inline functions
   */
  /***************************************************************************/

  FORCEINLINE Box2D&
  Box2D::operator+=(const Vector2 &Other) {
    if (m_bIsValid) {
      m_min.x = Math::min(m_min.x, Other.x);
      m_min.y = Math::min(m_min.y, Other.y);

      m_max.x = Math::max(m_max.x, Other.x);
      m_max.y = Math::max(m_max.y, Other.y);
    }
    else {
      m_min = m_max = Other;
      m_bIsValid = true;
    }

    return *this;
  }

  FORCEINLINE Box2D&
  Box2D::operator+=(const Box2D& Other) {
    if (m_bIsValid && Other.m_bIsValid) {
      m_min.x = Math::min(m_min.x, Other.m_min.x);
      m_min.y = Math::min(m_min.y, Other.m_min.y);

      m_max.x = Math::max(m_max.x, Other.m_max.x);
      m_max.y = Math::max(m_max.y, Other.m_max.y);
    }
    else if (Other.m_bIsValid) {
      *this = Other;
    }

    return *this;
  }

  FORCEINLINE Vector2
  Box2D::getClosestPointTo(const Vector2& Point) const {
    //Start by considering the point inside the box
    Vector2 ClosestPoint = Point;

    //Now clamp to inside box if it's outside
    if (Point.x < m_min.x) {
      ClosestPoint.x = m_min.x;
    }
    else if (Point.x > m_max.x) {
      ClosestPoint.x = m_max.x;
    }

    //Now clamp to inside box if it's outside
    if (Point.y < m_min.y) {
      ClosestPoint.y = m_min.y;
    }
    else if (Point.y > m_max.y) {
      ClosestPoint.y = m_max.y;
    }

    return ClosestPoint;
  }

  FORCEINLINE bool
  Box2D::intersect(const Box2D& Other) const {
    if ((m_min.x > Other.m_max.x) || (Other.m_min.x > m_max.x)) {
      return false;
    }

    if ((m_min.y > Other.m_max.y) || (Other.m_min.y > m_max.y)) {
      return false;
    }

    return true;
  }
}
