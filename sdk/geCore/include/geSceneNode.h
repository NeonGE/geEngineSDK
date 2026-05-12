/*****************************************************************************/
/**
 * @file    geSceneNode.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/04/24
 * @brief   This class represents a node in the scene graph.
 *
 * This class represents a node in the scene graph.
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
#include <geTransform.h>

namespace geEngineSDK {
  class SceneNode
  {
   public:
    SceneNode() = default;
    ~SceneNode() = default;

    SceneNode*
    getParent() const {
      return m_parent;
    }

    const Vector<SPtr<SceneNode>>&
    getChildren() const {
      return m_children;
    }

    void
    setLocalTransform(const Transform& t) {
      m_local = t;
      markDirty();
    }

    Transform&
    getLocalTransform() {
      markDirty();
      return m_local;
    }

    const Transform&
    getLocalTransform() const {
      return m_local;
    }

    const Matrix4&
    getWorldMatrix() const {
      return m_world;
    }

    void
    setVisible(bool value) {
      m_visible = value;
    }

    bool
    isVisible() const {
      return m_visible;
    }

    SPtr<SceneNode>
    createChild() {
      SPtr<SceneNode> child = ge_shared_ptr_new<SceneNode>();
      child->m_parent = this;
      m_children.push_back(child);
      child->markDirty();
      return child;
    }

    void
    attachChild(const SPtr<SceneNode>& child) {
      GE_ASSERT(child);
      GE_ASSERT(child.get() != this);

      if (child->m_parent) {
        child->m_parent->detachChild(child.get());
      }

      child->m_parent = this;
      m_children.push_back(child);
      child->markDirty();
    }

    SPtr<SceneNode>
    detachChild(SceneNode* child) {
      for (auto it = m_children.begin(); it != m_children.end(); ++it) {
        if (it->get() == child) {
          SPtr<SceneNode> result = *it;
          result->m_parent = nullptr;
          m_children.erase(it);
          result->markDirty();
          return result;
        }
      }

      return nullptr;
    }

    void
    updateWorldRecursive(const Matrix4* parentWorld = nullptr, bool parentDirty = false) {
      bool needsUpdate = m_dirty || parentDirty;
      if (needsUpdate) {
        Matrix4 localM = m_local.toMatrixWithScale();
        m_world = parentWorld ? (localM * (*parentWorld)) : localM;
        m_dirty = false;
      }

      for (auto& child : m_children) {
        child->updateWorldRecursive(&m_world, needsUpdate);
      }
    }

    void
    markDirty() {
      if (m_dirty) {
        return;
      }

      m_dirty = true;

      for (auto& child : m_children) {
        child->markDirty();
      }
    }

   private:
    SceneNode* m_parent = nullptr;
    Vector<SPtr<SceneNode>> m_children;

    Transform m_local;
    Matrix4 m_world = Matrix4::IDENTITY;

    bool m_dirty = true;
    bool m_visible = true;
  };

} // namespace geEngineSDK
