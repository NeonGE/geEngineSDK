#include "gePrerequisitesCore.h"
#include "geDataStream.h"
#include "geException.h"

#include "tinyexr.h"

namespace geEngineSDK {
  struct ExrImage
  {
    uint32 width = 0;
    uint32 height = 0;

    //Always RGBA float32, size = width * height * 4
    Vector<float> rgba;
  };

  class EXRLoader
  {
   public:
    static ExrImage
    loadFromMemory(const SPtr<DataStream>& data);
  };

  static void
  throwTinyExr(const char* prefix, const char* err) {
    String msg = prefix;
    if (err) {
      msg += ": "; msg += err;
    }
    
    GE_EXCEPT(InternalErrorException, msg);
  }

  ExrImage
  EXRLoader::loadFromMemory(const SPtr<DataStream>& data) {
    ExrImage out;

    Vector<uint8> pData;
    data->getAllData(pData);

    EXRVersion exrVersion;
    if (ParseEXRVersionFromMemory(&exrVersion,
                                  pData.data(),
                                  pData.size()) != TINYEXR_SUCCESS) {
      throw std::runtime_error("ParseEXRVersionFromMemory failed");
    }

    EXRHeader header;
    InitEXRHeader(&header);

    EXRImage image;
    InitEXRImage(&image);

    const char* err = nullptr;
    int32 ret = ParseEXRHeaderFromMemory(&header,
                                         &exrVersion,
                                         pData.data(),
                                         pData.size(),
                                         &err);
    if (ret != TINYEXR_SUCCESS) {
      throwTinyExr("ParseEXRHeaderFromMemory failed", err);
      FreeEXRErrorMessage(err);
    }

    //Convert all channels to float32
    for (int32 i = 0; i < header.num_channels; ++i) {
      header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT;
    }

    ret = LoadEXRImageFromMemory(&image,
                                 &header,
                                 pData.data(),
                                 pData.size(),
                                 &err);
    if (ret != TINYEXR_SUCCESS) {
      FreeEXRHeader(&header);
      throwTinyExr("LoadEXRImageFromMemory failed", err);
      FreeEXRErrorMessage(err);
    }

    out.width = static_cast<uint32>(image.width);
    out.height = static_cast<uint32>(image.height);
    out.rgba.assign(size_t(out.width) * size_t(out.height) * 4u, 1.0f);

    //TinyEXR delivers planar data by channel
    //We must map by name: R,G,B,A (defaults if missing)
    int32 idxR = -1, idxG = -1, idxB = -1, idxA = -1;
    for (int32 c = 0; c < header.num_channels; ++c) {
      const char* name = header.channels[c].name; // "R","G","B","A", etc.
      if (!name) {
        continue;
      }
      if      (std::strcmp(name, "R") == 0) idxR = c;
      else if (std::strcmp(name, "G") == 0) idxG = c;
      else if (std::strcmp(name, "B") == 0) idxB = c;
      else if (std::strcmp(name, "A") == 0) idxA = c;
    }

    auto copyChan = [&](int32 chanIdx, int32 dstComp, float defVal)
    {
      float* src = (chanIdx >= 0) ? reinterpret_cast<float*>(image.images[chanIdx]) : nullptr;
      const size_t n = size_t(out.width) * size_t(out.height);
      for (size_t i = 0; i < n; ++i) {
        out.rgba[i * 4u + size_t(dstComp)] = src ? src[i] : defVal;
      }
    };

    copyChan(idxR, 0, 0.0f);
    copyChan(idxG, 1, 0.0f);
    copyChan(idxB, 2, 0.0f);
    copyChan(idxA, 3, 1.0f);

    FreeEXRImage(&image);
    FreeEXRHeader(&header);

    return out;
  }
}
