/*****************************************************************************/
/**
 * @file    geAppInputEvents.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/02/19
 * @brief   Input events for the App.
 *
 * Input events for the App.
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
#include <geEvent.h>

namespace geEngineSDK {
  /*
   * @brief Input events and callback formats for the app
   */
  struct AppInputEvents
  {
    /*
     * @brief Callbacks for FOCUS_EVENT_CALLBACK.
     * @note  This callback is called when the window loses or gains focus.
     * @param  void
     */
    using FOCUS_EVENT_CALLBACK = void(void);

    /*
     * @brief Callbacks for RESIZE_EVENT_CALLBACK.
     * @note  This callback is called when the window is resized.
     * @param  width  New width of the window.
     * @param  height New height of the window.
     */
    using RESIZE_EVENT_CALLBACK = void(int32, int32);

    /*
     * @brief Callbacks for KEY_EVENT_CALLBACK.
     * @note  This callback is called when a key is pressed or released.
     * @param  keyCode   Key code of the key that was pressed or released.
     * @param  alt       True if the Alt key was pressed.
     * @param  ctrl      True if the Ctrl key was pressed.
     * @param  shift     True if the Shift key was pressed.
     * @param  systemKey True if the System key was pressed.
     */
    using KEY_EVENT_CALLBACK = void(int32, bool, bool, bool, bool);

    /*
     * @brief Callbacks for TEXT_EVENT_CALLBACK.
     * @note  This callback is called when a text character is entered.
     * @param  unicode Unicode character that was entered.
     */
    using TEXT_EVENT_CALLBACK = void(UNICHAR);

    /*
     * @brief Callbacks for MOUSE_EVENT_CALLBACK.
     * @note  This callback is called when the mouse is moved.
     * @param  x New X position of the mouse.
     * @param  y New Y position of the mouse.
     */
    using MOUSE_EVENT_CALLBACK = void(int32, int32);

    /*
     * @brief Callbacks for MOUSE_BUTTON_EVENT_CALLBACK.
     * @note  This callback is called when a mouse button is pressed or released.
     * @param  button Button that was pressed or released.
     * @param  x      X position of the mouse when the button was pressed or released.
     * @param  y      Y position of the mouse when the button was pressed or released.
     */
    using MOUSE_BUTTON_EVENT_CALLBACK = void(int32, int32, int32);

    /*
     * @brief Callbacks for MOUSE_WHEEL_EVENT_CALLBACK.
     * @note  This callback is called when the mouse wheel is scrolled.
     * @param  wheel   Wheel that was scrolled.
     * @param  delta   Amount of scrolling.
     * @param  x      X position of the mouse when the wheel was scrolled.
     * @param  y      Y position of the mouse when the wheel was scrolled.
     */
    using MOUSE_WHEEL_EVENT_CALLBACK = void(int32, float, int32, int32);

    /*
     * @brief Callbacks for MOUSE_EMPTY_EVENT_CALLBACK.
     * @note  This callback is called when the mouse enters or leaves the window.
     * @param  void
     */
    using MOUSE_EMPTY_EVENT_CALLBACK = void(void);

    /*
     * @brief Callbacks for JOYSTICK_MOVED_EVENT_CALLBACK.
     * @note  This callback is called when a joystick is moved.
     * @param  joystickId Joystick ID that was moved.
     * @param  axis       Axis that was moved.
     * @param  position   New position of the joystick.
     */
    using JOYSTICK_MOVED_EVENT_CALLBACK = void(uint32, int32, float);

    /*
     * @brief Callbacks for JOYSTICK_BUTTON_EVENT_CALLBACK.
     * @note  This callback is called when a joystick button is pressed or released.
     * @param  joystickId Joystick ID that was pressed or released.
     * @param  button     Button that was pressed or released.
     */
    using JOYSTICK_BUTTON_EVENT_CALLBACK = void(uint32, uint32);

    /*
     * @brief Callbacks for JOYSTICK_STATE_EVENT_CALLBACK.
     * @note  This callback is called when a joystick is connected or disconnected.
     * @param  joystickId Joystick ID that was connected or disconnected.
     */
    using JOYSTICK_STATE_EVENT_CALLBACK = void(uint32);

    // Callbacks for window events
    Event<FOCUS_EVENT_CALLBACK> onFocusLost;
    Event<FOCUS_EVENT_CALLBACK> onFocusGained;
    Event<RESIZE_EVENT_CALLBACK> onResize;

    // Callbacks for keyboard events
    Event<KEY_EVENT_CALLBACK> onKeyPressed;
    Event<KEY_EVENT_CALLBACK> onKeyReleased;
    Event<TEXT_EVENT_CALLBACK> onTextEntered;

    // Callbacks for mouse events
    Event<MOUSE_EVENT_CALLBACK> onMouseMoved;
    Event<MOUSE_BUTTON_EVENT_CALLBACK> onMouseButtonPressed;
    Event<MOUSE_BUTTON_EVENT_CALLBACK> onMouseButtonReleased;
    Event<MOUSE_WHEEL_EVENT_CALLBACK> onMouseWheelScrolled;
    Event<MOUSE_EMPTY_EVENT_CALLBACK> onMouseEntered;
    Event<MOUSE_EMPTY_EVENT_CALLBACK> onMouseLeft;

    // Callbacks for joystick events
    Event<JOYSTICK_MOVED_EVENT_CALLBACK> onJoystickMoved;
    Event<JOYSTICK_BUTTON_EVENT_CALLBACK> onJoystickButtonPressed;
    Event<JOYSTICK_BUTTON_EVENT_CALLBACK> onJoystickButtonReleased;
    Event<JOYSTICK_STATE_EVENT_CALLBACK> onJoystickConnected;
    Event<JOYSTICK_STATE_EVENT_CALLBACK> onJoystickDisconnected;
  };
}
