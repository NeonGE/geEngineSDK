/*****************************************************************************/
/**
 * @file    geDynLibManager.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geDynLibManager.h"
#include "geDynLib.h"

namespace geEngineSDK {
  using std::move;

  static bool
  operator<(const UPtr<DynLib>& lhs, const String& rhs) {
    return lhs->getName() < rhs;
  }

  static bool
  operator<(const String& lhs, const UPtr<DynLib>& rhs) {
    return lhs < rhs->getName();
  }

  static bool
  operator<(const UPtr<DynLib>& lhs, const UPtr<DynLib>& rhs) {
    return lhs->getName() < rhs->getName();
  }

  DynLib*
  DynLibManager::load(const String& name) {
    //Add the extension (.dll, .so, ...) if necessary.
    String filename = name;
    const SIZE_T length = filename.length();
    const String extension = String(".") + DynLib::EXTENSION;
    const SIZE_T extLength = extension.length();

    if (length <= extLength || filename.substr(length - extLength) != extension) {
      filename.append(extension);
    }

    IF_CONSTEXPR (nullptr != DynLib::PREFIX) {
      filename.insert(0, DynLib::PREFIX);
    }

    const auto& iterFind = m_loadedLibraries.lower_bound(filename);
    if (iterFind != m_loadedLibraries.end() && (*iterFind)->getName() == filename) {
      return iterFind->get();
    }

    auto newLib = ge_new<DynLib>(std::move(filename));
    m_loadedLibraries.emplace_hint(iterFind, newLib);
    return newLib;
  }

  void
  DynLibManager::unload(DynLib* lib) {
    const auto& iterFind = m_loadedLibraries.find(lib->getName());
    if (iterFind != m_loadedLibraries.end()) {
      m_loadedLibraries.erase(iterFind);
    }
    else {
      ge_delete(lib);
    }
  }

  DynLibManager&
  g_dynLibManager() {
    return DynLibManager::instance();
  }
}
