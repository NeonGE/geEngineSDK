/*****************************************************************************/
/**
 * @file    geScene.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/04/27
 * @brief   
 *
 * 
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geScene.h"

namespace geEngineSDK {
  SPtr<Actor>
  Scene::createActor(const String& name) {
    SPtr<Actor> actor = ge_shared_ptr_new<Actor>(this);
    actor->setName(name);

    m_actors.push_back(actor);
    return actor;
  }

  void
  Scene::destroyActor(Actor* actor) {
    if (!actor) {
      return;
    }

    for (auto it = m_actors.begin(); it != m_actors.end(); ++it) {
      if (it->get() == actor) {
        m_actors.erase(it);
        return;
      }
    }
  }

  const Vector<SPtr<Actor>>&
  Scene::getActors() const {
    return m_actors;
  }

  void
  Scene::update(float dt) {
    for (auto& actor : m_actors) {
      if (actor) {
        actor->update(dt);
      }
    }

    for (auto& actor : m_actors) {
      if (!actor) {
        continue;
      }

      SceneNode& node = actor->getSceneNode();
      if (!node.getParent()) {
        node.updateWorldRecursive(nullptr);
      }
    }
  }

} // namespace geEngineSDK
