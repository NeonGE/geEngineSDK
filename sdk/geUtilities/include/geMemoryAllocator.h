/*****************************************************************************/
/**
 * @file    geMemoryAllocator.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/16
 * @brief   Memory allocation objects and templates
 *
 * Define templates and classes used to allocate memory and create
 * object on the heap dynamically.
 * Also defines StdAlloc used to define the way STL objects will
 * allocate their objects
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

#undef min
#undef max

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include <new>
#include <limits>
#include <cstdint>
#include <utility>

#if USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_ANDROID)
# include <malloc.h>
#endif

namespace geEngineSDK {
  using std::forward;
  using std::size_t;
  using std::ptrdiff_t;

  class MemoryAllocatorBase;

#if USING(GE_PLATFORM_WINDOWS)
  inline void*
  platformAlignedAlloc16(size_t size) {
    return _aligned_malloc(size, 16);
  }

  inline void
  platformAlignedFree16(void* ptr) {
    _aligned_free(ptr);
  }

  inline void*
  platformAlignedAlloc(size_t size, size_t alignment) {
    return _aligned_malloc(size, alignment);
  }

  inline void
  platformAlignedFree(void* ptr) {
    _aligned_free(ptr);
  }
#elif USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_ANDROID)
  inline void*
  platformAlignedAlloc16(size_t size) {
    return ::memalign(16, size);
  }

  inline void
  platformAlignedFree16(void* ptr) {
    ::free(ptr);
  }

  inline void*
  platformAlignedAlloc(size_t size, size_t alignment) {
    return ::memalign(alignment, size);
  }

  inline void
  platformAlignedFree(void* ptr) {
    ::free(ptr);
  }
#else //16 byte alignment by default
  inline void*
  platformAlignedAlloc16(size_t size) {
    return ::malloc(size);
  }

  inline void
  platformAlignedFree16(void* ptr) {
    ::free(ptr);
  }

  inline void*
  platformAlignedAlloc(size_t size, size_t alignment) {
    void* data = ::malloc(size + (alignment - 1) + sizeof(void*));
    if (nullptr == data) {
      return nullptr;
    }

    char* alignedData = ((char*)data) + sizeof(void*);
    alignedData += (alignment - ((uintptr_t)alignedData) & (alignment - 1)) & (alignment - 1);

    ((void**)alignedData)[-1] = data;
    return alignedData;
  }

  inline void
  platformAlignedFree(void* ptr) {
    ::free(((void**)ptr)[-1]);
  }
#endif

  /**
   * @class MemoryCounter
   * @brief Thread safe class used for storing total number of memory
   *        allocations and deallocations, primarily for statistic purposes.
   */
  class MemoryCounter
  {
   public:
    static GE_UTILITIES_EXPORT uint64_t
    getNumAllocs() { 
      return m_allocs;
    }
    
    static GE_UTILITIES_EXPORT uint64_t
    getNumFrees() {
      return m_frees;
    }

   private:
    friend class MemoryAllocatorBase;

    /**
     * Thread local data can't be exported, so some magic to make it accessible
     * from MemoryAllocator
     */
    static GE_UTILITIES_EXPORT void
    incAllocCount() {
      ++m_allocs;
    }
    
    static GE_UTILITIES_EXPORT void
    incFreeCount() {
      ++m_frees;
    }

    static GE_THREADLOCAL uint64_t m_allocs;
    static GE_THREADLOCAL uint64_t m_frees;
  };

  /**
   * @class MemoryAllocatorBase
   * @brief Base class all memory allocators need to inherit.
   *        Provides allocation and free counting.
   */
  class MemoryAllocatorBase
  {
   protected:
    static void
      incAllocCount() {
      MemoryCounter::incAllocCount();
    }

    static void
      incFreeCount() {
      MemoryCounter::incFreeCount();
    }
  };

  /**
   * @class MemoryAllocator
   * @brief Provides a generic implementation. Specialize for specific
   *        categories as needed. For example you might implement a pool
   *        allocator for specific types in order to reduce allocation overhead.
   *        By default standard malloc/free are used.
   */
  template<class T>
  class MemoryAllocator : public MemoryAllocatorBase
  {
   public:
    static void*
    allocate(size_t bytes) {
#if GE_PROFILING_ENABLED
      incAllocCount();
#endif
      return malloc(bytes);
    }

    /**
     * @brief Allocates @p bytes and aligns them to the specified boundary (in bytes).
     *        If the alignment is less or equal to 16 it is more efficient to use the
     *        allocateAligned16() alternative of this method.
     *        Alignment must be power of two.
     */
    static void*
    allocateAligned(size_t bytes, size_t alignment) {
#if GE_PROFILING_ENABLED
      incAllocCount();
#endif
      return platformAlignedAlloc(bytes, alignment);
    }

    /**
     * @brief Allocates @p bytes and aligns them to a 16 byte boundary.
     */
    static void*
    allocateAligned16(size_t bytes) {
#if GE_PROFILING_ENABLED
      incAllocCount();
#endif
      return platformAlignedAlloc16(bytes);
    }

    static void
    free(void* ptr) {
#if GE_PROFILING_ENABLED
      incFreeCount();
#endif
      ::free(ptr);
    }

    /**
     * @brief Frees memory allocated with allocateAligned()
     */
    static void
    freeAligned(void* ptr) {
#if GE_PROFILING_ENABLED
      incFreeCount();
#endif
      platformAlignedFree(ptr);
    }

    /**
     * @brief Frees memory allocated with allocateAligned16()
     */
    static void
    freeAligned16(void* ptr) {
#if GE_PROFILING_ENABLED
      incFreeCount();
#endif
      platformAlignedFree16(ptr);
    }
  };

  /**
   * @brief General allocator provided by the OS. Use for persistent long term
   *        allocations, and allocations that don't happen often.
   */
  class GenAlloc
  {};

  /**
   * @brief Allocates the specified number of bytes.
   */
  template<class Alloc>
  void*
  ge_alloc(size_t count) {
    return MemoryAllocator<Alloc>::allocate(count);
  }

  /**
   * @brief Allocates enough bytes to hold the specified type, but doesn't construct it.
   */
  template<class T, class Alloc>
  T*
  ge_alloc() {
    return reinterpret_cast<T*>(MemoryAllocator<Alloc>::allocate(sizeof(T)));
  }

  /**
   * @brief Creates and constructs an array of "count" elements.
   */
  template<class T, class Alloc>
  T*
  ge_newN(size_t count) {
    auto ptr = reinterpret_cast<T*>(MemoryAllocator<Alloc>::allocate(sizeof(T) * count));

    for (size_t i = 0; i < count; ++i) {
      new (&ptr[i]) T;
    }

    return ptr;
  }

  /**
   * @brief Create a new object with the specified allocator and the specified parameters.
   */
  template<class T, class Alloc, class... Args>
  T*
  ge_new(Args &&...args) {
    return new (ge_alloc<T, Alloc>()) T(forward<Args>(args)...);
  }

  /**
   * @brief Frees all the bytes allocated at the specified location.
   */
  template<class Alloc>
  void
  ge_free(void* ptr) {
    MemoryAllocator<Alloc>::free(ptr);
  }

  /**
   * @brief Destructs and frees the specified object.
   */
  template<class T, class Alloc = GenAlloc>
  void
  ge_delete(T* ptr) {
    (ptr)->~T();
    MemoryAllocator<Alloc>::free(ptr);
  }

  /**
   * @brief Callable struct that acts as a proxy for ge_delete
   */
  template<class T, class Alloc = GenAlloc>
  struct Deleter
  {
    CONSTEXPR Deleter() _NOEXCEPT = default;

    /**
     * @brief Constructor enabling deleter conversion and therefore
     *        polymorphism with smart points (if they use the same allocator).
     */
    template<class T2, std::enable_if_t<std::is_convertible<T2*, T*>::value, int> = 0>
    CONSTEXPR Deleter(const Deleter<T2, Alloc>& other) _NOEXCEPT {
      GE_UNREFERENCED_PARAMETER(other);
    }

    void
    operator()(T* ptr) const {
      ge_delete<T, Alloc>(ptr);
    }
  };

  /**
   * @brief Destructs and frees the specified array of objects.
   */
  template<class T, class Alloc = GenAlloc>
  void
  ge_deleteN(T* ptr, size_t count) {
    for (size_t i = 0; i < count; ++i) {
      ptr[i].~T();
    }
    MemoryAllocator<Alloc>::free(ptr);
  }

  /***************************************************************************/
  /**
   * Default versions of all alloc/free/new/delete methods which call GenAlloc
   */
  /***************************************************************************/

  /**
   * @brief Allocates the specified number of bytes.
   */
  inline void*
  ge_alloc(size_t count) {
    return MemoryAllocator<GenAlloc>::allocate(count);
  }

  /**
   * @brief Allocates enough bytes to hold the specified type, but doesn't construct it.
   */
  template<class T>
  T*
  ge_alloc() {
    return reinterpret_cast<T*>(MemoryAllocator<GenAlloc>::allocate(sizeof(T)));
  }

  /**
   * @brief Allocates the specified number of bytes aligned to the provided boundary.
   *        Boundary is in bytes and must be a power of two.
   */
  inline void*
  ge_alloc_aligned(size_t count, size_t align) {
    return MemoryAllocator<GenAlloc>::allocateAligned(count, align);
  }

  /**
   * @brief Allocates the specified number of bytes aligned to a 16 bytes boundary.
   */
  inline void*
  ge_alloc_aligned16(size_t count) {
    return MemoryAllocator<GenAlloc>::allocateAligned16(count);
  }

  /**
   * @brief Creates and constructs an array of "count" elements.
   */
  template<class T>
  T*
  ge_allocN(size_t count) {
    return reinterpret_cast<T*>(MemoryAllocator<GenAlloc>::allocate(count * sizeof(T)));
  }

  /**
  * @brief Creates and constructs an array of "count" elements.
  */
  template<class T>
  T*
  ge_newN(size_t count) {
    T* ptr = reinterpret_cast<T*>(MemoryAllocator<GenAlloc>::allocate(sizeof(T) * count));
    for (size_t i = 0; i < count; ++i) {
      new (&ptr[i]) T;
    }

    return ptr;
  }

  /**
   * @brief Create a new object with the specified allocator and the specified parameters.
   */
  template<class T, class... Args>
  T*
  ge_new(Args &&...args) {
    return new (ge_alloc<T, GenAlloc>()) T(std::forward<Args>(args)...);
  }

  /**
   * @brief Frees all the bytes allocated at the specified location.
   */
  inline void
  ge_free(void* ptr) {
    MemoryAllocator<GenAlloc>::free(ptr);
  }

  /**
   * @brief Frees memory previously allocated with ge_alloc_aligned().
   */
  inline void
  ge_free_aligned(void* ptr) {
    MemoryAllocator<GenAlloc>::freeAligned(ptr);
  }

  /**
   * @brief Frees memory previously allocated with ge_alloc_aligned16().
   */
  inline void
  ge_free_aligned16(void* ptr) {
    MemoryAllocator<GenAlloc>::freeAligned16(ptr);
  }

  /***************************************************************************/
  /**                     MACRO VERSIONS
   * You will almost always want to use the template versions but in some cases
   * (private constructor / destructor) it is not possible. In which case you
   * may use these instead.
   */
  /***************************************************************************/
#define GE_PVT_NEW(T, ...)                                                    \
      new (ge_alloc<T>()) T(__VA_ARGS__)

#define GE_PVT_DELETE(T, ptr)                                                 \
      (ptr)->~T();                                                            \
      MemoryAllocator<GenAlloc>::free(ptr);

#define GE_PVT_DELETE_A(T, ptr, Alloc)                                        \
      (ptr)->~T();                                                            \
      MemoryAllocator<Alloc>::free(ptr);

  /**
   * @brief Allocator for the standard library that internally uses the
   *        Genesis Engine memory allocator.
   */
  template <class T, class Alloc = GenAlloc>
  class StdAlloc
  {
   public:
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    
    CONSTEXPR StdAlloc() = default;
    CONSTEXPR StdAlloc(StdAlloc&&) = default;
    CONSTEXPR StdAlloc(const StdAlloc&) = default;

    template<class U, class Alloc2>
    explicit CONSTEXPR StdAlloc(const StdAlloc<U, Alloc2>&) {}

    template<class U, class Alloc2>
    CONSTEXPR bool
    operator==(const StdAlloc<U, Alloc2>&) const _NOEXCEPT {
      return true;
    }

    template<class U, class Alloc2>
    CONSTEXPR bool
    operator!=(const StdAlloc<U, Alloc2>&) const _NOEXCEPT {
      return false;
    }

    template<class U>
    class rebind
    {
     public:
      using other = StdAlloc<U, Alloc>;
    };

    /**
     * @brief Allocate but don't initialize number elements of type T.
     */
    static T*
    allocate(const size_t num) {
      if (0 == num) {
        return nullptr;
      }

      if (num > std::numeric_limits<size_t>::max() / sizeof(T)){
        return nullptr; //Error
      }

      void* const pv = ge_alloc<Alloc>(num * sizeof(T));
      if (!pv) {
        return nullptr;
      }

      return static_cast<T*>(pv);
    }

    /**
     * @brief Deallocate storage p of deleted elements.
     */
    static void
    deallocate(pointer p, size_t) {
      ge_free<Alloc>(p);
    }

    static CONSTEXPR size_t
    max_size() {
      return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    static CONSTEXPR void
    destroy(pointer p) {
      p->~T();
    }

    /**
     * @brief This version of construct() (with a varying number of parameters)
     *        seems necessary in order to use some STL data structures from
     *        libstdc++-4.8, but compilation fails on OSX, hence the #if.
     */
    template<class... Args>
    static void
    construct(pointer p, Args&& ...args) {
      new(p) T(std::forward<Args>(args)...);
    }
  };
}

#include "geStackAlloc.h"
#include "geFreeAlloc.h"
#include "geFrameAlloc.h"
#include "geStaticAlloc.h"
#include "geMemAllocProfiler.h"
