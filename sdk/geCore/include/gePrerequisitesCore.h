/*****************************************************************************/
/**
 * @file    gePrerequisitesCore.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/09
 * @brief   Utility include with defines needed for the Core
 *
 * Minimum includes to use the core
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Most basic includes (required for everything else)
 */
/*****************************************************************************/
#include <gePrerequisitesUtilities.h>
#include "geCoreFeatures.h"

/*****************************************************************************/
/**
 * Library export specifics
 */
/*****************************************************************************/
#if USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
# if USING(GE_COMPILER_MSVC)
#   if defined( GE_STATIC_LIB )
#     define GE_CORE_EXPORT
#   else
#     if defined( GE_CORE_EXPORTS )
#       define GE_CORE_EXPORT __declspec( dllexport )
#     else
#       define GE_CORE_EXPORT __declspec( dllimport )
#     endif
#   endif
# else  //Any other Compiler
#   if defined( GE_STATIC_LIB )
#     define GE_CORE_EXPORT
#   else
#     if defined( GE_CORE_EXPORTS )
#       define GE_CORE_EXPORT __attribute__ ((dllexport))
#     else
#       define GE_CORE_EXPORT __attribute__ ((dllimport))
#     endif
#   endif
# endif
# define GE_CORE_HIDDEN
#else //Linux/Mac settings
# define GE_CORE_EXPORT __attribute__ ((visibility ("default")))
# define GE_CORE_HIDDEN __attribute__ ((visibility ("hidden")))
#endif

namespace geEngineSDK {
	/**
	 * Default thread policy for the framework.
	 * Performs special startup/shutdown on threads managed by thread pool.
	 */
	class GE_CORE_EXPORT ThreadDefaultPolicy
	{
	 public:
		static void onThreadStarted(const String& /*name*/) {
			MemStack::beginThread();
		}

		static void onThreadEnded(const String& /*name*/) {
			MemStack::endThread();
		}
	};
}
