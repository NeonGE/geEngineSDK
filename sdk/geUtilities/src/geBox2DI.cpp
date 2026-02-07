/*****************************************************************************/
/**
 * @file    geBox2DI.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/12
 * @brief   Implements a rectangular integer 2D Box.
 *
 * Implements a rectangular integer 2D Box.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geBox2DI.h"

#if USING(GE_REFLECTION)
# include "geRTTRMeta.h"
#endif

namespace geEngineSDK {
  Box2DI::Box2DI(const Vector2I* Points, const SIZE_T Count)
    : m_min(0, 0),
      m_max(0, 0),
      m_bIsValid(false) {
    for (SIZE_T PointItr = 0; PointItr < Count; ++PointItr) {
      *this += Points[PointItr];
    }
  }

  Box2DI::Box2DI(const Vector<Vector2I>& Points)
    : m_min(0, 0),
    m_max(0, 0),
    m_bIsValid(false) {
    for (const Vector2I& EachPoint : Points) {
      *this += EachPoint;
    }
  }

#if USING(GE_REFLECTION)
  RTTR_REGISTRATION
  {
    rttr::registration::class_<Box2DI>("Box2DI")
    .constructor<>()(
      metaScriptable(),
      metaTooltip("Default constructor with non-initialized values."),
      metaCategory("[Math]"))
    .constructor<const Vector2I&, const Vector2I&>()
      .constructor<const Vector2I&, const Vector2I&>()(
        metaScriptable(),
        metaTooltip("Constructor with initial values for min and max."),
        metaCategory("[Math]"))

    //Note: We don't register the constructors that take an array of points,
    //as they are not easily usable from scripting languages.
    //Properties
    .property("min", &Box2DI::m_min) (
      metaScriptable(),
      metaTooltip("Minimum point of the box."),
      metaCategory("[Math]"))
    .property("max", &Box2DI::m_max) (
      metaScriptable(),
      metaTooltip("Maximum point of the box."),
      metaCategory("[Math]"))

    //Operators
    .method("equals", &Box2DI::operator==)(
      metaScriptable(),
      metaTooltip("Checks if this box is equal to another box."),
      metaCategory("[Math]"))
    .method("addPoint",
      rttr::select_overload<Box2DI&(const Vector2I&)>(&Box2DI::operator+=))(
      metaScriptable(),
      metaTooltip("Expands this box to include a given point."),
      metaCategory("[Math]"))
    .method("addBox",
      rttr::select_overload<Box2DI&(const Box2DI&)>(&Box2DI::operator+=))(
      metaScriptable(),
      metaTooltip("Expands this box to include another box."),
      metaCategory("[Math]"))

    //Methods
    .method("computeSquaredDistanceToPoint", &Box2DI::computeSquaredDistanceToPoint)(
      metaScriptable(),
      metaTooltip("Computes the squared distance from a point to the box."),
      metaCategory("[Math]"))
    .method("expandBy", &Box2DI::expandBy)(
      metaScriptable(),
      metaTooltip("Expands the box by a given size."),
      metaCategory("[Math]"))
    .method("getArea", &Box2DI::getArea)(
      metaScriptable(),
      metaTooltip("Calculates the area of the box."),
      metaCategory("[Math]"))
    .method("getCenter", &Box2DI::getCenter)(
      metaScriptable(),
      metaTooltip("Gets the center point of the box."),
      metaCategory("[Math]"))
    .method("getSize", &Box2DI::getSize)(
      metaScriptable(),
      metaTooltip("Gets the size of the box."),
      metaCategory("[Math]"))
    .method("getClosestPointTo", &Box2DI::getClosestPointTo)(
      metaScriptable(),
      metaTooltip("Gets the closest point on the box to a given point."),
      metaCategory("[Math]"))
    .method("getExtent", &Box2DI::getExtent)(
      metaScriptable(),
      metaTooltip("Gets the extent (half-size) of the box."),
      metaCategory("[Math]"))
    .method("intersect", &Box2DI::intersect)(
      metaScriptable(),
      metaTooltip("Checks if this box intersects with another box."),
      metaCategory("[Math]"))
    .method("isInside",
      rttr::select_overload<bool(const Vector2I&) const>(&Box2DI::isInside))(
      metaScriptable(),
      metaTooltip("Checks if a point is inside the box."),
      metaCategory("[Math]"))
    .method("isInside",
      rttr::select_overload<bool(const Box2DI&) const>(&Box2DI::isInside))(
      metaScriptable(),
      metaTooltip("Checks if another box is entirely inside this box."),
      metaCategory("[Math]"))
    .method("isInsideOrOn", &Box2DI::isInsideOrOn)(
      metaScriptable(),
      metaTooltip("Checks if a point is inside or on the edge of the box."),
      metaCategory("[Math]"))
    .method("shiftBy", &Box2DI::shiftBy)(
      metaScriptable(),
      metaTooltip("Shifts the box by a given vector."),
      metaCategory("[Math]"))
    ;
  }

#endif // #if USING(GE_REFLECTION)

}//namespace geEngineSDK
