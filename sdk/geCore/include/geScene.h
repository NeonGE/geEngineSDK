/*****************************************************************************/
/**
 * @file    geScene.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/04/27
 * @brief   
 *
 * 
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
#include "geActor.h"

namespace geEngineSDK {
  class Scene
  {
   public:
    Scene() = default;
    ~Scene() = default;

    SPtr<Actor>
    createActor(const String& name = "");

    void
    destroyActor(Actor* actor);

    const Vector<SPtr<Actor>>&
    getActors() const;

    void
    update(float dt);

    template<class Func>
    void
    forEachActor(Func&& func) {
      for (auto& actor : m_actors) {
        if (actor) {
          func(*actor);
        }
      }
    }

   private:
    Vector<SPtr<Actor>> m_actors;
  };

} // namespace geEngineSDK
