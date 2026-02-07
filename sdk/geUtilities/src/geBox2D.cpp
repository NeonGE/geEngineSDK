/*****************************************************************************/
/**
 * @file    geBox2D.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/29
 * @brief   Implements a rectangular 2D Box.
 *
 * Implements a rectangular 2D Box.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geBox2D.h"

#if USING(GE_REFLECTION)
# include "geRTTRMeta.h"
#endif

namespace geEngineSDK {
  Box2D::Box2D(const Vector2* Points, const SIZE_T Count)
    : m_min(0.f, 0.f),
      m_max(0.f, 0.f),
      m_bIsValid(false) {
    for (SIZE_T PointItr = 0; PointItr < Count; ++PointItr) {
      *this += Points[PointItr];
    }
  }

  Box2D::Box2D(const Vector<Vector2>& Points)
    : m_min(0.f, 0.f),
    m_max(0.f, 0.f),
    m_bIsValid(false) {
    for (const Vector2& EachPoint : Points) {
      *this += EachPoint;
    }
  }

#if USING(GE_REFLECTION)
  RTTR_REGISTRATION
  {
    rttr::registration::class_<Box2D>("Box2D")
    .constructor<>()(
      metaScriptable(),
      metaTooltip("Default constructor with non-initialized values."),
      metaCategory("[Math]"))

    .constructor<const Vector2&, const Vector2&>()
      .constructor<const Vector2&, const Vector2&>()(
        metaScriptable(),
        metaTooltip("Constructor with initial values for min and max."),
        metaCategory("[Math]"))

    //Note: We don't register the constructors that take an array of points,
    //as they are not easily usable from scripting languages.
    .property("min", &Box2D::m_min)(
      metaScriptable(),
      metaTooltip("Minimum point of the box."),
      metaCategory("[Math]"))

    .property("max", &Box2D::m_max)(
      metaScriptable(),
      metaTooltip("Maximum point of the box."),
      metaCategory("[Math]"))

    //Methods
    .method("computeSquaredDistanceToPoint", &Box2D::computeSquaredDistanceToPoint)(
      metaScriptable(),
      metaTooltip("Computes the squared distance from a point to the box."),
      metaCategory("[Math]"))
    .method("expandBy", &Box2D::expandBy)(
      metaScriptable(),
      metaTooltip("Expands the box by a given size."),
      metaCategory("[Math]"))
    .method("getArea", &Box2D::getArea)(
      metaScriptable(),
      metaTooltip("Calculates the area of the box."),
      metaCategory("[Math]"))
    .method("getCenter", &Box2D::getCenter)(
      metaScriptable(),
      metaTooltip("Gets the center point of the box."),
      metaCategory("[Math]"))
    .method("getSize", &Box2D::getSize)(
      metaScriptable(),
      metaTooltip("Gets the size of the box."),
      metaCategory("[Math]"))
    .method("getClosestPointTo", &Box2D::getClosestPointTo)(
      metaScriptable(),
      metaTooltip("Gets the closest point on the box to a given point."),
      metaCategory("[Math]"))
    .method("shiftBy", &Box2D::shiftBy)(
      metaScriptable(),
      metaTooltip("Shifts the box by a given vector."),
      metaCategory("[Math]"))
    .method("getExtent", &Box2D::getExtent)(
      metaScriptable(),
      metaTooltip("Gets the extent (half-size) of the box."),
      metaCategory("[Math]"))
    .method("intersect", &Box2D::intersect)(
      metaScriptable(),
      metaTooltip("Checks if this box intersects with another box."),
      metaCategory("[Math]"))
    .method("isInside",
      rttr::select_overload<bool(const Vector2&) const>(&Box2D::isInside))(
      metaScriptable(),
      metaTooltip("Checks if a point is inside the box."),
      metaCategory("[Math]"))
    .method("isInside",
      rttr::select_overload<bool(const Box2D&) const>(&Box2D::isInside))(
      metaScriptable(),
      metaTooltip("Checks if another box is entirely inside this box."),
      metaCategory("[Math]"))
    ;
  }
#endif
}
