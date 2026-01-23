/*****************************************************************************/
/**
 * @file    geMemAllocProfiler.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/16
 * @brief   Specialized allocator for the profiler.
 *
 * Specialized allocator for profiler so we can avoid tracking internal
 * profiler memory allocations which would skew profiler results.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

namespace geEngineSDK {
  /**
   * @brief Specialized allocator for profiler so we can avoid tracking internal
   *        profiler memory allocations which would skew profiler results.
   */
  class ProfilerAlloc
  {};

  /**
   * @brief Memory allocator providing a generic implementation.
   *        Specialize for specific categories as needed.
   */
  template<>
  class MemoryAllocator<ProfilerAlloc> : public MemoryAllocatorBase
  {
   public:
    /**
     * @brief Allocates the given number of bytes.
     */
    static void*
    allocate(SIZE_T bytes) {
      return malloc(bytes);
    }

    /**
     * @brief Frees memory previously allocated with "allocate".
     */
    static void
    free(void* ptr) {
      ::free(ptr);
    }
  };
}
