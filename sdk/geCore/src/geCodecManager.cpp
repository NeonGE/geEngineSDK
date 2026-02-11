/*****************************************************************************/
/**
 * @file    geCodecManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/17
 * @brief   Codec Manager System.
 *
 * Codec Manager System. This system is responsible for managing codecs that
 * load and convert resources from various formats into the engine's resource
 * format. It provides a way to register codecs and handle their conversion.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geCodecManager.h"
#include "geICodec.h"

#include <geDynLibManager.h>
#include <geFileSystem.h>

namespace geEngineSDK {
  
  GE_LOG_CATEGORY_IMPL(CodecManager);

  void
  CodecManager::onStartUp() {
    _loadCodecs();
  }

  void
  CodecManager::onShutDown() {
    m_codecs.clear();
  }

  const SPtr<ICodec>
  CodecManager::getImportCodec(const CODEC_TYPE::E type, const String& fileExt) {
    for (const auto& codec : m_codecs) {
      if (codec->getType() == type && codec->canImport(fileExt)) {
        return codec;
      }
    }

    GE_LOG(kError,
           CodecManager,
           "No codec found for type {0} and extension {1}",
           CODEC_TYPE::E(type),
           fileExt);

    return nullptr;
  }

  const SPtr<ICodec>
  CodecManager::getExportCodec(const CODEC_TYPE::E type, const String& fileExt) {
    for (const auto& codec : m_codecs) {
      if (codec->getType() == type && codec->canExport(fileExt)) {
        return codec;
      }
    }
    GE_LOG(kError,
           CodecManager,
           "No codec found for type {0} and extension {1}",
           CODEC_TYPE::E(type),
           fileExt);
    return nullptr;
  }

  void
  CodecManager::_loadCodecs() {
    GE_ASSERT(g_dynLibManager().isStarted());

    //Load codecs from the plugins directory
    Path pluginsDir = FileSystem::getPluginsPath();
    
    if (pluginsDir.isDirectory()) {
      Vector<Path> files;
      Vector<Path> dirs;
      FileSystem::getChildren(pluginsDir, files, dirs);

      for (const auto& codecFile : files) {
        if(!StringUtil::match(codecFile.getExtension(), ".DLL", false)) {
          continue; // Skip non-dll files
        }

        auto fileName = codecFile.getFilename();
        StringUtil::toUpperCase(fileName);
        if (fileName.find("CODEC") == String::npos) {
          continue; // Skip non codec files
        }

        SPtr<ICodec> tmpCodec = ge_shared_ptr_new<ICodec>();
        tmpCodec->initFromFile(codecFile);

        //Register the codec and it's extensions
        m_codecs.push_back(tmpCodec);
      }
    }
    else {
      GE_LOG(kWarning,
             CodecManager,
             "Plugins directory not found: {0}", pluginsDir.toString());
    }
  }

}
