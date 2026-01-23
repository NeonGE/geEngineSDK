/*****************************************************************************/
/**
 * @file    geThreading.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/09
 * @brief   Defines for mutex and thread utilities
 *
 * Here are the defines of the mutex and threads for the engine
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

/**
 * @brief Define the auto mutex name
 */
#define GE_AUTO_MUTEX_NAME mutex

/****************************************************************************/
/**
 * Includes
 */
/****************************************************************************/
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

namespace geEngineSDK {
  /**
   * @brief Returns the number of logical CPU cores.
   */
#define GE_THREAD_HARDWARE_CONCURRENCY std::thread::hardware_concurrency()

   /**
    * @brief Returns the ThreadId of the current thread.
    */
#define GE_THREAD_CURRENT_ID std::this_thread::get_id()

    /**
     * @brief Causes the current thread to sleep for the provided amount of
     *        milliseconds.
     */
#define GE_THREAD_SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms));

     /**
      * @brief Wrapper for the C++ std::mutex.
      */
  using Mutex = std::mutex;

  /**
   * @brief Wrapper for the C++ std::recursive_mutex.
   */
  using RecursiveMutex = std::recursive_mutex;

  /**
   * @brief Wrapper for the C++ std::condition_variable.
   */
  using Signal = std::condition_variable;

  /**
   * @brief Wrapper for the C++ std::thread.
   */
  using Thread = std::thread;

  /**
   * @brief Wrapper for the C++ std::thread::id.
   */
  using ThreadId = std::thread::id;

  /**
   * @brief Wrapper for the C++ std::unique_lock<std::mutex>.
   */
  using Lock = std::unique_lock<Mutex>;

  /**
   * @brief Wrapper for the C++ std::unique_lock<std::recursive_mutex>.
   */
  using RecursiveLock = std::unique_lock<RecursiveMutex>;

  /**
   * @brief Policy that allows the calls its used in to pick between no locking
   *        and mutex locking through a template parameter.
   */
  template<bool LOCK>
  class LockingPolicy
  {};

  /**
   * @brief Scoped lock that provides RAII-style locking and accepts both a
   *        normal mutex and a locking policy as input.
   */
  template<bool LOCK>
  class ScopedLock
  {};

  /**
   * @brief Specialization of LockingPolicy that performs no locking.
   */
  template<>
  class LockingPolicy<false> final
  {
   public:
    LockingPolicy() = default;

    void
    lock() {}

    void
    unlock() {}
  };

  /**
   * @brief Specialization of LockingPolicy that uses a mutex for locking.
   */
  template<>
  class LockingPolicy<true> final
  {
   public:
    LockingPolicy() : m_lock(m_mutex, std::defer_lock) {}

    void
    lock() {
      m_lock.lock();
    };

    void
    unlock() {
      //Only unlock if the lock is currently held
      if (m_lock.owns_lock()) {
        m_lock.unlock();
      }
    }

   private:
    friend class ScopedLock<true>;

    Mutex m_mutex;
    Lock m_lock;
  };

  /**
   * @brief Scoped lock that performs no locking internally.
   *        Can only be used with a LockingPolicy.
   */
  template<>
  class ScopedLock<false>
  {
   public:
     explicit ScopedLock(LockingPolicy<false>& /*policy*/) {}
  };

  /**
   * @brief Scoped lock that automatically locks when created and unlocks when it goes out of scope.
   */
  template<>
  class ScopedLock<true>
  {
   public:
    explicit ScopedLock(LockingPolicy<true>& policy) : m_lockGuard(policy.m_mutex) {}

    explicit ScopedLock(Mutex& mutex) : m_lockGuard(mutex) {}

   private:
    std::lock_guard<Mutex> m_lockGuard;
  };
}
