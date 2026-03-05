#include "geImGuiPlatform.h"
#include <geMath.h>
#include <geString.h>
#include <geDebug.h>

#include <imgui.h>
#include <SFML/System/String.hpp>
#include <SFML/Window/Clipboard.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

static ImGuiKey
sfmlKeyToImGuiMod(sf::Keyboard::Key code) {
  switch (code) {
  case sf::Keyboard::Key::LControl:
  case sf::Keyboard::Key::RControl:
    return ImGuiMod_Ctrl;
  case sf::Keyboard::Key::LShift:
  case sf::Keyboard::Key::RShift:
    return ImGuiMod_Shift;
  case sf::Keyboard::Key::LAlt:
  case sf::Keyboard::Key::RAlt:
    return ImGuiMod_Alt;
  case sf::Keyboard::Key::LSystem:
  case sf::Keyboard::Key::RSystem:
    return ImGuiMod_Super;
  default:
    break;
  }
  return ImGuiKey_None;
}

static ImGuiKey
sfmlKeyToImGui(sf::Keyboard::Key k) {
  using K = sf::Keyboard::Key;
  using geEngineSDK::UnorderedMap;

  //Initialize only once (C++11 guarantees thread-safe on local static)
  static const UnorderedMap<K, ImGuiKey> s_map = {
    //Letters
    {K::A, ImGuiKey_A}, {K::B, ImGuiKey_B}, {K::C, ImGuiKey_C}, {K::D, ImGuiKey_D},
    {K::E, ImGuiKey_E}, {K::F, ImGuiKey_F}, {K::G, ImGuiKey_G}, {K::H, ImGuiKey_H},
    {K::I, ImGuiKey_I}, {K::J, ImGuiKey_J}, {K::K, ImGuiKey_K}, {K::L, ImGuiKey_L},
    {K::M, ImGuiKey_M}, {K::N, ImGuiKey_N}, {K::O, ImGuiKey_O}, {K::P, ImGuiKey_P},
    {K::Q, ImGuiKey_Q}, {K::R, ImGuiKey_R}, {K::S, ImGuiKey_S}, {K::T, ImGuiKey_T},
    {K::U, ImGuiKey_U}, {K::V, ImGuiKey_V}, {K::W, ImGuiKey_W}, {K::X, ImGuiKey_X},
    {K::Y, ImGuiKey_Y}, {K::Z, ImGuiKey_Z},

    //Top row digits
    {K::Num0, ImGuiKey_0}, {K::Num1, ImGuiKey_1}, {K::Num2, ImGuiKey_2},
    {K::Num3, ImGuiKey_3}, {K::Num4, ImGuiKey_4}, {K::Num5, ImGuiKey_5},
    {K::Num6, ImGuiKey_6}, {K::Num7, ImGuiKey_7}, {K::Num8, ImGuiKey_8},
    {K::Num9, ImGuiKey_9},

    //Modifiers / system
    {K::Escape,   ImGuiKey_Escape},
    {K::LControl, ImGuiKey_LeftCtrl},
    {K::LShift,   ImGuiKey_LeftShift},
    {K::LAlt,     ImGuiKey_LeftAlt},
    {K::LSystem,  ImGuiKey_LeftSuper},
    {K::RControl, ImGuiKey_RightCtrl},
    {K::RShift,   ImGuiKey_RightShift},
    {K::RAlt,     ImGuiKey_RightAlt},
    {K::RSystem,  ImGuiKey_RightSuper},
    {K::Menu,     ImGuiKey_Menu},

    //Punctuation
    {K::LBracket,   ImGuiKey_LeftBracket},
    {K::RBracket,   ImGuiKey_RightBracket},
    {K::Semicolon,  ImGuiKey_Semicolon},
    {K::Comma,      ImGuiKey_Comma},
    {K::Period,     ImGuiKey_Period},
    {K::Apostrophe, ImGuiKey_Apostrophe},
    {K::Slash,      ImGuiKey_Slash},
    {K::Backslash,  ImGuiKey_Backslash},
    {K::Grave,      ImGuiKey_GraveAccent},
    {K::Equal,      ImGuiKey_Equal},
    {K::Hyphen,     ImGuiKey_Minus},

    //Whitespace / editing
    {K::Space,     ImGuiKey_Space},
    {K::Enter,     ImGuiKey_Enter},
    {K::Backspace, ImGuiKey_Backspace},
    {K::Tab,       ImGuiKey_Tab},

    //Navigation
    {K::PageUp,   ImGuiKey_PageUp},
    {K::PageDown, ImGuiKey_PageDown},
    {K::End,      ImGuiKey_End},
    {K::Home,     ImGuiKey_Home},
    {K::Insert,   ImGuiKey_Insert},
    {K::Delete,   ImGuiKey_Delete},

    //Keypad ops (según tu mapping original)
    {K::Add,      ImGuiKey_KeypadAdd},
    {K::Subtract, ImGuiKey_KeypadSubtract},
    {K::Multiply, ImGuiKey_KeypadMultiply},
    {K::Divide,   ImGuiKey_KeypadDivide},

    //Arrows
    {K::Left,  ImGuiKey_LeftArrow},
    {K::Right, ImGuiKey_RightArrow},
    {K::Up,    ImGuiKey_UpArrow},
    {K::Down,  ImGuiKey_DownArrow},

    //Function keys
    {K::F1,  ImGuiKey_F1},  {K::F2,  ImGuiKey_F2},  {K::F3,  ImGuiKey_F3},
    {K::F4,  ImGuiKey_F4},  {K::F5,  ImGuiKey_F5},  {K::F6,  ImGuiKey_F6},
    {K::F7,  ImGuiKey_F7},  {K::F8,  ImGuiKey_F8},  {K::F9,  ImGuiKey_F9},
    {K::F10, ImGuiKey_F10}, {K::F11, ImGuiKey_F11}, {K::F12, ImGuiKey_F12},
  };

  if (auto it = s_map.find(k); it != s_map.end()) {
    return it->second;
  }

  return ImGuiKey_None;
}

namespace geEngineSDK {
  const char*
  ImGuiPlatform::getClipboardText(void* /*userData*/) {
    static String s;
    sf::String clip = sf::Clipboard::getString();
    s = clip.toAnsiString();  //TODO: We might want to make a better conversion
    return s.c_str();
  }

  void
  ImGuiPlatform::setClipboardText(void* /*userData*/, const char* text) {
    sf::Clipboard::setString(text ? text : "");
  }

  bool
  ImGuiPlatform::init() {
    ImGuiIO& io = ImGui::GetIO();

    //Common flags (adjust to your taste)
    io.BackendPlatformName = "ImGuiPlatform_geEngine";
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    //TODO: If we want to support this, it will need a little more work
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    //Clipboard
    io.SetClipboardTextFn = &ImGuiPlatform::setClipboardText;
    io.GetClipboardTextFn = &ImGuiPlatform::getClipboardText;

    return true;
  }

  void
  ImGuiPlatform::shutdown() {
    //Don't destroy the ImGui Context here
  }

  void
  ImGuiPlatform::registerEvents(const WeakSPtr<AppInputEvents>& inputEvents) {
    if (inputEvents.expired()) {
      GE_LOG(kError, Uncategorized, "Input events are expired");
      return;
    }

    auto input = inputEvents.lock();
    ImGuiIO& io = ImGui::GetIO();
    
    input->onResize.connect([&io](int32 width, int32 height) {
      io.DisplaySize = ImVec2(cast::st<float>(Math::max(8, width)),
                              cast::st<float>(Math::max(8, height)));
    });

    const auto handleKeyChanged = [&io](const auto& keyCode,
                                        const bool alt,
                                        const bool ctrl,
                                        const bool shift,
                                        const bool systemKey,
                                        bool down) {
        auto sfKey = cast::st<sf::Keyboard::Key>(keyCode);

        const ImGuiKey mod = sfmlKeyToImGuiMod(sfKey);
        if (mod != ImGuiKey_None) {
          io.AddKeyEvent(mod, down);
        }
        else {
          io.AddKeyEvent(ImGuiMod_Ctrl, ctrl);
          io.AddKeyEvent(ImGuiMod_Shift, shift);
          io.AddKeyEvent(ImGuiMod_Alt, alt);
          io.AddKeyEvent(ImGuiMod_Super, systemKey);
        }

        const ImGuiKey key = sfmlKeyToImGui(cast::st<sf::Keyboard::Key>(keyCode));
        io.AddKeyEvent(key, down);
        io.SetKeyEventNativeData(key, cast::st<int>(keyCode), -1);
      };

    input->onKeyPressed.connect([&io, handleKeyChanged](int32 keyCode,
      bool alt, bool ctrl, bool shift, bool systemKey) {
        handleKeyChanged(keyCode, alt, ctrl, shift, systemKey, true);
        if (io.WantCaptureKeyboard) {
          return;
        }
      });

    input->onKeyReleased.connect([&io, handleKeyChanged](int32 keyCode,
      bool alt, bool ctrl, bool shift, bool systemKey) {
        handleKeyChanged(keyCode, alt, ctrl, shift, systemKey, false);
        if (io.WantCaptureKeyboard) {
          return;
        }
      });

    input->onTextEntered.connect([&io](UNICHAR text) {
      io.AddInputCharacterUTF16(static_cast<uint16>(text));
    });

    input->onMouseButtonPressed.connect([&io](int32 button, int32 /*x*/, int32 /*y*/) {
      io.AddMouseButtonEvent(button, true);
      if (io.WantCaptureMouse) {
        return;
      }
    });

    input->onMouseButtonReleased.connect([&io](int32 button, int32 /*x*/, int32 /*y*/) {
      io.AddMouseButtonEvent(button, false);
      if (io.WantCaptureMouse) {
        return;
      }
    });

    input->onMouseWheelScrolled.connect([&io](int32 wheel,
                                                    float delta,
                                                    int32 /*x*/,
                                                    int32 /*y*/) {
        if (wheel == cast::st<int32>(sf::Mouse::Wheel::Vertical) ||
          (wheel == cast::st<int32>(sf::Mouse::Wheel::Horizontal) && io.KeyShift)) {
          io.AddMouseWheelEvent(0, delta);
        }
        else if (wheel == cast::st<int32>(sf::Mouse::Wheel::Horizontal)) {
          io.AddMouseWheelEvent(delta, 0);
        }
        if (io.WantCaptureMouse) {
          return;
        }
      });

    input->onMouseMoved.connect([&io](int32 x, int32 y) {
      io.AddMousePosEvent(cast::st<float>(x), cast::st<float>(y));
      if (io.WantCaptureMouse) {
        return;
      }
    });

    input->onFocusLost.connect([&io]() {
      io.AddFocusEvent(false);
      io.AddKeyEvent(ImGuiMod_Ctrl, false);
      io.AddKeyEvent(ImGuiMod_Shift, false);
      io.AddKeyEvent(ImGuiMod_Alt, false);
      io.AddKeyEvent(ImGuiMod_Super, false);
    });

    input->onFocusGained.connect([&io]() {
      io.AddFocusEvent(true);
    });
  }

  void
  ImGuiPlatform::newFrame(float dtSeconds) {
    ImGuiIO& io = ImGui::GetIO();

    //Display size

    //Elapsed time
    io.DeltaTime = (dtSeconds > 0.0f) ? dtSeconds : (1.0f / 60.0f);
  }
}
