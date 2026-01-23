/*****************************************************************************/
/**
 * @file    gePlatformDefines.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/09
 * @brief   Some definitions to describe the target platform
 *
 * This definitions are meant to describe the target platform
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

#include "gePlatformUsing.h"

/*****************************************************************************/
/**
 * Initial platform/compiler-related stuff to set.
 */
/*****************************************************************************/
#if defined(_WIN32) || defined(_WIN64)
#   define GE_PLATFORM_WINDOWS   IN_USE
#else
#   define GE_PLATFORM_WINDOWS   NOT_IN_USE
#endif

#if defined(__APPLE__) && defined(__MACH__)
#   include <TargetConditionals.h>
#   if TARGET_OS_MAC && !TARGET_OS_IPHONE
#     define GE_PLATFORM_OSX     IN_USE
#   else
#     define GE_PLATFORM_OSX     NOT_IN_USE
#   endif
#   if TARGET_OS_IPHONE
#     define GE_PLATFORM_IOS     IN_USE
#   else
#     define GE_PLATFORM_IOS     NOT_IN_USE
#   endif
#else
#   define GE_PLATFORM_OSX       NOT_IN_USE
#   define GE_PLATFORM_IOS       NOT_IN_USE
#endif

#if defined(__ANDROID__)
#   define GE_PLATFORM_ANDROID   IN_USE
#else
#   define GE_PLATFORM_ANDROID   NOT_IN_USE
#endif

#if defined(__linux__) && !defined(__ANDROID__)
#   define GE_PLATFORM_LINUX     IN_USE
#else
#   define GE_PLATFORM_LINUX     NOT_IN_USE
#endif

#if defined(__ORBIS__)
#   define GE_PLATFORM_PS4       IN_USE
#else
#   define GE_PLATFORM_PS4       NOT_IN_USE
#endif

#if defined(__PROSPERO__)
#   define GE_PLATFORM_PS5       IN_USE
#else
#   define GE_PLATFORM_PS5       NOT_IN_USE
#endif

#if defined(_DURANGO) || defined(_XBOX_ONE) || defined(_GAMING_XBOX)
#   define GE_PLATFORM_XBOX      IN_USE
#else
#   define GE_PLATFORM_XBOX      NOT_IN_USE
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#   define GE_COMPILER_MSVC      IN_USE
#else
#   define GE_COMPILER_MSVC      NOT_IN_USE
#endif

#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#   define GE_COMPILER_GNUC      IN_USE
#else
#   define GE_COMPILER_GNUC      NOT_IN_USE
#endif

#if defined(__INTEL_COMPILER)
#   define GE_COMPILER_INTEL     IN_USE
#else
#   define GE_COMPILER_INTEL     NOT_IN_USE
#endif

#if defined(__clang__)
#   define GE_COMPILER_CLANG     IN_USE
#else
#   define GE_COMPILER_CLANG     NOT_IN_USE
#endif

/*****************************************************************************/
/**
 * Check for C++11 support
 */
/*****************************************************************************/
#if USING(GE_COMPILER_MSVC)
#   define GE_CPP11_OR_LATER    USE_IF(_MSVC_LANG >= 201103L)
#   define GE_CPP14_OR_LATER    USE_IF(_MSVC_LANG >= 201402L)
#   define GE_CPP17_OR_LATER    USE_IF(_MSVC_LANG >= 201703L)
#   define GE_CPP20_OR_LATER    USE_IF(_MSVC_LANG >= 202002L)
#else
#   define GE_CPP11_OR_LATER    USE_IF(__cplusplus >= 201103L)
#   define GE_CPP14_OR_LATER    USE_IF(__cplusplus >= 201402L)
#   define GE_CPP17_OR_LATER    USE_IF(__cplusplus >= 201703L)
#   define GE_CPP20_OR_LATER    USE_IF(__cplusplus >= 202002L)
#endif

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#   define GE_ENDIAN_LITTLE    IN_USE
#   define GE_ENDIAN_BIG       NOT_IN_USE
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#   define GE_ENDIAN_LITTLE    NOT_IN_USE
#   define GE_ENDIAN_BIG       IN_USE
#elif USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
#   define GE_ENDIAN_LITTLE    IN_USE
#   define GE_ENDIAN_BIG       NOT_IN_USE
#else
  //If it's not a known platform, check the byte order to determine endianness
  #include <cstdint>

  inline bool
  isLittleEndian() {
    uint16_t number = 0x1;
    return *reinterpret_cast<uint8_t*>(&number) == 0x1;
  }

  #if isLittleEndian()
  #   define GE_ENDIAN_LITTLE    IN_USE
  #   define GE_ENDIAN_BIG       NOT_IN_USE
  #else
  #   define GE_ENDIAN_LITTLE    NOT_IN_USE
  #   define GE_ENDIAN_BIG       IN_USE
  #endif
#endif

/*****************************************************************************/
/**
 *Compiler type and version.
 */
/*****************************************************************************/
#if USING(GE_COMPILER_CLANG)
#   define GE_COMP_VER    __clang_version__
#   define GE_THREADLOCAL __thread
#   define GE_STDCALL     __attribute__((stdcall))
#   define GE_CDECL       __attribute__((cdecl))
#   define GE_FALLTHROUGH [[clang::fallthrough]]

#elif USING(GE_COMPILER_GNUC) //Check after Clang, as Clang also defines __GNUC__
#   define GE_COMP_VER    (((__GNUC__)*100) + (__GNUC_MINOR__*10) + __GNUC_PATCHLEVEL__)
#   define GE_THREADLOCAL __thread
#   define GE_STDCALL     __attribute__((stdcall))
#   define GE_CDECL       __attribute__((cdecl))
#   define GE_FALLTHROUGH __attribute__((fallthrough))

#elif USING(GE_COMPILER_INTEL)
#   define GE_COMP_VER    __INTEL_COMPILER
#   define GE_STDCALL     __stdcall
#   define GE_CDECL       __cdecl
#   define GE_FALLTHROUGH
    /** 
     * GE_THREADLOCAL is defined based on platform because Intel compiler
     * handles it differently.
     */
#   if USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
#     define GE_THREADLOCAL __declspec(thread)  // Windows uses __declspec(thread)
#   else
#     define GE_THREADLOCAL __thread            // Unix-based systems use __thread
#   endif

#elif USING(GE_COMPILER_MSVC)
#   define GE_COMP_VER _MSC_VER
#   define GE_THREADLOCAL __declspec(thread)
#   define GE_STDCALL     __stdcall
#   define GE_CDECL       __cdecl
#   define GE_FALLTHROUGH
#   undef __PRETTY_FUNCTION__
#   define __PRETTY_FUNCTION__ __FUNCSIG__

#else
#   error "No known compiler detected."
#endif

/*****************************************************************************/
/**
 * See if we can use __forceinline or if we need to use __inline instead
 */
/*****************************************************************************/
#if USING(GE_COMPILER_MSVC)                   //If compiling on Visual Studio
#   undef FORCEINLINE
#   if GE_COMP_VER >= 1200                    //Visual Studio 6 or higher
#     define FORCEINLINE __forceinline        //Use __forceinline
#     ifndef RESTRICT
#       define RESTRICT __restrict            //No alias hint
#     endif
#   else
#     define FORCEINLINE __inline             //Use __inline
#     ifndef RESTRICT
#       define RESTRICT __restrict            //No alias hint
#     endif
#   endif
#elif USING(GE_COMPILER_GNUC) || USING(GE_COMPILER_CLANG) //GCC or Clang
#   if !defined(FORCEINLINE)
#     define FORCEINLINE __inline__           //Use __inline__ for GCC/Clang
#     ifndef RESTRICT
#       define RESTRICT __restrict            //No alias hint
#     endif
#   endif
#elif USING(GE_COMPILER_INTEL)                //If compiling on Intel Compiler
#   if !defined(FORCEINLINE)
#     define FORCEINLINE __forceinline        //Use __forceinline for Intel
#     ifndef RESTRICT
#       define RESTRICT __restrict            //No alias hint
#     endif
#   endif
#else                                         //Any other compiler
#   define FORCEINLINE inline                 //Use inline as default
#   ifndef RESTRICT
#     define RESTRICT                         //No alias hint available
#   endif
#endif

/*****************************************************************************/
/**
 * Find the architecture type
 */
/*****************************************************************************/
#define GE_ARCHITECTURE_x86_32 NOT_IN_USE  // Intel x86 32-bit
#define GE_ARCHITECTURE_x86_64 NOT_IN_USE  // Intel x86 64-bit
#define GE_ARCHITECTURE_ARM_32 NOT_IN_USE  // ARM 32-bit
#define GE_ARCHITECTURE_ARM_64 NOT_IN_USE  // ARM 64-bit

//Detect x86 64-bit (Windows, macOS, PS4, PS5, and Xbox)
#if defined(_M_X64) || defined(__x86_64__) || defined(__amd64__) ||           \
    USING(GE_PLATFORM_PS4) || USING(GE_PLATFORM_PS5) ||                       \
    USING(GE_PLATFORM_XBOX)
#   undef GE_ARCHITECTURE_x86_64
#   define GE_ARCHITECTURE_x86_64 IN_USE

//Detect x86 32-bit (for older systems or emulators)
#elif defined(_M_IX86) || defined(__i386__)
#   undef GE_ARCHITECTURE_x86_32
#   define GE_ARCHITECTURE_x86_32 IN_USE

//Detect ARM 64-bit (iOS, Android, macOS on Apple Silicon)
#elif defined(_M_ARM64) || defined(__aarch64__) ||                            \
      (defined(__APPLE__) && defined(TARGET_OS_MAC) && defined(__aarch64__))
#   undef GE_ARCHITECTURE_ARM_64
#   define GE_ARCHITECTURE_ARM_64 IN_USE

//Detect ARM 32-bit (for older ARM devices or Android emulators)
#elif defined(_M_ARM) || defined(__arm__)
#   undef GE_ARCHITECTURE_ARM_32
#   define GE_ARCHITECTURE_ARM_32 IN_USE
#endif

//Verify configurations (optional)
#if (USING(GE_ARCHITECTURE_x86_32) + USING(GE_ARCHITECTURE_x86_64) +          \
     USING(GE_ARCHITECTURE_ARM_32) + USING(GE_ARCHITECTURE_ARM_64)) != 1
#error "No Architecture or Multiple defined simultaneously."
#endif

/*****************************************************************************/
/**
 * Memory Alignment macros
 */
/*****************************************************************************/
#if USING(GE_COMPILER_MSVC)
#   define ALIGN_AS(n)  __declspec(align(n))
#elif USING(GE_COMPILER_GNUC) || USING(GE_COMPILER_CLANG)
#   define ALIGN_AS(n) __attribute__((aligned(n)))
#endif

#ifndef CONSTEXPR
#   if USING(GE_CPP11_OR_LATER)
#     define CONSTEXPR constexpr
#   else
#     define CONSTEXPR
#   endif
#endif

#if USING(GE_CPP17_OR_LATER)
#   define IF_CONSTEXPR if constexpr
#else
#   define IF_CONSTEXPR if
#endif

/*****************************************************************************/
/**
 * For throw override
 */
/*****************************************************************************/
#if USING(GE_COMPILER_MSVC) || USING(GE_COMPILER_INTEL) || USING(GE_COMPILER_GNUC)
#   if USING(GE_CPP11_OR_LATER)
#     define _NOEXCEPT noexcept
#   else
#     define _NOEXCEPT
#   endif
#else
#   ifndef _NOEXCEPT
#     define _NOEXCEPT
#   endif
#endif

/*****************************************************************************/
/**
 * Use before a function declaration to warn that callers should not ignore the
 * return value.
 */
/*****************************************************************************/
#if !defined(GE_NODISCARD) && defined(__has_cpp_attribute)
#   if USING(GE_CPP17_OR_LATER)
#     if __has_cpp_attribute(nodiscard)
#       define GE_NODISCARD [[nodiscard]]
#     endif
#   endif
#endif

#ifndef GE_NODISCARD
#   define GE_NODISCARD
#endif

/*****************************************************************************/
/**
 * Use before a function declaration to indicate that the function never returns.
 */
/*****************************************************************************/
#if !defined(GE_NORETURN) && defined(__has_cpp_attribute)
#   if USING(GE_CPP11_OR_LATER)
#     if __has_cpp_attribute(noreturn)
#       define GE_NORETURN [[noreturn]]
#     endif
#   endif
#endif

#ifndef GE_NORETURN
# define GE_NORETURN
#endif

/*****************************************************************************/
/**
 * Library export specifics
 */
/*****************************************************************************/
#if USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
#   if USING(GE_COMPILER_MSVC)
#     if defined( GE_STATIC_LIB )
#       define GE_UTILITIES_EXPORT
#     else
#       if defined( GE_UTILITIES_EXPORTS )
#         define GE_UTILITIES_EXPORT __declspec( dllexport )
#       else
#         define GE_UTILITIES_EXPORT __declspec( dllimport )
#       endif
#     endif
#   else  //Any other Compiler
#     if defined( GE_STATIC_LIB )
#       define GE_UTILITIES_EXPORT
#     else
#       if defined( GE_UTILITIES_EXPORTS )
#         define GE_UTILITIES_EXPORT __attribute__ ((dllexport))
#       else
#         define GE_UTILITIES_EXPORT __attribute__ ((dllimport))
#       endif
#     endif
#   endif
#   define GE_UTILITIES_HIDDEN
#elif USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_OSX) ||\
      USING(GE_PLATFORM_PS4) || USING(GE_PLATFORM_PS5) ||\
      USING(GE_PLATFORM_ANDROID) || USING(GE_PLATFORM_IOS)
    //Unix-like systems (Linux, macOS, PS4, PS5, Android, IOS)
#   if defined(GE_STATIC_LIB)
#     define GE_UTILITIES_EXPORT
#   else
#     define GE_UTILITIES_EXPORT __attribute__((visibility("default")))
#   endif
#   define GE_UTILITIES_HIDDEN __attribute__((visibility("hidden")))
#else
#   error "Unsupported platform detected for GE_UTILITIES_EXPORT."
#endif

//DLL export for plug ins
#if USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
#   if USING(GE_COMPILER_MSVC)
#     define GE_PLUGIN_EXPORT __declspec(dllexport)
#   else
#     define GE_PLUGIN_EXPORT __attribute__ ((dllexport))
#   endif
#elif USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_OSX) ||\
      USING(GE_PLATFORM_PS4) || USING(GE_PLATFORM_PS5) ||\
      USING(GE_PLATFORM_ANDROID) || USING(GE_PLATFORM_IOS)
    //Unix-like systems (Linux, macOS, PS4, PS5, Android, IOS)
#   define GE_PLUGIN_EXPORT __attribute__ ((visibility ("default")))
#endif

// Determine debug mode across all platforms
#if defined(_DEBUG) || defined(DEBUG)
#   define GE_DEBUG_MODE IN_USE      //Specifies that we are in a DEBUG build
#else
#   define GE_DEBUG_MODE NOT_IN_USE  //Specifies that we are NOT in a DEBUG build
#endif

/*****************************************************************************/
/**
 * Unix-like systems specific Settings
 */
/*****************************************************************************/
#if USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_OSX) ||\
    USING(GE_PLATFORM_PS4) || USING(GE_PLATFORM_PS5) ||\
    USING(GE_PLATFORM_ANDROID) || USING(GE_PLATFORM_IOS)
#   define stricmp strcasecmp
#endif

/*****************************************************************************/
/**
 * Definition of Debug macros
 */
/*****************************************************************************/
#if USING(GE_DEBUG_MODE)
#   define GE_DEBUG_ONLY(x) x
#   define GE_ASSERT(x) assert(x)
#else
#   define GE_DEBUG_ONLY(x)
#   define GE_ASSERT(x)
#endif

/*****************************************************************************/
/**
 * Helper macros used for migration of c++17 to c++20
 */
/*****************************************************************************/
#define U8STRING(x) x

/*****************************************************************************/
/**
 * Disable some compiler warnings
 */
/*****************************************************************************/

//If we are compiling with Visual Studio
#if USING(GE_COMPILER_MSVC)
  /**
   * TODO:  This is not deactivated anywhere, therefore it applies to any file
   * that includes this header. Right now I don't have an easier way to apply
   * these warnings globally so I'm keeping it this way.
   */

  //Secure versions aren't multi platform, so we won't be using them
# define _CRT_SECURE_NO_WARNINGS

  /**
   * Disable: nonstandard extension used: nameless struct/union.
   * Happens when a struct or union is defined without a name.
   */
# pragma warning(disable : 4201)

  /**
   * Disable: "<type> needs to have DLL interface to be used by clients'
   * Happens on STL member variables which are not public therefore is ok
   */
# pragma warning (disable: 4251)

  //Disable: 'X' Function call with parameters that may be unsafe
# pragma warning(disable: 4996) 

  /**
   * Disable: decorated name length exceeded, name was truncated. Happens with
   * really long type names. Even fairly standard use of std::unordered_map
   * with custom parameters, meaning I can't really do much to avoid it.
   * It shouldn't effect execution but might cause problems if you compile
   * library with one compiler and use it in another.
   */
# pragma warning(disable: 4503)

  /**
   * Disable: C++ exception handler used, but unwind semantics are not enabled
   * We don't care about this as any exception is meant to crash the program.
   */
#	pragma warning(disable: 4530)

#	pragma warning(disable: 4577)

#if USING(GE_CPP14_OR_LATER)
#   pragma warning(disable: 4127)
#endif

#endif

/*****************************************************************************/
/**
 * Engine version
 */
/*****************************************************************************/
#define GE_VERSION_MAJOR 0                    //Engine version mayor value
#define GE_VERSION_MINOR 5                    //Engine version minor value
#define GE_VERSION_PATCH 0                    //Engine version patch value
#define GE_EDITOR_BUILD 1                     //This is an Editor Build

//Define if on a crash we want to report warnings on unknown symbols
#define GE_DEBUG_DETAILED_SYMBOLS IN_USE

//Features implementations
#define GE_FEATURE_THREADING IN_USE

#define GE_USE_GENERIC_FILESYSTEM     USE_IF(!USING(GE_PLATFORM_WINDOWS) && USING(GE_CPP17_OR_LATER))
/*****************************************************************************/
