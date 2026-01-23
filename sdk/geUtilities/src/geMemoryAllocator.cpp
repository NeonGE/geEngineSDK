/*****************************************************************************/
/**
 * @file    geMemoryAllocator.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/16
 * @brief   Memory allocation objects and templates
 *
 * Define templates and classes used to allocate memory and create object on
 * the heap dynamically. Also defines StdAlloc used to define the way STL
 * objects will allocate their objects.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"

namespace geEngineSDK {
  GE_THREADLOCAL uint64_t MemoryCounter::m_allocs = 0;
  GE_THREADLOCAL uint64_t MemoryCounter::m_frees = 0;
}
