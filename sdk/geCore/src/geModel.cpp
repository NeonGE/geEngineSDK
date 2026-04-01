/*****************************************************************************/
/**
 * @file    geModel.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/03/12
 * @brief   Model resource types implementation.
 *
 * Model resource types implementation.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geModel.h"

namespace geEngineSDK {

  /***************************************************************************/
  /**
   * SubMesh
   */
  /***************************************************************************/
  SubMesh::SubMesh(const String& name)
    : m_name(name)
  {}

  bool
  SubMesh::isValid() const {
    return m_vertexCount > 0 && m_indexCount > 0;
  }

  /***************************************************************************/
  /**
   * ModelNode
   */
  /***************************************************************************/
  ModelNode::ModelNode(const String& name)
    : m_name(name)
  {}

  bool
  ModelNode::hasParent() const {
    return m_parentIndex >= 0;
  }

  /***************************************************************************/
  /**
   * MeshData
   */
  /***************************************************************************/
  MeshData::MeshData(const String& name)
    : m_name(name)
  {}

  bool
  MeshData::hasValidBuffers() const {
    return nullptr != m_vertexBuffer && nullptr != m_indexBuffer;
  }

  bool
  MeshData::isSkinned() const {
    for (const auto& subMesh : m_subMeshes) {
      if (subMesh.m_isSkinned) {
        return true;
      }
    }

    return false;
  }

  void
  MeshData::updateBounds() {
    m_bounds = AABox::EMPTY;

    for (const auto& subMesh : m_subMeshes) {
      if (subMesh.isValid()) {
        m_bounds += subMesh.m_bounds;
      }
    }

    if (m_bounds.m_isValid) {
      m_boundingSphere = Sphere(m_bounds.getCenter(), m_bounds.getExtent().getMax());
    }
    else {
      m_boundingSphere = Sphere();
    }
  }

  /***************************************************************************/
  /**
   * Model
   */
  /***************************************************************************/
  uint32
  Model::getNumMeshes() const {
    return cast::st<uint32>(m_meshes.size());
  }

  uint32
  Model::getNumNodes() const {
    return cast::st<uint32>(m_nodes.size());
  }

  uint32
  Model::getNumSubMeshes() const {
    uint32 count = 0;
    for (const auto& mesh : m_meshes) {
      count += cast::st<uint32>(mesh.m_subMeshes.size());
    }

    return count;
  }

  void
  Model::clear() {
    m_nodes.clear();
    m_meshes.clear();

    m_bounds = AABox::EMPTY;
    m_boundingSphere = Sphere();
  }

  void
  Model::updateBounds() {
    m_bounds = AABox::EMPTY;

    for (auto& mesh : m_meshes) {
      mesh.updateBounds();
      if (!mesh.m_subMeshes.empty()) {
        m_bounds += mesh.m_bounds;
      }
    }

    if (m_bounds.m_isValid) {
      m_boundingSphere = Sphere(m_bounds.getCenter(), m_bounds.getExtent().getMax());
    }
    else {
      m_boundingSphere = Sphere();
    }
  }

  int32
  Model::findNodeIndex(const String& name) const {
    for (uint32 i = 0; i < cast::st<uint32>(m_nodes.size()); ++i) {
      if (m_nodes[i].m_name == name) {
        return cast::st<int32>(i);
      }
    }

    return -1;
  }

  int32
  Model::findMeshIndex(const String& name) const {
    for (uint32 i = 0; i < cast::st<uint32>(m_meshes.size()); ++i) {
      if (m_meshes[i].m_name == name) {
        return cast::st<int32>(i);
      }
    }

    return -1;
  }

  int32
  Model::getRootNodeIndex() const {
    for (uint32 i = 0; i < cast::st<uint32>(m_nodes.size()); ++i) {
      if (m_nodes[i].m_parentIndex < 0) {
        return cast::st<int32>(i);
      }
    }

    return -1;
  }

  bool
  Model::validateHierarchy() const {
    for (uint32 i = 0; i < m_nodes.size(); ++i) {
      const ModelNode& node = m_nodes[i];

      if (node.m_parentIndex >= 0) {
        if (node.m_parentIndex >= cast::st<int32>(m_nodes.size())) {
          return false;
        }
      }

      for (uint32 child : node.m_children) {
        if (child >= m_nodes.size()) {
          return false;
        }

        if (m_nodes[child].m_parentIndex != cast::st<int32>(i)) {
          return false;
        }
      }
    }

    return true;
  }

  void
  Model::updateWorldTransforms() {
    if (m_nodes.empty()) {
      return;
    }

    for (uint32 i = 0; i < cast::st<uint32>(m_nodes.size()); ++i) {
      ModelNode& node = m_nodes[i];
      if (node.m_parentIndex < 0) {
        node.m_worldTransform = node.m_localTransform;
        updateWorldTransform(i);
      }
    }
  }

  void
  Model::updateWorldTransform(uint32 nodeIndex) {
    GE_ASSERT(nodeIndex < m_nodes.size());

    ModelNode& node = m_nodes[nodeIndex];

    for (uint32 childIndex : node.m_children) {
      GE_ASSERT(childIndex < m_nodes.size());

      ModelNode& child = m_nodes[childIndex];
      child.m_worldTransform = child.m_localTransform * node.m_worldTransform;

      updateWorldTransform(childIndex);
    }
  }

  bool
  Model::load(const Path& filePath) {
    GE_UNREFERENCED_PARAMETER(filePath);
    return false;
  }

  void
  Model::unload() {

  }

  bool
  Model::isLoaded() const {
    return false;
  }

  SIZE_T
  Model::getMemoryUsage() const {
    return 0;
  }

} // namespace geEngineSDK
