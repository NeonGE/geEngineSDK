/*****************************************************************************/
/**
 * @file    geComponent.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/04/24
 * @brief   This class represents a component in the scene.
 *
 * This class represents a component in the scene. A component is a building
 * block of an actor, which defines its behavior and appearance. A component
 * is attached to an actor, and can access its owner actor and other components
 * attached to the same actor. A component can be updated every frame, and can
 * be added or removed from an actor at runtime.
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

namespace geEngineSDK {
  class Actor;

  class Component
  {
   public:
    virtual ~Component() = default;

    Actor*
    getOwner() const {
      return m_owner;
    }

    virtual void
    onAttach() {}

    virtual void
    onDetach() {}

    virtual void
    update(float dt) {}

    virtual uint32
    getTypeId() const = 0;

   protected:
    Actor* m_owner = nullptr;

    friend class Actor;
  };

} // namespace geEngineSDK
