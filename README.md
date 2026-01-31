[![Coverage Status](https://coveralls.io/repos/github/NeonGE/geEngineSDK/badge.svg)](https://coveralls.io/github/NeonGE/geEngineSDK)

# geEngineSDK

**geEngineSDK** is a modern, cross-platform, C++ game engine and rendering framework designed for high-performance real-time graphics, advanced rendering research, and professional production workflows.

It is built from the ground up with a strong focus on:

- Physically Based Rendering (PBR)
- Advanced shader pipelines (HLSL / compute / post-processing)
- Robust asset and animation systems
- Cross-platform architecture
- Modular plugin-based resource handling
- Engine tooling and developer productivity

This engine is not a minimal demo framework — it is a **production-grade engine architecture** designed to scale to complex projects, large scenes, and advanced rendering techniques.

---

## Philosophy

geEngineSDK follows these principles:

- **Everything is explicit** – no hidden magic, no opaque systems.
- **Renderer first** – graphics and shading pipelines are first-class citizens.
- **Cross-platform by design** – Windows, Linux, macOS, consoles.
- **Data-oriented and cache-friendly design**
- **Modular and extensible** via plugins and codecs
- **Tooling is part of the engine**, not an afterthought
- **Engine architecture clarity** over shortcuts

---

## High Level Architecture

The engine is composed of several major subsystems:

- Rendering Core (DirectX 11/12, HLSL, compute shaders)
- Asset Pipeline (textures, meshes, animations, codecs)
- Scene Graph + ECS integration
- Animation and Skeleton system
- Resource Manager with async loading
- Plugin-based codec system
- Cross-platform platform utilities
- Engine tooling (debug, inspectors, editors, thumbnail providers, etc.)

---

## Key Features

### Rendering

- Physically Based Rendering (Cook-Torrance, GGX, Disney, Clear-Coat)
- IBL, HDR pipelines, tone mapping (ACES, AGX)
- TAA, SSAO, DoF, Bloom, Volumetrics
- Transmission, SSS, advanced BRDF experiments
- Compute-driven post-processing
- Support for future DXR / ray tracing integration
- HDR skymaps and exposure control
- Advanced shader experimentation platform

### Assets & Resources

- DDS / HDR / BCn texture pipeline (NVTT integration)
- Mesh loading (OBJ, GLTF, custom formats)
- Assimp-based animation and skeleton loading
- Progressive mesh / LOD research
- UUID-based resource identification
- File watchers and hot-reload
- Plugin/DLL-based resource codecs

### Scene & Animation

- Scene Graph designed to work with ECS
- Skeleton, bones, bind poses, animation clips
- Proper local/global transform propagation
- Skinned mesh binding
- Animation pose application pipeline

### Tooling

- ImGui integration
- Thumbnail providers
- Debug visualizers (normals, wireframes, overlays)
- Shader experimentation tools
- Platform utilities abstraction (Win32 / POSIX / consoles)

---

## Supported Platforms & Toolchains

| Platform | Compiler | Status |
|----------|----------|--------|
| Windows  | MSVC     | ✅ Primary |
| Linux    | GCC / Clang | ✅ Supported |
| macOS    | Clang    | 🚧 In progress |
| Consoles | PS5 / Xbox | 🚧 Architectural support |

Build system:

- CMake (multi-platform)
- Designed for CI (CircleCI / containers)

---

## Building

```bash
git clone <repo>
mkdir build
cd build
cmake ..
cmake --build .
```

The project is designed to build cleanly on both Windows and Linux using the same CMake configuration.

---

## Directory Layout

```
/sdk
    /geRendering
    /geUtilities
    /geAnimation
    /geResources
    /gePlatform
    /geMath
/tools
/tests
```

Each module is designed to be as independent as possible and follows strict interface boundaries.

---

## Third-Party Libraries

- Assimp
- NVTT 3
- EnTT (ECS integration)
- Catch2 (testing)
- ICU (Unicode handling)
- ImGui
- minizip-ng / compression utilities

All dependencies are integrated via CMake and FetchContent where possible.

---

## Design Principles

This engine is built with long-term maintainability in mind:

- No RTTI
- Minimal dynamic allocation in hot paths
- Extensive use of custom math types (Matrix4, Quaternion, Transform, etc.)
- Clear separation between local and global transforms
- Strict platform abstraction layers
- Extensive unit testing for math, transforms, quaternions, and animation logic

---

## Current Status

geEngineSDK is under active development and continuous refinement.

Major focus areas:

- Advanced rendering techniques
- Robust animation correctness
- Cross-platform stability
- Tooling and inspection utilities

---

## Intended Use

This engine is intended for:

- Rendering research
- Teaching advanced graphics programming
- Professional-grade game engine experimentation
- Real production projects

---

## License

Private / Proprietary (for now)
