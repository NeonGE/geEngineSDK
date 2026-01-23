/*****************************************************************************/
/**
 * @file    geTimer.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/18
 * @brief   Timer class used for querying high precision timers.
 *
 * Timer class used for querying high precision timers.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geTimer.h"

namespace geEngineSDK {
  using namespace std::chrono;

  Timer::Timer() {
    reset();
  }

  void
  Timer::reset() {
    m_startTime = m_highResClock.now();
  }

  uint64
  Timer::getMilliseconds() const {
    auto newTime = m_highResClock.now();
    duration<double> dur = newTime - m_startTime;
    return duration_cast<milliseconds>(dur).count();
  }

  uint64
  Timer::getMicroseconds() const {
    auto newTime = m_highResClock.now();
    duration<double> dur = newTime - m_startTime;
    return duration_cast<microseconds>(dur).count();
  }

  uint64
  Timer::getStartMs() const {
    nanoseconds startTimeNs = m_startTime.time_since_epoch();
    return duration_cast<milliseconds>(startTimeNs).count();
  }
}
