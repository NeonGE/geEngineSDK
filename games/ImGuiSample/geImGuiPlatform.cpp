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
  switch (k)
  {
  case sf::Keyboard::Key::A: return ImGuiKey_A;
  case sf::Keyboard::Key::B: return ImGuiKey_B;
  case sf::Keyboard::Key::C: return ImGuiKey_C;
  case sf::Keyboard::Key::D: return ImGuiKey_D;
  case sf::Keyboard::Key::E: return ImGuiKey_E;
  case sf::Keyboard::Key::F: return ImGuiKey_F;
  case sf::Keyboard::Key::G: return ImGuiKey_G;
  case sf::Keyboard::Key::H: return ImGuiKey_H;
  case sf::Keyboard::Key::I: return ImGuiKey_I;
  case sf::Keyboard::Key::J: return ImGuiKey_J;
  case sf::Keyboard::Key::K: return ImGuiKey_K;
  case sf::Keyboard::Key::L: return ImGuiKey_L;
  case sf::Keyboard::Key::M: return ImGuiKey_M;
  case sf::Keyboard::Key::N: return ImGuiKey_N;
  case sf::Keyboard::Key::O: return ImGuiKey_O;
  case sf::Keyboard::Key::P: return ImGuiKey_P;
  case sf::Keyboard::Key::Q: return ImGuiKey_Q;
  case sf::Keyboard::Key::R: return ImGuiKey_R;
  case sf::Keyboard::Key::S: return ImGuiKey_S;
  case sf::Keyboard::Key::T: return ImGuiKey_T;
  case sf::Keyboard::Key::U: return ImGuiKey_U;
  case sf::Keyboard::Key::V: return ImGuiKey_V;
  case sf::Keyboard::Key::W: return ImGuiKey_W;
  case sf::Keyboard::Key::X: return ImGuiKey_X;
  case sf::Keyboard::Key::Y: return ImGuiKey_Y;
  case sf::Keyboard::Key::Z: return ImGuiKey_Z;

  case sf::Keyboard::Key::Num0: return ImGuiKey_0;
  case sf::Keyboard::Key::Num1: return ImGuiKey_1;
  case sf::Keyboard::Key::Num2: return ImGuiKey_2;
  case sf::Keyboard::Key::Num3: return ImGuiKey_3;
  case sf::Keyboard::Key::Num4: return ImGuiKey_4;
  case sf::Keyboard::Key::Num5: return ImGuiKey_5;
  case sf::Keyboard::Key::Num6: return ImGuiKey_6;
  case sf::Keyboard::Key::Num7: return ImGuiKey_7;
  case sf::Keyboard::Key::Num8: return ImGuiKey_8;
  case sf::Keyboard::Key::Num9: return ImGuiKey_9;

  case sf::Keyboard::Key::Escape:    return ImGuiKey_Escape;
  case sf::Keyboard::Key::LControl:  return ImGuiKey_LeftCtrl;
  case sf::Keyboard::Key::LShift:    return ImGuiKey_LeftShift;
  case sf::Keyboard::Key::LAlt:      return ImGuiKey_LeftAlt;
  case sf::Keyboard::Key::LSystem:   return ImGuiKey_LeftSuper;
  case sf::Keyboard::Key::RControl:  return ImGuiKey_RightCtrl;
  case sf::Keyboard::Key::RShift:    return ImGuiKey_RightShift;
  case sf::Keyboard::Key::RAlt:      return ImGuiKey_RightAlt;
  case sf::Keyboard::Key::RSystem:   return ImGuiKey_RightSuper;
  case sf::Keyboard::Key::Menu:      return ImGuiKey_Menu;

  case sf::Keyboard::Key::LBracket:   return ImGuiKey_LeftBracket;
  case sf::Keyboard::Key::RBracket:   return ImGuiKey_RightBracket;
  case sf::Keyboard::Key::Semicolon:  return ImGuiKey_Semicolon;
  case sf::Keyboard::Key::Comma:      return ImGuiKey_Comma;
  case sf::Keyboard::Key::Period:     return ImGuiKey_Period;
  case sf::Keyboard::Key::Apostrophe: return ImGuiKey_Apostrophe;
  case sf::Keyboard::Key::Slash:      return ImGuiKey_Slash;
  case sf::Keyboard::Key::Backslash:  return ImGuiKey_Backslash;
  case sf::Keyboard::Key::Grave:      return ImGuiKey_GraveAccent;
  case sf::Keyboard::Key::Equal:      return ImGuiKey_Equal;
  case sf::Keyboard::Key::Hyphen:     return ImGuiKey_Minus;

  case sf::Keyboard::Key::Space:     return ImGuiKey_Space;
  case sf::Keyboard::Key::Enter:     return ImGuiKey_Enter;
  case sf::Keyboard::Key::Backspace: return ImGuiKey_Backspace;
  case sf::Keyboard::Key::Tab:       return ImGuiKey_Tab;

  case sf::Keyboard::Key::PageUp:    return ImGuiKey_PageUp;
  case sf::Keyboard::Key::PageDown:  return ImGuiKey_PageDown;
  case sf::Keyboard::Key::End:       return ImGuiKey_End;
  case sf::Keyboard::Key::Home:      return ImGuiKey_Home;
  case sf::Keyboard::Key::Insert:    return ImGuiKey_Insert;
  case sf::Keyboard::Key::Delete:    return ImGuiKey_Delete;

  case sf::Keyboard::Key::Add:       return ImGuiKey_KeypadAdd;
  case sf::Keyboard::Key::Subtract:  return ImGuiKey_KeypadSubtract;
  case sf::Keyboard::Key::Multiply:  return ImGuiKey_KeypadMultiply;
  case sf::Keyboard::Key::Divide:    return ImGuiKey_KeypadDivide;

  case sf::Keyboard::Key::Left:      return ImGuiKey_LeftArrow;
  case sf::Keyboard::Key::Right:     return ImGuiKey_RightArrow;
  case sf::Keyboard::Key::Up:        return ImGuiKey_UpArrow;
  case sf::Keyboard::Key::Down:      return ImGuiKey_DownArrow;

  case sf::Keyboard::Key::F1:  return ImGuiKey_F1;
  case sf::Keyboard::Key::F2:  return ImGuiKey_F2;
  case sf::Keyboard::Key::F3:  return ImGuiKey_F3;
  case sf::Keyboard::Key::F4:  return ImGuiKey_F4;
  case sf::Keyboard::Key::F5:  return ImGuiKey_F5;
  case sf::Keyboard::Key::F6:  return ImGuiKey_F6;
  case sf::Keyboard::Key::F7:  return ImGuiKey_F7;
  case sf::Keyboard::Key::F8:  return ImGuiKey_F8;
  case sf::Keyboard::Key::F9:  return ImGuiKey_F9;
  case sf::Keyboard::Key::F10: return ImGuiKey_F10;
  case sf::Keyboard::Key::F11: return ImGuiKey_F11;
  case sf::Keyboard::Key::F12: return ImGuiKey_F12;

  default: break;
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
    
    input->onResize.connect([this, &io](int32 width, int32 height) {
      io.DisplaySize = ImVec2(cast::st<float>(Math::max(8, width)),
                              cast::st<float>(Math::max(8, height)));
    });

    const auto handleKeyChanged = [](const auto& keyCode,
      const bool alt,
      const bool ctrl,
      const bool shift,
      const bool systemKey,
      bool down) {
        ImGuiIO& io = ImGui::GetIO();
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
      };

    input->onKeyPressed.connect([this, &io, handleKeyChanged](int32 keyCode,
      bool alt, bool ctrl, bool shift, bool systemKey) {
        handleKeyChanged(keyCode, alt, ctrl, shift, systemKey, true);
        if (io.WantCaptureKeyboard) {
          return;
        }
      });

    input->onKeyReleased.connect([this, handleKeyChanged](int32 keyCode,
      bool alt, bool ctrl, bool shift, bool systemKey) {
        handleKeyChanged(keyCode, alt, ctrl, shift, systemKey, false);
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureKeyboard) {
          return;
        }
      });

    input->onTextEntered.connect([this](UNICHAR text) {
      ImGuiIO& io = ImGui::GetIO();
      io.AddInputCharacterUTF16(static_cast<uint16>(text));
    });

    input->onMouseButtonPressed.connect([this, &io](int32 button, int32 /*x*/, int32 /*y*/) {
      io.AddMouseButtonEvent(button, true);
      if (io.WantCaptureMouse) {
        return;
      }
    });

    input->onMouseButtonReleased.connect([this, &io](int32 button, int32 /*x*/, int32 /*y*/) {
      io.AddMouseButtonEvent(button, false);
      if (io.WantCaptureMouse) {
        return;
      }
    });

    input->onMouseWheelScrolled.connect([this, &io](int32 wheel,
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

    input->onMouseMoved.connect([this, &io](int32 x, int32 y) {
      io.AddMousePosEvent(cast::st<float>(x), cast::st<float>(y));
      if (io.WantCaptureMouse) {
        return;
      }
    });

    input->onFocusLost.connect([this, &io]() {
      io.AddFocusEvent(false);
      io.AddKeyEvent(ImGuiMod_Ctrl, false);
      io.AddKeyEvent(ImGuiMod_Shift, false);
      io.AddKeyEvent(ImGuiMod_Alt, false);
      io.AddKeyEvent(ImGuiMod_Super, false);
    });

    input->onFocusGained.connect([this, &io]() {
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
