/*****************************************************************************/
/**
 * @file    geFrameAlloc.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/06
 * @brief   Frame allocator
 *
 * Performs very fast allocations but can only free all of its memory at once.
 * Perfect for allocations that last just a single frame.
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
#include <new>

#include "gePlatformDefines.h"
#include "gePlatformTypes.h"
#include "geNumericLimits.h"
#include "geStdHeaders.h"
#include "geThreading.h"

namespace geEngineSDK {
  using std::forward;
  using std::atomic;
  using std::size_t;
  using std::ptrdiff_t;
  using std::basic_string;
  using std::char_traits;

  using std::vector;
  using std::stack;
  using std::queue;
  using std::deque;
  using std::map;
  using std::hash;
  using std::less;
  using std::equal_to;
  using std::set;
  using std::pair;
  using std::unordered_set;
  using std::unordered_map;

  /**
   * @brief Frame allocator. Performs very fast allocations but can only free all of its
   *        memory at once. Perfect for allocations that last just a single frame.
   * @note  Not thread safe with an exception. alloc() and clear() methods need to be called
   *        from the same thread. Dealloc() is thread safe and can be called from any thread.
   */
  class GE_UTILITIES_EXPORT FrameAlloc
  {
   private:
    /**
     * @brief A single block of memory within a frame allocator.
     */
    class MemBlock
    {
     public:
      explicit MemBlock(SIZE_T size) : m_size(size) {}
      ~MemBlock() = default;

      /**
       * @brief Allocates a piece of memory within the block. Caller must
       *        ensure the block has enough empty space.
       */
      byte*
      alloc(SIZE_T amount);

      /**
       * @brief Releases all allocations within a block but doesn't actually free the memory.
       */
      void
      clear();

     public:
      byte* m_data = nullptr;
      SIZE_T m_freePtr = 0;
      SIZE_T m_size;
    };

   public:
    explicit FrameAlloc(SIZE_T blockSize = 1024 * 1024);
    ~FrameAlloc();

    /**
     * @brief Allocates a new block of memory of the specified size.
     * @param[in] amount  Amount of memory to allocate, in bytes.
     * @note  Not thread safe.
     */
    byte*
    alloc(SIZE_T amount);

    /**
     * @brief Allocates a new block of memory of the specified size aligned to
     *        the specified boundary. If the alignment is less or equal to 16 it is
     *        more efficient to use the allocAligned16() alternative of this method.
     * @param[in] amount  Amount of memory to allocate, in bytes.
     * @param[in] alignment Alignment of the allocated memory. Must be power of two.
     * @note  Not thread safe.
     */
    byte*
    allocAligned(SIZE_T amount, SIZE_T alignment);

    /**
     * @brief Allocates and constructs a new object.
     * @note  Not thread safe.
     */
    template<class T, class... Args>
    T*
    construct(Args &&...args) {
      return new (reinterpret_cast<T*>(alloc(sizeof(T)))) T(forward<Args>(args)...);
    }

    /**
     * @brief Destructs and deallocates an object.
     * @note  Not thread safe.
     */
    template<class T>
    void
    destruct(T* data) {
      data->~T();
      free(reinterpret_cast<byte*>(data));
    }

    /**
     * @brief Deallocates a previously allocated block of memory.
     * @note  No deallocation is actually done here. This method is only used
     *        for debug purposes so it is easier to track down memory leaks and
     *        corruption.
     * @note  Thread safe.
     */
    void
    free(byte* data);

    /**
     * @brief Deallocates and destructs a previously allocated object.
     * @note  No deallocation is actually done here. This method is only used
     *        to call the destructor and for debug purposes so it is easier to 
     *        track down memory leaks and corruption.
     * @note  Thread safe.
     */
    template<class T>
    void
    free(T* obj) {
      if (nullptr != obj) {
        obj->~T();
      }
      free(reinterpret_cast<byte*>(obj));
    }

    /**
     * @brief Starts a new frame. Next call to ::Clear will only clear memory
     *        allocated past this point.
     */
    void
    markFrame();

    /**
     * @brief Deallocates all allocated memory since the last call to markFrame()
     *        (or all the memory if there was no call to markFrame()).
     * @note  Not thread safe.
     */
    void
    clear();

    /**
     * @brief Changes the frame allocator owner thread. After the owner thread
     *        has changed only allocations from that thread can be made.
     */
    void
    setOwnerThread(ThreadId thread);

   private:
    /**
     * @brief Allocates a dynamic block of memory of the wanted size. The exact
     *        allocation size might be slightly higher in order to store block
     *        meta data.
     */
    MemBlock*
    allocBlock(SIZE_T wantedSize);

    /**
    * @brief Frees a memory block.
    */
    void
    deallocBlock(MemBlock* block);

   private:
    SIZE_T m_blockSize;
    Vector<MemBlock*> m_blocks;
    MemBlock* m_freeBlock;
    uint32 m_nextBlockIdx;
    atomic<SIZE_T> m_totalAllocBytes;
    void* m_lastFrame;

#if USING(GE_DEBUG_MODE)
    ThreadId m_ownerThread;
#endif
  };

  /**
   * @brief Version of FrameAlloc that allows blocks size to be provided
   *        through the template argument instead of the constructor.
   */
  template<int BlockSize>
  class TFrameAlloc : public FrameAlloc
  {
   public:
    TFrameAlloc() : FrameAlloc(BlockSize) {}
  };

  /**
   * @brief Allocator for the standard library that internally uses a frame
   *        allocator.
   */
  template <class T>
  class StdFrameAlloc
  {
   public:
    using value_type = T;
    using pointer = value_type * ;
    using const_pointer = const value_type*;
    using reference = value_type & ;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    StdFrameAlloc() _NOEXCEPT = default;
    explicit StdFrameAlloc(FrameAlloc* pAlloc) _NOEXCEPT : m_FrameAlloc(pAlloc) {}

    template<class U>
    StdFrameAlloc(const StdFrameAlloc<U>& refAlloc) _NOEXCEPT
      : m_FrameAlloc(refAlloc.m_FrameAlloc) {
    }

    template<class U>
    bool
    operator==(const StdFrameAlloc<U>&) const _NOEXCEPT {
      return true;
    }

    template<class U>
    bool
    operator!=(const StdFrameAlloc<U>&) const _NOEXCEPT {
      return false;
    }

    template<class U>
    class rebind
    {
     public:
      using other = StdFrameAlloc<U>;
    };

    /**
     * @brief Allocate but don't initialize number elements of type T.
     */
    T*
    allocate(const size_t num) const {
      if (0 == num) {
        return nullptr;
      }

      if (num > (NumLimit::MAX_SIZET / sizeof(T))){
        return nullptr; //Error
      }

      void* const pv = m_FrameAlloc->alloc(num * sizeof(T));
      if (!pv) {
        return nullptr; //Error
      }

      return static_cast<T*>(pv);
    }

    /**
     * @brief Deallocate storage p of deleted elements.
     */
    void
    deallocate(T* p, size_t) const _NOEXCEPT {
      m_FrameAlloc->free(reinterpret_cast<byte*>(p));
    }

   public:
    FrameAlloc* m_FrameAlloc = nullptr;

    size_t
    max_size() const {
      return NumLimit::MAX_SIZET / sizeof(T);
    }

    void
    construct(pointer p, const_reference t) {
      new (p) T(t);
    }
    
    void
    destroy(pointer p) {
      p->~T();
    }

    template<class U, class... Args>
    void
    construct(U* p, Args&& ...args) {
      new(p) U(forward<Args>(args)...);
    }
  };

  /**
   * @brief Return that all specializations of this allocator are interchangeable.
   */
  template <class T1, class T2>
  bool
  operator==(const StdFrameAlloc<T1>&, const StdFrameAlloc<T2>&) throw() {
    return true;
  }

  /**
   * @brief Return that all specializations of this allocator are interchangeable.
   */
  template <class T1, class T2>
  bool
  operator!=(const StdFrameAlloc<T1>&, const StdFrameAlloc<T2>&) throw() {
    return false;
  }

  /**
   * @brief Returns a global, application wide FrameAlloc. Each thread gets its
   *        own frame allocator.
   * @note  Thread safe.
   */
  GE_UTILITIES_EXPORT FrameAlloc&
  g_frameAlloc();

  /**
   * @brief Allocates some memory using the global frame allocator.
   * @param[in] numBytes  Number of bytes to allocate.
   */
  GE_UTILITIES_EXPORT byte*
  ge_frame_alloc(SIZE_T numBytes);

  /**
   * @brief Allocates the specified number of bytes aligned to the provided
   *        boundary, using the global frame allocator. Boundary is in bytes
   *        and must be a power of two.
   */
  GE_UTILITIES_EXPORT byte*
  ge_frame_alloc_aligned(SIZE_T count, SIZE_T align);

  /**
   * @brief Deallocates memory allocated with the global frame allocator.
   * @note  Must be called on the same thread the memory was allocated on.
   */
  GE_UTILITIES_EXPORT void
  ge_frame_free(void* data);

  /**
   * @brief Frees memory previously allocated with ge_frame_alloc_aligned().
   * @note  Must be called on the same thread the memory was allocated on.
   */
  GE_UTILITIES_EXPORT void
  ge_frame_free_aligned(void* data);

  /**
   * @brief Allocates enough memory to hold the object of specified type using
   *        the global frame allocator, but does not construct the object.
   */
  template<class T>
  T*
  ge_frame_alloc() {
    return reinterpret_cast<T*>(ge_frame_alloc(sizeof(T)));
  }

  /**
   * @brief Allocates enough memory to hold N objects of specified type using
   *        the global frame allocator, but does not construct the object.
   */
  template<class T>
  T*
  ge_frame_alloc(SIZE_T count) {
    return reinterpret_cast<T*>(ge_frame_alloc(sizeof(T) * count));
  }

  /**
   * @brief Allocates enough memory to hold the object(s) of specified type
   *        using the global frame allocator, and constructs them.
   */
  template<class T>
  T*
  ge_frame_new(SIZE_T count = 0) {
    T* data = ge_frame_alloc<T>(count);

    for (SIZE_T i = 0; i < count; ++i) {
      new ((void*)&data[i]) T;
    }
    return data;
  }

  /**
   * @brief Allocates enough memory to hold the object(s) of specified type
   *        using the global frame allocator, and constructs them.
   */
  template<class T, class... Args>
  T*
  ge_frame_new(Args &&...args, SIZE_T count = 0) {
    T* data = ge_frame_alloc<T>(count);

    for (SIZE_T i = 0; i < count; ++i) {
      new ((void*)&data[i]) T(forward<Args>(args)...);
    }
    return data;
  }

  /**
   * @brief Destructs and deallocates an object allocated with the global frame
   *        allocator.
   * @note  Must be called on the same thread the memory was allocated on.
   */
  template<class T>
  void
  ge_frame_delete(T* data) {
    data->~T();
    ge_frame_free(reinterpret_cast<byte*>(data));
  }

  /**
   * @brief Destructs and deallocates an array of objects allocated with the
   *        global frame allocator.
   * @note  Must be called on the same thread the memory was allocated on.
   */
  template<class T>
  void
  ge_frame_delete(T* data, SIZE_T count) {
    for (SIZE_T i = 0; i < count; ++i) {
      data[i].~T();
    }
    ge_frame_free(reinterpret_cast<byte*>(data));
  }

  /**
   * @copydoc FrameAlloc::markFrame
   */
  GE_UTILITIES_EXPORT void
  ge_frame_mark();

  /**
   * @copydoc FrameAlloc::clear
   */
  GE_UTILITIES_EXPORT void
  ge_frame_clear();

  /**
   * @brief String allocated with a frame allocator.
   */
  typedef basic_string<ANSICHAR,
                       char_traits<ANSICHAR>,
                       StdAlloc<ANSICHAR, FrameAlloc>>
                       FrameString;

  /**
   * @brief WString allocated with a frame allocator.
   */
  typedef basic_string<UNICHAR,
                       char_traits<UNICHAR>,
                       StdAlloc<UNICHAR, FrameAlloc>>
                       FrameWString;

  template<typename T, typename A = StdAlloc<T, FrameAlloc>>
  using FrameVector = vector<T, A>;

  template<typename T, typename A = StdAlloc<T, FrameAlloc>>
  using FrameStack = stack<T, deque<T, A>>;

  template <typename T, typename A = StdAlloc<T, FrameAlloc>>
  using FrameQueue = queue<T, deque<T, A>>;

  template<typename T, typename P = less<T>, typename A = StdAlloc<T, FrameAlloc>>
  using FrameSet = set<T, P, A>;

  template<typename K, 
           typename V,
           typename P = less<K>,
           typename A = StdAlloc<pair<const K, V>, FrameAlloc>>
  using FrameMap = map<K, V, P, A>;

  template<typename T,
           typename H = hash<T>,
           typename C = equal_to<T>,
           typename A = StdAlloc<T, FrameAlloc>>
  using FrameUnorderedSet = unordered_set<T, H, C, A>;

  template<typename K,
           typename V,
           typename H = hash<K>,
           typename C = equal_to<K>,
           typename A = StdAlloc<pair<const K, V>, FrameAlloc>>
  using FrameUnorderedMap = unordered_map<K, V, H, C, A>;

  extern GE_THREADLOCAL FrameAlloc* _globalFrameAlloc;

  /**
   * @brief Specialized memory allocator implementations that allows use of a
   *        global frame allocator in normal new/delete/free/dealloc operators.
   */
  template<>
  class MemoryAllocator<FrameAlloc> : public MemoryAllocatorBase
  {
  public:
    /**
     * @copydoc MemoryAllocator::allocate
     */
    static void*
    allocate(size_t bytes) {
      return ge_frame_alloc(bytes);
    }

    /**
     * @copydoc MemoryAllocator::allocateAligned
     */
    static void*
    allocateAligned(size_t bytes, size_t alignment) {
#if GE_PROFILING_ENABLED
      incAllocCount();
#endif
      return ge_frame_alloc_aligned(bytes, alignment);
    }

    /**
     * @copydoc MemoryAllocator::allocateAligned16
     */
    static void*
    allocateAligned16(size_t bytes) {
#if GE_PROFILING_ENABLED
      incAllocCount();
#endif
      return ge_frame_alloc_aligned(bytes, 16);
    }

    /**
     * @copydoc MemoryAllocator::free
     */
    static void
    free(void* ptr) {
      ge_frame_free(ptr);
    }

    /**
     * @copydoc MemoryAllocator::freeAligned
     */
    static void
    freeAligned(void* ptr) {
#if GE_PROFILING_ENABLED
      incFreeCount();
#endif
      ge_frame_free_aligned(ptr);
    }

    /**
     * @copydoc MemoryAllocator::freeAligned16
     */
    static void
    freeAligned16(void* ptr) {
#if GE_PROFILING_ENABLED
      incFreeCount();
#endif
      ge_frame_free_aligned(ptr);
    }
  };
}
