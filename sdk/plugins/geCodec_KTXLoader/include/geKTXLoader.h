#pragma once

#include <gePrerequisitesCore.h>
#include <geGraphicsTypes.h>
#include <geException.h>
#include <ktx.h>

using namespace geEngineSDK;

typedef enum VkFormat {
  VK_FORMAT_UNDEFINED = 0,
  VK_FORMAT_R4G4_UNORM_PACK8 = 1,
  VK_FORMAT_R4G4B4A4_UNORM_PACK16 = 2,
  VK_FORMAT_B4G4R4A4_UNORM_PACK16 = 3,
  VK_FORMAT_R5G6B5_UNORM_PACK16 = 4,
  VK_FORMAT_B5G6R5_UNORM_PACK16 = 5,
  VK_FORMAT_R5G5B5A1_UNORM_PACK16 = 6,
  VK_FORMAT_B5G5R5A1_UNORM_PACK16 = 7,
  VK_FORMAT_A1R5G5B5_UNORM_PACK16 = 8,
  VK_FORMAT_R8_UNORM = 9,
  VK_FORMAT_R8_SNORM = 10,
  VK_FORMAT_R8_USCALED = 11,
  VK_FORMAT_R8_SSCALED = 12,
  VK_FORMAT_R8_UINT = 13,
  VK_FORMAT_R8_SINT = 14,
  VK_FORMAT_R8_SRGB = 15,
  VK_FORMAT_R8G8_UNORM = 16,
  VK_FORMAT_R8G8_SNORM = 17,
  VK_FORMAT_R8G8_USCALED = 18,
  VK_FORMAT_R8G8_SSCALED = 19,
  VK_FORMAT_R8G8_UINT = 20,
  VK_FORMAT_R8G8_SINT = 21,
  VK_FORMAT_R8G8_SRGB = 22,
  VK_FORMAT_R8G8B8_UNORM = 23,
  VK_FORMAT_R8G8B8_SNORM = 24,
  VK_FORMAT_R8G8B8_USCALED = 25,
  VK_FORMAT_R8G8B8_SSCALED = 26,
  VK_FORMAT_R8G8B8_UINT = 27,
  VK_FORMAT_R8G8B8_SINT = 28,
  VK_FORMAT_R8G8B8_SRGB = 29,
  VK_FORMAT_B8G8R8_UNORM = 30,
  VK_FORMAT_B8G8R8_SNORM = 31,
  VK_FORMAT_B8G8R8_USCALED = 32,
  VK_FORMAT_B8G8R8_SSCALED = 33,
  VK_FORMAT_B8G8R8_UINT = 34,
  VK_FORMAT_B8G8R8_SINT = 35,
  VK_FORMAT_B8G8R8_SRGB = 36,
  VK_FORMAT_R8G8B8A8_UNORM = 37,
  VK_FORMAT_R8G8B8A8_SNORM = 38,
  VK_FORMAT_R8G8B8A8_USCALED = 39,
  VK_FORMAT_R8G8B8A8_SSCALED = 40,
  VK_FORMAT_R8G8B8A8_UINT = 41,
  VK_FORMAT_R8G8B8A8_SINT = 42,
  VK_FORMAT_R8G8B8A8_SRGB = 43,
  VK_FORMAT_B8G8R8A8_UNORM = 44,
  VK_FORMAT_B8G8R8A8_SNORM = 45,
  VK_FORMAT_B8G8R8A8_USCALED = 46,
  VK_FORMAT_B8G8R8A8_SSCALED = 47,
  VK_FORMAT_B8G8R8A8_UINT = 48,
  VK_FORMAT_B8G8R8A8_SINT = 49,
  VK_FORMAT_B8G8R8A8_SRGB = 50,
  VK_FORMAT_A8B8G8R8_UNORM_PACK32 = 51,
  VK_FORMAT_A8B8G8R8_SNORM_PACK32 = 52,
  VK_FORMAT_A8B8G8R8_USCALED_PACK32 = 53,
  VK_FORMAT_A8B8G8R8_SSCALED_PACK32 = 54,
  VK_FORMAT_A8B8G8R8_UINT_PACK32 = 55,
  VK_FORMAT_A8B8G8R8_SINT_PACK32 = 56,
  VK_FORMAT_A8B8G8R8_SRGB_PACK32 = 57,
  VK_FORMAT_A2R10G10B10_UNORM_PACK32 = 58,
  VK_FORMAT_A2R10G10B10_SNORM_PACK32 = 59,
  VK_FORMAT_A2R10G10B10_USCALED_PACK32 = 60,
  VK_FORMAT_A2R10G10B10_SSCALED_PACK32 = 61,
  VK_FORMAT_A2R10G10B10_UINT_PACK32 = 62,
  VK_FORMAT_A2R10G10B10_SINT_PACK32 = 63,
  VK_FORMAT_A2B10G10R10_UNORM_PACK32 = 64,
  VK_FORMAT_A2B10G10R10_SNORM_PACK32 = 65,
  VK_FORMAT_A2B10G10R10_USCALED_PACK32 = 66,
  VK_FORMAT_A2B10G10R10_SSCALED_PACK32 = 67,
  VK_FORMAT_A2B10G10R10_UINT_PACK32 = 68,
  VK_FORMAT_A2B10G10R10_SINT_PACK32 = 69,
  VK_FORMAT_R16_UNORM = 70,
  VK_FORMAT_R16_SNORM = 71,
  VK_FORMAT_R16_USCALED = 72,
  VK_FORMAT_R16_SSCALED = 73,
  VK_FORMAT_R16_UINT = 74,
  VK_FORMAT_R16_SINT = 75,
  VK_FORMAT_R16_SFLOAT = 76,
  VK_FORMAT_R16G16_UNORM = 77,
  VK_FORMAT_R16G16_SNORM = 78,
  VK_FORMAT_R16G16_USCALED = 79,
  VK_FORMAT_R16G16_SSCALED = 80,
  VK_FORMAT_R16G16_UINT = 81,
  VK_FORMAT_R16G16_SINT = 82,
  VK_FORMAT_R16G16_SFLOAT = 83,
  VK_FORMAT_R16G16B16_UNORM = 84,
  VK_FORMAT_R16G16B16_SNORM = 85,
  VK_FORMAT_R16G16B16_USCALED = 86,
  VK_FORMAT_R16G16B16_SSCALED = 87,
  VK_FORMAT_R16G16B16_UINT = 88,
  VK_FORMAT_R16G16B16_SINT = 89,
  VK_FORMAT_R16G16B16_SFLOAT = 90,
  VK_FORMAT_R16G16B16A16_UNORM = 91,
  VK_FORMAT_R16G16B16A16_SNORM = 92,
  VK_FORMAT_R16G16B16A16_USCALED = 93,
  VK_FORMAT_R16G16B16A16_SSCALED = 94,
  VK_FORMAT_R16G16B16A16_UINT = 95,
  VK_FORMAT_R16G16B16A16_SINT = 96,
  VK_FORMAT_R16G16B16A16_SFLOAT = 97,
  VK_FORMAT_R32_UINT = 98,
  VK_FORMAT_R32_SINT = 99,
  VK_FORMAT_R32_SFLOAT = 100,
  VK_FORMAT_R32G32_UINT = 101,
  VK_FORMAT_R32G32_SINT = 102,
  VK_FORMAT_R32G32_SFLOAT = 103,
  VK_FORMAT_R32G32B32_UINT = 104,
  VK_FORMAT_R32G32B32_SINT = 105,
  VK_FORMAT_R32G32B32_SFLOAT = 106,
  VK_FORMAT_R32G32B32A32_UINT = 107,
  VK_FORMAT_R32G32B32A32_SINT = 108,
  VK_FORMAT_R32G32B32A32_SFLOAT = 109,
  VK_FORMAT_R64_UINT = 110,
  VK_FORMAT_R64_SINT = 111,
  VK_FORMAT_R64_SFLOAT = 112,
  VK_FORMAT_R64G64_UINT = 113,
  VK_FORMAT_R64G64_SINT = 114,
  VK_FORMAT_R64G64_SFLOAT = 115,
  VK_FORMAT_R64G64B64_UINT = 116,
  VK_FORMAT_R64G64B64_SINT = 117,
  VK_FORMAT_R64G64B64_SFLOAT = 118,
  VK_FORMAT_R64G64B64A64_UINT = 119,
  VK_FORMAT_R64G64B64A64_SINT = 120,
  VK_FORMAT_R64G64B64A64_SFLOAT = 121,
  VK_FORMAT_B10G11R11_UFLOAT_PACK32 = 122,
  VK_FORMAT_E5B9G9R9_UFLOAT_PACK32 = 123,
  VK_FORMAT_D16_UNORM = 124,
  VK_FORMAT_X8_D24_UNORM_PACK32 = 125,
  VK_FORMAT_D32_SFLOAT = 126,
  VK_FORMAT_S8_UINT = 127,
  VK_FORMAT_D16_UNORM_S8_UINT = 128,
  VK_FORMAT_D24_UNORM_S8_UINT = 129,
  VK_FORMAT_D32_SFLOAT_S8_UINT = 130,
  VK_FORMAT_BC1_RGB_UNORM_BLOCK = 131,
  VK_FORMAT_BC1_RGB_SRGB_BLOCK = 132,
  VK_FORMAT_BC1_RGBA_UNORM_BLOCK = 133,
  VK_FORMAT_BC1_RGBA_SRGB_BLOCK = 134,
  VK_FORMAT_BC2_UNORM_BLOCK = 135,
  VK_FORMAT_BC2_SRGB_BLOCK = 136,
  VK_FORMAT_BC3_UNORM_BLOCK = 137,
  VK_FORMAT_BC3_SRGB_BLOCK = 138,
  VK_FORMAT_BC4_UNORM_BLOCK = 139,
  VK_FORMAT_BC4_SNORM_BLOCK = 140,
  VK_FORMAT_BC5_UNORM_BLOCK = 141,
  VK_FORMAT_BC5_SNORM_BLOCK = 142,
  VK_FORMAT_BC6H_UFLOAT_BLOCK = 143,
  VK_FORMAT_BC6H_SFLOAT_BLOCK = 144,
  VK_FORMAT_BC7_UNORM_BLOCK = 145,
  VK_FORMAT_BC7_SRGB_BLOCK = 146,
  VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK = 147,
  VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK = 148,
  VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK = 149,
  VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK = 150,
  VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK = 151,
  VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK = 152,
  VK_FORMAT_EAC_R11_UNORM_BLOCK = 153,
  VK_FORMAT_EAC_R11_SNORM_BLOCK = 154,
  VK_FORMAT_EAC_R11G11_UNORM_BLOCK = 155,
  VK_FORMAT_EAC_R11G11_SNORM_BLOCK = 156,
  VK_FORMAT_ASTC_4x4_UNORM_BLOCK = 157,
  VK_FORMAT_ASTC_4x4_SRGB_BLOCK = 158,
  VK_FORMAT_ASTC_5x4_UNORM_BLOCK = 159,
  VK_FORMAT_ASTC_5x4_SRGB_BLOCK = 160,
  VK_FORMAT_ASTC_5x5_UNORM_BLOCK = 161,
  VK_FORMAT_ASTC_5x5_SRGB_BLOCK = 162,
  VK_FORMAT_ASTC_6x5_UNORM_BLOCK = 163,
  VK_FORMAT_ASTC_6x5_SRGB_BLOCK = 164,
  VK_FORMAT_ASTC_6x6_UNORM_BLOCK = 165,
  VK_FORMAT_ASTC_6x6_SRGB_BLOCK = 166,
  VK_FORMAT_ASTC_8x5_UNORM_BLOCK = 167,
  VK_FORMAT_ASTC_8x5_SRGB_BLOCK = 168,
  VK_FORMAT_ASTC_8x6_UNORM_BLOCK = 169,
  VK_FORMAT_ASTC_8x6_SRGB_BLOCK = 170,
  VK_FORMAT_ASTC_8x8_UNORM_BLOCK = 171,
  VK_FORMAT_ASTC_8x8_SRGB_BLOCK = 172,
  VK_FORMAT_ASTC_10x5_UNORM_BLOCK = 173,
  VK_FORMAT_ASTC_10x5_SRGB_BLOCK = 174,
  VK_FORMAT_ASTC_10x6_UNORM_BLOCK = 175,
  VK_FORMAT_ASTC_10x6_SRGB_BLOCK = 176,
  VK_FORMAT_ASTC_10x8_UNORM_BLOCK = 177,
  VK_FORMAT_ASTC_10x8_SRGB_BLOCK = 178,
  VK_FORMAT_ASTC_10x10_UNORM_BLOCK = 179,
  VK_FORMAT_ASTC_10x10_SRGB_BLOCK = 180,
  VK_FORMAT_ASTC_12x10_UNORM_BLOCK = 181,
  VK_FORMAT_ASTC_12x10_SRGB_BLOCK = 182,
  VK_FORMAT_ASTC_12x12_UNORM_BLOCK = 183,
  VK_FORMAT_ASTC_12x12_SRGB_BLOCK = 184,
  VK_FORMAT_G8B8G8R8_422_UNORM = 1000156000,
  VK_FORMAT_B8G8R8G8_422_UNORM = 1000156001,
  VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM = 1000156002,
  VK_FORMAT_G8_B8R8_2PLANE_420_UNORM = 1000156003,
  VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM = 1000156004,
  VK_FORMAT_G8_B8R8_2PLANE_422_UNORM = 1000156005,
  VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM = 1000156006,
  VK_FORMAT_R10X6_UNORM_PACK16 = 1000156007,
  VK_FORMAT_R10X6G10X6_UNORM_2PACK16 = 1000156008,
  VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16 = 1000156009,
  VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16 = 1000156010,
  VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16 = 1000156011,
  VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 = 1000156012,
  VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 = 1000156013,
  VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 = 1000156014,
  VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 = 1000156015,
  VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 = 1000156016,
  VK_FORMAT_R12X4_UNORM_PACK16 = 1000156017,
  VK_FORMAT_R12X4G12X4_UNORM_2PACK16 = 1000156018,
  VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16 = 1000156019,
  VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16 = 1000156020,
  VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16 = 1000156021,
  VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 = 1000156022,
  VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 = 1000156023,
  VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 = 1000156024,
  VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 = 1000156025,
  VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 = 1000156026,
  VK_FORMAT_G16B16G16R16_422_UNORM = 1000156027,
  VK_FORMAT_B16G16R16G16_422_UNORM = 1000156028,
  VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM = 1000156029,
  VK_FORMAT_G16_B16R16_2PLANE_420_UNORM = 1000156030,
  VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM = 1000156031,
  VK_FORMAT_G16_B16R16_2PLANE_422_UNORM = 1000156032,
  VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM = 1000156033,
  VK_FORMAT_G8_B8R8_2PLANE_444_UNORM = 1000330000,
  VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16 = 1000330001,
  VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16 = 1000330002,
  VK_FORMAT_G16_B16R16_2PLANE_444_UNORM = 1000330003,
  VK_FORMAT_A4R4G4B4_UNORM_PACK16 = 1000340000,
  VK_FORMAT_A4B4G4R4_UNORM_PACK16 = 1000340001,
  VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK = 1000066000,
  VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK = 1000066001,
  VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK = 1000066002,
  VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK = 1000066003,
  VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK = 1000066004,
  VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK = 1000066005,
  VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK = 1000066006,
  VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK = 1000066007,
  VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK = 1000066008,
  VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK = 1000066009,
  VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK = 1000066010,
  VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK = 1000066011,
  VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK = 1000066012,
  VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK = 1000066013,
  VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG = 1000054000,
  VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG = 1000054001,
  VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG = 1000054002,
  VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG = 1000054003,
  VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG = 1000054004,
  VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG = 1000054005,
  VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG = 1000054006,
  VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG = 1000054007,
  VK_FORMAT_ASTC_3x3x3_UNORM_BLOCK_EXT = 1000288000,
  VK_FORMAT_ASTC_3x3x3_SRGB_BLOCK_EXT = 1000288001,
  VK_FORMAT_ASTC_3x3x3_SFLOAT_BLOCK_EXT = 1000288002,
  VK_FORMAT_ASTC_4x3x3_UNORM_BLOCK_EXT = 1000288003,
  VK_FORMAT_ASTC_4x3x3_SRGB_BLOCK_EXT = 1000288004,
  VK_FORMAT_ASTC_4x3x3_SFLOAT_BLOCK_EXT = 1000288005,
  VK_FORMAT_ASTC_4x4x3_UNORM_BLOCK_EXT = 1000288006,
  VK_FORMAT_ASTC_4x4x3_SRGB_BLOCK_EXT = 1000288007,
  VK_FORMAT_ASTC_4x4x3_SFLOAT_BLOCK_EXT = 1000288008,
  VK_FORMAT_ASTC_4x4x4_UNORM_BLOCK_EXT = 1000288009,
  VK_FORMAT_ASTC_4x4x4_SRGB_BLOCK_EXT = 1000288010,
  VK_FORMAT_ASTC_4x4x4_SFLOAT_BLOCK_EXT = 1000288011,
  VK_FORMAT_ASTC_5x4x4_UNORM_BLOCK_EXT = 1000288012,
  VK_FORMAT_ASTC_5x4x4_SRGB_BLOCK_EXT = 1000288013,
  VK_FORMAT_ASTC_5x4x4_SFLOAT_BLOCK_EXT = 1000288014,
  VK_FORMAT_ASTC_5x5x4_UNORM_BLOCK_EXT = 1000288015,
  VK_FORMAT_ASTC_5x5x4_SRGB_BLOCK_EXT = 1000288016,
  VK_FORMAT_ASTC_5x5x4_SFLOAT_BLOCK_EXT = 1000288017,
  VK_FORMAT_ASTC_5x5x5_UNORM_BLOCK_EXT = 1000288018,
  VK_FORMAT_ASTC_5x5x5_SRGB_BLOCK_EXT = 1000288019,
  VK_FORMAT_ASTC_5x5x5_SFLOAT_BLOCK_EXT = 1000288020,
  VK_FORMAT_ASTC_6x5x5_UNORM_BLOCK_EXT = 1000288021,
  VK_FORMAT_ASTC_6x5x5_SRGB_BLOCK_EXT = 1000288022,
  VK_FORMAT_ASTC_6x5x5_SFLOAT_BLOCK_EXT = 1000288023,
  VK_FORMAT_ASTC_6x6x5_UNORM_BLOCK_EXT = 1000288024,
  VK_FORMAT_ASTC_6x6x5_SRGB_BLOCK_EXT = 1000288025,
  VK_FORMAT_ASTC_6x6x5_SFLOAT_BLOCK_EXT = 1000288026,
  VK_FORMAT_ASTC_6x6x6_UNORM_BLOCK_EXT = 1000288027,
  VK_FORMAT_ASTC_6x6x6_SRGB_BLOCK_EXT = 1000288028,
  VK_FORMAT_ASTC_6x6x6_SFLOAT_BLOCK_EXT = 1000288029,
  VK_FORMAT_R16G16_SFIXED5_NV = 1000464000,
  VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR = 1000470000,
  VK_FORMAT_A8_UNORM_KHR = 1000470001,
  VK_FORMAT_MAX_ENUM = 0x7FFFFFFF
} VkFormat;

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
  uint64 slicePitch = 0;
  uint64 offset = 0;
  uint64 size = 0;
};

struct TextureData
{
  TextureDesc desc;
  Vector<SubresourceInfo> subresources;
  Vector<uint8> blob;
};

struct LoadOptions
{
  //if KTX2 have Basis (ETC1S/UASTC), we will transcode it to BCn
  bool transcodeBasisToBC = true;

  //Objective formate when transcoded
  //Color sRGB/linear usually: BC7 or BC1/BC3; normal map: BC5; HDR: BC6H
  enum class BasisTarget {
    BC1,
    BC3,
    BC5,
    BC6H_UF16,
    BC7
  }basisTarget = BasisTarget::BC7;

  //For KTX2: allows the transcoding if its required
  ktx_transcode_flags transcodeFlags = KTX_TF_HIGH_QUALITY;
};

// ---------------------------------------------
// Format utilities
// ---------------------------------------------
static inline bool
formatHasAlpha(GRAPHICS_FORMAT::E f) {
  switch (f) {
  case GRAPHICS_FORMAT::kR8G8B8A8_UNORM:
  case GRAPHICS_FORMAT::kB8G8R8A8_UNORM:
  case GRAPHICS_FORMAT::kR16G16B16A16_FLOAT:
  case GRAPHICS_FORMAT::kR32G32B32A32_FLOAT:
  case GRAPHICS_FORMAT::kBC2_UNORM:
  case GRAPHICS_FORMAT::kBC3_UNORM:
  case GRAPHICS_FORMAT::kBC7_UNORM:
    return true;

  case GRAPHICS_FORMAT::kB8G8R8X8_UNORM:
  case GRAPHICS_FORMAT::kBC1_UNORM:
  case GRAPHICS_FORMAT::kBC4_UNORM:
  case GRAPHICS_FORMAT::kBC5_UNORM:
  case GRAPHICS_FORMAT::kBC6H_UF16:
  case GRAPHICS_FORMAT::kBC6H_SF16:
  default:
    return false;
  }
}

// KTX2 uses VkFormat on its header.
// -----------------------------
static inline GRAPHICS_FORMAT::E
mapVkFormatToEngineFormat(VkFormat vkFmt) {
  switch (vkFmt) {
    // Uncompressed
  case VK_FORMAT_R8G8B8A8_UNORM: return GRAPHICS_FORMAT::kR8G8B8A8_UNORM;
  case VK_FORMAT_B8G8R8A8_UNORM: return GRAPHICS_FORMAT::kB8G8R8A8_UNORM;
  case VK_FORMAT_B8G8R8A8_SRGB:  return GRAPHICS_FORMAT::kB8G8R8A8_UNORM_SRGB;
  case VK_FORMAT_R16G16B16A16_SFLOAT: return GRAPHICS_FORMAT::kR16G16B16A16_FLOAT;
  case VK_FORMAT_R32G32B32A32_SFLOAT: return GRAPHICS_FORMAT::kR32G32B32A32_FLOAT;

    // BCn
  case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
  case VK_FORMAT_BC1_RGB_UNORM_BLOCK:  return GRAPHICS_FORMAT::kBC1_UNORM;
  case VK_FORMAT_BC2_UNORM_BLOCK:      return GRAPHICS_FORMAT::kBC2_UNORM;
  case VK_FORMAT_BC3_UNORM_BLOCK:      return GRAPHICS_FORMAT::kBC3_UNORM;
  case VK_FORMAT_BC4_UNORM_BLOCK:      return GRAPHICS_FORMAT::kBC4_UNORM;
  case VK_FORMAT_BC5_UNORM_BLOCK:      return GRAPHICS_FORMAT::kBC5_UNORM;
  case VK_FORMAT_BC6H_UFLOAT_BLOCK:    return GRAPHICS_FORMAT::kBC6H_UF16;
  case VK_FORMAT_BC6H_SFLOAT_BLOCK:    return GRAPHICS_FORMAT::kBC6H_SF16;
  case VK_FORMAT_BC7_UNORM_BLOCK:      return GRAPHICS_FORMAT::kBC7_UNORM;

  default: return GRAPHICS_FORMAT::kUNKNOWN;
  }
}

static inline ktx_transcode_fmt_e
toKtxTranscodeFmt(LoadOptions::BasisTarget t) {
  switch (t) {
  case LoadOptions::BasisTarget::BC1:      return KTX_TTF_BC1_RGB;
  case LoadOptions::BasisTarget::BC3:      return KTX_TTF_BC3_RGBA;
  case LoadOptions::BasisTarget::BC5:      return KTX_TTF_BC5_RG;
  //case LoadOptions::BasisTarget::BC6H_UF16:return KTX_TTF_BC6H_RGB_UFLOAT;
  case LoadOptions::BasisTarget::BC7:      return KTX_TTF_BC7_RGBA;
  default:                                 return KTX_TTF_BC7_RGBA;
  }
}

static GRAPHICS_FORMAT::E
mapGLInternalFormatToEngineFormat(uint32 glInternalFormat) {
  switch (glInternalFormat)
  {
    //Uncompressed
  case 0x8058: /*GL_RGBA8*/  return GRAPHICS_FORMAT::kR8G8B8A8_UNORM;
  case 0x8C43: /*GL_SRGB8_ALPHA8*/ return GRAPHICS_FORMAT::kR8G8B8A8_UNORM_SRGB;
  case 0x881A: /*GL_RGBA16F*/ return GRAPHICS_FORMAT::kR16G16B16A16_FLOAT;
  case 0x8814: /*GL_RGBA32F*/ return GRAPHICS_FORMAT::kR32G32B32A32_FLOAT;

    //BCn (S3TC/DXTC)
  case 0x83F1: /*GL_COMPRESSED_RGBA_S3TC_DXT1_EXT*/ return GRAPHICS_FORMAT::kBC1_UNORM;
  case 0x83F2: /*GL_COMPRESSED_RGBA_S3TC_DXT3_EXT*/ return GRAPHICS_FORMAT::kBC2_UNORM;
  case 0x83F3: /*GL_COMPRESSED_RGBA_S3TC_DXT5_EXT*/ return GRAPHICS_FORMAT::kBC3_UNORM;

    //"RGB-only" DXT1
  case 0x83F0: /*GL_COMPRESSED_RGB_S3TC_DXT1_EXT*/  return GRAPHICS_FORMAT::kBC1_UNORM;

    //RGTC
  case 0x8DBB: /*GL_COMPRESSED_RED_RGTC1*/   return GRAPHICS_FORMAT::kBC4_UNORM;
  case 0x8DBD: /*GL_COMPRESSED_RG_RGTC2*/    return GRAPHICS_FORMAT::kBC5_UNORM;

    //BPTC
  case 0x8E8C: /*GL_COMPRESSED_RGBA_BPTC_UNORM*/ return GRAPHICS_FORMAT::kBC7_UNORM;
  case 0x8E8D: /*GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM*/ return GRAPHICS_FORMAT::kBC7_UNORM_SRGB;
  case 0x8E8F: /*GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT*/ return GRAPHICS_FORMAT::kBC6H_UF16;
  case 0x8E8E: /*GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT*/   return GRAPHICS_FORMAT::kBC6H_SF16;

  default:
    return GRAPHICS_FORMAT::kUNKNOWN;
  }
}

static inline uint32
mipDim(uint32 base, uint32 mip) {
  return std::max(1u, base >> mip);
}

class KtxLoader
{
 public:
  static TextureData
  loadFromMemory(const void* data, size_t size, const LoadOptions& opt = {}) {
    if (!data || size < 12) {
      throw std::runtime_error("KTX: buffer too small.");
    }

    ktxTexture* tex = nullptr;
    const KTX_error_code ec = ktxTexture_CreateFromMemory(
      reinterpret_cast<const ktx_uint8_t*>(data),
      static_cast<ktx_size_t>(size),
      KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
      &tex
    );

    if (ec != KTX_SUCCESS || !tex) {
      throw std::runtime_error("KTX: failed to parse texture.");
    }

    //Ensures destroy even in case of a throw
    struct Guard
    {
      ktxTexture* t;
      ~Guard() {
        if (t) {
          ktxTexture_Destroy(t);
        }
      }
    }guard{ tex };

    if (tex->classId == ktxTexture2_c) {
      ktxTexture2* t2 = reinterpret_cast<ktxTexture2*>(tex);

      //Only if it's supercompressed (Basis / UASTC)
      if (t2->supercompressionScheme == KTX_SS_BASIS_LZ) {
        if (opt.transcodeBasisToBC) {
          const auto outFmt = toKtxTranscodeFmt(opt.basisTarget);

          KTX_error_code result = ktxTexture2_TranscodeBasis(t2,
                                                             outFmt,
                                                             opt.transcodeFlags);
          if (result != KTX_SUCCESS) {
            throw std::runtime_error("KTX2 Basis transcode failed");
          }
        }
      }
    }

    TextureData out{};

    //Dimensions / base layout
    out.desc.width = tex->baseWidth;
    out.desc.height = std::max(1u, tex->baseHeight);
    out.desc.depth = std::max(1u, tex->baseDepth);
    out.desc.mipCount = std::max(1u, tex->numLevels);
    out.desc.arraySize = std::max(1u, tex->numLayers);
    out.desc.isCubemap = tex->isCubemap != 0;

    if (tex->isArray) {
      //numLayers alredy represents the real arraySize
    }

    //Dimensions
    switch (tex->numDimensions) {
    case 1:
      out.desc.dimension = TextureDimension::Tex1D;
      out.desc.height = 1;
      out.desc.depth = 1;
      break;
    case 2:
      out.desc.dimension = TextureDimension::Tex2D;
      out.desc.depth = 1;
      break;
    case 3:
      out.desc.dimension = TextureDimension::Tex3D;
      break;
    default:
      throw std::runtime_error("KTX: invalid numDimensions.");
    }

    GRAPHICS_FORMAT::E fmt = GRAPHICS_FORMAT::kUNKNOWN;

    if (tex->classId == ktxTexture2_c) {
      auto* t2 = reinterpret_cast<ktxTexture2*>(tex);
      VkFormat vkFmt = static_cast<VkFormat>(t2->vkFormat);
      fmt = mapVkFormatToEngineFormat(vkFmt);
    }
    else if (tex->classId == ktxTexture1_c) {
      auto* t1 = reinterpret_cast<ktxTexture1*>(tex);
      fmt = mapGLInternalFormatToEngineFormat(t1->glInternalformat);
    }

    out.desc.format = fmt;
    if (out.desc.format == GRAPHICS_FORMAT::kUNKNOWN) {
      throw std::runtime_error("KTX: unsupported KTX1/KTX2 format (add mapping).");
    }

    //Cubemap: DX11 models it as an array of 6 layers
    //In KTX: numFaces=6 it's a cubemap
    const uint32 faces = out.desc.isCubemap ? std::max(1u, tex->numFaces) : 1u;
    const uint32 layers = std::max(1u, tex->numLayers);
    const uint32 effectiveArray = layers * faces;
    out.desc.arraySize = effectiveArray;

    bool alpha = formatHasAlpha(out.desc.format);
    if (tex->classId == ktxTexture1_c) {
      auto* t1 = reinterpret_cast<ktxTexture1*>(tex);
      if (t1->glInternalformat == 0x83F1 /*GL_COMPRESSED_RGBA_S3TC_DXT1_EXT*/){
        alpha = true; // BC1a (1-bit)
      }
    }
    out.desc.hasAlpha = alpha;

    //Payload pointer
    const uint8* src = reinterpret_cast<const uint8*>(ktxTexture_GetData(tex));
    const ktx_size_t srcSize = ktxTexture_GetDataSize(tex);
    if (!src || srcSize == 0) {
      throw std::runtime_error("KTX: no image data.");
    }

    //Builds subresources y pack the blob tight
    out.subresources.reserve(static_cast<size_t>(effectiveArray) * out.desc.mipCount);

    uint64 running = 0;

    //In KTX2, cubemaps: faceSlice is the face; layer is the index of the array in cubemaps.
    for (uint32 layer = 0; layer < layers; ++layer) {
      for (uint32 face = 0; face < faces; ++face) {
        const uint32 arraySlice = layer * faces + face;

        for (uint32 level = 0; level < out.desc.mipCount; ++level) {
          ktx_size_t off = 0;
          KTX_error_code oec = ktxTexture_GetImageOffset(tex, level, layer, face, &off);
          if (oec != KTX_SUCCESS) {
            throw std::runtime_error("KTX: GetImageOffset failed.");
          }

          const uint32 w = mipDim(out.desc.width, level);
          const uint32 h = (out.desc.dimension == TextureDimension::Tex1D) ?
            1u : mipDim(out.desc.height, level);
          const uint32 d = (out.desc.dimension == TextureDimension::Tex3D) ?
            mipDim(out.desc.depth, level) : 1u;

          const ktx_size_t imageBytesPerSlice = ktxTexture_GetImageSize(tex, level); // <-- clave :contentReference[oaicite:1]{index=1}
          const uint32 rowPitch = ktxTexture_GetRowPitch(tex, level);

          // Para 2D/array/cubemap: imageBytes = imageBytesPerSlice
          // Para 3D: imageSize() es por depth-slice, así que multiplica por depth del mip
          uint32 mipDepth = 1;
          if (tex->numDimensions == 3) {
            mipDepth = std::max(1u, tex->baseDepth >> level);
          }

          const uint64 imageBytes = (uint64)imageBytesPerSlice * (uint64)mipDepth;

          if (off + (ktx_size_t)imageBytes > srcSize) {
            throw std::runtime_error("KTX: truncated image data.");
          }

          SubresourceInfo s{};
          s.mip = level;
          s.arraySlice = arraySlice;
          s.width = w; s.height = h; s.depth = d;
          s.rowPitch = rowPitch;
          s.slicePitch = imageBytesPerSlice;
          s.offset = running;
          s.size = imageBytes;

          out.subresources.push_back(s);
          running += imageBytes;
        }
      }
    }

    out.blob.resize(static_cast<size_t>(running));

    //Efective copy
    for (const auto& s : out.subresources) {
      ktx_size_t off = 0;
      //Note: we need to recompute the offset with (level, layer, face).
      //Re-derive layer/face from arraySlice:
      const uint32 layer = s.arraySlice / faces;
      const uint32 face = s.arraySlice % faces;

      if (ktxTexture_GetImageOffset(tex, s.mip, layer, face, &off) != KTX_SUCCESS) {
        throw std::runtime_error("KTX: GetImageOffset failed (copy).");
      }

      std::memcpy(out.blob.data() + static_cast<size_t>(s.offset),
                  src + off,
                  static_cast<size_t>(s.size));
    }

    return out;
  }
};
