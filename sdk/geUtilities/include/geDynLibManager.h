/*****************************************************************************/
/**
 * @file    geDynLibManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/06/04
 * @brief   This manager keeps track of all the open dynamic-loading libraries.
 *
 * This manager keeps track of all the open dynamic-loading libraries, opens
 * them and returns references to already-open libraries.
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
#include "gePrerequisitesUtilities.h"
#include "geModule.h"
#include "geDynLib.h"

namespace geEngineSDK {
  /**
   * @brief This manager keeps a track of all the open dynamic-loading libraries,
   *        opens them and returns references to already-open libraries.
   * @note  Not thread safe.
   */
  class GE_UTILITIES_EXPORT DynLibManager : public Module<DynLibManager>
  {
   public:
    /**
     * @brief Loads the given file as a dynamic library.
     * @param[in] name The name of the library.
     * @note The name parameter should only be the name of the library.
     *       Prefix and extension and Path will be deduced in this function.
     */
    DynLib*
    load(const String& logicalName);

    /**
     * @brief Unloads the given library.
     */
    void
    unload(DynLib* lib);

    String
    buildPlatformFilename(const String& logicalName);

   private:
     Path
     resolveOrThrow(const String& logicalName);

    Set<UPtr<DynLib>, std::less<>> m_loadedLibraries;
  };

  /** Easy way of accessing DynLibManager. */
  GE_UTILITIES_EXPORT DynLibManager&
  g_dynLibManager();
}
