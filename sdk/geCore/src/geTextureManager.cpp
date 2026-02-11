/*****************************************************************************/
/**
 * @file    geTextureManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/15
 * @brief   Texture Manager System.
 *
 * Texture Manager System that handles the loading, unloading, and management
 * of textures in the rendering system.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geTextureManager.h"
#include "geRenderAPI.h"
#include "geCodecManager.h"

#include <geFileSystem.h>
#include <geDataStream.h>
#include <geColor.h>
#include <geMath.h>
#include <geUUID.h>
#include <geFloat16.h>
#include <geFloat16Color.h>

namespace geEngineSDK {
  GE_LOG_CATEGORY_IMPL(TextureManager);

  SPtr<Texture> TextureManager::DEFAULT_ERROR;
  SPtr<Texture> TextureManager::DEFAULT_TRANSPARENT;
  SPtr<Texture> TextureManager::DEFAULT_BLACK;
  SPtr<Texture> TextureManager::DEFAULT_WHITE;
  SPtr<Texture> TextureManager::DEFAULT_NORMAL;
  SPtr<Texture> TextureManager::DEFAULT_CHESS;

  void
  TextureManager::onStartUp() {
    RenderAPI& graphMan = RenderAPI::instance();

    //Create the default textures
    Vector<Color> imgPxls;
    uint32 imgSize = 128;
    imgPxls.resize(imgSize * imgSize);
    int32 scale;

    imgSize = 128;
    scale = 4;
    for (uint32 y = 0; y < imgSize; ++y) {
      for (uint32 x = 0; x < imgSize; ++x) {
        int32 chessboard = x / scale + y / scale;
        imgPxls[y * imgSize + x] = chessboard % 2 ? Color::Magenta : Color::Black;
      }
    }
    DEFAULT_ERROR = graphMan.createTexture(imgSize, imgSize, GRAPHICS_FORMAT::kB8G8R8A8_UNORM);
    DEFAULT_ERROR->setPath("ERROR.DEFAULT");
    DEFAULT_ERROR->setDebugName("ERROR.DEFAULT");
    graphMan.writeToResource(DEFAULT_ERROR,
                             0,
                             nullptr,
                             imgPxls.data(),
                             imgSize * 4, imgSize * imgSize * 4);

    imgSize = 128;
    scale = 4;
    for (uint32 y = 0; y < imgSize; ++y) {
      for (uint32 x = 0; x < imgSize; ++x) {
        int32 chessboard = x / scale + y / scale;
        imgPxls[y * imgSize + x] = chessboard % 2 ? Color::White : Color::Black;
      }
    }
    DEFAULT_CHESS = graphMan.createTexture(imgSize, imgSize, GRAPHICS_FORMAT::kB8G8R8A8_UNORM);
    DEFAULT_CHESS->setPath("CHESS.DEFAULT");
    DEFAULT_CHESS->setDebugName("CHESS.DEFAULT");
    graphMan.writeToResource(DEFAULT_CHESS,
                             0,
                             nullptr,
                             imgPxls.data(),
                             imgSize * 4, imgSize * imgSize * 4);

    imgPxls.resize(16);
    for (auto& color : imgPxls) {
      color = Color::Black;
    }
    DEFAULT_BLACK = graphMan.createTexture(4, 4, GRAPHICS_FORMAT::kB8G8R8A8_UNORM);
    DEFAULT_BLACK->setPath("BLACK.DEFAULT");
    DEFAULT_BLACK->setDebugName("BLACK.DEFAULT");
    graphMan.writeToResource(DEFAULT_BLACK, 0, nullptr, imgPxls.data(), 16, 64);

    for (auto& color : imgPxls) {
      color = Color::White;
    }
    DEFAULT_WHITE = graphMan.createTexture(4, 4, GRAPHICS_FORMAT::kB8G8R8A8_UNORM);
    DEFAULT_WHITE->setPath("WHITE.DEFAULT");
    DEFAULT_WHITE->setDebugName("WHITE.DEFAULT");
    graphMan.writeToResource(DEFAULT_WHITE, 0, nullptr, imgPxls.data(), 16, 64);

    for (auto& color : imgPxls) {
      color = Color::Transparent;
    }
    DEFAULT_TRANSPARENT = graphMan.createTexture(4, 4, GRAPHICS_FORMAT::kB8G8R8A8_UNORM);
    DEFAULT_TRANSPARENT->setPath("TRANSPARENT.DEFAULT");
    DEFAULT_TRANSPARENT->setDebugName("TRANSPARENT.DEFAULT");
    graphMan.writeToResource(DEFAULT_TRANSPARENT, 0, nullptr, imgPxls.data(), 16, 64);

    for (auto& color : imgPxls) {
      color = Color(127, 127, 255);
    }
    DEFAULT_NORMAL = graphMan.createTexture(4, 4, GRAPHICS_FORMAT::kB8G8R8A8_UNORM);
    DEFAULT_NORMAL->setPath("NORMAL.DEFAULT");
    DEFAULT_NORMAL->setDebugName("NORMAL.DEFAULT");
    graphMan.writeToResource(DEFAULT_NORMAL, 0, nullptr, imgPxls.data(), 16, 64);

    //Add the default textures to the loaded textures
    {
      Lock lock(m_mutex);
      m_loadedTextures[StringID("ERROR.DEFAULT").id()] = DEFAULT_ERROR;
      m_loadedTextures[StringID("CHESS.DEFAULT").id()] = DEFAULT_CHESS;
      m_loadedTextures[StringID("BLACK.DEFAULT").id()] = DEFAULT_BLACK;
      m_loadedTextures[StringID("WHITE.DEFAULT").id()] = DEFAULT_WHITE;
      m_loadedTextures[StringID("TRANSPARENT.DEFAULT").id()] = DEFAULT_TRANSPARENT;
      m_loadedTextures[StringID("NORMAL.DEFAULT").id()] = DEFAULT_NORMAL;
    }

    m_fileChangeCB.connect(
      [this](const WString& filePath) {
        Path path(toString(filePath));
        reload(path);
      });

    auto& fileTracker = FileTracker::instance();
    m_systemID = fileTracker.subscribe("TextureManager", m_fileChangeCB);
  }

  void
  TextureManager::onShutDown() {
    m_fileChangeCB.clear();

    DEFAULT_ERROR = nullptr;
    DEFAULT_TRANSPARENT = nullptr;
    DEFAULT_BLACK = nullptr;
    DEFAULT_WHITE = nullptr;
    DEFAULT_NORMAL = nullptr;
    DEFAULT_CHESS = nullptr;

    m_loadedTextures.clear();
  }

  bool
  TextureManager::isDefaultTexture(const WeakSPtr<Texture>& pTexture) {
    if (pTexture.expired()) {
      return false;
    }

    auto& path = pTexture.lock()->getPath();
    return StringUtil::match(path.getExtension(), "DEFAULT", false);
  }

  void
  TextureManager::reload(const Path& filePath) {
    load(filePath, false, true);
  }

  bool
  TextureManager::isLoaded(const Path& filePath) {
    StringID fileID(filePath.toString());
    Lock lock(m_mutex);
    return m_loadedTextures.find(fileID.id()) != m_loadedTextures.end();
  }

  void
  TextureManager::garbageCollector() {
    //Check all the objects in the manager and removes all those with only 1 ref
    Lock lock(m_mutex);
    auto pTex = m_loadedTextures.cbegin();
    while (pTex != m_loadedTextures.cend()) {
      if (1 == pTex->second.use_count()) {
        pTex = m_loadedTextures.erase(pTex);
        continue;
      }
      ++pTex;
    }
  }

  SIZE_T
  TextureManager::getMemoryUsage() {
    SIZE_T totalSize = 0;
    Lock lock(m_mutex);
    for (auto& tex : m_loadedTextures) {
      totalSize += tex.second->getMemoryUsage();
    }
    return totalSize;
  }

  SPtr<Texture>
  TextureManager::load(const Path& filePath,
                       bool useCacheIfAvailable,
                       bool bReload) {
    if (filePath.getExtension().empty()) {
      GE_LOG(kWarning,
             TextureManager,
             "Texture file path has no extension: {0}. Cannot load texture.",
             filePath.toPlatformString());
    }

    StringID fileID(filePath.toString());
    Path realPath = filePath;
    Path TextureCachePath;

    if (StringUtil::match(filePath.getExtension(), ".DEFAULT", false)) {
      //If the file is a default texture, return the default texture
      if (isLoaded(filePath)) {
        Lock lock(m_mutex);
        return m_loadedTextures[fileID.id()];
      }
    }

    if (useCacheIfAvailable && !bReload) {
      if (!StringUtil::match(filePath.getExtension(), ".DDS", false)) {
        //Check if there's a cached DDS texture
        geEngineSDK::UUID fileuuid(filePath);
        TextureCachePath = "Saved/TextureCache/";
        TextureCachePath += fileuuid.toString() + ".dds";
        if (FileSystem::exists(TextureCachePath)) {
          realPath = TextureCachePath;
        }
        else {
          TextureCachePath = "";
        }
      }
    }

    if (!bReload && isLoaded(filePath)) {
      //If the texture is already loaded, return it
      Lock lock(m_mutex);
      return m_loadedTextures[fileID.id()];
    }

    if (!FileSystem::exists(realPath)) {
      GE_LOG(kWarning,
             TextureManager,
             "Texture not found: {0}. Trying in Root Folder...",
             filePath.toPlatformString());

      Path fileInRoot(filePath.getFilename(true));
      if (!FileSystem::exists(fileInRoot)) {
        GE_LOG(kWarning,
               TextureManager,
               "Texture: {0}. Not Found in Root Folder...",
               filePath.toPlatformString());
        return DEFAULT_ERROR;
      }

      realPath = fileInRoot;
    }

    auto& codecMan = CodecManager::instance();

    auto& pCodec = codecMan.getImportCodec(CODEC_TYPE::IMAGE, realPath.getExtension());
    if (!pCodec) { //If there's no codec for this kind of file
      return nullptr;
    }

    //Load the texture using the codec
    SPtr<Resource> pTexResource;
    pCodec->importResource(realPath, useCacheIfAvailable, pTexResource);
    if (!pTexResource) {
      GE_LOG(kError,
             TextureManager,
             "Failed to load texture: {0}. Codec returned null.",
             realPath.toPlatformString());
      return DEFAULT_ERROR;
    }
    auto pTexture = std::reinterpret_pointer_cast<Texture>(pTexResource);
    
    {
      Lock lock(m_mutex);
      auto it = m_loadedTextures.find(fileID.id());
      if (it != m_loadedTextures.end()) {
        it->second->moveFrom(*pTexture);
        pTexture = nullptr;
        pTexture = it->second; //Reuse the existing texture
      }
      else {
        m_loadedTextures[fileID.id()] = pTexture;
      }
    }

#if USING(FILE_TRACKER)
    auto& fileTracker = FileTracker::instance();
    fileTracker.addFiles(m_systemID, { filePath });
#endif

    pTexture->setPath(filePath);
    pTexture->setCookedPath(TextureCachePath);

    return pTexture;
  }

}
