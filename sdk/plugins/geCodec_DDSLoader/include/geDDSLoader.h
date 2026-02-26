#pragma once

#include <gePrerequisitesCore.h>
#include <geGraphicsTypes.h>
#include <geException.h>

using namespace geEngineSDK;

enum class TextureDimension : uint32
{
  Tex1D,
  Tex2D,
  Tex3D
};

struct TextureDesc
{
  TextureDimension dimension = TextureDimension::Tex2D;
  GRAPHICS_FORMAT::E format = GRAPHICS_FORMAT::kUNKNOWN;

  uint32 width = 0;
  uint32 height = 0;
  uint32 depth = 1;

  uint32 mipCount = 1;
  uint32 arraySize = 1;
  bool isCubemap = false;
  bool hasAlpha = false;
};

struct SubresourceInfo
{
  uint32 mip = 0;
  uint32 arraySlice = 0;
  uint32 width = 0;
  uint32 height = 0;
  uint32 depth = 1;

  uint32 rowPitch = 0;
  uint32 slicePitch = 0;
  uint64 offset = 0;
  uint64 size = 0;
};

struct DdsTextureData
{
  TextureDesc desc;
  Vector<SubresourceInfo> subresources;
  Vector<uint8> blob;
};

struct LoadOptions
{
  bool forceTightPacking = true;
};

static inline uint32
max1(uint32 v) { return v ? v : 1; }

// ---------------------------------------------
// DDS structs (packed like file)
// ---------------------------------------------
#pragma pack(push, 1)

struct DDS_PIXELFORMAT
{
  uint32 size;
  uint32 flags;
  uint32 fourCC;
  uint32 rgbBitCount;
  uint32 rBitMask;
  uint32 gBitMask;
  uint32 bBitMask;
  uint32 aBitMask;
};

struct DDS_HEADER
{
  uint32 size;
  uint32 flags;
  uint32 height;
  uint32 width;
  uint32 pitchOrLinearSize;
  uint32 depth;
  uint32 mipMapCount;
  uint32 reserved1[11];
  DDS_PIXELFORMAT ddspf;
  uint32 caps;
  uint32 caps2;
  uint32 caps3;
  uint32 caps4;
  uint32 reserved2;
};

struct DDS_HEADER_DXT10
{
  uint32 dxgiFormat;
  uint32 resourceDimension; // 2D/3D/1D
  uint32 miscFlag;          // cubemap flag
  uint32 arraySize;
  uint32 miscFlags2;
};

#pragma pack(pop)

//DDS constants
static constexpr uint32 DDS_MAGIC = 0x20534444u; // 'DDS '

static constexpr uint32 DDS_FOURCC = 0x00000004u;
static constexpr uint32 DDS_RGB = 0x00000040u;
static constexpr uint32 DDS_RGBA = 0x00000041u;
static constexpr uint32 DDS_LUMINANCE = 0x00020000u;
static constexpr uint32 DDS_LUMINANCEA = 0x00020001u;
static constexpr uint32 DDS_ALPHA = 0x00000002u;

static constexpr uint32 DDSCAPS2_CUBEMAP = 0x00000200u;
static constexpr uint32 DDSCAPS2_VOLUME = 0x00200000u;

//fourCC helper
static inline uint32
makeFourCC(char a, char b, char c, char d) {
  return (uint32)(uint8)a |
        ((uint32)(uint8)b << 8) |
        ((uint32)(uint8)c << 16) |
        ((uint32)(uint8)d << 24);
}

static constexpr uint32 FOURCC_DXT1 = 0x31545844u; // 'DXT1'
static constexpr uint32 FOURCC_DXT3 = 0x33545844u; // 'DXT3'
static constexpr uint32 FOURCC_DXT5 = 0x35545844u; // 'DXT5'
static constexpr uint32 FOURCC_BC4U = 0x55344342u; // 'BC4U'
static constexpr uint32 FOURCC_BC5U = 0x55354342u; // 'BC5U'
static constexpr uint32 FOURCC_DX10 = 0x30315844u; // 'DX10'

// ---------------------------------------------
// Format utilities
// ---------------------------------------------
static inline bool
isBlockCompressed(GRAPHICS_FORMAT::E f) {
  switch (f) {
  case GRAPHICS_FORMAT::kBC1_UNORM:
  case GRAPHICS_FORMAT::kBC2_UNORM:
  case GRAPHICS_FORMAT::kBC3_UNORM:
  case GRAPHICS_FORMAT::kBC4_UNORM:
  case GRAPHICS_FORMAT::kBC5_UNORM:
  case GRAPHICS_FORMAT::kBC6H_UF16:
  case GRAPHICS_FORMAT::kBC6H_SF16:
  case GRAPHICS_FORMAT::kBC7_UNORM:
    return true;
  default:
    return false;
  }
}

static inline uint32
bcBlockBytes(GRAPHICS_FORMAT::E f) {
  switch (f) {
  case GRAPHICS_FORMAT::kBC1_UNORM:
  case GRAPHICS_FORMAT::kBC4_UNORM:
    return 8;
  case GRAPHICS_FORMAT::kBC2_UNORM:
  case GRAPHICS_FORMAT::kBC3_UNORM:
  case GRAPHICS_FORMAT::kBC5_UNORM:
  case GRAPHICS_FORMAT::kBC6H_UF16:
  case GRAPHICS_FORMAT::kBC6H_SF16:
  case GRAPHICS_FORMAT::kBC7_UNORM:
    return 16;
  default:
    return 0;
  }
}

static inline uint32
bytesPerPixel(GRAPHICS_FORMAT::E f) {
  switch (f) {
  case GRAPHICS_FORMAT::kR8_UNORM: return 1;
  case GRAPHICS_FORMAT::kR8G8_UNORM: return 2;
  case GRAPHICS_FORMAT::kR8G8B8A8_UNORM: return 4;
  case GRAPHICS_FORMAT::kB8G8R8A8_UNORM: return 4;
  case GRAPHICS_FORMAT::kB8G8R8X8_UNORM: return 4;
  case GRAPHICS_FORMAT::kR16_FLOAT: return 2;
  case GRAPHICS_FORMAT::kR16G16_FLOAT: return 4;
  case GRAPHICS_FORMAT::kR16G16B16A16_FLOAT: return 8;
  case GRAPHICS_FORMAT::kR32_FLOAT: return 4;
  case GRAPHICS_FORMAT::kR32G32_FLOAT: return 8;
  case GRAPHICS_FORMAT::kR32G32B32A32_FLOAT: return 16;
  case GRAPHICS_FORMAT::kR10G10B10A2_UNORM: return 4;
  default: return 0;
  }
}

static inline bool
formatHasAlpha(GRAPHICS_FORMAT::E f) {
  switch (f) {
  case GRAPHICS_FORMAT::kR8G8B8A8_UNORM:
  case GRAPHICS_FORMAT::kB8G8R8A8_UNORM:
  case GRAPHICS_FORMAT::kR16G16B16A16_FLOAT:
  case GRAPHICS_FORMAT::kR32G32B32A32_FLOAT:
  case GRAPHICS_FORMAT::kR10G10B10A2_UNORM:
    return true;

  case GRAPHICS_FORMAT::kB8G8R8X8_UNORM:
    return false;

    // ---- BC ----
  case GRAPHICS_FORMAT::kBC2_UNORM:
  case GRAPHICS_FORMAT::kBC3_UNORM:
  case GRAPHICS_FORMAT::kBC7_UNORM:
    return true;

  case GRAPHICS_FORMAT::kBC1_UNORM:
    return false;

  case GRAPHICS_FORMAT::kBC4_UNORM:
  case GRAPHICS_FORMAT::kBC5_UNORM:
  case GRAPHICS_FORMAT::kBC6H_UF16:
  case GRAPHICS_FORMAT::kBC6H_SF16:
    return false;

  default:
    return false;
  }
}

static inline void
calcSubresourceLayout(GRAPHICS_FORMAT::E fmt, uint32 w, uint32 h, uint32 d,
                      uint32& outRowPitch, uint32& outSlicePitch, uint64& outSize) {
  w = max1(w); h = max1(h); d = max1(d);

  if (isBlockCompressed(fmt)) {
    const uint32 blockBytes = bcBlockBytes(fmt);
    const uint32 bw = (w + 3) / 4;
    const uint32 bh = (h + 3) / 4;

    outRowPitch = bw * blockBytes;
    const uint32 slice = outRowPitch * bh;  //bytes per “image” (2D)
    outSlicePitch = slice;                  //for BC, slicePitch = image bytes
    outSize = (uint64)slice * (uint64)d;    //3D: d slices
  }
  else {
    const uint32 bpp = bytesPerPixel(fmt);
    if (bpp == 0) {
      GE_EXCEPT(InvalidStateException, "Unsupported or unknown pixel format.")
    }
    outRowPitch = w * bpp;
    outSlicePitch = outRowPitch * h;
    outSize = (uint64)outSlicePitch * (uint64)d;
  }
}

// ---------------------------------------------
// Format mapping: legacy DDS_PIXELFORMAT -> Format
// and DXGI integer -> Format
// ---------------------------------------------
static inline bool
isBitMask(const DDS_PIXELFORMAT& p, uint32 rb, uint32 gb, uint32 bb, uint32 ab) {
  return p.rBitMask == rb && p.gBitMask == gb && p.bBitMask == bb && p.aBitMask == ab;
}

static inline GRAPHICS_FORMAT::E
mapLegacyPixelFormat(const DDS_PIXELFORMAT& p) {
  if (p.flags & DDS_FOURCC) {
    if (p.fourCC == FOURCC_DXT1) return GRAPHICS_FORMAT::kBC1_UNORM;
    if (p.fourCC == FOURCC_DXT3) return GRAPHICS_FORMAT::kBC2_UNORM;
    if (p.fourCC == FOURCC_DXT5) return GRAPHICS_FORMAT::kBC3_UNORM;
    if (p.fourCC == FOURCC_BC4U) return GRAPHICS_FORMAT::kBC4_UNORM;
    if (p.fourCC == FOURCC_BC5U) return GRAPHICS_FORMAT::kBC5_UNORM;
    //BC6/BC7 usualy comes with DX10, but some tools set weird fourCCs "BC6H/BC7L".
    return GRAPHICS_FORMAT::kUNKNOWN;
  }

  //Uncompressed RGB/RGBA common masks
  if ((p.flags & DDS_RGBA) && p.rgbBitCount == 32) {  //RGBA8  
    if (isBitMask(p, 0x000000ffu, 0x0000ff00u, 0x00ff0000u, 0xff000000u)){
      return GRAPHICS_FORMAT::kR8G8B8A8_UNORM; // little-endian RGBA
    }
    // BGRA8
    if (isBitMask(p, 0x00ff0000u, 0x0000ff00u, 0x000000ffu, 0xff000000u)) {
      return GRAPHICS_FORMAT::kB8G8R8A8_UNORM;
    }
    // BGRX8
    if (isBitMask(p, 0x00ff0000u, 0x0000ff00u, 0x000000ffu, 0x00000000u)) {
      return GRAPHICS_FORMAT::kB8G8R8X8_UNORM;
    }
  }

  if ((p.flags & DDS_RGB) && p.rgbBitCount == 24) {
    // 24-bit is a pain (no DXGI canonical). Many engines avoid it.
    return GRAPHICS_FORMAT::kUNKNOWN;
  }

  if ((p.flags & DDS_LUMINANCE) && p.rgbBitCount == 8) {
    return GRAPHICS_FORMAT::kR8_UNORM;
  }

  if ((p.flags & DDS_LUMINANCEA) && p.rgbBitCount == 16) {
    return GRAPHICS_FORMAT::kR8G8_UNORM;
  }

  return GRAPHICS_FORMAT::kUNKNOWN;
}

class DdsLoader
{
 public:
  static DdsTextureData
  loadFromMemory(const void* data, size_t size, const LoadOptions& opt = {}) {
    GE_UNREFERENCED_PARAMETER(opt);

    if (!data || size < 4 + sizeof(DDS_HEADER))
      throw std::runtime_error("DDS: buffer too small.");

    const uint8* bytes = reinterpret_cast<const uint8*>(data);

    uint32 magic = 0;
    std::memcpy(&magic, bytes, 4);
    if (magic != DDS_MAGIC)
      throw std::runtime_error("DDS: invalid magic.");

    const DDS_HEADER* hdr = reinterpret_cast<const DDS_HEADER*>(bytes + 4);
    if (hdr->size != 124 || hdr->ddspf.size != 32)
      throw std::runtime_error("DDS: invalid header sizes.");

    size_t offset = 4 + sizeof(DDS_HEADER);

    TextureDesc desc;
    desc.width = hdr->width;
    desc.height = hdr->height;
    desc.depth = (hdr->caps2 & DDSCAPS2_VOLUME) ? std::max(1u, hdr->depth) : 1u;
    desc.mipCount = std::max(1u, hdr->mipMapCount);

    //Detect DX10 extended header
    bool hasDX10 = (hdr->ddspf.flags & DDS_FOURCC) && (hdr->ddspf.fourCC == FOURCC_DX10);
    bool bc1HasAlpha = false;

    uint32 arraySize = 1;
    bool isCube = (hdr->caps2 & DDSCAPS2_CUBEMAP) != 0;

    if (hasDX10) {
      if (size < offset + sizeof(DDS_HEADER_DXT10))
        throw std::runtime_error("DDS: missing DX10 header.");

      const DDS_HEADER_DXT10* dx10 = reinterpret_cast<const DDS_HEADER_DXT10*>(bytes + offset);
      offset += sizeof(DDS_HEADER_DXT10);

      desc.format = cast::st<GRAPHICS_FORMAT::E>(dx10->dxgiFormat);
      if (desc.format == GRAPHICS_FORMAT::kUNKNOWN) {
        throw std::runtime_error("DDS: unsupported DXGI format.");
      }

      arraySize = std::max(1u, dx10->arraySize);
      // resourceDimension: 2=TEXTURE1D, 3=TEXTURE2D, 4=TEXTURE3D (D3D10_RESOURCE_DIMENSION)
      switch (dx10->resourceDimension) {
      case 2: desc.dimension = TextureDimension::Tex1D; desc.height = 1; desc.depth = 1; break;
      case 3: desc.dimension = TextureDimension::Tex2D; desc.depth = 1; break;
      case 4: desc.dimension = TextureDimension::Tex3D; break;
      default: throw std::runtime_error("DDS: unsupported resource dimension.");
      }

      // miscFlag bit 0x4 = TEXTURECUBE in D3D10
      if (dx10->miscFlag & 0x4u) isCube = true;
    }
    else {
      desc.format = mapLegacyPixelFormat(hdr->ddspf);
      if (desc.format == GRAPHICS_FORMAT::kUNKNOWN)
        throw std::runtime_error("DDS: unsupported legacy pixel format (consider DX10 DDS).");

      // dimension inference
      desc.dimension = (hdr->caps2 & DDSCAPS2_VOLUME) ? TextureDimension::Tex3D :
        TextureDimension::Tex2D;

      if (desc.format == GRAPHICS_FORMAT::kBC1_UNORM && hdr->ddspf.flags & DDS_RGBA) {
        bc1HasAlpha = true;
      }
    }

    desc.hasAlpha = formatHasAlpha(desc.format);
    desc.hasAlpha |= bc1HasAlpha;

    // Cubemap: array slices are faces * arraySize (DX10 can have array of cubemaps)
    desc.isCubemap = isCube;
    if (desc.isCubemap) {
      // DDS cubemap implies 6 faces
      desc.arraySize = arraySize * 6;
      desc.dimension = TextureDimension::Tex2D;
      desc.depth = 1;
    }
    else {
      desc.arraySize = arraySize;
    }

    if (desc.width == 0 || desc.height == 0)
      throw std::runtime_error("DDS: invalid dimensions.");

    // Now parse pixel data
    if (offset >= size)
      throw std::runtime_error("DDS: no pixel data.");

    const uint8* pixelData = bytes + offset;
    const size_t pixelBytes = size - offset;

    DdsTextureData out;
    out.desc = desc;

    // Build subresource table + pack blob
    // Order: arraySlice major, then mip minor (common)
    uint64 running = 0;
    out.subresources.reserve((size_t)desc.arraySize * (size_t)desc.mipCount);

    auto mipDim = [](uint32 base, uint32 mip) -> uint32 { return std::max(1u, base >> mip); };

    // We will read sequentially from file data in same order
    size_t srcCursor = 0;

    for (uint32 a = 0; a < desc.arraySize; ++a) {
      for (uint32 m = 0; m < desc.mipCount; ++m) {
        SubresourceInfo s{};
        s.arraySlice = a;
        s.mip = m;
        s.width = mipDim(desc.width, m);
        s.height = (desc.dimension == TextureDimension::Tex1D) ? 1u : mipDim(desc.height, m);
        s.depth = (desc.dimension == TextureDimension::Tex3D) ? mipDim(desc.depth, m) : 1u;

        uint64 subSize = 0;
        calcSubresourceLayout(desc.format,
          s.width,
          s.height,
          s.depth,
          s.rowPitch,
          s.slicePitch,
          subSize);
        s.size = subSize;

        // Bounds check against file payload
        if (srcCursor + (size_t)subSize > pixelBytes)
          throw std::runtime_error("DDS: truncated pixel data.");

        s.offset = running;
        running += subSize;

        out.subresources.push_back(s);
        srcCursor += (size_t)subSize;
      }
    }

    // Copy packed blob
    out.blob.resize((size_t)running);

    srcCursor = 0;
    for (const auto& s : out.subresources) {
      std::memcpy(out.blob.data() + (size_t)s.offset, pixelData + srcCursor, (size_t)s.size);
      srcCursor += (size_t)s.size;
    }

    return out;
  }
};
