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

#include "geDDSLoader.h"

using namespace geEngineSDK;


static String CODEC_NAME = "DDS Image Loader Codec";
static String CODEC_DESC = "This codec implements the DDS Loader found in "
                           "the AMD Compressonator Library";

static Vector<String> CODEC_EXTENSIONS_IMPORT = {
  ".dds"
};

static Vector<String> CODEC_EXTENSIONS_EXPORT = {
  
};

extern "C"
{
  GE_PLUGIN_EXPORT CODEC_TYPE::E
  CodecType(void) {
    return CODEC_TYPE::IMAGE;
  }

  GE_PLUGIN_EXPORT void
  CodecVersion(uint32& major, uint32& minor, uint32& patch) {
    major = 1;
    minor = 0;
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
    Vector<uint8>fileData;
    pFileData->getAllData(fileData);
    auto textureData = DdsLoader::loadFromMemory(fileData.data(), fileData.size());

    auto pTexture = renderAPI.createTexture(textureData.desc.width,
                                            textureData.desc.height,
                                            textureData.desc.format,
                                            BIND_FLAG::SHADER_RESOURCE,
                                            textureData.desc.mipCount,
                                            RESOURCE_USAGE::DEFAULT,
                                            0,
                                            1,
                                            false,
                                            textureData.desc.isCubemap,
                                            textureData.desc.arraySize);
    if (!pTexture) {
      return;
    }

    pTexture->setAlpha(textureData.desc.hasAlpha);

    for (const auto& subResource : textureData.subresources) {
      const uint32 srIndex = renderAPI.calcSubresource(subResource.mip,
                                                       subResource.arraySlice,
                                                       textureData.desc.mipCount);
      const uint8* srcData = textureData.blob.data() + subResource.offset;

      renderAPI.writeToResource(pTexture,
                                srIndex,
                                nullptr,
                                srcData,
                                subResource.rowPitch,
                                subResource.slicePitch);
    }

    outRes = pTexture;
  }

  GE_PLUGIN_EXPORT bool
  CodecExport(const SPtr<Resource>& /*resource*/, const Path& /*filePath*/) {
    return false;
  }
}
