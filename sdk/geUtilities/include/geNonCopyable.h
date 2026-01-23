/*****************************************************************************/
/**
 * @file    geNonCopyable.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/06/03
 * @brief   Interface to prevent copies any type that implements it.
 *
 * Interface to prevent copies be made of any type that implements it.
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

namespace geEngineSDK {
  /**
   * @brief Interface that prevents copies be made of any type that implements it.
   */
  class INonCopyable
  {
   protected:
    INonCopyable() = default;
    ~INonCopyable() = default;

   private:
    INonCopyable(const INonCopyable&) = delete;
    INonCopyable& operator=(const INonCopyable&) = delete;
  };
}
