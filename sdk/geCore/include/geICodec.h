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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"
#include "geResource.h"

#include <geDebug.h>

namespace geEngineSDK {
  using std::function;

  namespace CODEC_TYPE {
    enum E {
      IMAGE = 0,      // Codec for images
      AUDIO = 1,      // Codec for audio files
      MODEL = 2,      // Codec for 3D models
      ANIMATION = 3,  // Codec for animations
    };
  }

  #define CODEC_TYPE_FN_NAME         "CodecType"
  #define CODEC_VERSION_FN_NAME      "CodecVersion"
  #define CODEC_NAME_FN_NAME         "CodecName"
  #define CODEC_DESCRIPTION_FN_NAME  "CodecDescription"
  #define CODEC_EXTENSIONS_FN_NAME   "CodecExtensions"
  #define CODEC_CANIMPORT_FN_NAME    "CodecCanImport"
  #define CODEC_CANEXPORT_FN_NAME    "CodecCanExport"
  #define CODEC_IMPORT_FN_NAME       "CodecImport"
  #define CODEC_EXPORT_FN_NAME       "CodecExport"

  using CodecTypeFn = CODEC_TYPE::E(void);
  using CodecTypePtr = CODEC_TYPE::E(*)(void);

  using CodecVersionFn = void(uint32& major, uint32& minor, uint32& patch);
  using CodecVersionPtr = void(*)(uint32& major, uint32& minor, uint32& patch);

  using CodecNameFn = const String&();
  using CodecNamePtr = const String&(*)(void);

  using CodecDescFn = const String&();
  using CodecDescPtr = const String& (*)(void);

  using CodecExtensionsFn = const Vector<String>&();
  using CodecExtensionsPtr = const Vector<String>& (*)(void);

  using CodecCanImportFn = bool(const Path& filePath);
  using CodecCanImportPtr = bool (*)(const Path& filePath);

  using CodecCanExportFn = bool(const Path& filePath);
  using CodecCanExportPtr = bool (*)(const Path& filePath);

  using CodecImportFn = void(const Path& filePath,
                             bool useCacheIfAvailable,
                             SPtr<Resource>& outRes);
  using CodecImportPtr = void(*)(const Path& filePath,
                                 bool useCacheIfAvailable,
                                 SPtr<Resource>& outRes);

  using CodecExportFn = bool(const SPtr<Resource>& resource, const Path& filePath);
  using CodecExportPtr = bool (*)(const SPtr<Resource>& resource, const Path& filePath);

  class GE_CORE_EXPORT ICodec
  {
   public:
    virtual ~ICodec() = default;

    bool
    initFromFile(const Path& dynFile);
    
    function<CodecTypeFn> getType;
    function<CodecVersionFn> getVersion;
    function<CodecNameFn> getName;
    function<CodecDescFn> getDescription;
    function<CodecExtensionsFn> getExtensions;
    function<CodecCanImportFn> canImport;
    function<CodecCanExportFn> canExport;
    function<CodecImportFn> importResource;
    function<CodecExportFn> exportResource;
  };

  GE_LOG_CATEGORY(ICodec, 700);
}
