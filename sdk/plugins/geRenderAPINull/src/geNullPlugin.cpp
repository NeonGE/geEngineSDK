/*****************************************************************************/
/**
 * @file    geNullPlugin.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/02/17
 * @brief   Plugin entry point for the Graphics API Null Driver.
 *
 * Plugin entry point for the Graphics API Null Driver.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/

#include <gePrerequisitesCore.h>
#include "NullRenderAPI.h"

using namespace geEngineSDK;

extern "C" GE_PLUGIN_EXPORT void InitPlugin()
{
    //Create the RenderAPI instance
    RenderAPI::startUp<NullRenderAPI>();
}
