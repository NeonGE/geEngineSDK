/*****************************************************************************/
/**
 * @file    geActor.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/04/24
 * @brief   This class represents a node in the scene graph.
 *
 * This class represents a node in the scene graph.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geActor.h"
#include "geScene.h"

namespace geEngineSDK {
  Actor::Actor(Scene* scene)
    : m_scene(scene) {
    m_sceneNode = ge_shared_ptr_new<SceneNode>();
  }

  Actor::~Actor() {
    for (auto& component : m_components) {
      if (component) {
        component->onDetach();
        component->m_owner = nullptr;
      }
    }

    m_components.clear();
  }

  Scene*
  Actor::getScene() const {
    return m_scene;
  }

  SceneNode&
  Actor::getSceneNode() {
    return *m_sceneNode;
  }

  const SceneNode&
  Actor::getSceneNode() const {
    return *m_sceneNode;
  }

  void
  Actor::setName(const String& name) {
    m_name = name;
  }

  const String&
  Actor::getName() const {
    return m_name;
  }

  void
  Actor::setActive(bool active) {
    m_active = active;
  }

  bool
  Actor::isActive() const {
    return m_active;
  }

  void
  Actor::update(float dt) {
    if (!m_active) {
      return;
    }

    for (auto& component : m_components) {
      if (component) {
        component->update(dt);
      }
    }
  }

} // namespace geEngineSDK
