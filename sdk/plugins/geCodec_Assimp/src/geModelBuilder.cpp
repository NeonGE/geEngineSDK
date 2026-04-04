#include "geModelBuilder.h"
#include <geRenderAPI.h>

namespace geEngineSDK {

  uint32
  getIndexElementSize(INDEX_BUFFER_FORMAT::E indexType) {
    switch (indexType) {
    case INDEX_BUFFER_FORMAT::R16_UINT:
      return sizeof(uint16);
    case INDEX_BUFFER_FORMAT::R32_UINT:
      return sizeof(uint32);
    default:
      GE_ASSERT(false);
      return sizeof(uint32);
    }
  }

  INDEX_BUFFER_FORMAT::E
  chooseIndexType(uint32 vertexCount) {
    return vertexCount <= NumLimit::MAX_UINT16 ?
                          INDEX_BUFFER_FORMAT::R16_UINT :
                          INDEX_BUFFER_FORMAT::R32_UINT;
  }

  Vector<uint8>
  buildIndexBufferData(const Vector<uint32>& indices, INDEX_BUFFER_FORMAT::E indexType) {
    Vector<uint8> out;

    if (indexType == INDEX_BUFFER_FORMAT::R16_UINT) {
      out.resize(indices.size() * sizeof(uint16));

      for (uint32 i = 0; i < cast::st<uint32>(indices.size()); ++i) {
        GE_ASSERT(indices[i] <= NumLimit::MAX_UINT16);

        uint16 value = cast::st<uint16>(indices[i]);
        memcpy(&out[i * sizeof(uint16)], &value, sizeof(uint16));
      }
    }
    else {
      out.resize(indices.size() * sizeof(uint32));
      memcpy(out.data(), indices.data(), out.size());
    }

    return out;
  }

  void
  ModelBuilder::clear() {
    m_nodes.clear();
    m_meshGroups.clear();
    m_bounds = AABox::EMPTY;
    m_boundingSphere = Sphere();
  }

  uint32
  ModelBuilder::addNode(const ModelNode& node) {
    m_nodes.push_back(node);
    return cast::st<uint32>(m_nodes.size() - 1);
  }

  void
  ModelBuilder::setNodeParent(uint32 nodeIndex, int32 parentIndex) {
    GE_ASSERT(nodeIndex < m_nodes.size());

    ModelNode& node = m_nodes[nodeIndex];

    if (node.m_parentIndex >= 0) {
      GE_ASSERT(node.m_parentIndex < cast::st<int32>(m_nodes.size()));

      ModelNode& oldParent = m_nodes[node.m_parentIndex];

      for (auto it = oldParent.m_children.begin(); it != oldParent.m_children.end(); ++it) {
        if (*it == nodeIndex) {
          oldParent.m_children.erase(it);
          break;
        }
      }
    }

    node.m_parentIndex = parentIndex;

    if (parentIndex >= 0) {
      GE_ASSERT(parentIndex < cast::st<int32>(m_nodes.size()));
      m_nodes[parentIndex].m_children.push_back(nodeIndex);
    }
  }

  uint32
  ModelBuilder::addMeshGroup(const String& name,
                             bool isSkinned,
                             const SPtr<VertexDeclaration>& vertexDecl,
                             PRIMITIVE_TOPOLOGY::E topology) {
    GE_ASSERT(nullptr != vertexDecl);

    ModelBuilderMeshGroup group;
    group.name = name;
    group.isSkinned = isSkinned;
    group.vertexDecl = vertexDecl;
    group.vertexSize = vertexDecl->getProperties().getVertexSize(0);
    group.topology = topology;

    m_meshGroups.push_back(group);
    return cast::st<uint32>(m_meshGroups.size() - 1);
  }

  uint32
  ModelBuilder::findOrCreateStaticMeshGroup(const String& name,
                                            const SPtr<VertexDeclaration>& vertexDecl,
                                            PRIMITIVE_TOPOLOGY::E topology) {
    for (uint32 i = 0; i < cast::st<uint32>(m_meshGroups.size()); ++i) {
      const ModelBuilderMeshGroup& group = m_meshGroups[i];

      if (group.isSkinned) {
        continue;
      }

      if (group.topology != topology) {
        continue;
      }

      if (group.vertexDecl->getProperties() != vertexDecl->getProperties()) {
        continue;
      }

      return i;
    }

    return addMeshGroup(name, false, vertexDecl, topology);
  }

  uint32
  ModelBuilder::appendStaticSubMesh(uint32 meshGroupIndex,
                                    const String& name,
                                    uint32 nodeIndex,
                                    uint32 materialIndex,
                                    const Vector<uint8>& vertices,
                                    const Vector<uint32>& indices,
                                    const AABox& bounds,
                                    uint32 faceCount) {
    GE_ASSERT(meshGroupIndex < m_meshGroups.size());
    GE_ASSERT(nodeIndex < m_nodes.size());

    ModelBuilderMeshGroup& group = m_meshGroups[meshGroupIndex];

    GE_ASSERT(!group.isSkinned);
    GE_ASSERT(group.vertexSize > 0);
    GE_ASSERT(vertices.size() % group.vertexSize == 0);

    ModelBuilderSubMesh subMesh;
    subMesh.name = name;
    subMesh.nodeIndex = nodeIndex;
    subMesh.materialIndex = materialIndex;
    subMesh.faceCount = faceCount;
    subMesh.isSkinned = false;

    subMesh.firstVertex = cast::st<uint32>(group.vertices.size() / group.vertexSize);
    subMesh.vertexCount = cast::st<uint32>(vertices.size() / group.vertexSize);

    subMesh.firstIndex = cast::st<uint32>(group.indices.size());
    subMesh.indexCount = cast::st<uint32>(indices.size());

    subMesh.bounds = bounds;

    if (subMesh.bounds.m_isValid) {
      subMesh.boundingSphere = Sphere(subMesh.bounds.getCenter(),
                                      subMesh.bounds.getExtent().getMax());
    }
    else {
      subMesh.boundingSphere = Sphere();
    }

    group.vertices.insert(group.vertices.end(), vertices.begin(), vertices.end());

    for (uint32 index : indices) {
      GE_ASSERT(index < subMesh.vertexCount);
      group.indices.push_back(subMesh.firstVertex + index);
    }

    const uint32 subMeshIndex = cast::st<uint32>(group.subMeshes.size());
    group.subMeshes.push_back(subMesh);

    group.bounds += subMesh.bounds;

    if (group.bounds.m_isValid) {
      group.boundingSphere = Sphere(group.bounds.getCenter(),
                                    group.bounds.getExtent().getMax());
    }
    else {
      group.boundingSphere = Sphere();
    }

    NodeSubMeshRef ref;
    ref.meshIndex = meshGroupIndex;
    ref.subMeshIndex = subMeshIndex;

    m_nodes[nodeIndex].m_subMeshes.push_back(ref);

    return subMeshIndex;
  }

  void
  ModelBuilder::updateBounds() {
    m_bounds = AABox::EMPTY;

    for (auto& group : m_meshGroups) {
      group.bounds = AABox::EMPTY;

      for (const auto& subMesh : group.subMeshes) {
        if (subMesh.indexCount > 0 && subMesh.vertexCount > 0) {
          group.bounds += subMesh.bounds;
        }
      }

      if (group.bounds.m_isValid) {
        group.boundingSphere =
          Sphere(group.bounds.getCenter(),
            group.bounds.getExtent().getMax());
      }
      else {
        group.boundingSphere = Sphere();
      }

      m_bounds += group.bounds;
    }

    if (m_bounds.m_isValid) {
      m_boundingSphere =
        Sphere(m_bounds.getCenter(), m_bounds.getExtent().getMax());
    }
    else {
      m_boundingSphere = Sphere();
    }
  }

  SPtr<Model>
  ModelBuilder::build() const {
    auto model = ge_shared_ptr_new<Model>();
    model->m_nodes = m_nodes;
    model->m_meshes.clear();
    model->m_meshes.reserve(m_meshGroups.size());

    auto& renderAPI = RenderAPI::instance();

    for (uint32 meshGroupIndex = 0;
         meshGroupIndex < cast::st<uint32>(m_meshGroups.size());
         ++meshGroupIndex) {
      const ModelBuilderMeshGroup& group = m_meshGroups[meshGroupIndex];

      // Skip empty groups
      if (nullptr == group.vertexDecl || group.vertexSize == 0 ||
          group.vertices.empty() || group.subMeshes.empty()) {
        continue;
      }

      MeshData meshData;
      meshData.m_name = group.name;
      meshData.m_topology = group.topology;
      meshData.m_bounds = group.bounds;
      meshData.m_boundingSphere = group.boundingSphere;

      const uint32 vertexCount =
        cast::st<uint32>(group.vertices.size() / group.vertexSize);

      const INDEX_BUFFER_FORMAT::E indexType = chooseIndexType(vertexCount);
      Vector<uint8> indexData = buildIndexBufferData(group.indices, indexType);

      //Vertex buffer creation
      meshData.m_vertexBuffer = renderAPI.createVertexBuffer(group.vertexDecl,
                                                             group.vertices.size(),
                                                             group.vertices.data());

      // Index buffer creation
      meshData.m_indexBuffer = renderAPI.createIndexBuffer(indexData.size(),
                                                           indexData.data(),
                                                           indexType);

      meshData.m_subMeshes.reserve(group.subMeshes.size());

      for (const auto& builderSubMesh : group.subMeshes) {
        SubMesh subMesh;
        subMesh.m_name = builderSubMesh.name;

        //Indices are already global in the mesh group.
        subMesh.m_baseVertex = 0;

        subMesh.m_vertexCount = builderSubMesh.vertexCount;
        subMesh.m_firstIndex = builderSubMesh.firstIndex;
        subMesh.m_indexCount = builderSubMesh.indexCount;
        subMesh.m_materialIndex = builderSubMesh.materialIndex;
        subMesh.m_nodeIndex = builderSubMesh.nodeIndex;
        subMesh.m_faceCount = builderSubMesh.faceCount;
        subMesh.m_isSkinned = builderSubMesh.isSkinned;
        subMesh.m_bounds = builderSubMesh.bounds;
        subMesh.m_boundingSphere = builderSubMesh.boundingSphere;
        meshData.m_subMeshes.push_back(subMesh);
      }

      const uint32 finalMeshIndex = cast::st<uint32>(model->m_meshes.size());
      model->m_meshes.push_back(meshData);

      //Remap node references from builder meshGroupIndex -> final model mesh index
      for (auto& node : model->m_nodes) {
        for (auto& ref : node.m_subMeshes) {
          if (ref.meshIndex == meshGroupIndex) {
            ref.meshIndex = finalMeshIndex;
          }
        }
      }
    }

    model->updateWorldTransforms();
    model->updateBounds();

    return model;
  }

} // namespace geEngineSDK