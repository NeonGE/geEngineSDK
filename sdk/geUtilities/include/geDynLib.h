/*****************************************************************************/
/**
 * @file    geDynLib.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/09/25
 * @brief   Class that holds data about a dynamic library.
 *
 * Class that holds data about a dynamic library.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"

#if USING(GE_PLATFORM_WINDOWS)
  struct HINSTANCE__;
  using hInstance = struct HINSTANCE__*;
#endif

#if USING(GE_PLATFORM_WINDOWS)
# define DYNLIB_HANDLE hInstance
# define DYNLIB_LOAD(a) LoadLibraryEx(a, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)
# define DYNLIB_GETSYM(a, b) GetProcAddress(a, b)
# define DYNLIB_UNLOAD(a) !FreeLibrary(a)

#elif USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_OSX)
# define DYNLIB_HANDLE void*
# define DYNLIB_LOAD(a) dlopen(a, RTLD_LAZY | RTLD_GLOBAL)
# define DYNLIB_GETSYM(a, b) dlsym(a, b)
# define DYNLIB_UNLOAD(a) dlclose(a)

#elif USING(GE_PLATFORM_PS4) || USING(GE_PLATFORM_PS5)
# define DYNLIB_HANDLE SceKernelModule
# define DYNLIB_LOAD(a, b, c, d, e, f) sceKernelLoadStartModule(a, b, c, d, e, f)
# define DYNLIB_GETSYM(a, b, c) sceKernelDlsym(a, b, c)
# define DYNLIB_UNLOAD(a, b, c, d, e, f) sceKernelStopUnloadModule(a, b, c, d, e, f)

#else
# error "No dynamic library loading functions defined for this platform."
#endif

namespace geEngineSDK {
  /**
   * @brief Class that holds data about a dynamic library.
   */
  class GE_UTILITIES_EXPORT DynLib final
  {
   public:
#if USING(GE_PLATFORM_LINUX)
    static CONSTEXPR const char* EXTENSION = "so";
    static CONSTEXPR const char* PREFIX = "lib";
#elif USING(GE_PLATFORM_OSX)
    static CONSTEXPR const char* EXTENSION = "dylib";
    static CONSTEXPR const char* PREFIX = "lib";
#elif USING(GE_PLATFORM_WINDOWS)
    static CONSTEXPR const char* EXTENSION = "dll";
    static CONSTEXPR const char* PREFIX = nullptr;
#elif USING(GE_PLATFORM_PS4) || USING(GE_PLATFORM_PS5)
    static CONSTEXPR const char* EXTENSION = "prx";
    static CONSTEXPR const char* PREFIX = "a";
#else
# error "No dynamic library loading functions defined for this platform."
#endif

    /**
     * @brief Constructs the dynamic library object and loads the library with
     *        the specified name.
     */
    DynLib(String name);
    ~DynLib();

    /**
     * @brief Loads the library. Does nothing if library is already loaded.
     */
    void
    load();

    /**
     * @brief Unloads the library. Does nothing if library is not loaded.
     */
    void
    unload();

    /**
     * @brief Get the name of the library.
     */
    const String&
    getName() const {
      return m_name;
    }

    /**
     * @brief Returns the address of the given symbol from the loaded library.
     * @param[in] strName The name of the symbol to search for.
     * @return  If the function succeeds, the returned value is a handle to the symbol.
     *          Otherwise null.
     */
    void*
    getSymbol(const String& strName) const;

   protected:
    friend class DynLibManager;

    /**
     * @brief Gets the last loading error.
     */
    static String
    dynlibError();

   protected:
    const String m_name;
    DYNLIB_HANDLE m_hInst = 0;  //Handle to the loaded library.
  };
}
