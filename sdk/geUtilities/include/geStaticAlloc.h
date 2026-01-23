/*****************************************************************************/
/**
 * @file    geStaticAlloc.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/11
 * @brief   Static allocator
 *
 * Static allocator that attempts to perform zero heap (dynamic) allocations by
 * always keeping an active preallocated buffer. The allocator provides a fixed
 * amount of preallocated memory, and if the size of the allocated data goes
 * over that limit the allocator will fall back to dynamic heap allocations
 * using the selected allocator.
 *
 * Static allocations can only be freed if memory is deallocated in opposite
 * order it is allocated. Otherwise static memory gets orphaned until a call to
 * clear().
 * Dynamic memory allocations behave depending on the selected allocator.
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
#include "geNumericLimits.h"

namespace geEngineSDK {
  using std::forward;
  using std::size_t;
  using std::ptrdiff_t;
  using std::vector;

  /**
   * @tparam  BlockSize   Size of the initially allocated static block, and
   *                      minimum size of any dynamically allocated memory.
   * @tparam  DynamicAllocator  Allocator to fall-back to when static buffer is
   *                            full.
   */
  template<int BlockSize = 512, class DynamicAllocator = TFrameAlloc<BlockSize>>
  class StaticAlloc
  {
   private:
    /**
     * @brief A single block of memory within a static allocator.
     */
    class MemBlock
    {
     public:
      MemBlock(byte* data, SIZE_T size)
        : m_data(data),
          m_size(size)
      {}

      /**
       * @brief Allocates a piece of memory within the block.
       *        Caller must ensure the block has enough empty space.
       */
      byte*
      alloc(SIZE_T amount) {
        byte* freePtr = &m_data[m_freePtr];
        m_freePtr += amount;

        return freePtr;
      }

      /**
       * @brief Frees a piece of memory within the block. If the memory isn't
       *        the last allocated memory, no deallocation happens and that
       *        memory is instead orphaned.
       */
      void
      free(byte* data, SIZE_T allocSize) {
        if ((data + allocSize) == (m_data + m_freePtr)) {
          m_freePtr -= allocSize;
        }
      }

      /**
       * @brief Releases all allocations within a block but doesn't actually 
       *        free the memory.
       */
      void
      clear() {
        m_freePtr = 0;
      }

      byte* m_data = nullptr;
      SIZE_T m_freePtr = 0;
      SIZE_T m_size = 0;
      MemBlock* m_nextBlock = nullptr;
    };

   public:
    StaticAlloc() = default;
    ~StaticAlloc() = default;

    /**
     * @brief  Allocates a new piece of memory of the specified size.
     * @param[in]  amount  Amount of memory to allocate, in bytes.
     */
    byte*
    alloc(SIZE_T amount) {
      if (0 == amount) {
        return nullptr;
      }

      GE_DEBUG_ONLY(amount += sizeof(SIZE_T));

      SIZE_T freeMem = BlockSize - m_freePtr;

      byte* data;
      if (amount > freeMem) {
        data = m_dynamicAlloc.alloc(amount);
      }
      else {
        data = &m_staticData[m_freePtr];
        m_freePtr += amount;
      }

#if USING(GE_DEBUG_MODE)
      m_totalAllocBytes += amount;

      SIZE_T* storedSize = reinterpret_cast<SIZE_T*>(data);
      *storedSize = amount;

      return data + sizeof(SIZE_T);
#else
      return data;
#endif
    }

    /**
     * @brief Deallocates a previously allocated piece of memory.
     */
    void
    free(void* data, SIZE_T allocSize) {
      if (nullptr == data) {
        return;
      }

      byte* dataPtr = reinterpret_cast<byte*>(data);
#if USING(GE_DEBUG_MODE)
      dataPtr -= sizeof(SIZE_T);

      SIZE_T* storedSize = reinterpret_cast<SIZE_T*>(dataPtr);
      m_totalAllocBytes -= *storedSize;
#endif
      if (data > m_staticData && data < (m_staticData + BlockSize)) {
        if (((reinterpret_cast<byte*>(data)) + allocSize) == (m_staticData + m_freePtr)) {
          m_freePtr -= allocSize;
        }
      }
      else {
        m_dynamicAlloc.free(dataPtr);
      }
    }

    /**
     * @brief Deallocates a previously allocated piece of memory.
     */
    void
    free(void* data) {
      if (nullptr == data) {
        return;
      }

      //Dealloc is only used for debug and can be removed if needed.
      //All the actual deallocation happens in clear()

      byte* dataPtr = reinterpret_cast<byte*>(data);
#if USING(GE_DEBUG_MODE)
      dataPtr -= sizeof(SIZE_T);

      SIZE_T* storedSize = reinterpret_cast<SIZE_T*>(dataPtr);
      m_totalAllocBytes -= *storedSize;
#endif
      if (data < m_staticData || data >= (m_staticData + BlockSize)) {
        m_dynamicAlloc.free(dataPtr);
      }
    }

    /**
     * @brief Allocates enough memory to hold the object(s) of specified type
     *        using the static allocator, and constructs them.
     */
    template<class T>
    T*
    construct(SIZE_T count = 0) {
      T* data = reinterpret_cast<T*>(alloc(sizeof(T) * count));

      for (SIZE_T i = 0; i < count; ++i) {
        new ((void*)(&data[i])) T;
      }

      return data;
    }

    /**
    * @brief Allocates enough memory to hold the object(s) of specified type
    *        using the static allocator, and constructs them.
    */
    template<class T, class... Args>
    T*
    construct(Args &&...args, SIZE_T count = 0) {
      T* data = reinterpret_cast<T*>(alloc(sizeof(T) * count));

      for (SIZE_T i = 0; i < count; ++i) {
        new ((void*)(&data[i])) T(forward<Args>(args)...);
      }

      return data;
    }

    /**
     * @brief Destructs and deallocates an object allocated with the static
     *        allocator.
     */
    template<class T>
    void
    destruct(T* data) {
      data->~T();
      free(data, sizeof(T));
    }

    /**
     * @brief Destructs and deallocates an array of objects allocated with the
     *        static frame allocator.
     */
    template<class T>
    void
    destruct(T* data, SIZE_T count) {
      for (SIZE_T i = 0; i < count; ++i) {
        data[i].~T();
      }
      free(data, sizeof(T) * count);
    }

    /**
     * @brief Frees the internal memory buffers. All external allocations must
     *        be freed before calling this.
     */
    void
    clear() {
      GE_ASSERT(0 == m_totalAllocBytes);
      m_freePtr = 0;
      m_dynamicAlloc.clear();
    }

   private:
    byte m_staticData[BlockSize];
    SIZE_T m_freePtr = 0;
    DynamicAllocator m_dynamicAlloc;
    SIZE_T m_totalAllocBytes = 0;
  };

  /**
   * @brief Allocator for the standard library that internally uses a static
   *        allocator.
   */
  template<int32 BlockSize, class T>
  class StdStaticAlloc
  {
   public:
    using value_type = T;
    using pointer = value_type * ;
    using const_pointer = const value_type*;
    using reference = value_type & ;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    StdStaticAlloc() = default;

    explicit StdStaticAlloc(StaticAlloc<BlockSize, FreeAlloc>* refAlloc) _NOEXCEPT
      : m_staticAlloc(refAlloc)
    {}

    template<class U>
    StdStaticAlloc(const StdStaticAlloc<BlockSize, U>& refAlloc) _NOEXCEPT
      : m_staticAlloc(refAlloc.m_staticAlloc)
    {}

    template<class U>
    class rebind
    {
     public:
      using other = StdStaticAlloc<BlockSize, U>;
    };

    /**
     * @brief Allocate but don't initialize number elements of type T.
     */
    T*
    allocate(const size_t num) const {
      if (0 == num) {
        return nullptr;
      }

      if (num > NumLimit::MAX_SIZET / sizeof(T)) {
        return nullptr; //Error
      }

      void* const pv = m_staticAlloc->alloc(num * sizeof(T));
      if (!pv) {
        return nullptr; //Error
      }

      return static_cast<T*>(pv);
    }

    /**
     * @brief Deallocate storage p of deleted elements.
     */
    void
    deallocate(T* p, size_t num) const _NOEXCEPT {
      m_staticAlloc->free(reinterpret_cast<byte*>(p), num);
    }

    StaticAlloc<BlockSize, FreeAlloc>* m_staticAlloc = nullptr;

    size_t
    max_size() const {
      return NumLimit::MAX_UINT32 / sizeof(T);
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
    construct(U* p, Args&&... args) {
      new(p) U(forward<Args>(args)...);
    }

    template<class T1, int32 N1, class T2, int32 N2>
    friend bool
    operator==(const StdStaticAlloc<N1, T1>& a,
               const StdStaticAlloc<N2, T2>& b) throw();

  };

  /**
   * @brief Return that all specializations of this allocator are
   *        interchangeable.
   */
  template<class T1, int N1, class T2, int N2>
  bool
  operator==(const StdStaticAlloc<N1, T1>& a,
             const StdStaticAlloc<N2, T2>& b) throw() {
    return N1 == N2 && a.m_staticAlloc == b.m_staticAlloc;
  }

  /**
   * @brief Return that all specializations of this allocator are
   *        interchangeable.
   */
  template<class T1, int N1, class T2, int N2>
  bool
  operator!=(const StdStaticAlloc<N1, T1>& a,
             const StdStaticAlloc<N2, T2>& b) throw() {
    return !(a == b);
  }

  /**
   * @brief Equivalent to Vector, except it avoids any dynamic allocations
   *        until the number of elements exceeds @p Count.
   * Requires allocator to be explicitly provided.
   */
  template<typename T, int32 Count>
  using StaticVector = vector<T, StdStaticAlloc<sizeof(T) * Count, T>>;
}
