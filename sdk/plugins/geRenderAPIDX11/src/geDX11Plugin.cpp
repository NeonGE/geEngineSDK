/*****************************************************************************/
/**
 * @file    geDX11Plugin.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/29
 * @brief   Plugin entry point for the Graphics API.
 *
 * Plugin entry point for the Graphics API.
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
#include "DX11RenderAPI.h"

using namespace geEngineSDK;

extern "C" GE_PLUGIN_EXPORT void InitPlugin()
{
    //Create the RenderAPI instance
    RenderAPI::startUp<DX11RenderAPI>();
}
