/*****************************************************************************/
/**
* @file   geMemStack.cpp
* @author Samuel Prince (samuel.prince.quezada@gmail.com)
* @date   2015/02/16
* @brief  Classes used to create our own Memory Stacks
*
* This file defines all the needed classes to create a Memory Stack.
* Normally it's used in combination with STL objects.
*
* @bug	   No known bugs.
*/
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geStackAlloc.h"

namespace geEngineSDK {
  GE_THREADLOCAL MemStackInternal<1024 * 1024>* MemStack::threadMemStack = nullptr;

  void
  MemStack::beginThread() {
    if (nullptr != threadMemStack) {
      endThread();
    }

    threadMemStack = ge_new<MemStackInternal<1024 * 1024>>();
  }

  void
  MemStack::endThread() {
    if (nullptr != threadMemStack) {
      ge_delete(threadMemStack);
      threadMemStack = nullptr;
    }
  }

  byte*
  MemStack::alloc(SIZE_T numBytes) {
    GE_ASSERT(nullptr != threadMemStack &&
              "Stack allocation failed. Did you call BeginThread?");
    return threadMemStack->alloc(numBytes);
  }

  void
  MemStack::deallocLast(byte* data) {
    GE_ASSERT(nullptr != threadMemStack&&
              "Stack deallocation failed. Did you call BeginThread?");
    threadMemStack->dealloc(data);
  }
}
