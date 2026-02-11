/*****************************************************************************/
/**
 * @file    geICodec.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/17
 * @brief   Interface of a Codec.
 *
 * Interface of a Codec. A codec is a module that can import and/or export
 * resources from and to a specific format.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geICodec.h"

#include <geDynLibManager.h>

#include <functional>

namespace geEngineSDK {
  GE_LOG_CATEGORY_IMPL(ICodec);

  bool
  ICodec::initFromFile(const Path& dynFile) {
    GE_ASSERT(g_dynLibManager().isStarted());

    //Load the codec dynamically
    auto codec = g_dynLibManager().load(dynFile.toString());
    if (!codec) {
      GE_LOG(kError, ICodec, "Failed to load codec from {0}", dynFile.toString());
      return false;
    }

    //Get the codec functions
    getType = cast::re<CodecTypePtr>(codec->getSymbol(CODEC_TYPE_FN_NAME));
    getVersion = cast::re<CodecVersionPtr>(codec->getSymbol(CODEC_VERSION_FN_NAME));
    getName = cast::re<CodecNamePtr>(codec->getSymbol(CODEC_NAME_FN_NAME));
    getDescription = cast::re<CodecDescPtr>(codec->getSymbol(CODEC_DESCRIPTION_FN_NAME));
    getExtensions = cast::re<CodecExtensionsPtr>(codec->getSymbol(CODEC_EXTENSIONS_FN_NAME));
    canImport = cast::re<CodecCanImportPtr>(codec->getSymbol(CODEC_CANIMPORT_FN_NAME));
    canExport = cast::re<CodecCanExportPtr>(codec->getSymbol(CODEC_CANEXPORT_FN_NAME));
    importResource = cast::re<CodecImportPtr>(codec->getSymbol(CODEC_IMPORT_FN_NAME));
    exportResource = cast::re<CodecExportPtr>(codec->getSymbol(CODEC_EXPORT_FN_NAME));

    if(getType == nullptr || getVersion == nullptr || getName == nullptr ||
       getDescription == nullptr || getExtensions == nullptr || canImport == nullptr ||
       canExport == nullptr || importResource == nullptr || exportResource == nullptr) {
      GE_LOG(kError, ICodec, "Failed to load codec functions from {0}", dynFile.toString());
      return false;
    }

    return true;
  }

}
