/*****************************************************************************/
/**
 * @file    geCodecDLL.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/18
 * @brief   Plugin entry point for the Codec.
 *
 * Plugin entry point for the Codec.
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
#include <geICodec.h>
#include <geRenderAPI.h>
#include <geMountManager.h>

#include "geEXRLoader.h"

using namespace geEngineSDK;

static String CODEC_NAME = "EXR Image Codec";
static String CODEC_DESC = "This codec implements the TinyEXR Library "
                           "and expose it as a codec";
static Vector<String> CODEC_EXTENSIONS_IMPORT = {
  ".exr"
};

static Vector<String> CODEC_EXTENSIONS_EXPORT = {
  ".exr"
};

extern "C"
{
  GE_PLUGIN_EXPORT CODEC_TYPE::E
  CodecType(void) {
    return CODEC_TYPE::IMAGE;
  }

  GE_PLUGIN_EXPORT void
  CodecVersion(uint32& major, uint32& minor, uint32& patch) {
    major = 2;
    minor = 30;
    patch = 0;
  }

  GE_PLUGIN_EXPORT const String&
  CodecName(void) {
    return CODEC_NAME;
  }

  GE_PLUGIN_EXPORT const String&
  CodecDescription(void) {
    return CODEC_DESC;
  }

  GE_PLUGIN_EXPORT const Vector<String>&
  CodecExtensions(void) {
    return CODEC_EXTENSIONS_IMPORT;
  }

  GE_PLUGIN_EXPORT bool
  CodecCanImport(const Path& filePath) {
    // Check if the file extension is one of the supported formats
    String ext = filePath.getExtension();
    for (const auto& supportedExt : CODEC_EXTENSIONS_IMPORT) {
      if (StringUtil::match(ext, supportedExt, false)) {
        return true;
      }
    }

    return false;
  }

  GE_PLUGIN_EXPORT bool
  CodecCanExport(const Path& filePath) {
    // Check if the file extension is one of the supported formats to export
    String ext = filePath.getExtension();
    for (const auto& supportedExt : CODEC_EXTENSIONS_EXPORT) {
      if (StringUtil::match(ext, supportedExt, false)) {
        return true;
      }
    }

    return false;
  }

  GE_PLUGIN_EXPORT void
  CodecImport(const Path& filePath, bool useCacheIfAvailable, SPtr<Resource>& outRes) {
    GE_UNREFERENCED_PARAMETER(useCacheIfAvailable);

    //Import the image using stb_image
    if (!CodecCanImport(filePath)) {
      GE_LOG(kError,
             Generic,
             String("Cannot import file: {0}. Unsupported format."), filePath);
      return;
    }

    auto& renderAPI = RenderAPI::instance();
    auto& mountman = MountManager::instance();

    auto pFileData = mountman.open(filePath);
    auto textureData = EXRLoader::loadFromMemory(pFileData);

    auto pTexture = renderAPI.createTexture(textureData.width,
                                            textureData.height,
                                            GRAPHICS_FORMAT::kR32G32B32A32_FLOAT,
                                            BIND_FLAG::SHADER_RESOURCE,
                                            0);
    if (!pTexture) {
      return;
    }

    pTexture->setAlpha(false);

    renderAPI.writeToResource(pTexture,
                              0,
                              nullptr,
                              textureData.rgba.data(),
                              textureData.width * 16,
                              textureData.width * textureData.height * 16);

    renderAPI.generateMips(pTexture);

    outRes = pTexture;
  }

  GE_PLUGIN_EXPORT bool
  CodecExport(const SPtr<Resource>& resource, const Path& filePath) {
    //Check if we can export this resource
    if (!CodecCanExport(filePath)) {
      GE_LOG(kError,
             Generic,
             "Cannot export resource: {0}. Unsupported format.", filePath.toString());
      return false;
    }

    return false;
  }
}
