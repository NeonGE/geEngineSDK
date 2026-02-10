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
    const auto newTime = m_highResClock.now();
    const auto dur = newTime - m_startTime;
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    GE_ASSERT(ms >= 0);
    return static_cast<uint64>(ms);
  }

  uint64
  Timer::getMicroseconds() const {
    const auto newTime = m_highResClock.now();
    const auto dur = newTime - m_startTime;
    const auto us = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
    GE_ASSERT(us >= 0);
    return static_cast<uint64>(us);
  }

  uint64
  Timer::getStartMs() const {
    const auto startNs = m_startTime.time_since_epoch();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(startNs).count();
    GE_ASSERT(ms >= 0);
    return static_cast<uint64>(ms);
  }
}
