/*****************************************************************************/
/**
 * @file    geActor.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/04/24
 * @brief   This class represents an actor in the scene.
 *
 * This class represents an actor in the scene. An actor is a container for
 * components, which are the building blocks of the game objects in the engine.
 * An actor has a scene node, which defines its position, rotation and scale in
 * the scene, and a list of components that define its behavior and appearance.
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
#include "geSceneNode.h"
#include "geComponent.h"

namespace geEngineSDK {
  class Scene;

  class GE_CORE_EXPORT Actor
  {
   public:
    explicit Actor(Scene* scene);
    ~Actor();

    Scene*
    getScene() const;

    SceneNode&
    getSceneNode();

    const SceneNode&
    getSceneNode() const;

    void
    setName(const String& name);

    const String&
    getName() const;

    void
    setActive(bool active);

    bool
    isActive() const;

    template<class T, class... Args>
    SPtr<T>
    addComponent(Args&&... args) {
      SPtr<T> component = ge_shared_ptr_new<T>(std::forward<Args>(args)...);
      component->m_owner = this;

      m_components.push_back(component);
      component->onAttach();

      return component;
    }

    template<class T>
    T*
    getComponent(uint32 index = 0) const {
      const uint32 typeId = T::kTypeId;
      uint32 count = 0;

      for (const auto& component : m_components) {
        if (component && component->getTypeId() == typeId) {
          if (index == count) {
            return static_cast<T*>(component.get());
          }
          count++;
        }
      }

      return nullptr;
    }

    template<class T>
    Vector<T*>
    getComponents() const {
      const uint32 typeId = T::kTypeId;
      Vector<T*> result;

      for (const auto& component : m_components) {
        if (component && component->getTypeId() == typeId) {
          result.push_back(static_cast<T*>(component.get()));
        }
      }

      return result;
    }

    template<class T>
    bool
    removeComponent(uint32 index = 0) {
      const uint32 typeId = T::kTypeId;
      uint32 count = 0;

      for (auto it = m_components.begin(); it != m_components.end(); ++it) {
        if ((*it)->getTypeId() == typeId) {
          if (index == count) {
            (*it)->onDetach();
            (*it)->m_owner = nullptr;
            m_components.erase(it);
            return true;
          }
          count++;
        }
      }

      return false;
    }

    void
    update(float dt);

   private:
    Scene* m_scene = nullptr;
    SPtr<SceneNode> m_sceneNode;

    String m_name;
    bool m_active = true;

    Vector<SPtr<Component>> m_components;
  };

} // namespace geEngineSDK
