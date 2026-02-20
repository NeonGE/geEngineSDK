#include <geCoreBaseApp.h>
#include <geRenderAPI.h>
#include <geDebug.h>

#include "geImGuiPlatform.h"

#if USING(GE_PLATFORM_WINDOWS)
# include "backends/imgui_impl_dx11.h"
#else // USING(GE_PLATFORM_LINUX)
# include "backends/imgui_impl_opengl3.h"
#endif

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

      onCreate.connect([this, inputEvents]() {
        auto& graphMan = RenderAPI::instance();

        //Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        m_imgui.init();
        m_imgui.registerEvents(inputEvents);
#if USING(GE_PLATFORM_WINDOWS)
        auto graphicsDevice = graphMan.getDevice();
        ImGui_ImplDX11_Init(cast::re<ID3D11Device*>(graphicsDevice.pDevicePtr),
                            cast::re<ID3D11DeviceContext*>(graphicsDevice.pCommandPtr));
#else
        ImGui_ImplOpenGL3_Init("#version 330");
#endif
      });

      onDestroy.connect([this]() {
#if USING(GE_PLATFORM_WINDOWS)
        ImGui_ImplDX11_Shutdown();
#else
        ImGui_ImplOpenGL3_Shutdown();
#endif
        m_imgui.shutdown();
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

#if USING(GE_PLATFORM_WINDOWS)
        ImGui_ImplDX11_NewFrame();
#else
        ImGui_ImplOpenGL3_NewFrame();
#endif

        ImGuiIO& io = ImGui::GetIO();
        auto size = getWindowSize();
        io.DisplaySize = ImVec2((float)size.x, (float)size.y);

        m_imgui.newFrame(0.0f);
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
#if USING(GE_PLATFORM_WINDOWS)
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#else
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
      });
    }

   private:
    ImGuiPlatform m_imgui;

  };
} //geEngineSDK

using namespace geEngineSDK;

int main() {
  ImGuiSampleApp app;
  return app.run();
}
