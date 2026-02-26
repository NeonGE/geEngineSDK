/*****************************************************************************/
/**
 * @file    geDeferredCallManager.h
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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"
#include <geModule.h>

namespace geEngineSDK {
  using std::invoke_result_t;
  using std::is_void_v;
  using std::forward;
  using std::make_tuple;
  using std::apply;

  constexpr float DEFERRED_BUDGET_RATIO = 0.02f;  // 2% of a frame
  constexpr uint64 MIN_DEFERRED_BUDGET_US = 200;  // 200 micro seconds
  constexpr uint64 MAX_DEFERRED_BUDGET_US = 2000; // 2000 micro seconds

  class GE_CORE_EXPORT DeferredCallManager : public Module<DeferredCallManager>
  {
   public:
    using Task = function<void()>;

    DeferredCallManager() = default;
    ~DeferredCallManager();

    //Enque callable + args
    template<class F, class... Args>
    void
    queueDeferredCall(F&& f, Args&&... args) {
      using R = invoke_result_t<F, Args...>;
      static_assert(is_void_v<R>,
        "DeferredCallManager::queueDeferredCall requires a callable that returns void.");

      //Captures callable + args per value (moving when aplies).
      m_tasks.emplace_back(
        [fn = forward<F>(f), tup = make_tuple(forward<Args>(args)...)]() mutable
        {
          apply(std::move(fn), std::move(tup));
        }
      );
    }

    void
    update(float deltaTime);

    uint32
    pumpFor(uint64 budgetUs);

    void
    clear() {
      m_tasks.clear();
    }

    bool
    empty() const {
      return m_tasks.empty();
    }

    SIZE_T
    size() const {
      return m_tasks.size();
    }

   private:
    Vector<Task> m_tasks;
  };
}
