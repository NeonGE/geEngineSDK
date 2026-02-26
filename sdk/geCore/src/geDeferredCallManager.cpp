/*****************************************************************************/
/**
 * @file    geDeferredCallManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/02/25
 * @brief   Module used to send deferred calls.
 *
 * Module used to send deferred calls (function calls that will wait until the
 * next frame
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geDeferredCallManager.h"
#include <geMath.h>
#include <geTimer.h>

namespace geEngineSDK {
  DeferredCallManager::~DeferredCallManager() {
    clear();
  }

  void
  DeferredCallManager::update(float deltaTime) {
    uint64 budgetUs = Math::clamp(cast::st<uint64>(deltaTime *
                                    DEFERRED_BUDGET_RATIO * 1000.0f),
                                  MIN_DEFERRED_BUDGET_US,
                                  MAX_DEFERRED_BUDGET_US);
    pumpFor(budgetUs);
  }

  uint32
  DeferredCallManager::pumpFor(uint64 budgetUs) {
    uint32 executed = 0;
    Timer timer;

    while (!m_tasks.empty()) {
      if (timer.getMicroseconds() >= budgetUs) {
        break;
      }

      auto t = std::move(m_tasks.back());
      m_tasks.pop_back();
      t();
      ++executed;
    }

    return executed;
  }
}
