/*****************************************************************************/
/**
 * @file    geFloat16Color.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/05/05
 * @brief   RGBA Color made up of Float16.
 *
 * Class to use color on a reduced memory and precision ambient
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geFloat16.h"
#include "geColor.h"

namespace geEngineSDK {
  class LinearColor;

  /**
   * @brief RGBA Color made up of Float16.
   */
  class GE_UTILITIES_EXPORT Float16Color
  {
   public:
    Float16Color() {}
    Float16Color(const Float16Color& Src) : r(Src.r), g(Src.g), b(Src.b), a(Src.a) {}
    explicit Float16Color(const LinearColor& Src) : r(Src.r), g(Src.g), b(Src.b), a(Src.a) {}

    /**
     * @brief	Assignment operator
     */
    Float16Color&
    operator=(const Float16Color& Src) {
      r = Src.r;
      g = Src.g;
      b = Src.b;
      a = Src.a;
      return *this;
    }

    /**
     * @brief Checks whether two colors are identical.
     * @param Src The other color.
     * @return true if the two colors are identical, otherwise false.
     */
    bool
    operator==(const Float16Color& Src) const {
      return ((r == Src.r) && (g == Src.g) && (b == Src.b) && (a == Src.a));
    }

   public:
    Float16 r;
    Float16 g;
    Float16 b;
    Float16 a;
  };
}
