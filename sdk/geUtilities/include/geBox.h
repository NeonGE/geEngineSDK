/*****************************************************************************/
/**
 * @file    geBox.h
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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geVector3.h"
#include "geTransform.h"

namespace geEngineSDK {
  /**
   * @brief Implements an axis-aligned box.
   * Boxes describe an axis-aligned extent in three dimensions. They are used
   * for many different things in the Engine and in games, such as bounding
   * volumes, collision detection and visibility calculation.
   */
  class AABox final
  {
   public:
    /**
     * @brief Default constructor (no initialization).
     */
    AABox() = default;

    /**
    * @brief Creates and initializes a new box with zero extent and marks it as
    *        invalid.
    * @param FORCE_INIT enum.
    */
    explicit AABox(FORCE_INIT::E) {
      init();
    }

    /**
     * @brief Creates and initializes a new box from the specified extents.
     * @param InMin The box's minimum point.
     * @param InMax The box's maximum point.
     */
    AABox(const Vector3& InMin, const Vector3& InMax) {
      m_min.x = Math::min(InMin.x, InMax.x);
      m_min.y = Math::min(InMin.y, InMax.y);
      m_min.z = Math::min(InMin.z, InMax.z);

      m_max.x = Math::max(InMin.x, InMax.x);
      m_max.y = Math::max(InMin.y, InMax.y);
      m_max.z = Math::max(InMin.z, InMax.z);

      m_isValid = 1;
    }

    /**
     * @brief Creates and initializes a new box from the given set of points.
     * @param Points Array of Points to create for the bounding volume.
     * @param Count The number of points.
     */
    GE_UTILITIES_EXPORT AABox(const Vector3* Points, SIZE_T Count);

    /**
     * @brief Creates and initializes a new box from an array of points.
     * @param Points Array of Points to create for the bounding volume.
     */
    GE_UTILITIES_EXPORT AABox(const Vector<Vector3>& Points);

   public:
    /**
     * @brief Compares two boxes for equality.
     * @return true if the boxes are equal, false otherwise.
     */
    FORCEINLINE bool
    operator==(const AABox& Other) const {
      return (m_min == Other.m_min) && (m_max == Other.m_max);
    }

    /**
     * @brief Adds to this bounding box to include a given point.
     * @param Other the point to increase the bounding volume to.
     * @return Reference to this bounding box after resizing to include the other point.
     */
    FORCEINLINE AABox&
    operator+=(const Vector3 &Other);

    /**
     * @brief Gets the result of addition to this bounding volume.
     * @param Other The other point to add to this.
     * @return A new bounding volume.
     */
    FORCEINLINE AABox
    operator+(const Vector3& Other) const {
      return AABox(*this) += Other;
    }

    /**
     * @brief Adds to this bounding box to include a new bounding volume.
     * @param Other the bounding volume to increase the bounding volume to.
     * @return Reference to this bounding volume after resizing to include the
     *         other bounding volume.
     */
    FORCEINLINE AABox&
    operator+=(const AABox& Other);

    /**
    * @brief Gets the result of addition to this bounding volume.
    * @param Other The other volume to add to this.
    * @return A new bounding volume.
    */
    FORCEINLINE AABox operator+(const AABox& Other) const {
      return AABox(*this) += Other;
    }

    /**
    * @brief Gets reference to the min or max of this bounding volume.
    * @param Index the index into points of the bounding volume.
    * @return a reference to a point of the bounding volume.
    */
    FORCEINLINE Vector3&
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
    computeSquaredDistanceToPoint(const Vector3& Point) const {
      return computeSquaredDistanceFromBoxToPoint(m_min, m_max, Point);
    }

    /**
     * @brief Increases the box size.
     * @param W The size to increase the volume by.
     * @return A new bounding box.
     */
    FORCEINLINE AABox
    expandBy(float W) const {
      return AABox(m_min - Vector3(W, W, W), m_max + Vector3(W, W, W));
    }

    /**
     * @brief Increases the box size.
     * @param V The size to increase the volume by.
     * @return A new bounding box.
     */
    FORCEINLINE AABox
    expandBy(const Vector3& V) const {
      return AABox(m_min - V, m_max + V);
    }

    /**
     * @brief Increases the box size.
     * @param Neg The size to increase the volume by in the negative direction
     *        (positive values move the bounds outwards)
     * @param Pos The size to increase the volume by in the positive direction
     *        (positive values move the bounds outwards)
     * @return A new bounding box.
     */
    AABox
    expandBy(const Vector3& Neg, const Vector3& Pos) const {
      return AABox(m_min - Neg, m_max + Pos);
    }

    /**
     * @brief Shifts the bounding box position.
     * @param Offset The vector to shift the box by.
     * @return A new bounding box.
     */
    FORCEINLINE AABox
    shiftBy(const Vector3& Offset) const {
      return AABox(m_min + Offset, m_max + Offset);
    }

    /**
     * @brief Moves the center of bounding box to new destination.
     * @param The destination point to move center of box to.
     * @return A new bounding box.
     */
    FORCEINLINE AABox
    moveTo(const Vector3& Destination) const {
      const Vector3 Offset = Destination - getCenter();
      return AABox(m_min + Offset, m_max + Offset);
    }

    /**
     * @brief Gets the center point of this box.
     * @return The center point.
     * @see GetCenterAndExtents, GetExtent, GetSize, GetVolume
     */
    FORCEINLINE Vector3
    getCenter() const {
      return (m_min + m_max) * 0.5f;
    }

    /**
     * @brief Gets the center and extents of this box.
     * @param center[out] Will contain the box center point.
     * @param Extents[out] Will contain the extent around the center.
     * @see GetCenter, GetExtent, GetSize, GetVolume
     */
    FORCEINLINE void
    getCenterAndExtents(Vector3& center, Vector3& Extents) const {
      Extents = getExtent();
      center = m_min + Extents;
    }

    /**
     * @brief Calculates the closest point on or inside the box to a given
     *        point in space.
     * @param Point The point in space.
     * @return The closest point on or inside the box.
     */
    FORCEINLINE Vector3
    getClosestPointTo(const Vector3& Point) const;

    /**
     * @brief Gets the extents of this box.
     * @return The box extents.
     * @see GetCenter, GetCenterAndExtents, GetSize, GetVolume
     */
    FORCEINLINE Vector3
    getExtent() const {
      return 0.5f * (m_max - m_min);
    }

    /**
     * @brief Gets a reference to the specified point of the bounding box.
     * @param PointIndex The index of the extrema point to return.
     * @return A reference to the point.
     */
    FORCEINLINE Vector3&
    getExtrema(uint32 PointIndex) {
      return (&m_min)[PointIndex];
    }

    /**
     * @brief Gets a read-only reference to the specified point of the box.
     * @param PointIndex The index of extrema point to return.
     * @return A read-only reference to the point.
     */
    FORCEINLINE const Vector3&
    getExtrema(uint32 PointIndex) const {
      return (&m_min)[PointIndex];
    }

    /**
     * @brief Gets the size of this box.
     * @return The box size.
     * @see GetCenter, GetCenterAndExtents, GetExtent, GetVolume
     */
    FORCEINLINE Vector3
    getSize() const {
      return (m_max - m_min);
    }

    /**
     * @brief Gets the volume of this box.
     * @return The box volume.
     * @see GetCenter, GetCenterAndExtents, GetExtent, GetSize
     */
    FORCEINLINE float
    getVolume() const {
      return ((m_max.x - m_min.x) * (m_max.y - m_min.y) * (m_max.z - m_min.z));
    }

    /**
     * @brief Set the initial values of the bounding box to Zero.
     */
    FORCEINLINE void
    init() {
      m_min = m_max = Vector3::ZERO;
      m_isValid = 0;
    }

    /**
     * @brief Checks whether the given bounding box intersects this box.
     * @param Other The bounding box to intersect with.
     * @return true if the boxes intersect, false otherwise.
     */
    FORCEINLINE bool
    intersect(const AABox& other) const;

    /**
     * @brief Checks whether the given bounding box intersects this bounding
     *        box in the XY plane.
     * @param Other The bounding box to test intersection.
     * @return true if the boxes intersect in the XY Plane, false otherwise.
     */
    FORCEINLINE bool
    intersectXY(const AABox& Other) const;

    /**
     * @brief Returns the overlap AABox of two box
     * @param Other The bounding box to test overlap
     * @return the overlap box. It can be 0 if they don't overlap
     */
    GE_UTILITIES_EXPORT AABox
    overlap(const AABox& Other) const;

    /**
     * @brief Gets a bounding volume transformed by an inverted Transform object.
     * @param M The transformation object to perform the inversely transform this box with.
     * @return	The transformed box.
     */
    GE_UTILITIES_EXPORT AABox
    inverseTransformBy(const Transform& M) const;

    /**
     * @brief Checks whether the given location is inside this box.
     * @param In The location to test for inside the bounding volume.
     * @return true if location is inside this volume.
     * @see IsInsideXY
     */
    FORCEINLINE bool
    isInside(const Vector3& In) const {
      return ((In.x > m_min.x) && (In.x < m_max.x) &&
              (In.y > m_min.y) && (In.y < m_max.y) &&
              (In.z > m_min.z) && (In.z < m_max.z));
    }

    /**
     * @brief Checks whether the given location is inside or on this box.
     * @param In The location to test for inside the bounding volume.
     * @return true if location is inside this volume.
     * @see IsInsideXY
     */
    FORCEINLINE bool
    isInsideOrOn(const Vector3& In) const {
      return ((In.x >= m_min.x) && (In.x <= m_max.x) &&
              (In.y >= m_min.y) && (In.y <= m_max.y) &&
              (In.z >= m_min.z) && (In.z <= m_max.z));
    }

    /**
     * @brief Checks whether a given box is fully encapsulated by this box.
     * @param Other The box to test for encapsulation within the bounding volume.
     * @return true if box is inside this volume.
     */
    FORCEINLINE bool
    isInside(const AABox& Other) const {
      return (isInside(Other.m_min) && isInside(Other.m_max));
    }

    /**
     * @brief Checks whether the given location is inside this box in the XY plane.
     * @param In The location to test for inside the bounding box.
     * @return true if location is inside this box in the XY plane.
     * @see IsInside
     */
    FORCEINLINE bool
    isInsideXY(const Vector3& In) const {
      return ((In.x > m_min.x) && (In.x < m_max.x) &&
              (In.y > m_min.y) && (In.y < m_max.y));
    }

    /**
     * @brief Checks whether the given box is fully encapsulated by this box in the XY plane.
     * @param Other The box to test for encapsulation within the bounding box.
     * @return true if box is inside this box in the XY plane.
     */
    FORCEINLINE bool
    isInsideXY(const AABox& Other) const {
      return (isInsideXY(Other.m_min) && isInsideXY(Other.m_max));
    }

    /**
     * @brief Gets a bounding volume transformed by a matrix.
     * @param M The matrix to transform by.
     * @return The transformed box.
     * @see TransformProjectBy
     */
    GE_UTILITIES_EXPORT AABox
    transformBy(const Matrix4& M) const;

    /**
     * @brief Gets a bounding volume transformed by a Transform object.
     * @param M The transformation object.
     * @return The transformed box.
     * @see TransformProjectBy
     */
    GE_UTILITIES_EXPORT AABox
    transformBy(const Transform& M) const;

    /**
     * @brief Transforms and projects a world bounding box to screen space
     * @param ProjM The projection matrix.
     * @return The transformed box.
     * @see TransformBy
     */
    GE_UTILITIES_EXPORT AABox
    transformProjectBy(const Matrix4& ProjM) const;

   public:
    /**
     * @brief Utility function to build an AABB from Origin and Extent
     * @param Origin The location of the bounding box.
     * @param Extent Half size of the bounding box.
     * @return A new axis-aligned bounding box.
     */
    static AABox
    buildAABB(const Vector3& Origin, const Vector3& Extent) {
      AABox NewBox(Origin - Extent, Origin + Extent);
      return NewBox;
    }

   public:
    /**
     * @brief Holds the box's minimum point.
     */
    Vector3 m_min;

    /**
     * @brief Holds the box's maximum point.
     */
    Vector3 m_max;

    /**
     * @brief Holds a flag indicating whether this box is valid.
     */
    uint8 m_isValid;
  };

  /***************************************************************************/
  /**
   * Box inline functions
   */
  /***************************************************************************/

  FORCEINLINE AABox&
  AABox::operator+=(const Vector3 &Other) {
    if (m_isValid) {
      m_min.x = Math::min(m_min.x, Other.x);
      m_min.y = Math::min(m_min.y, Other.y);
      m_min.z = Math::min(m_min.z, Other.z);

      m_max.x = Math::max(m_max.x, Other.x);
      m_max.y = Math::max(m_max.y, Other.y);
      m_max.z = Math::max(m_max.z, Other.z);
    }
    else {
      m_min = m_max = Other;
      m_isValid = 1;
    }

    return *this;
  }

  FORCEINLINE AABox&
  AABox::operator+=(const AABox& Other) {
    if (m_isValid && Other.m_isValid) {
      m_min.x = Math::min(m_min.x, Other.m_min.x);
      m_min.y = Math::min(m_min.y, Other.m_min.y);
      m_min.z = Math::min(m_min.z, Other.m_min.z);

      m_max.x = Math::max(m_max.x, Other.m_max.x);
      m_max.y = Math::max(m_max.y, Other.m_max.y);
      m_max.z = Math::max(m_max.z, Other.m_max.z);
    }
    else if (Other.m_isValid) {
      *this = Other;
    }

    return *this;
  }

  FORCEINLINE Vector3
  AABox::getClosestPointTo(const Vector3& Point) const {
    //Start by considering the point inside the box
    Vector3 ClosestPoint = Point;

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

    //Now clamp to inside box if it's outside.
    if (Point.z < m_min.z) {
      ClosestPoint.z = m_min.z;
    }
    else if (Point.z > m_max.z) {
      ClosestPoint.z = m_max.z;
    }

    return ClosestPoint;
  }

  FORCEINLINE bool
  AABox::intersect(const AABox& Other) const {
    if ((m_min.x > Other.m_max.x) || (Other.m_min.x > m_max.x)) {
      return false;
    }

    if ((m_min.y > Other.m_max.y) || (Other.m_min.y > m_max.y)) {
      return false;
    }

    if ((m_min.z > Other.m_max.z) || (Other.m_min.z > m_max.z)) {
      return false;
    }

    return true;
  }

  FORCEINLINE bool
  AABox::intersectXY(const AABox& Other) const {
    if ((m_min.x > Other.m_max.x) || (Other.m_min.x > m_max.x)) {
      return false;
    }

    if ((m_min.y > Other.m_max.y) || (Other.m_min.y > m_max.y)) {
      return false;
    }

    return true;
  }

  /***************************************************************************/
  /**
   * Math inline functions
   */
  /***************************************************************************/

  inline bool
  Math::pointBoxIntersection(const Vector3&	Point, const AABox& Box) {
    if (Point.x >= Box.m_min.x && Point.x <= Box.m_max.x &&
        Point.y >= Box.m_min.y && Point.y <= Box.m_max.y &&
        Point.z >= Box.m_min.z && Point.z <= Box.m_max.z) {
      return true;
    }
    return false;
  }

  inline bool
  Math::lineBoxIntersection(const AABox& Box,
                            const Vector3& Start,
                            const Vector3& End,
                            const Vector3& Direction) {
    return lineBoxIntersection(Box, Start, End, Direction, Direction.reciprocal());
  }

  inline bool
  Math::lineBoxIntersection(const AABox& Box,
                            const Vector3& Start,
                            const Vector3& End,
                            const Vector3& Direction,
                            const Vector3& OneOverDirection) {
    Vector3 Time;
    bool bStartIsOutside = false;

    if (Start.x < Box.m_min.x) {
      bStartIsOutside = true;
      if (End.x >= Box.m_min.x) {
        Time.x = (Box.m_min.x - Start.x) * OneOverDirection.x;
      }
      else {
        return false;
      }
    }
    else if (Start.x > Box.m_max.x) {
      bStartIsOutside = true;
      if (End.x <= Box.m_max.x) {
        Time.x = (Box.m_max.x - Start.x) * OneOverDirection.x;
      }
      else {
        return false;
      }
    }
    else {
      Time.x = 0.0f;
    }

    if (Start.y < Box.m_min.y) {
      bStartIsOutside = true;
      if (End.y >= Box.m_min.y) {
        Time.y = (Box.m_min.y - Start.y) * OneOverDirection.y;
      }
      else {
        return false;
      }
    }
    else if (Start.y > Box.m_max.y) {
      bStartIsOutside = true;
      if (End.y <= Box.m_max.y) {
        Time.y = (Box.m_max.y - Start.y) * OneOverDirection.y;
      }
      else {
        return false;
      }
    }
    else {
      Time.y = 0.0f;
    }

    if (Start.z < Box.m_min.z) {
      bStartIsOutside = true;
      if (End.z >= Box.m_min.z) {
        Time.z = (Box.m_min.z - Start.z) * OneOverDirection.z;
      }
      else {
        return false;
      }
    }
    else if (Start.z > Box.m_max.z) {
      bStartIsOutside = true;
      if (End.z <= Box.m_max.z) {
        Time.z = (Box.m_max.z - Start.z) * OneOverDirection.z;
      }
      else {
        return false;
      }
    }
    else {
      Time.z = 0.0f;
    }

    if (bStartIsOutside) {
      const float	MaxTime = Math::max3(Time.x, Time.y, Time.z);

      if (MaxTime >= 0.0f && MaxTime <= 1.0f) {
        const Vector3 Hit = Start + Direction * MaxTime;
        const float BOX_SIDE_THRESHOLD = 0.1f;
        if (Hit.x > Box.m_min.x - BOX_SIDE_THRESHOLD &&
            Hit.x < Box.m_max.x + BOX_SIDE_THRESHOLD &&
            Hit.y > Box.m_min.y - BOX_SIDE_THRESHOLD &&
            Hit.y < Box.m_max.y + BOX_SIDE_THRESHOLD &&
            Hit.z > Box.m_min.z - BOX_SIDE_THRESHOLD &&
            Hit.z < Box.m_max.z + BOX_SIDE_THRESHOLD) {
          return true;
        }
      }
      return false;
    }
    else {
      return true;
    }
  }

  /**
   * @brief Performs a sphere vs box intersection test using Arvo's algorithm:
   *  for each i in (x, y, z)
   *    if(SphereCenter(i) < BoxMin(i)) d2+=(SphereCenter(i)-BoxMin(i))^2
   *		elif(SphereCenter(i) > BoxMax(i)) d2+=(SphereCenter(i)-BoxMax(i))^2
   * @param Sphere the center of the sphere being tested against the AABB
   * @param RadiusSquared the size of the sphere being tested
   * @param AABB the box being tested against
   * @return Whether the sphere/box intersect or not.
   */
  FORCEINLINE bool
  Math::sphereAABBIntersection(const Vector3& SphereCenter,
                               const float RadiusSquared,
                               const AABox& AABB) {
    //Accumulates the distance as we iterate axis
    float DistSquared = 0.f;

    //Check each axis for min/max and add the distance accordingly
    //NOTE: Loop manually unrolled for > 2x speed up
    if (SphereCenter.x < AABB.m_min.x) {
      DistSquared += Math::square(SphereCenter.x - AABB.m_min.x);
    }
    else if (SphereCenter.x > AABB.m_max.x) {
      DistSquared += Math::square(SphereCenter.x - AABB.m_max.x);
    }
    if (SphereCenter.y < AABB.m_min.y) {
      DistSquared += Math::square(SphereCenter.y - AABB.m_min.y);
    }
    else if (SphereCenter.y > AABB.m_max.y) {
      DistSquared += Math::square(SphereCenter.y - AABB.m_max.y);
    }
    if (SphereCenter.z < AABB.m_min.z) {
      DistSquared += Math::square(SphereCenter.z - AABB.m_min.z);
    }
    else if (SphereCenter.z > AABB.m_max.z) {
      DistSquared += Math::square(SphereCenter.z - AABB.m_max.z);
    }
    
    //If the distance is less than or equal to the radius, they intersect
    return DistSquared <= RadiusSquared;
  }
}
