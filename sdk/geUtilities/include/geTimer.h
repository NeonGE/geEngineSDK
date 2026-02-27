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
 * 
 * @update  2026-02-26 Converted to a template so that we can create different
 *          types of timers
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
#include <type_traits>

namespace geEngineSDK {
  using namespace std::chrono;

  /**
   * @brief Template Timer class used for querying timers.
   */
  template<class ClockT>
  class BasicTimer
  {
   public:
    using Clock = ClockT;
    using TimePoint = typename Clock::time_point;

    BasicTimer() {
      reset();
    }

    void
    reset() {
      m_startTime = Clock::now();
    }

    uint64
    getMilliseconds() const {
      return cast::st<uint64>(duration_cast<milliseconds>(Clock::now() - m_startTime).count());
    }

    uint64
    getMicroseconds() const {
      return cast::st<uint64>(duration_cast<microseconds>(Clock::now() - m_startTime).count());
    }

    uint64
    getStartMs() const {
      // “start” relativo al epoch del clock (solo útil si lo comparas con el mismo Clock).
      return cast::st<uint64>(duration_cast<milliseconds>(
        m_startTime.time_since_epoch()).count());
    }

   private:
    TimePoint m_startTime;
  };

  using SteadyTimer = BasicTimer<steady_clock>; //For budgets, deltas, schedules
  using SystemTimer = BasicTimer<system_clock>; //For timestamps of "real time"
  using HighResOrSteadyClock =
    std::conditional_t<high_resolution_clock::is_steady, high_resolution_clock,
                    steady_clock>;
  using HighResTimer = BasicTimer<HighResOrSteadyClock>;

  //This keeps compatibility the default timer is steady
  using Timer = SteadyTimer;
}
