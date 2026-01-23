/*****************************************************************************/
/**
 * @file    geThreadPool.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/06/05
 * @brief   Class that maintains a pool of threads we can easily use.
 *
 * Class that maintains a pool of threads we can easily retrieve and use for
 * any task. This saves on the cost of creating and destroying threads.
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
#include "geModule.h"

namespace geEngineSDK {
  using std::function;
  using std::atomic_uint;

  class ThreadPool;

  /**
   * @brief Handle to a thread managed by ThreadPool.
   */
  class GE_UTILITIES_EXPORT HThread
  {
   public:
    HThread() = default;
    HThread(ThreadPool* pool, uint32 threadId);

    /**
     * @brief Block the calling thread until the thread this handle points to completes.
     */
    void
    blockUntilComplete();

   private:
    uint32 m_threadId = 0;
    ThreadPool* m_pool = nullptr;
  };

  /**
   * @brief Wrapper around a thread that is used within ThreadPool.
   */
  class GE_UTILITIES_EXPORT PooledThread
  {
   public:
    PooledThread(const String& name) : m_name(name) {}
    virtual ~PooledThread() = default;

    /**
     * @brief Initializes the pooled thread. Must be called right after the
     *        object is constructed.
     */
    void
    initialize();

    /**
     * @brief Starts executing the given worker method.
     * @note  Caller must ensure worker method is not null and that the thread
     *        is currently idle, otherwise undefined behavior will occur.
     */
    void
    start(const function<void()>& workerMethod, uint32 id);

    /**
     * @brief Attempts to join the currently running thread and destroys it.
     *        Caller must ensure that any worker method currently running
     *        properly returns, otherwise this will block indefinitely.
     */
    void
    destroy();

    /**
     * @brief Returns true if the thread is idle and new worker method can be
     *        scheduled on it.
     */
    bool
    isIdle();

    /**
     * @brief Returns how long has the thread been idle. Value is undefined if
     *        thread is not idle.
     */
    time_t
    idleTime();

    /**
     * @brief Sets a name of the thread.
     */
    void
    setName(const String& name);

    /**
     * @brief Gets unique ID of the currently executing thread.
     */
    uint32
    getId() const;

    /**
     * @brief Blocks the current thread until this thread completes.
     *        Returns immediately if the thread is idle.
     */
    void
    blockUntilComplete();

    /**
     * @brief Called when the thread is first created.
     */
    virtual void
    onThreadStarted(const String& name) = 0;

    /**
     * @brief Called when the thread is being shut down.
     */
    virtual void
    onThreadEnded(const String& name) = 0;

   protected:
    friend class HThread;

    /**
     * @brief Primary worker method that is ran when the thread is first initialized.
     */
    void
    run();

   private:
    /**
     * @brief Calls the worker method (separated to fix the use of __try __except)
     */
    void
    workingMethodRun(const function<void()>& worker);

   protected:
    function<void()> m_workerMethod;

    String m_name;
    uint32 m_id = 0;
    bool m_idle = true;
    bool m_threadStarted = false;
    bool m_threadReady = false;

    time_t m_idleTime = 0;

    Thread* m_thread = nullptr;
    mutable Mutex m_mutex;

    Signal m_startedCond;
    Signal m_readyCond;
    Signal m_workerEndedCond;
  };

  /**
   * @copydoc PooledThread
   * @tparam  ThreadPolicy Allows you specify a policy with methods that will
   *          get called whenever a new thread is created or when a thread is
   *          destroyed.
   */
  template<class ThreadPolicy>
  class TPooledThread : public PooledThread
  {
   public:
    using PooledThread::PooledThread;

    /**
     * @copydoc PooledThread::onThreadStarted
     */
    void
    onThreadStarted(const String& name) override {
      ThreadPolicy::onThreadStarted(name);
    }

    /**
     * @copydoc PooledThread::onThreadEnded
     */
    void
    onThreadEnded(const String& name) override {
      ThreadPolicy::onThreadEnded(name);
    }
  };

  /**
   * @brief Class that maintains a pool of threads we can easily retrieve and use
   *        for any task. This saves on the cost of creating and destroying threads.
   */
  class GE_UTILITIES_EXPORT ThreadPool : public Module<ThreadPool>
  {
   public:
    /**
     * @brief Constructs a new thread pool
     * @param[in] threadCapacity  Default thread capacity, the pool will always
     *            try to keep this many threads available.
     * @param[in] maxCapacity (optional) Maximum number of threads the pool can
     *            create. If we go over this limit an exception will be thrown.
     * @param[in] idleTimeout (optional) How many seconds do threads need to be
     *            idle before we remove them from the pool.
     */
    ThreadPool(SIZE_T threadCapacity, SIZE_T maxCapacity = 16, uint32 idleTimeout = 60);
    virtual ~ThreadPool();

    /**
     * @brief Find an unused thread (or creates a new one) and runs the
     *        specified worker method on it.
     * @param[in] name  A name you may use for more easily identifying the thread.
     * @param[in] workerMethod  The worker method to be called by the thread.
     * @return  A thread handle you may use for monitoring the thread execution.
     */
    HThread
    run(const String& name, const function<void()>& workerMethod);

    /**
     * @brief Stops all threads and destroys them. Caller must ensure each
     *        threads worker method returns otherwise this will never return.
     */
    void
    stopAll();

    /**
     * @brief Clear any unused threads that are over the capacity.
     */
    void
    clearUnused();

    /**
     * @brief Returns the number of unused threads in the pool.
     */
    SIZE_T
    getNumAvailable() const;

    /**
     * @brief Returns the number of running threads in the pool.
     */
    SIZE_T
    getNumActive() const;

    /**
     * @brief Returns the total number of created threads in the pool
     *        (both running and unused).
     */
    SIZE_T
    getNumAllocated() const;

   protected:
    friend class HThread;

    /**
     * @brief Creates a new thread to be used by the pool.
     */
    virtual PooledThread*
    createThread(const String& name) = 0;

    /**
     * @brief Destroys the specified thread. Caller needs to make sure the
     *        thread is actually shut down beforehand.
     */
    void
    destroyThread(PooledThread* thread);

    /**
     * @brief Returns the first unused thread if one exists, otherwise creates a new one.
     * @param[in] name  Name to assign the thread.
     * @note  Throws an exception if we have reached our maximum thread capacity.
     */
    PooledThread*
    getThread(const String& name);

    Vector<PooledThread*> m_threads;
    SIZE_T m_defaultCapacity;
    SIZE_T m_maxCapacity;
    uint32 m_idleTimeout;

    /**
     * Unused check counter
     */
    uint32 m_age = 0;

    atomic_uint m_uniqueId;
    mutable Mutex m_mutex;
  };

  /**
   * @brief Policy used for thread start & end used by the ThreadPool.
   */
  class ThreadNoPolicy
  {
   public:
    static void
    onThreadStarted(const String&) {}
    
    static void
    onThreadEnded(const String&) {}
  };

  /**
   * @copydoc ThreadPool
   * @tparam  ThreadPolicy Allows you specify a policy with methods that will
   *          get called whenever a new thread is created or when a thread is
   *          destroyed.
   */
  template<class ThreadPolicy = ThreadNoPolicy>
  class TThreadPool : public ThreadPool
  {
   public:
    TThreadPool(uint32 threadCapacity, uint32 maxCapacity = 16, uint32 idleTimeout = 60)
      : ThreadPool(threadCapacity, maxCapacity, idleTimeout) {}

   protected:
    /**
     * @copydoc ThreadPool::createThread
     */
    PooledThread*
    createThread(const String& name) override {
      PooledThread* newThread = ge_new<TPooledThread<ThreadPolicy>>(name);
      newThread->initialize();
      return newThread;
    }
  };
}
