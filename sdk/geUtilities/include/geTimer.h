/*****************************************************************************/
/**
 * @file    geTimer.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/18
 * @brief   Timer class used for querying high precision timers.
 *
 * Timer class used for querying high precision timers.
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
#include <chrono>

namespace geEngineSDK {
  using std::chrono::high_resolution_clock;
  using std::chrono::time_point;

  /**
   * @brief Timer class used for querying high precision timers.
   */
  class GE_UTILITIES_EXPORT Timer
  {
   public:
    /**
     * @brief Construct the timer and start timing.
     */
    Timer();

    /**
     * @brief Reset the timer to zero.
     */
    void
    reset();

    /**
     * @brief Returns time in milliseconds since timer was initialized or last reset.
     */
    uint64
    getMilliseconds() const;

    /**
     * @brief Returns time in microseconds since timer was initialized or last reset.
     */
    uint64
    getMicroseconds() const;

    /**
     * @brief Returns the time at which the timer was initialized, in milliseconds.
     * @return  Time in milliseconds.
     */
    uint64
    getStartMs() const;

   private:
    high_resolution_clock m_highResClock;
    time_point<high_resolution_clock> m_startTime;
  };
}
