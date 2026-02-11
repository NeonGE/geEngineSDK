/*****************************************************************************/
/**
 * @file    geGraphicsTypes.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/26
 * @brief   Graphics objects types.
 *
 * Graphics objects types.
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

namespace geEngineSDK {

  //Forward declarations
  class Texture;

  //Structure for the description of the graphics device
  struct ADAPTER_DESC
  {
    WCHAR description[128];
    uint32 vendorId;
    uint32 deviceId;
    uint32 subSysId;
    uint32 revision;
    SIZE_T dedicatedVideoMemory;
    SIZE_T dedicatedSystemMemory;
    SIZE_T dharedSystemMemory;
    struct _LUID {
      uint32 LowPart;
      int32 HighPart;
    }AdapterLuid;
  };

  //Texture format types
  namespace GRAPHICS_FORMAT {
    enum E {
      kUNKNOWN = 0,
      kR32G32B32A32_TYPELESS = 1,
      kR32G32B32A32_FLOAT = 2,
      kR32G32B32A32_UINT = 3,
      kR32G32B32A32_SINT = 4,
      kR32G32B32_TYPELESS = 5,
      kR32G32B32_FLOAT = 6,
      kR32G32B32_UINT = 7,
      kR32G32B32_SINT = 8,
      kR16G16B16A16_TYPELESS = 9,
      kR16G16B16A16_FLOAT = 10,
      kR16G16B16A16_UNORM = 11,
      kR16G16B16A16_UINT = 12,
      kR16G16B16A16_SNORM = 13,
      kR16G16B16A16_SINT = 14,
      kR32G32_TYPELESS = 15,
      kR32G32_FLOAT = 16,
      kR32G32_UINT = 17,
      kR32G32_SINT = 18,
      kR32G8X24_TYPELESS = 19,
      kD32_FLOAT_S8X24_UINT = 20,
      kR32_FLOAT_X8X24_TYPELESS = 21,
      kX32_TYPELESS_G8X24_UINT = 22,
      kR10G10B10A2_TYPELESS = 23,
      kR10G10B10A2_UNORM = 24,
      kR10G10B10A2_UINT = 25,
      kR11G11B10_FLOAT = 26,
      kR8G8B8A8_TYPELESS = 27,
      kR8G8B8A8_UNORM = 28,
      kR8G8B8A8_UNORM_SRGB = 29,
      kR8G8B8A8_UINT = 30,
      kR8G8B8A8_SNORM = 31,
      kR8G8B8A8_SINT = 32,
      kR16G16_TYPELESS = 33,
      kR16G16_FLOAT = 34,
      kR16G16_UNORM = 35,
      kR16G16_UINT = 36,
      kR16G16_SNORM = 37,
      kR16G16_SINT = 38,
      kR32_TYPELESS = 39,
      kD32_FLOAT = 40,
      kR32_FLOAT = 41,
      kR32_UINT = 42,
      kR32_SINT = 43,
      kR24G8_TYPELESS = 44,
      kD24_UNORM_S8_UINT = 45,
      kR24_UNORM_X8_TYPELESS = 46,
      kX24_TYPELESS_G8_UINT = 47,
      kR8G8_TYPELESS = 48,
      kR8G8_UNORM = 49,
      kR8G8_UINT = 50,
      kR8G8_SNORM = 51,
      kR8G8_SINT = 52,
      kR16_TYPELESS = 53,
      kR16_FLOAT = 54,
      kD16_UNORM = 55,
      kR16_UNORM = 56,
      kR16_UINT = 57,
      kR16_SNORM = 58,
      kR16_SINT = 59,
      kR8_TYPELESS = 60,
      kR8_UNORM = 61,
      kR8_UINT = 62,
      kR8_SNORM = 63,
      kR8_SINT = 64,
      kA8_UNORM = 65,
      kR1_UNORM = 66,
      kR9G9B9E5_SHAREDEXP = 67,
      kR8G8_B8G8_UNORM = 68,
      kG8R8_G8B8_UNORM = 69,
      kBC1_TYPELESS = 70,
      kBC1_UNORM = 71,
      kBC1_UNORM_SRGB = 72,
      kBC2_TYPELESS = 73,
      kBC2_UNORM = 74,
      kBC2_UNORM_SRGB = 75,
      kBC3_TYPELESS = 76,
      kBC3_UNORM = 77,
      kBC3_UNORM_SRGB = 78,
      kBC4_TYPELESS = 79,
      kBC4_UNORM = 80,
      kBC4_SNORM = 81,
      kBC5_TYPELESS = 82,
      kBC5_UNORM = 83,
      kBC5_SNORM = 84,
      kB5G6R5_UNORM = 85,
      kB5G5R5A1_UNORM = 86,
      kB8G8R8A8_UNORM = 87,
      kB8G8R8X8_UNORM = 88,
      kR10G10B10_XR_BIAS_A2_UNORM = 89,
      kB8G8R8A8_TYPELESS = 90,
      kB8G8R8A8_UNORM_SRGB = 91,
      kB8G8R8X8_TYPELESS = 92,
      kB8G8R8X8_UNORM_SRGB = 93,
      kBC6H_TYPELESS = 94,
      kBC6H_UF16 = 95,
      kBC6H_SF16 = 96,
      kBC7_TYPELESS = 97,
      kBC7_UNORM = 98,
      kBC7_UNORM_SRGB = 99,
      kAYUV = 100,
      kY410 = 101,
      kY416 = 102,
      kNV12 = 103,
      kP010 = 104,
      kP016 = 105,
      k420_OPAQUE = 106,
      kYUY2 = 107,
      kY210 = 108,
      kY216 = 109,
      kNV11 = 110,
      kAI44 = 111,
      kIA44 = 112,
      kP8 = 113,
      kA8P8 = 114,
      kB4G4R4A4_UNORM = 115,

      kP208 = 130,
      kV208 = 131,
      kV408 = 132,

      kSAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 189,
      kSAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,
    };
  }

  namespace PRIMITIVE_TOPOLOGY {
    enum E {
      UNDEFINED = 0,
      POINTLIST = 1,
      LINELIST = 2,
      LINESTRIP = 3,
      TRIANGLELIST = 4,
      TRIANGLESTRIP = 5,
      TRIANGLEFAN = 6,
      LINELIST_ADJ = 10,
      LINESTRIP_ADJ = 11,
      TRIANGLELIST_ADJ = 12,
      TRIANGLESTRIP_ADJ = 13,
    };
  }

  namespace INDEX_BUFFER_FORMAT {
    enum E {
      R16_UINT = 1, // 16-bit unsigned integer index buffer
      R32_UINT = 2  // 32-bit unsigned integer index buffer
    };
  }

  namespace RESOURCE_USAGE {
    enum E {
      DEFAULT = 0,
      IMMUTABLE = 1,
      DYNAMIC = 2,
      STAGING = 3
    };
  }

  namespace BIND_FLAG {
    enum E {
      VERTEX_BUFFER = 0x1L,
      INDEX_BUFFER = 0x2L,
      CONSTANT_BUFFER = 0x4L,
      SHADER_RESOURCE = 0x8L,
      STREAM_OUTPUT = 0x10L,
      RENDER_TARGET = 0x20L,
      DEPTH_STENCIL = 0x40L,
      UNORDERED_ACCESS = 0x80L,
      DECODER = 0x200L,
      VIDEO_ENCODER = 0x400L
    };
  }

  namespace CLEAR_FLAG {
    enum E {
      DEPTH = 0x1L,
      STENCIL = 0x2L
    };
  }

  struct GRAPHICS_BOX
  {
    uint32 left;
    uint32 top;
    uint32 front;
    uint32 right;
    uint32 bottom;
    uint32 back;
  };

  struct GRAPHICS_VIEWPORT
  {
    float x;
    float y;
    float width;
    float height;
    float zNear;
    float zFar;
  };

  struct TEXTURE2D_DESC
  {
    uint32 width = 0;
    uint32 height = 0;
    uint32 mipLevels = 0;
    uint32 arraySize = 0;
    GRAPHICS_FORMAT::E format = GRAPHICS_FORMAT::kUNKNOWN;
    struct SAMPLE_DESC
    {
      uint32 count = 0;
      uint32 quality = 0;
    }sampleDesc;
    uint32 usage = RESOURCE_USAGE::DEFAULT;
    uint32 bindFlags = 0;
    uint32 cpuAccessFlags = 0;
    uint32 miscFlags = 0;
  };

  struct RenderTarget
  {
    RenderTarget(WeakSPtr<Texture> pRT, int32 mipLevel = 0)
      : pRenderTarget(pRT),
        mipLevel(mipLevel)
    {}

    WeakSPtr<Texture> pRenderTarget;
    int32 mipLevel;
  };

  struct UAVTarget
  {
    UAVTarget(WeakSPtr<Texture> pUAV, int32 mipLevel = 0)
      : pUAVTarget(pUAV),
        mipLevel(mipLevel)
    {}

    WeakSPtr<Texture> pUAVTarget;
    int32 mipLevel;
  };

  namespace CPU_ACCESS_FLAG {
    enum E {
      WRITE = 0x10000L,
      READ = 0x20000L
    };
  }

  struct MappedSubresource
  {
    void* pData;
    uint32 rowPitch;
    uint32 depthPitch;
  };

  namespace VERTEX_ELEMENT_SEMANTIC {
    enum E {
      POSITION = 0,     // Position in 3D space
      NORMAL = 1,       // Normal vector for lighting calculations
      TANGENT = 2,      // Tangent vector for normal mapping
      BITANGENT = 3,    // Bitangent vector for normal mapping
      COLOR = 4,        // Vertex color
      TEXCOORD = 5,     // Texture coordinates (UV mapping)
      BLENDINDICES = 6, // Indices for skinning (bone indices)
      BLENDWEIGHT = 7,  // Weights for skinning (bone weights)
      POSITIONT = 8,    // For tessellation
      PSIZE = 9,        // For point sprites
      CUSTOM = 10,      // Custom semantic for user-defined data
    };
  }

  namespace VERTEX_ELEMENT_TYPE {
    enum E {
      FLOAT1 = 0,       // 1D floating point value
      FLOAT2 = 1,       // 2D floating point value
      FLOAT3 = 2,       // 3D floating point value
      FLOAT4 = 3,       // 4D floating point value
      COLOR = 4,        // Color encoded in 32-bits (8-bits per channel).
      SHORT1 = 5,       // 1D 16-bit signed integer value
      SHORT2 = 6,       // 2D 16-bit signed integer value
      SHORT4 = 8,       // 4D 16-bit signed integer value
      UBYTE4 = 9,       // 4D 8-bit unsigned integer value
      COLOR_ARGB = 10,  // 32-bits Color (8-bits per channel) in ARGB order)
      COLOR_ABGR = 11,  // 32-bits Color (8-bits per channel) in ABGR order)
      UINT4 = 12,       // 4D 32-bit unsigned integer value
      INT4 = 13,        // 4D 32-bit signed integer value
      USHORT1 = 14,     // 1D 16-bit unsigned integer value
      USHORT2 = 15,     // 2D 16-bit unsigned integer value
      USHORT4 = 17,     // 4D 16-bit unsigned integer value
      INT1 = 18,        // 1D 32-bit signed integer value
      INT2 = 19,        // 2D 32-bit signed integer value
      INT3 = 20,        // 3D 32-bit signed integer value
      UINT1 = 21,       // 1D 32-bit signed integer value
      UINT2 = 22,       // 2D 32-bit signed integer value
      UINT3 = 23,       // 3D 32-bit signed integer value
      UBYTE4_NORM = 24, // 4D 8-bit uint value normalized to [0, 1] range
      COUNT,            // Keep at end before UNKNOWN
      UNKNOWN = 0xffff
    };
  }

  struct ShaderMacro
  {
    String name;
    String definition;
  };

  namespace FILL_MODE {
    enum E {
      WIREFRAME = 2,
      SOLID = 3
    };
  }

  namespace CULL_MODE {
    enum E {
      NONE = 1,
      FRONT = 2,
      BACK = 3
    };
  }

  namespace CONSERVATIVE_RASTERIZATION_MODE {
    enum E {
      OFF = 0,
      ON = 1
    };
  }

  struct RASTERIZER_DESC
  {
    FILL_MODE::E fillMode;
    CULL_MODE::E cullMode;
    int32 frontCounterClockwise;
    int32 depthBias;
    float depthBiasClamp;
    float slopeScaledDepthBias;
    int32 depthClipEnable;
    int32 scissorEnable;
    int32 multisampleEnable;
    int32 antialiasedLineEnable;
    uint32 forcedSampleCount;
    CONSERVATIVE_RASTERIZATION_MODE::E conservativeRaster;

    explicit RASTERIZER_DESC(FORCE_INIT::E) {
      fillMode = FILL_MODE::SOLID;
      cullMode = CULL_MODE::BACK;
      frontCounterClockwise = 0;
      depthBias = 0;
      depthBiasClamp = 0.0f;
      slopeScaledDepthBias = 0.0f;
      depthClipEnable = 1;
      scissorEnable = 0;
      multisampleEnable = 0;
      antialiasedLineEnable = 0;
      forcedSampleCount = 0;
      conservativeRaster = CONSERVATIVE_RASTERIZATION_MODE::OFF;
    }
  };

  namespace COMPARISON_FUNC {
    enum E {
      NEVER = 1,
      LESS = 2,
      EQUAL = 3,
      LESS_EQUAL = 4,
      GREATER = 5,
      NOT_EQUAL = 6,
      GREATER_EQUAL = 7,
      ALWAYS = 8
    };
  }

  namespace DEPTH_WRITE_MASK {
    enum E {
      ZERO = 0,
      ALL = 1
    };
  }

  namespace STENCIL_OP {
    enum E {
      KEEP = 1,
      ZERO = 2,
      REPLACE = 3,
      INCR_SAT = 4,
      DECR_SAT = 5,
      INVERT = 6,
      INCR = 7,
      DECR = 8
    };
  }

  struct DEPTH_STENCILOP_DESC
  {
    int32 stencilFailOp;
    int32 stencilDepthFailOp;
    int32 stencilPassOp;
    int32 stencilFunc;
  };

  struct DEPTH_STENCIL_DESC
  {
    int32 depthEnable;
    DEPTH_WRITE_MASK::E depthWriteMask;
    COMPARISON_FUNC::E depthFunc;
    int32 stencilEnable;
    uint8 stencilReadMask;
    uint8 stencilWriteMask;
    DEPTH_STENCILOP_DESC frontFace;
    DEPTH_STENCILOP_DESC backFace;

    explicit DEPTH_STENCIL_DESC(FORCE_INIT::E)
    {
      depthEnable = 1;
      depthWriteMask = DEPTH_WRITE_MASK::ALL;
      depthFunc = COMPARISON_FUNC::LESS;
      stencilEnable = 0;
      stencilReadMask = 0xff;
      stencilWriteMask = 0xff;
      const DEPTH_STENCILOP_DESC defaultStencilOp = {
        STENCIL_OP::KEEP,
        STENCIL_OP::KEEP,
        STENCIL_OP::KEEP,
        COMPARISON_FUNC::ALWAYS
      };
      frontFace = defaultStencilOp;
      backFace = defaultStencilOp;
    }
  };

  namespace SAMPLER_FILTER {
    enum E {
      MIN_MAG_MIP_POINT = 0,
      MIN_MAG_POINT_MIP_LINEAR = 0x1,
      MIN_POINT_MAG_LINEAR_MIP_POINT = 0x4,
      MIN_POINT_MAG_MIP_LINEAR = 0x5,
      MIN_LINEAR_MAG_MIP_POINT = 0x10,
      MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x11,
      MIN_MAG_LINEAR_MIP_POINT = 0x14,
      MIN_MAG_MIP_LINEAR = 0x15,
      ANISOTROPIC = 0x55,
      COMPARISON_MIN_MAG_MIP_POINT = 0x80,
      COMPARISON_MIN_MAG_POINT_MIP_LINEAR = 0x81,
      COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x84,
      COMPARISON_MIN_POINT_MAG_MIP_LINEAR = 0x85,
      COMPARISON_MIN_LINEAR_MAG_MIP_POINT = 0x90,
      COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
      COMPARISON_MIN_MAG_LINEAR_MIP_POINT = 0x94,
      COMPARISON_MIN_MAG_MIP_LINEAR = 0x95,
      COMPARISON_ANISOTROPIC = 0xd5,
      MINIMUM_MIN_MAG_MIP_POINT = 0x100,
      MINIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x101,
      MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x104,
      MINIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x105,
      MINIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x110,
      MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x111,
      MINIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x114,
      MINIMUM_MIN_MAG_MIP_LINEAR = 0x115,
      MINIMUM_ANISOTROPIC = 0x155,
      MAXIMUM_MIN_MAG_MIP_POINT = 0x180,
      MAXIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x181,
      MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x184,
      MAXIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x185,
      MAXIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x190,
      MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x191,
      MAXIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x194,
      MAXIMUM_MIN_MAG_MIP_LINEAR = 0x195,
      MAXIMUM_ANISOTROPIC = 0x1d5
    };
  }

  namespace TEXTURE_ADDRESS_MODE {
    enum E {
      WRAP = 1,
      MIRROR = 2,
      CLAMP = 3,
      BORDER = 4,
      MIRROR_ONCE = 5
    };
  }

  struct SAMPLER_DESC
  {
    SAMPLER_FILTER::E filter;
    TEXTURE_ADDRESS_MODE::E addressU;
    TEXTURE_ADDRESS_MODE::E addressV;
    TEXTURE_ADDRESS_MODE::E addressW;
    float mipLODBias;
    uint32 maxAnisotropy;
    COMPARISON_FUNC::E comparisonFunc;
    float borderColor[4];
    float minLOD;
    float maxLOD;

    explicit SAMPLER_DESC(FORCE_INIT::E)
    {
      filter = SAMPLER_FILTER::MIN_MAG_MIP_LINEAR;
      addressU = TEXTURE_ADDRESS_MODE::CLAMP;
      addressV = TEXTURE_ADDRESS_MODE::CLAMP;
      addressW = TEXTURE_ADDRESS_MODE::CLAMP;
      mipLODBias = 0;
      maxAnisotropy = 1;
      comparisonFunc = COMPARISON_FUNC::NEVER;
      borderColor[0] = 1.0f;
      borderColor[1] = 1.0f;
      borderColor[2] = 1.0f;
      borderColor[3] = 1.0f;
      minLOD = -3.402823466e+38F; // -FLT_MAX
      maxLOD = 3.402823466e+38F;  //  FLT_MAX
    }
  };

  namespace BLEND {
    enum E {
      ZERO = 1,
      ONE = 2,
      SRC_COLOR = 3,
      INV_SRC_COLOR = 4,
      SRC_ALPHA = 5,
      INV_SRC_ALPHA = 6,
      DEST_ALPHA = 7,
      INV_DEST_ALPHA = 8,
      DEST_COLOR = 9,
      INV_DEST_COLOR = 10,
      SRC_ALPHA_SAT = 11,
      BLEND_FACTOR = 14,
      INV_BLEND_FACTOR = 15,
      SRC1_COLOR = 16,
      INV_SRC1_COLOR = 17,
      SRC1_ALPHA = 18,
      INV_SRC1_ALPHA = 19
    };
  }

  namespace BLEND_OP {
    enum E {
      ADD = 1,
      SUBTRACT = 2,
      REV_SUBTRACT = 3,
      MIN = 4,
      MAX = 5
    };
  }

  namespace COLOR_WRITE_ENABLE {
    enum E {
      RED = 1,
      GREEN = 2,
      BLUE = 4,
      ALPHA = 8,
      ALL = (((RED | GREEN) | BLUE) | ALPHA)
    };
  }

  namespace LOGIC_OP {
    enum E {
      CLEAR = 0,
      SET = (CLEAR + 1),
      COPY = (SET + 1),
      COPY_INVERTED = (COPY + 1),
      NOOP = (COPY_INVERTED + 1),
      INVERT = (NOOP + 1),
      AND = (INVERT + 1),
      NAND = (AND + 1),
      OR = (NAND + 1),
      NOR = (OR + 1),
      XOR = (NOR + 1),
      EQUIV = (XOR + 1),
      AND_REVERSE = (EQUIV + 1),
      AND_INVERTED = (AND_REVERSE + 1),
      OR_REVERSE = (AND_INVERTED + 1),
      OR_INVERTED = (OR_REVERSE + 1)
    };
  }

  struct RENDER_TARGET_BLEND_DESC
  {
    uint32 blendEnable;
    uint32 logicOpEnable;
    BLEND::E srcBlend;
    BLEND::E destBlend;
    BLEND_OP::E blendOp;
    BLEND::E srcBlendAlpha;
    BLEND::E destBlendAlpha;
    BLEND_OP::E blendOpAlpha;
    LOGIC_OP::E logicOp;
    uint8 renderTargetWriteMask;
  };

  struct BLEND_DESC
  {
    uint32 alphaToCoverageEnable;
    uint32 independentBlendEnable;
    RENDER_TARGET_BLEND_DESC renderTarget[8];

    explicit BLEND_DESC(FORCE_INIT::E) {
      alphaToCoverageEnable = 0;
      independentBlendEnable = 0;
      const RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
      {
          0,0,
          BLEND::ONE, BLEND::ZERO, BLEND_OP::ADD,
          BLEND::ONE, BLEND::ZERO, BLEND_OP::ADD,
          LOGIC_OP::NOOP,
          COLOR_WRITE_ENABLE::ALL,
      };

      for (uint32 i = 0; i < 8; ++i) {
        renderTarget[i] = defaultRenderTargetBlendDesc;
      }
    }
  };

} // namespace geEngineSDK
