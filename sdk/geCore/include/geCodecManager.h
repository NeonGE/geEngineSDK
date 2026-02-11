/*****************************************************************************/
/**
 * @file    geCodecManager.h
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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"
#include "geICodec.h"

#include <geModule.h>
#include <geDebug.h>

namespace geEngineSDK {

  class ICodec;

  class GE_CORE_EXPORT CodecManager : public Module<CodecManager>
  {
   public:
    CodecManager() = default;
    ~CodecManager() = default;
  
    const SPtr<ICodec>
    getImportCodec(const CODEC_TYPE::E type, const String& fileExt);

    const SPtr<ICodec>
    getExportCodec(const CODEC_TYPE::E type, const String& fileExt);

   protected:
    void
    onStartUp() override;
    
    void
    onShutDown() override;

   private:
    void
    _loadCodecs();

    Vector<SPtr<ICodec>> m_codecs; // List of codecs loaded by the importer
  };

  GE_LOG_CATEGORY(CodecManager, 600);

}
