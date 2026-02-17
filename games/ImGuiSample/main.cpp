#include <geCoreBaseApp.h>
#include <geRenderAPI.h>
#include <geDebug.h>

#include <d3d11_4.h>
#include "imgui_helper.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

namespace geEngineSDK {
  class ImGuiSampleApp : public GE_COREBASE_CLASS
  {
   public:
    ImGuiSampleApp() = default;
    virtual ~ImGuiSampleApp() = default;

    void
    onSuscribeInputs(const WeakSPtr<AppInputEvents>& inputEvents) override {
      if (inputEvents.expired()) {
        GE_LOG(kError, Uncategorized, "Input events are expired");
        return;
      }

      auto input = inputEvents.lock();

      onCreate.connect([this]() {
        auto& graphMan = RenderAPI::instance();
        auto graphicsDevice = graphMan.getDevice();

        //Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(getNativeHandle());
        ImGui_ImplDX11_Init(cast::re<ID3D11Device*>(graphicsDevice.pDevicePtr),
                            cast::re<ID3D11DeviceContext*>(graphicsDevice.pCommandPtr));
      });

      onDestroy.connect([this]() {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
      });

      onUpdate.connect([this](float deltaTime) {
        GE_UNREFERENCED_PARAMETER(deltaTime);
      });

      onRender.connect([this]() {
        auto& graphMan = RenderAPI::instance();
        auto renderTarget = graphMan.getBackBuffer();
        graphMan.clearRenderTarget(renderTarget, LinearColor::Blue);
        graphMan.setRenderTargets({ renderTarget }, WeakSPtr<Texture>());

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
      });

      onRender.connect([this]() {
        ImGui::Text("Hello, world!");

        static bool bShowDemoWindow = true;
        ImGui::ShowDemoWindow(&bShowDemoWindow);
      });

      onRender.connect([this]() {
        //Render dear imgui into framebuffer
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
      });

      input->onResize.connect([this](int32 width, int32 height) {
        ImGuiIO& io = ImGui::GetIO();
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

        const ImGuiKey mod = keycodeToImGuiMod(sfKey);
        if (mod != ImGuiKey_None) {
          io.AddKeyEvent(mod, down);
        }
        else {
          io.AddKeyEvent(ImGuiMod_Ctrl, ctrl);
          io.AddKeyEvent(ImGuiMod_Shift, shift);
          io.AddKeyEvent(ImGuiMod_Alt, alt);
          io.AddKeyEvent(ImGuiMod_Super, systemKey);
        }

        const ImGuiKey key = keycodeToImGuiKey(sfKey);
        io.AddKeyEvent(key, down);
        io.SetKeyEventNativeData(key, cast::st<int>(keyCode), -1);
      };

      input->onKeyPressed.connect([this, handleKeyChanged](int32 keyCode,
        bool alt, bool ctrl, bool shift, bool systemKey) {
          handleKeyChanged(keyCode, alt, ctrl, shift, systemKey, true);
          ImGuiIO& io = ImGui::GetIO();
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

      input->onTextEntered.connect([this](UNICHAR text){
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacterUTF16(static_cast<uint16>(text));
      });

      input->onMouseButtonPressed.connect([this](int32 button, int32 /*x*/, int32 /*y*/) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(button, true);
        if (io.WantCaptureMouse) {
          return;
        }
      });

      input->onMouseButtonReleased.connect([this](int32 button, int32 /*x*/, int32 /*y*/) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(button, false);
        if (io.WantCaptureMouse) {
          return;
        }
      });

      input->onMouseWheelScrolled.connect([this](int32 wheel,
                                                 float delta,
                                                 int32 /*x*/,
                                                 int32 /*y*/) {
        ImGuiIO& io = ImGui::GetIO();
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

      input->onMouseMoved.connect([this](int32 x, int32 y) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMousePosEvent(cast::st<float>(x), cast::st<float>(y));
        if (io.WantCaptureMouse) {
          return;
        }
      });

      input->onFocusLost.connect([this]() {
        ImGuiIO& io = ImGui::GetIO();
        io.AddFocusEvent(false);
      });

      input->onFocusGained.connect([this]() {
        ImGuiIO& io = ImGui::GetIO();
        io.AddFocusEvent(true);
      });
    }

  };
} //geEngineSDK

using namespace geEngineSDK;

int main() {
  ImGuiSampleApp app;
  return app.run();
}
