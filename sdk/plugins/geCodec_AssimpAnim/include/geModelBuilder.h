#pragma once

#include <geModel.h>
#include <geVertexDeclaration.h>

namespace geEngineSDK {

  struct ModelBuilderSubMesh
  {
    String name;

    uint32 firstVertex = 0;
    uint32 vertexCount = 0;

    uint32 firstIndex = 0;
    uint32 indexCount = 0;

    uint32 materialIndex = 0;
    uint32 nodeIndex = NumLimit::MAX_UINT32;

    uint32 faceCount = 0;

    bool isSkinned = false;
    uint32 skinBindingIndex = NumLimit::MAX_UINT32;

    AABox bounds = AABox::EMPTY;
    Sphere boundingSphere;
  };

  struct ModelBuilderMeshGroup
  {
    String name;

    bool isSkinned = false;

    SPtr<VertexDeclaration> vertexDecl;
    uint32 vertexSize = 0;

    PRIMITIVE_TOPOLOGY::E topology = PRIMITIVE_TOPOLOGY::TRIANGLELIST;

    Vector<uint8> vertices;
    Vector<uint32> indices;
    Vector<ModelBuilderSubMesh> subMeshes;

    AABox bounds = AABox::EMPTY;
    Sphere boundingSphere;
  };

  class ModelBuilder
  {
   public:
    void
    clear();

    uint32
    addNode(const ModelNode& node);

    void
    setNodeParent(uint32 nodeIndex, int32 parentIndex);

    uint32
    addMeshGroup(const String& name,
                 bool isSkinned,
                 const SPtr<VertexDeclaration>& vertexDecl,
                 PRIMITIVE_TOPOLOGY::E topology);

    uint32
    findOrCreateStaticMeshGroup(const String& name,
                                const SPtr<VertexDeclaration>& vertexDecl,
                                PRIMITIVE_TOPOLOGY::E topology);

    uint32
    findOrCreateSkinnedMeshGroup(const String& name,
                                 const SPtr<VertexDeclaration>& vertexDecl,
                                 PRIMITIVE_TOPOLOGY::E topology);

    uint32
    addSkinBinding(const String& name, const Vector<Matrix4>& boneOffsets);

    uint32
    appendStaticSubMesh(uint32 meshGroupIndex,
                        const String& name,
                        uint32 nodeIndex,
                        uint32 materialIndex,
                        const Vector<uint8>& vertices,
                        const Vector<uint32>& indices,
                        const AABox& bounds,
                        uint32 faceCount);

    uint32
    appendSkinnedSubMesh(uint32 meshGroupIndex,
                         const String& name,
                         uint32 nodeIndex,
                         uint32 materialIndex,
                         uint32 skinBindingIndex,
                         const Vector<uint8>& vertices,
                         const Vector<uint32>& indices,
                         const AABox& bounds,
                         uint32 faceCount);

    void
    updateBounds();

    SPtr<Model>
    build() const;

   public:
    Vector<ModelNode> m_nodes;
    Vector<ModelBuilderMeshGroup> m_meshGroups;

    String m_skeletonName;
    Vector<SkinBinding> m_skinBindings;

    AABox m_bounds = AABox::EMPTY;
    Sphere m_boundingSphere;
  };

}