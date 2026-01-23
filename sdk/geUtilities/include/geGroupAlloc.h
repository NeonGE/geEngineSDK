/*****************************************************************************/
/**
 * @file    geGroupAlloc.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/14
 * @brief   Provides an easy way to group multiple allocations under one.
 *
 * Provides an easy way to group multiple allocations under a single (actual)
 * allocation. Requires the user to first call reserve() methods for all
 * requested data elements, followed by init(), after which allocation /
 * deallocation can follow using construct / destruct or alloc / free methods.
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

namespace geEngineSDK {
  using std::forward;

  class GroupAlloc : INonCopyable
  {
   public:
    GroupAlloc() = default;

    GroupAlloc(GroupAlloc&& other) _NOEXCEPT
      : m_data(other.m_data),
        m_dataPtr(other.m_dataPtr),
        m_numBytes(other.m_numBytes) {
      other.m_data = nullptr;
      other.m_dataPtr = nullptr;
      other.m_numBytes = 0;
    }

    ~GroupAlloc() {
      if (m_numBytes > 0) {
        ge_free(m_data);
      }
    }

    GroupAlloc& operator=(GroupAlloc&& other) _NOEXCEPT {
      if (this == &other) {
        return *this;
      }

      if (m_numBytes > 0) {
        ge_free(m_data);
      }

      m_data = other.m_data;
      m_dataPtr = other.m_dataPtr;
      m_numBytes = other.m_numBytes;

      other.m_data = nullptr;
      other.m_dataPtr = nullptr;
      other.m_numBytes = 0;

      return *this;
    }

    /**
     * @brief Allocates internal memory as reserved by previous calls to reserve().
     *        Must be called before any calls to construct or alloc.
     */
    void
    init() {
      GE_ASSERT(nullptr == m_data);

      if (m_numBytes > 0) {
        m_data = reinterpret_cast<byte*>(ge_alloc(m_numBytes));
      }

      m_dataPtr = m_data;
    }

    /**
     * @brief Reserves the specified amount of bytes to allocate. Multiple
     *        calls to reserve() are cumulative. After all needed memory is
     *        reserved, call init(), followed by actual allocation via
     *        construct() or alloc() methods. If you need to change the size of
     *        your allocation, free your memory by using free(), followed by a
     *        call to clear(). Then reserve(), init() and alloc() again.
     */
    GroupAlloc&
    reserve(SIZE_T amount) {
      GE_ASSERT(nullptr == m_data);
      m_numBytes += amount;
      return *this;
    }

    /**
     * @brief Reserves the specified amount of bytes to allocate. Multiple
     *        calls to reserve() are cumulative. After all needed memory is
     *        reserved, call init(), followed by actual allocation via
     *        construct() or alloc() methods. If you need to change the size of
     *        your allocation, free your memory by using free(), followed by a
     *        call to clear(). Then reserve(), init() and alloc() again.
     */
    template<class T>
    GroupAlloc&
    reserve(SIZE_T count = 1) {
      GE_ASSERT(nullptr == m_data);
      m_numBytes += sizeof(T) * count;
      return *this;
    }

    /**
     * @brief Allocates a new piece of memory of the specified size.
     * @param[in] amount  Amount of memory to allocate, in bytes.
     */
    byte*
    alloc(SIZE_T amount) {
      GE_ASSERT(m_dataPtr + amount <= (m_data + m_numBytes));
      byte* output = m_dataPtr;
      m_dataPtr += amount;
      return output;
    }

    /**
     * @brief Allocates enough memory to hold @p count elements of the specified type.
     * @param[in] count Number of elements to allocate.
     */
    template<class T>
    T*
    alloc(SIZE_T count = 1) {
      return reinterpret_cast<T*>(alloc(sizeof(T) * count));
    }

    /**
     * @brief Deallocates a previously allocated piece of memory.
     */
    void
    free(void* /*data*/) {
      //Do nothing
    }

    /**
     * @brief Frees any internally allocated buffers. All elements must be
     *        previously freed by calling free().
     */
    void
    clear() {
      //NOTE: A debug check if user actually freed the memory could be helpful
      if (m_data) {
        ge_free(m_data);
      }

      m_numBytes = 0;
      m_data = nullptr;
      m_dataPtr = nullptr;
    }

    /**
     * @brief Allocates enough memory to hold the object(s) of specified type
     *        using the static allocator, and constructs them.
     */
    template<class T>
    T*
    construct(SIZE_T count = 1) {
      T* data = reinterpret_cast<T*>(alloc(sizeof(T) * count));
      for (SIZE_T i = 0; i < count; ++i) {
        new (reinterpret_cast<void*>(&data[i])) T;
      }

      return data;
    }

    /**
     * @brief Allocates enough memory to hold the object(s) of specified type
     *        using the static allocator, and constructs them.
     */
    template<class T, class... Args>
    T*
    construct(Args&&... args, SIZE_T count = 1) {
      T* data = reinterpret_cast<T*>(alloc(sizeof(T) * count));
      for (SIZE_T i = 0; i < count; ++i) {
        new (reinterpret_cast<void*>(&data[i])) T(forward<Args>(args)...);
      }
      return data;
    }

    /**
     * @brief Destructs and deallocates an object allocated with the static allocator.
     */
    template<class T>
    void
    destruct(T* data) {
      data->~T();
      free(data);
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
      free(data);
    }

   private:
    byte* m_data = nullptr;
    byte* m_dataPtr = nullptr;
    SIZE_T m_numBytes = 0;
  };
}
