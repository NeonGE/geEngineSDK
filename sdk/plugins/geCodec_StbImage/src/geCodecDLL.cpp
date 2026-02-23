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
#include <geFloat16Color.h>
#include <geMountManager.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x) GE_ASSERT(x)
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBIW_ASSERT(x) GE_ASSERT(x)
#include <stb_image_write.h>

using namespace geEngineSDK;

static String CODEC_NAME = "StbImage Image Codec";
static String CODEC_DESC = "This codec implements the StbImage Library "
                           "and expose it as a codec";
static Vector<String> CODEC_EXTENSIONS_IMPORT = {
  ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".hdr", ".psd", ".gif", ".pic", ".pnm"
};

static Vector<String> CODEC_EXTENSIONS_EXPORT = {
  ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".hdr"
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

    //Check if this is a HDR image
    bool isHDR = false;
    if (StringUtil::match(filePath.getExtension(), ".HDR", false)) {
      //This means we need to load it as a float image
      isHDR = true;
    }

    auto& renderAPI = RenderAPI::instance();
    int width, height, channels;

    auto& mountman = MountManager::instance();
    auto pFileData = mountman.open(filePath);
    Vector<uint8>fileData;
    pFileData->getAllData(fileData);

    if(isHDR) {
      float* pImageData = stbi_loadf_from_memory(fileData.data(),
                                                 cast::st<int32>(fileData.size()),
                                                 &width,
                                                 &height,
                                                 &channels,
                                                 4);
      if (!pImageData) {
        GE_LOG(kError,
               Generic,
               "Failed to load HDR image: {0}. Error: {1}",
               filePath.toString(),
               stbi_failure_reason());
        return;
      }

      //Use the RenderAPI to create a texture resource
      auto pTexture = renderAPI.createTexture(width,
                                              height,
                                              GRAPHICS_FORMAT::kR32G32B32A32_FLOAT,
                                              BIND_FLAG::SHADER_RESOURCE,
                                              0);

      //The alpha is not needed for HDR textures
      pTexture->setAlpha(false);

      //Write the data
      renderAPI.writeToResource(pTexture, 0, nullptr, pImageData, width * 16, 0, 0);

      //Free the image data
      stbi_image_free(pImageData);

      //Set the output resource
      outRes = pTexture;
    }
    else {
      uint8* pImageData = stbi_load_from_memory(fileData.data(),
                                                cast::st<int32>(fileData.size()),
                                                &width,
                                                &height,
                                                &channels,
                                                4);
      if (!pImageData) {
        GE_LOG(kError,
               Generic,
               "Failed to load image: {0}. Error: {1}",
               filePath.toString(),
               stbi_failure_reason());
        return;
      }

      //Use the RenderAPI to create a texture resource
      auto pTexture = renderAPI.createTexture(width,
                                              height,
                                              GRAPHICS_FORMAT::kR8G8B8A8_UNORM,
                                              BIND_FLAG::SHADER_RESOURCE,
                                              0);

      //Check for alpha channel
      bool hasAlpha = (channels == 4);
      if(hasAlpha) {
        //Make sure to check if the alpha channel is actually used
        hasAlpha = false;
        for (int32 i = 3; i < width * height * 4; i += 4) {
          if (pImageData[i] < 255) {
            hasAlpha = true;
            break;
          }
        }
      }
      pTexture->setAlpha(hasAlpha);

      //Write the data
      renderAPI.writeToResource(pTexture, 0, nullptr, pImageData, width * 4, 0, 0);

      //Free the image data
      stbi_image_free(pImageData);

      //Set the output resource
      outRes = pTexture;
    }

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

    //Reinterpret the resource as a texture so we can send it to the RenderAPI,
    //copy it to a scratch texture and then write it to the file
    auto pTexture = std::static_pointer_cast<Texture>(resource);
    if (!pTexture) {
      GE_LOG(kError,
             Generic,
             "Failed to export resource: {0}. Resource is not a texture.",
             filePath.toString());
      return false;
    }

    //Copy the texture to a scratch texture
    auto& renderAPI = RenderAPI::instance();

    //Get the texture description
    auto pTextureDesc = pTexture->getDesc();
    auto pScratchTexture = renderAPI.createTexture(pTextureDesc.width,
                                                   pTextureDesc.height,
                                                   pTextureDesc.format,
                                                   0,
                                                   pTextureDesc.mipLevels,
                                                   RESOURCE_USAGE::STAGING,
                                                   CPU_ACCESS_FLAG::READ);

    //Copy the texture data to the scratch texture
    renderAPI.copyResource(pTexture, pScratchTexture);

    //Map the scratch texture to get the data
    auto mappedRes = renderAPI.mapToRead(pScratchTexture);
    bool bReturn = false;

    //Read the data from the mapped resource taking into account the format
    if(pTextureDesc.format == GRAPHICS_FORMAT::kR32G32B32A32_FLOAT) {
      //HDR texture, we need to write it as a float image
      bReturn = stbi_write_hdr(filePath.toString().c_str(),
                               pTextureDesc.width,
                               pTextureDesc.height,
                               4,
                               (const float*)mappedRes.pData);
    }
    else if (pTextureDesc.format == GRAPHICS_FORMAT::kR16G16B16A16_FLOAT) {
      //16-bit float texture, we need to write it as a float image
      //First we need to convert it to float
      Vector<float> floatData;
      {
        Vector<Float16Color> float16Data;
        float16Data.resize(pTextureDesc.width * pTextureDesc.height);
        const uint16* pData = reinterpret_cast<const uint16_t*>(mappedRes.pData);
        memcpy(&float16Data[0].r,
          pData,
          pTextureDesc.width * pTextureDesc.height * sizeof(Float16Color));

        //Create a buffer of floats
        floatData.resize(pTextureDesc.width * pTextureDesc.height * 4);
        for (size_t i = 0; i < float16Data.size(); ++i) {
          auto& color = float16Data[i];
          floatData[i * 4 + 0] = color.r.getFloat();
          floatData[i * 4 + 1] = color.g.getFloat();
          floatData[i * 4 + 2] = color.b.getFloat();
          floatData[i * 4 + 3] = color.a.getFloat();
        }
      }

      bReturn = stbi_write_hdr(filePath.toString().c_str(),
                               pTextureDesc.width,
                               pTextureDesc.height,
                               4,
                               (const float*)floatData.data());
    }
    else {
      GE_ASSERT(pTextureDesc.format == GRAPHICS_FORMAT::kR8G8B8A8_UNORM ||
                pTextureDesc.format == GRAPHICS_FORMAT::kB8G8R8A8_UNORM);

      //Regular texture, check the extension to determine the format
      String ext = filePath.getExtension();
      if (StringUtil::match(ext, ".png", false)) {
        stbi_flip_vertically_on_write(0);
        bReturn = stbi_write_png(filePath.toString().c_str(),
                                 pTextureDesc.width,
                                 pTextureDesc.height,
                                 4,
                                 mappedRes.pData,
                                 mappedRes.rowPitch);
      }
      else if (StringUtil::match(ext, ".jpg", false) ||
               StringUtil::match(ext, ".jpeg", false)) {
        stbi_flip_vertically_on_write(0);
        bReturn = stbi_write_jpg(filePath.toString().c_str(),
                                 pTextureDesc.width,
                                 pTextureDesc.height,
                                 4,
                                 mappedRes.pData,
                                 100); //Quality set to 100%
      }
      else if (StringUtil::match(ext, ".bmp", false)) {
        stbi_flip_vertically_on_write(0);
        bReturn = stbi_write_bmp(filePath.toString().c_str(),
                                 pTextureDesc.width,
                                 pTextureDesc.height,
                                 4,
                                 mappedRes.pData);
      }
      else if (StringUtil::match(ext, ".tga", false)) {
        stbi_flip_vertically_on_write(0);
        bReturn = stbi_write_tga(filePath.toString().c_str(),
                                 pTextureDesc.width,
                                 pTextureDesc.height,
                                 4,
                                 mappedRes.pData);
      }
      else {
        GE_LOG(kError, Generic, String("Unsupported export format: {0}"), ext);
      }
    }

    //unmap the resource
    renderAPI.unmap(pScratchTexture);

    return bReturn;
  }
}
