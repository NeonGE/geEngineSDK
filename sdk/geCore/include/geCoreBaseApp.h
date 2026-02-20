/*****************************************************************************/
/**
 * @file    geCoreBaseApp.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2023/03/04
 * @brief   Base Logic for all Apps created with the geEngine.
 *
 * Base Logic for all Apps created with the geEngine.
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
#include "geAppInputEvents.h"
#include <geVector2I.h>

#include <SFML/Window.hpp>

#define GE_COREBASE_CLASS geCoreBaseApp

namespace geEngineSDK {
  using sf::WindowBase;
  using geEngineSDK::Event;

  class GE_CORE_EXPORT GE_COREBASE_CLASS
  {
   public:
    GE_COREBASE_CLASS();
    virtual ~GE_COREBASE_CLASS();

    int32
    run();

   protected:

    virtual void
    onSuscribeInputs(const WeakSPtr<AppInputEvents>& inputEvents) = 0;

    /**
     * @brief Initializes and creates the rendering API context.
     * 
     * @note This function should be called after the window is created.
     *       If the user creates a window manually, they should call this
     *       function after the window is created.
     */
    void
    createRenderAPI();

    virtual Vector2I
    getWindowSize() const;

   private:
    void
    startMountManager();

    void
    gameLoop();

    void
    createWindow();

    void
    initSystems();

    void
    registerInputEvents();

    void
    destroySystems();

   private:
    void
    handleWindowEvent(const sf::Event& wndEvent);

    void
    resize(int32 width, int32 height);

    void
    update(float deltaTime);

    void
    render();

   public:
    void
    setWindow(WindowBase* window);

    WindowHandle
    getNativeHandle() {
      if (!m_window) {
#if USING(GE_PLATFORM_LINUX)
        //Linux uses a unsigned long for the native handle
        return 0;
#else
        //Every other platform uses a pointer for the native handle
        return nullptr;
#endif
      }
      return m_window->getNativeHandle();
    }

    void
    setFocus(bool bStatus = true) {
      m_windowHasFocus = bStatus;
    }

    bool
    hasFocus() const {
      return m_windowHasFocus;
    }

   protected:
    SPtr<AppInputEvents> m_pInputEvents;

    Event<void(void)> onCreate;
    Event<void(void)> onDestroy;
    Event<void(float)> onUpdate;
    Event<void(void)> onRender;

    Vector2I m_clientSize = Vector2I(1280, 720);

   private:
    bool m_windowHasFocus = false;
    WindowBase* m_window = nullptr;
  };
}
