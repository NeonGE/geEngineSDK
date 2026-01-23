/*****************************************************************************/
/**
 * @file    geFreeAlloc.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/02/17
 * @brief   Free allocator with no limitations.
 *
 * Free allocator with no limitations, using traditional malloc/free under the
 * hood.
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
namespace geEngineSDK {
  /**
   * @brief Free allocator with no limitations, using traditional malloc/free
   *        under the hood.
   */
  class FreeAlloc
  {
   public:
    /**
     * @brief Allocates memory.
     */
    byte*
    alloc(SIZE_T amount) {
      return reinterpret_cast<byte*>(malloc(amount));
    }

    /**
     * @brief Deallocates a previously allocated piece of memory.
     */
    void
    free(void* data) {
      ::free(data);
    }

    /**
     * @brief Unused
     */
    void
    clear() {
      //Do nothing
    }
  };
}
