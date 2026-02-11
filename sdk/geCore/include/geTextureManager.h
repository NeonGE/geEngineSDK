/*****************************************************************************/
/**
 * @file    geTextureManager.h
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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"

#include <geModule.h>
#include <geDebug.h>
#include <geStringID.h>
#include <geEvent.h>
#include "geFileTracker.h"

namespace geEngineSDK {

  class Texture;

  class GE_CORE_EXPORT TextureManager : public Module<TextureManager>
  {
   public:
    TextureManager() = default;
    ~TextureManager() = default;

    /**
     * @brief Load a texture from the specified file path.
     * @param filePath The path to the texture file.
     * @param useCacheIfAvailable If true, will use cached textures if available.
     * @param bReload If true, will reload the texture even if it is already loaded.
     * @return A shared pointer to the loaded texture.
     */
    SPtr<Texture>
    load(const Path& filePath,
         bool useCacheIfAvailable = false,
         bool bReload = false);

    /**
     * @brief Reloads data or configuration from the specified file path.
     * @param filePath The path to the file to reload.
     */
    void
    reload(const Path& filePath);

    bool
    isLoaded(const Path& filePath);

    void
    garbageCollector();

    SIZE_T
    getMemoryUsage();

    bool
    isDefaultTexture(const WeakSPtr<Texture>& pTexture);

   protected:
    void
    onStartUp() override;

    void
    onShutDown() override;

    UnorderedMap<uint32, SPtr<Texture>> m_loadedTextures;
    Mutex m_mutex;

   public:
    //Textures created by the manager, we have this so they are not unloaded
    //by the garbage collector
    static SPtr<Texture> DEFAULT_ERROR;
    static SPtr<Texture> DEFAULT_TRANSPARENT;
    static SPtr<Texture> DEFAULT_BLACK;
    static SPtr<Texture> DEFAULT_WHITE;
    static SPtr<Texture> DEFAULT_NORMAL;
    static SPtr<Texture> DEFAULT_CHESS;

   private:
#if USING(GE_FILE_TRACKER)
    uint32 m_systemID;
    ChangeCallback m_fileChangeCB;
#endif
  };

  GE_LOG_CATEGORY(TextureManager, 200);
}
