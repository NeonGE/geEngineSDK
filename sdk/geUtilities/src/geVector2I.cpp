/*****************************************************************************/
/**
 * @file    geVector2I.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/14
 * @brief   Structure for integer points in 2-d space.
 *
 * Structure for integer points in 2-d space.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geVector2I.h"

#if USING(GE_REFLECTION)
# include "geRTTRMeta.h"
#endif

namespace geEngineSDK {
  const Vector2I Vector2I::ZERO = Vector2I(0, 0);

#if USING(GE_REFLECTION)
  RTTR_REGISTRATION
  {
    using namespace rttr;
    registration::class_<Vector2I>("Vector2I")
      .constructor<>()(
        metaScriptable(),
        metaTooltip("Default constructor with non-initialized values."),
        metaCategory("[Math]"))

      .constructor<int32, int32>()(
        metaScriptable(),
        metaTooltip("Constructs and initializes a Vector2I from the given components."),
        metaCategory("[Math]"))

      //Properties
      .property("x", &Vector2I::x)(
        metaScriptable(),
        metaCategory("[Math]"),
        metaTooltip("X component"))

      .property("y", &Vector2I::y)(
        metaScriptable(),
        metaCategory("[Math]"),
        metaTooltip("Y component"))

      //Constants
      .property_readonly("ZERO", &Vector2I::ZERO)(
        metaScriptable(),
        metaReadOnly(),
        metaTooltip("Constant vector (0, 0)."),
        metaCategory("[Math]"))

      //Methods
      .method("size", &Vector2I::size)(
        metaScriptable(),
        metaTooltip("Calculates the size (magnitude) of this vector."),
        metaCategory("[Math]"))

      .method("sizeSquared", &Vector2I::sizeSquared)(
        metaScriptable(),
        metaTooltip("Calculates the squared size of this vector."),
        metaCategory("[Math]"))

      .method("dot", &Vector2I::dot)(
        metaScriptable(),
        metaTooltip("Calculates the dot (scalar) product of this vector with another."),
        metaCategory("[Math]"))

      .method("manhattanDistance", &Vector2I::manhattanDist)(
        metaScriptable(),
        metaTooltip("Calculates the Manhattan distance between this vector and another."),
        metaCategory("[Math]"))

      .method("getMin", &Vector2I::getMin)(
        metaScriptable(),
        metaTooltip("Gets the smaller of the vector's two components."),
        metaCategory("[Math]"))

      .method("getMax", &Vector2I::getMax)(
        metaScriptable(),
        metaTooltip("Gets the larger of the vector's two components."),
        metaCategory("[Math]"))

      //methods for operator overloads
      .method("add",
        rttr::select_overload<Vector2I(const Vector2I&) const>(&Vector2I::operator+))(
        metaScriptable(),
        metaTooltip("Adds two vectors component-wise."),
        metaCategory("[Math]"))

      .method("sub",
        rttr::select_overload<Vector2I(const Vector2I&) const>(&Vector2I::operator-))(
        metaScriptable(),
        metaTooltip("Subtracts two vectors component-wise."),
        metaCategory("[Math]"))

      .method("equals", &Vector2I::operator==)(
        metaScriptable(),
        metaTooltip("Compares two vectors for equality."),
        metaCategory("[Math]"))

    ;
  }
#endif // USING(GE_REFLECTION)
}
