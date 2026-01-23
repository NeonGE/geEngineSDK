/*****************************************************************************/
/**
 * @file    geMemStack.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/16
 * @brief   Classes used to create our own Memory Stacks
 *
 * This file defines all the needed classes to create a Memory Stack.
 * Normally it's used in combination with STL objects.
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
#include <stack>
#include <cassert>

#include "gePlatformTypes.h"
#include "geStdHeaders.h"
#include "geThreading.h"
#include "geMacroUtil.h"

namespace geEngineSDK {
  using std::exchange;
  using std::forward;

  /**
  * @brief  Describes a memory stack of a certain block capacity.
  *         See "MemoryStack" for more information.
  * @tparam BlockCapacity Minimum size of a block. Larger blocks mean less memory
  *         allocations, but also potentially more wasted memory. If an allocation
  *         requests more bytes than BlockCapacity, first largest multiple is used instead.
  */
  template<SIZE_T BlockCapacity = 1024 * 1024>	//Default to 1.0 MB
  class MemStackInternal
  {
   private:
    /**
     * @brief A single block of memory of "BlockCapacity" size. A pointer to
     *        the first free address is stored, and a remaining size.
     */
    class MemBlock
    {
     public:
      explicit MemBlock(SIZE_T size) : m_size(size) {}
      ~MemBlock() = default;

      /**
       * @brief Returns the first free address and increments the free pointer.
       *        Caller needs to ensure the remaining block size is adequate before calling.
       */
      byte*
      alloc(SIZE_T amount) {
        byte* freePtr = &m_data[m_freePtr];
        m_freePtr += amount;
        return freePtr;
      }

      /**
       * @brief Deallocates the provided pointer. Deallocation must happen in
       *        opposite order from allocation otherwise corruption will occur.
       * @note  Pointer to "data" isn't actually needed, but is provided for
       *        debug purposes in order to more easily track out-of-order deallocations.
       */
      void
      dealloc(const byte* data, SIZE_T amount) {
        m_freePtr -= amount;
        GE_ASSERT((&m_data[m_freePtr]) == data && 
                  "Out of order stack deallocation detected. Deallocations need"
                  "to happen in order opposite of allocations.");
#if !USING(GE_DEBUG_MODE)
        GE_UNREFERENCED_PARAMETER(data);
#endif
      }
     public:
      byte* m_data = nullptr;
      SIZE_T m_freePtr = 0;
      SIZE_T m_size = 0;
      MemBlock* m_nextBlock = nullptr;
      MemBlock* m_prevBlock = nullptr;
    };

   private:
    MemBlock* m_freeBlock = nullptr;

   public:
    MemStackInternal() {
      m_freeBlock = allocBlock(BlockCapacity);
    }

    ~MemStackInternal() {
      GE_ASSERT(0 == m_freeBlock->m_freePtr && 
                "Not all blocks were released before shutting down the stack allocator.");

      MemBlock* curBlock = m_freeBlock;
      while (nullptr != curBlock) {
        MemBlock* nextBlock = curBlock->m_nextBlock;
        deallocBlock(curBlock);
        curBlock = nextBlock;
      }
    }

    /**
     * @brief Allocates the given amount of memory on the stack.
     * @param	amount	The amount to allocate in bytes.
     * @note  Allocates the memory in the currently active block if it is large
     *        enough, otherwise a new block is allocated. If the allocation is 
     *        larger than default block size a separate block will be allocated 
     *        only for that allocation, making it essentially a slower heap allocator.
     *
     *			Each allocation comes with a 4 byte overhead.
     */
    byte*
    alloc(SIZE_T amount) {
      amount += sizeof(SIZE_T);

      SIZE_T freeMem = m_freeBlock->m_size - m_freeBlock->m_freePtr;
      if (amount > freeMem) {
        allocBlock(amount);
      }

      byte* data = m_freeBlock->alloc(amount);

      SIZE_T* storedSize = reinterpret_cast<SIZE_T*>(data);
      *storedSize = amount;

      return data + sizeof(SIZE_T);
    }

    /**
     * @brief Deallocates the given memory. Data must be deallocated in
     *        opposite order then when it was allocated.
     */
    void
    dealloc(byte* data) {
      data -= sizeof(SIZE_T);

      SIZE_T* storedSize = reinterpret_cast<SIZE_T*>(data);
      m_freeBlock->dealloc(data, *storedSize);

      if (0 == m_freeBlock->m_freePtr) {
        MemBlock* emptyBlock = m_freeBlock;

        if (nullptr != emptyBlock->m_prevBlock) {
          m_freeBlock = emptyBlock->m_prevBlock;
        }

        //Merge with next block
        if (nullptr != emptyBlock->m_nextBlock) {
          SIZE_T totalSize = emptyBlock->m_size + emptyBlock->m_nextBlock->m_size;

          if (nullptr != emptyBlock->m_prevBlock) {
            emptyBlock->m_prevBlock->m_nextBlock = nullptr;
          }
          else {
            m_freeBlock = nullptr;
          }

          deallocBlock(emptyBlock->m_nextBlock);
          deallocBlock(emptyBlock);

          allocBlock(totalSize);
        }
      }
    }

   private:
    /**
     * @brief Allocates a new block of memory using a heap allocator. Block
     *        will never be smaller than "BlockCapacity" no matter the "wantedSize".
     */
    MemBlock*
    allocBlock(SIZE_T wantedSize) {
      SIZE_T blockSize = BlockCapacity;
      if (wantedSize > blockSize) {
        blockSize = wantedSize;
      }

      MemBlock* newBlock = nullptr;
      MemBlock* curBlock = m_freeBlock;

      while (nullptr != curBlock) {
        MemBlock* nextBlock = curBlock->m_nextBlock;
        if (nullptr != nextBlock && nextBlock->m_size >= blockSize) {
          newBlock = nextBlock;
          break;
        }

        curBlock = nextBlock;
      }

      if (nullptr == newBlock) {
        auto data = reinterpret_cast<byte*>(ge_alloc(blockSize + sizeof(MemBlock)));
        newBlock = new (data)MemBlock(blockSize);
        data += sizeof(MemBlock);

        newBlock->m_data = data;
        newBlock->m_prevBlock = m_freeBlock;

        if (nullptr != m_freeBlock) {
          if (nullptr != m_freeBlock->m_nextBlock) {
            m_freeBlock->m_nextBlock->m_prevBlock = newBlock;
          }
          newBlock->m_nextBlock = m_freeBlock->m_nextBlock;
          m_freeBlock->m_nextBlock = newBlock;
        }
      }

      m_freeBlock = newBlock;
      return newBlock;
    }

    /**
     * @brief Deallocates a block of memory.
     */
    void
    deallocBlock(MemBlock* block) {
      block->~MemBlock();
      ge_free(block);
    }
  };

  /**
   * @brief One of the fastest, but also very limiting type of allocator.
   *        All deallocations must happen in opposite order from allocations.
   *
   * @note  It's mostly useful when you need to allocate something temporarily on the heap,
   *        usually something that gets allocated and freed within the same method.
   *
   *      Each allocation comes with a pretty hefty 4 byte memory overhead,
   *      so don't use it for small allocations.
   *
   *      Thread safe. But you cannot allocate on one thread and deallocate on another.
   *      Threads will keep separate stacks internally. Make sure to call
   *      BeginThread/EndThread for any thread this stack is used on.
   */
  class MemStack
  {
   public:
    /**
      * @brief  Sets up the stack with the currently active thread. You need to
      *         call this on any thread before doing any allocations or deallocations
      */
    static GE_UTILITIES_EXPORT void
    beginThread();

    /**
     * @brief Cleans up the stack for the current thread. You may not perform any allocations
     *        or deallocations after this is called, unless you call BeginThread again.
     */
    static GE_UTILITIES_EXPORT void
    endThread();

    /**
     * @copydoc MemoryStackInternal::alloc(SIZE_T)
     */
    static GE_UTILITIES_EXPORT byte*
    alloc(SIZE_T numBytes);

    /**
     * @copydoc MemoryStackInternal::dealloc(byte*)
     */
    static GE_UTILITIES_EXPORT void
    deallocLast(byte* data);

   private:
    static GE_THREADLOCAL MemStackInternal<1024 * 1024>* threadMemStack;
  };

  /**
   * @copydoc MemoryStackInternal::alloc
   */
  inline void*
  ge_stack_alloc(SIZE_T numBytes) {
    return reinterpret_cast<void*>(MemStack::alloc(numBytes));
  }

  /**
   * @brief Allocates enough memory to hold the specified type, on the stack,
   *        but does not initialize the object.
   * @see   MemoryStackInternal::alloc()
   */
  template<class T>
  T*
  ge_stack_alloc() {
    return reinterpret_cast<T*>(MemStack::alloc(sizeof(T)));
  }

  /**
   * @brief Allocates enough memory to hold N objects of the specified type,
   *        on the stack, but does not initialize the objects.
   * @see   MemoryStackInternal::alloc()
   */
  template<class T>
  T*
  ge_stack_alloc(SIZE_T numBytes) {
    return reinterpret_cast<T*>(MemStack::alloc(sizeof(T) * numBytes));
  }

  /**
   * @brief Allocates enough memory to hold the specified type, on the stack,
   *        and initializes the object using the parameterless constructor.
   * @see   MemoryStackInternal::alloc()
   */
  template<class T>
  T*
  ge_stack_new(SIZE_T count = 0) {
    T* data = ge_stack_alloc<T>(count);
    for (uint32 i = 0; i < count; ++i) {
      new (reinterpret_cast<void*>(&data[i])) T;
    }

    return data;
  }

  /**
   * @brief Allocates enough memory to hold the specified type, on the stack,
   *        and constructs the object.
   * @see   MemoryStackInternal::alloc()
   */
  template<class T, class... Args>
  T*
  ge_stack_new(Args&& ...args, SIZE_T count = 0) {
    T* data = ge_stack_alloc<T>(count);
    for (SIZE_T i = 0; i<count; ++i) {
      new (reinterpret_cast<void*>(&data[i])) T(std::forward<Args>(args)...);
    }
    return data;
  }

  /**
   * @brief Destructs and deallocates last allocated entry currently located on stack.
   * @see   MemoryStackInternal::dealloc()
   */
  template<class T>
  void
  ge_stack_delete(T* data) {
    data->~T();
    MemStack::deallocLast(reinterpret_cast<byte*>(data));
  }

  /**
   * @brief Destructs an array of objects and deallocates last allocated entry
   *        currently located on stack.
   * @see   MemoryStackInternal::dealloc()
   */
  template<class T>
  void
  ge_stack_delete(T* data, SIZE_T count) {
    for (SIZE_T i = 0; i<count; ++i) {
      data[i].~T();
    }

    MemStack::deallocLast(reinterpret_cast<byte*>(data));
  }

  inline void
  ge_stack_delete(void* data, SIZE_T /*count*/) {
    MemStack::deallocLast(reinterpret_cast<byte*>(data));
  }

  /**
   * @copydoc MemoryStackInternal::dealloc()
   */
  inline void
  ge_stack_free(void* data) {
    return MemStack::deallocLast(reinterpret_cast<byte*>(data));
  }

  /**
   * @brief An object used to transparently clean up a stack allocation when
   *        it's no longer in scope. Make sure to take great care not to free
   *        non-managed stack allocations out of order or to free the stack
   *        allocation managed by this object!
   */
  template<typename T>
  struct StackMemory
  {
    /**
     * @brief Provide implicit conversion to the allocated buffer so that users
     *        of this code can "pretend" this object is a pointer to the stack
     *        buffer that they wanted.
     */
    CONSTEXPR operator T*
    () const & _NOEXCEPT {
      return m_ptr;
    }

    /**
     * @brief This ensures that the result of ge_managed_stack_alloc() doesn't
     *        get passed to a function call as a temporary, or immediately
     *        assigned as a T*. Instead, the user of this class is forced to
     *        deal with this class as itself, when handling the return value
     *        of ge_managed_stack_alloc() preventing an immediate
     *        (and erroneous) call to ge_stack_free().
     */
    CONSTEXPR operator T*
    () const && _NOEXCEPT = delete;

    explicit CONSTEXPR StackMemory(T* p, size_t count = 1)
      : m_ptr(p),
        m_count(count)
    {}

    /**
     * @brief Needed until c++17
     */
    StackMemory(StackMemory && other)
      : m_ptr(exchange(other.m_ptr, nullptr)),
        m_count(exchange(other.m_count, 0))
    {}

    StackMemory(StackMemory const&) = delete;

    StackMemory&
    operator=(StackMemory &&) = delete;

    StackMemory&
    operator=(StackMemory const&) = delete;

    /**
     * @brief Frees the stack allocation.
     */
    ~StackMemory() {
      if (nullptr != m_ptr) {
        if (1 <= m_count) {
          ge_stack_delete(m_ptr, static_cast<uint32>(m_count));
        }
        else {
          ge_stack_free(m_ptr);
        }
      }
    }

   private:
    T* m_ptr = nullptr;
    SIZE_T m_count = 0;
  };

  /**
   * @brief Same as ge_stack_alloc() except the returned object takes care of
   *        automatically cleaning up when it goes out of scope.
   */
  inline StackMemory<void>
  ge_managed_stack_alloc(size_t amount) {
    return StackMemory<void>(ge_stack_alloc(amount));
  }

  /**
   * @brief Same as ge_stack_alloc() except the returned object takes care of
   *        automatically cleaning up when it goes out of scope.
   */
  template<class T>
  StackMemory<T>
  ge_managed_stack_alloc() {
    return StackMemory<T>(ge_stack_alloc<T>());
  }

  /**
   * @brief Same as bs_stack_alloc() except the returned object takes care of
   *        automatically cleaning up when it goes out of scope.
   */
  template<class T>
  StackMemory<T>
  ge_managed_stack_alloc(size_t amount) {
    return StackMemory<T>(ge_stack_alloc<T>(amount));
  }

  /**
   * @brief Same as bs_stack_new() except the returned object takes care of
   *        automatically cleaning up when it goes out of scope.
   */
  template<class T>
  StackMemory<T>
  ge_managed_stack_new(size_t count = 1) {
    return StackMemory<T>(ge_stack_new<T>(count), count);
  }

  /**
   * @brief Same as bs_stack_new() except the returned object takes care of
   *        automatically cleaning up when it goes out of scope.
   */
  template<class T, class... Args>
  StackMemory<T>
  ge_managed_stack_new(Args && ... args, size_t count = 1) {
    return StackMemory<T>(ge_stack_new<T>(forward<Args>(args)..., count), count);
  }

  /**
   * @brief Allows use of a stack allocator by using normal
   *        new/delete/free/dealloc operators.
   * @see   MemStack
   */
  class StackAlloc
  {};

  /**
   * @brief Specialized memory allocator implementations that allows use of a
   *        stack allocator in normal new/delete/free/dealloc operators.
   * @see   MemStack
   */
  template<>
  class MemoryAllocator<StackAlloc> : public MemoryAllocatorBase
  {
   public:
    static void*
    allocate(size_t bytes) {
      return ge_stack_alloc(bytes);
    }

    static void
    free(void* ptr) {
      ge_stack_free(ptr);
    }
  };
}
