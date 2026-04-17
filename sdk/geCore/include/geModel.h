/*****************************************************************************/
/**
 * @file    geModel.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/03/12
 * @brief   Model resource types.
 *
 * Contains the basic data structures used to represent a model resource in
 * the engine. This includes node hierarchy information, mesh containers and
 * sub-mesh draw ranges.
 *
 * These classes are intentionally focused on shared asset data only.
 * Per-instance state such as animation playback or material overrides should
 * live elsewhere.
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
#include "geGraphicsTypes.h"
#include "geResource.h"
#include "geMatrix4.h"
#include "geBox.h"
#include "geSphere.h"

namespace geEngineSDK {

  class VertexBuffer;
  class InputLayout;
  class IndexBuffer;
  class Skeleton;

  struct GE_CORE_EXPORT SkinBinding
  {
    String m_name;
    Vector<Matrix4> m_boneOffsets;
  };

  /**
   * @brief Describes a drawable range inside a mesh buffer.
   *
   * A SubMesh does not own GPU resources. It simply references a range inside
   * a shared vertex/index buffer pair.
   */
  class GE_CORE_EXPORT SubMesh
  {
   public:
    SubMesh() = default;
    explicit SubMesh(const String& name);

    /**
     * @brief Returns true if this submesh contains valid geometry.
     */
    bool
    isValid() const;

   public:
    String m_name;

    uint32 m_firstIndex = 0;
    uint32 m_indexCount = 0;

    int32  m_baseVertex = 0;
    uint32 m_vertexCount = 0;

    uint32 m_materialIndex = 0;
    uint32 m_nodeIndex = NumLimit::MAX_UINT32;
    uint32 m_faceCount = 0;

    bool m_isSkinned = false;
    uint32 m_skinBindingIndex = NumLimit::MAX_UINT32;

    AABox m_bounds = AABox::EMPTY;
    Sphere m_boundingSphere;
  };

  struct NodeSubMeshRef
  {
    uint32 meshIndex = NumLimit::MAX_UINT32;
    uint32 subMeshIndex = NumLimit::MAX_UINT32;
  };

  /**
   * @brief Scene/model node used to store imported hierarchy.
   *
   * Nodes can represent transforms, pivots, attachment points or mesh owners.
   * This is intentionally independent from any import library representation.
   */
  class GE_CORE_EXPORT ModelNode
  {
   public:
    ModelNode() = default;
    explicit ModelNode(const String& name);

    /**
     * @brief Returns true if the node has a valid parent.
     */
    bool
    hasParent() const;

   public:
    String m_name;

    int32 m_parentIndex = -1;
    Vector<uint32> m_children;

    Matrix4 m_localTransform = Matrix4::IDENTITY;
    Matrix4 m_worldTransform = Matrix4::IDENTITY;

    Vector<NodeSubMeshRef> m_subMeshes;
  };

  struct GE_CORE_EXPORT ModelBone
  {
    String  m_name;
    uint32  m_nodeIndex = NumLimit::MAX_UINT32;

    //Assimp aiBone::mOffsetMatrix converted to Matrix4
    //This is the inverse bind in mesh/model space.
    Matrix4 m_offsetMatrix = Matrix4::IDENTITY;
  };

  /**
   * @brief GPU mesh data plus submesh ranges.
   *
   * A MeshData usually owns one vertex buffer and one index buffer, and then
   * exposes one or more SubMesh entries that reference draw ranges inside them.
   */
  class GE_CORE_EXPORT MeshData
  {
   public:
    MeshData() = default;
    explicit MeshData(const String& name);

    /**
     * @brief Returns true if the mesh owns both vertex and index buffers.
     */
    bool
    hasValidBuffers() const;

    /**
     * @brief Returns true if any of its submeshes are skinned.
     */
    bool
    isSkinned() const;

    /**
     * @brief Recomputes the mesh bounds from all submeshes.
     */
    void
    updateBounds();

   public:
    String m_name;

    SPtr<VertexBuffer> m_vertexBuffer;
    SPtr<IndexBuffer> m_indexBuffer;
    SPtr<InputLayout> m_inputLayout;

    PRIMITIVE_TOPOLOGY::E m_topology = PRIMITIVE_TOPOLOGY::TRIANGLELIST;

    Vector<SubMesh> m_subMeshes;

    AABox m_bounds = AABox::EMPTY;
    Sphere m_boundingSphere;
  };

  /**
   * @brief Shared model resource.
   *
   * Model contains the imported node hierarchy and a set of mesh containers.
   * It represents immutable/shared asset data and is intended to be reused by
   * multiple instances.
   */
  class GE_CORE_EXPORT Model : public Resource
  {
   public:
    Model() = default;
    virtual ~Model() = default;

    /**
     * @brief Returns number of mesh containers in this model.
     */
    uint32
    getNumMeshes() const;

    /**
     * @brief Returns number of nodes in this model hierarchy.
     */
    uint32
    getNumNodes() const;

    /**
     * @brief Returns total number of submeshes across all meshes.
     */
    uint32
    getNumSubMeshes() const;

    uint32
    getNumSkinBindings() const;

    /**
     * @brief Clears all model data.
     */
    void
    clear();

    /**
     * @brief Recomputes model bounds from all meshes.
     */
    void
    updateBounds();

    /**
     * @brief Finds a node by name.
     *
     * @return Node index or -1 if not found.
     */
    int32
    findNodeIndex(const String& name) const;

    /**
     * @brief Finds a mesh by name.
     *
     * @return Mesh index or -1 if not found.
     */
    int32
    findMeshIndex(const String& name) const;

    int32
    findSkinBindingIndex(const String& name) const;

    /**
     * @brief Returns the root node index if one exists.
     *
     * @return Root node index or -1 if none exists.
     */
    int32
    getRootNodeIndex() const;

    /**
     * @brief Helper that checks if the hierarchy of the model is valid.
     *
     * @return true if is valid or false if it's not.
     */
    bool
    validateHierarchy() const;

    /**
     * @brief Recomputes world transforms for all nodes.
     */
    void
    updateWorldTransforms();

    /**
     * @brief Recomputes world transform starting at the specified node.
     */
    void
    updateWorldTransform(uint32 nodeIndex);

    //Resource overrides
    bool
    load(const Path& filePath) override;

    void
    unload() override;

    bool
    isLoaded() const override;

    SIZE_T
    getMemoryUsage() const override;

  public:
    Vector<ModelNode> m_nodes;
    Vector<MeshData> m_meshes;

    SPtr<Skeleton> m_skeleton;
    Vector<SkinBinding> m_skinBindings;

    AABox m_bounds = AABox::EMPTY;
    Sphere m_boundingSphere;
  };

} // namespace geEngineSDK
