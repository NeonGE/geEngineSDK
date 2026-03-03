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
#include "geFileSystem.h"

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
  DynLibManager::load(const String& logicalName) {
    //If the library is already loaded
    const auto& iterFind = m_loadedLibraries.lower_bound(logicalName);
    if (iterFind != m_loadedLibraries.end() && (*iterFind)->getName() == logicalName) {
      return iterFind->get(); //Return the handle
    }

    String candidate = logicalName;

    const bool looksLikePath = candidate.find('/') != String::npos ||
                               candidate.find('\\') != String::npos;

    Path resolvedPath;
    if (looksLikePath) {
      resolvedPath = Path(candidate);
    }
    else {
      const String platformFile = buildPlatformFilename(candidate);
      resolvedPath = resolveOrThrow(platformFile);
    }

    auto newLib = ge_new<DynLib>(logicalName);
    newLib->loadFromFile(resolvedPath);
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

  String
  DynLibManager::buildPlatformFilename(const String& logicalName) {
    String filename = logicalName;

    //Check if the extension has already been sent
    const String ext = DynLib::EXTENSION;
    if (filename.length() <= ext.length() ||
        filename.substr(filename.length() - ext.length()) != ext) {
      filename.append(ext);
    }

    //Add prefix only it it's missing
    IF_CONSTEXPR(nullptr != DynLib::PREFIX) {
      const String prefix = DynLib::PREFIX;
      if (filename.length() < prefix.length() ||
          filename.substr(0, prefix.length()) != prefix) {
        filename.insert(0, prefix);
      }
    }

    return filename;
  }

  Path
  DynLibManager::resolveOrThrow(const String& logicalName) {
    //Here we use the filesystem to check if/where the file exists
    
    //The first point to check will be the Plugins folder
    Path testPath = FileSystem::getPluginsPath() + logicalName;
    if (FileSystem::exists(testPath)) {
      return testPath;
    }

    //Second test point is the App directory
    testPath = FileSystem::getWorkingDirectoryPath() + logicalName;
    if (FileSystem::exists(testPath)) {
      return testPath;
    }

    //If we havent found the file, send an exception
    GE_EXCEPT(FileNotFoundException,
      "Could not resolve dynamic library '" + logicalName +
      "'. Provide a resolver (VFS) or implement direct file existence checks.");

    GE_UNREACHABLE();
  }

  DynLibManager&
  g_dynLibManager() {
    return DynLibManager::instance();
  }
}
