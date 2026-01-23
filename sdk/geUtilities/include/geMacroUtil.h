/*****************************************************************************/
/**
 * @file    geMacroUtil.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/16
 * @brief   Some utility macros defined for use on the libraries.
 *
 * Some utility macros defined for use on the libraries.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Macro utilities
 */
/*****************************************************************************/
#if USING(GE_COMPILER_MSVC)
# define MULTI_LINE_MACRO_BEGIN do {
# define MULTI_LINE_MACRO_END         \
    __pragma(warning(push))           \
    __pragma(warning(disable:4127))   \
    } while(0)                        \
    __pragma(warning(pop))
#else
# define MULTI_LINE_MACRO_BEGIN do {
# define MULTI_LINE_MACRO_END } while(0)
#endif

/*****************************************************************************/
/**
 * Parameter utilities.
 * There is a UNREFERENCED_PARAMETER in WinNT, but is not compliant
 */
/*****************************************************************************/
#ifndef GE_UNREFERENCED_PARAMETER
# define GE_UNREFERENCED_PARAMETER(P) (void)P
#endif

/*****************************************************************************/
/**
 * Offset of a member in a struct
 * @param member Member to get the offset of
 * @return Offset of the member in the struct
 */
/*****************************************************************************/
template<typename T, typename U>
size_t
offsetOf(U T::* member) {
  return reinterpret_cast<size_t>(&(reinterpret_cast<T*>(0)->*member));
}

/*****************************************************************************/
/**
 * Safe release of a COM pointer.
 * @param ptr Pointer to the COM object to release
 */
 /*****************************************************************************/
template<typename T>
void
safeRelease(T*& ptr) {
  if (ptr) {
    ptr->Release();
    ptr = nullptr;
  }
}

namespace cast {
  template<typename To, typename From>
  FORCEINLINE To re(From&& from) {
    return reinterpret_cast<To>(from);
  }

  template<typename To, typename From>
  FORCEINLINE To st(From&& from) {
    return static_cast<To>(from);
  }
}
