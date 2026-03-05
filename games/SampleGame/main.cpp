#include <geCoreBaseApp.h>
#include <geDebug.h>

namespace geEngineSDK {
  class SampleGameApp final : public GE_COREBASE_CLASS
  {
   public:
    SampleGameApp() = default;
    ~SampleGameApp() = default;

    void
    onSuscribeInputs(const WeakSPtr<AppInputEvents>& inputEvents) override {
      if (inputEvents.expired()) {
        GE_LOG(kError, Uncategorized, "Input events are expired");
        return;
      }

      auto input = inputEvents.lock();

      onCreate.connect([]() {});
      onDestroy.connect([]() {});
      onUpdate.connect([](float deltaTime) {
        GE_UNREFERENCED_PARAMETER(deltaTime);
      });
      onRender.connect([]() {});
    }
  };
} //geEngineSDK

using namespace geEngineSDK;

SPtr<SampleGameApp> g_pApp;

int main() {
  g_pApp = ge_shared_ptr_new<SampleGameApp>();
  return g_pApp->run();
}
