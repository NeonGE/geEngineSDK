/*****************************************************************************/
/**
 * @file    geThreadPool.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geThreadPool.h"
#include "geDebug.h"
#include "geMath.h"

#if USING(GE_PLATFORM_WINDOWS)
# include "Win32/geMinWindows.h"
# if USING(GE_COMPILER_MSVC)
  //disable: nonstandard extension used: 'X' uses SEH and 'Y' has destructor
  //We don't care about this as any exception is meant to crash the program.
#   pragma warning(disable: 4509)
# endif
#endif

namespace geEngineSDK {
  using std::bind;
  using std::function;
  using std::time;

  /**
   * The thread pool will check for unused threads every UNUSED_CHECK_PERIOD
   * getThread() calls
   */
  static CONSTEXPR int32 UNUSED_CHECK_PERIOD = 32;

  HThread::HThread(ThreadPool* pool, uint32 threadId)
    : m_threadId(threadId),
      m_pool(pool) {}

  void
  HThread::blockUntilComplete() {
    PooledThread* parentThread = nullptr;
    {//Scope for the Lock operation
      Lock lock(m_pool->m_mutex);

      for (auto& refThread : m_pool->m_threads) {
        if (refThread->getId() == m_threadId) {
          parentThread = refThread;
          break;
        }
      }
    }

    if (nullptr != parentThread) {
      Lock lock(parentThread->m_mutex);
      if (parentThread->m_id == m_threadId) { //Check again in case it changed
        while (!parentThread->m_idle) {
          parentThread->m_workerEndedCond.wait(lock);
        }
      }
    }
  }

  void
  PooledThread::initialize() {
    m_thread = ge_new<Thread>(bind(&PooledThread::run, this));
    Lock lock(m_mutex);

    while (!m_threadStarted) {
      m_startedCond.wait(lock);
    }
  }

  void
  PooledThread::start(const function<void()>& workerMethod, uint32 id) {
    {//Scope for the Lock operation
      Lock lock(m_mutex);

      m_workerMethod = workerMethod;
      m_idle = false;
      m_idleTime = time(nullptr);
      m_threadReady = true;
      m_id = id;
    }
    m_readyCond.notify_one();
  }

  void
  PooledThread::run() {
    onThreadStarted(m_name);
    
    {
      Lock lock(m_mutex);
      m_threadStarted = true;
    }

    m_startedCond.notify_one();

    while (true) {
      function<void()> worker = nullptr;
      
      {
        {
          Lock lock(m_mutex);

          while (!m_threadReady) {
            m_readyCond.wait(lock);
          }
          worker = m_workerMethod;
        }

        if (nullptr == worker) {
          onThreadEnded(m_name);
          return;
        }
      }

      workingMethodRun(worker);

      {
        Lock lock(m_mutex);

        m_idle = true;
        m_idleTime = time(nullptr);
        m_threadReady = false;
        m_workerMethod = nullptr; //Make sure to clear as it could have bound shared pointers

        m_workerEndedCond.notify_one();
      }
    }
  }

  void
  PooledThread::workingMethodRun(const function<void()>& worker) {
#if USING(GE_PLATFORM_WINDOWS)
    __try {
      worker();
    }
    __except (g_crashHandler().reportCrash(GetExceptionInformation())) {
      PlatformUtility::terminate(true);
    }
#else
    worker();
    GE_LOG(kWarning, Generic, "Starting a thread with no error handling.");
#endif
  }

  void
  PooledThread::destroy() {
    blockUntilComplete();
    {
      Lock lock(m_mutex);
      m_workerMethod = nullptr;
      m_threadReady = true;
    }
    m_readyCond.notify_one();
    m_thread->join();
    ge_delete(m_thread);
  }

  void
  PooledThread::blockUntilComplete() {
    Lock lock(m_mutex);
    while (!m_idle) {
      m_workerEndedCond.wait(lock);
    }
  }

  bool
  PooledThread::isIdle() {
    Lock lock(m_mutex);
    return m_idle;
  }

  time_t
  PooledThread::idleTime() {
    Lock lock(m_mutex);
    return (time(nullptr) - m_idleTime);
  }

  void
  PooledThread::setName(const String& name) {
    m_name = name;
  }

  uint32
  PooledThread::getId() const {
    Lock lock(m_mutex);
    return m_id;
  }

  ThreadPool::ThreadPool(SIZE_T threadCapacity, SIZE_T maxCapacity, uint32 idleTimeout)
    : m_defaultCapacity(threadCapacity),
      m_maxCapacity(maxCapacity),
      m_idleTimeout(idleTimeout)
  {}

  ThreadPool::~ThreadPool() {
    stopAll();
  }

  HThread
  ThreadPool::run(const String& name, const function<void()>& workerMethod) {
    PooledThread* pThread = getThread(name);
    pThread->start(workerMethod, ++m_uniqueId);
    return HThread(this, pThread->getId());
  }

  void
  ThreadPool::stopAll() {
    Lock lock(m_mutex);
    for (auto& myThread : m_threads) {
      destroyThread(myThread);
    }
    m_threads.clear();
  }

  void
  ThreadPool::clearUnused() {
    Lock lock(m_mutex);
    m_age = 0;

    if (m_threads.size() <= m_defaultCapacity) {
      return;
    }

    Vector<PooledThread*> idleThreads;
    Vector<PooledThread*> expiredThreads;
    Vector<PooledThread*> activeThreads;

    idleThreads.reserve(m_threads.size());
    expiredThreads.reserve(m_threads.size());
    activeThreads.reserve(m_threads.size());

    for (auto& pThread : m_threads) {
      if (pThread->isIdle()) {
        if (pThread->idleTime() >= m_idleTimeout) {
          expiredThreads.push_back(pThread);
        }
        else {
          idleThreads.push_back(pThread);
        }
      }
      else {
        activeThreads.push_back(pThread);
      }
    }

    idleThreads.insert(idleThreads.end(), expiredThreads.begin(), expiredThreads.end());
    SIZE_T limit = Math::min(idleThreads.size(), m_defaultCapacity);
    SIZE_T i = 0;
    m_threads.clear();

    for (auto& pThread : idleThreads) {
      if (i < limit) {
        m_threads.push_back(pThread);
      }
      else {
        destroyThread(pThread);
      }
    }

    m_threads.insert(m_threads.end(), activeThreads.begin(), activeThreads.end());
  }

  void
  ThreadPool::destroyThread(PooledThread* thread) {
    thread->destroy();
    ge_delete(thread);
  }

  PooledThread*
  ThreadPool::getThread(const String& name) {
    uint32 age = 0;
    {
      Lock lock(m_mutex);
      age = ++m_age;
    }

    if (UNUSED_CHECK_PERIOD == age) {
      clearUnused();
    }

    Lock lock(m_mutex);

    for (auto& pThread : m_threads) {
      if (pThread->isIdle()) {
        pThread->setName(name);
        return pThread;
      }
    }

    if (m_threads.size() >= m_maxCapacity) {
      GE_EXCEPT(InvalidStateException,
                "Unable to create a new thread in the pool because "          \
                "maximum capacity has been reached.");
    }

    PooledThread* newThread = createThread(name);
    m_threads.push_back(newThread);

    return newThread;
  }

  SIZE_T
  ThreadPool::getNumAvailable() const {
    SIZE_T numAvailable = m_maxCapacity;

    Lock lock(m_mutex);
    for (auto& pThread : m_threads) {
      if (!pThread->isIdle()) {
        --numAvailable;
      }
    }
    return numAvailable;
  }

  SIZE_T
  ThreadPool::getNumActive() const {
    SIZE_T numActive = 0;

    Lock lock(m_mutex);
    for (auto& pThread : m_threads) {
      if (!pThread->isIdle()) {
        ++numActive;
      }
    }
    return numActive;
  }

  SIZE_T
  ThreadPool::getNumAllocated() const {
    Lock lock(m_mutex);
    return m_threads.size();
  }
}
