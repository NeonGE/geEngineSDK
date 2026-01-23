/*****************************************************************************/
/**
 * @file    gePrerequisitesUtilities.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/09
 * @brief   Utility include with the most basic defines needed
 *
 * Utility include with the most basic defines needed for compilation
 * This is the file that must be included for use on external objects
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
#include <cassert>

/*****************************************************************************/
/**
 * GE_PROFILING_ENABLED - Enabled/Disable Profiling
 */
/*****************************************************************************/
#define GE_PROFILING_ENABLED	0

/*****************************************************************************/
/**
 * Version tracking constants
 */
/*****************************************************************************/
#define GE_VER_DEV      1           //Development Version
#define GE_VER_PREVIEW  2           //Preview Version
#define GE_VER          GE_VER_DEV  //This is the one that will be checked

/*****************************************************************************/
/**
 * Platform specific stuff
 */
/*****************************************************************************/
#include "gePlatformDefines.h"

/*****************************************************************************/
/**
 * Include of short-hand names for various built-in types
 */
/*****************************************************************************/
#include "gePlatformTypes.h"        //Platform types
#include "geMacroUtil.h"            //Utility Macros
#include "geMemoryAllocator.h"      //Memory Allocator
#include "geThreading.h"            //Threading Defines
#include "geStdHeaders.h"           //Commonly used standard headers

#include "geFwdDeclUtil.h"          //Forward declarations

#include "geString.h"               //String objects
#include "geMessageHandlerFwd.h"    //Forward declarations of the message objects
#include "geFlags.h"                //Flags template
#include "geUtil.h"                 //Hashing utilities
#include "geEvent.h"                //Event objects and handlers
#include "gePlatformUtility.h"      //Utilities for the specific operating system
#include "geNonCopyable.h"          //Interface for Non-copyable objects
#include "geSmallVector.h"          //Small vector
#include "gePath.h"                 //Paths to files
#include "geCrashHandler.h"         //Crash handling functions
