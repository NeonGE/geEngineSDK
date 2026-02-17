#include <geCoreBaseApp.h>
#include <geDebug.h>

namespace geEngineSDK {
  class SampleGameApp : public GE_COREBASE_CLASS
  {
   public:
    SampleGameApp() = default;
    virtual ~SampleGameApp() = default;

    void
    onSuscribeInputs(const WeakSPtr<AppInputEvents>& inputEvents) override {
      if (inputEvents.expired()) {
        GE_LOG(kError, Uncategorized, "Input events are expired");
        return;
      }

      auto input = inputEvents.lock();

      onCreate.connect([this]() {});
      onDestroy.connect([this]() {});
      onUpdate.connect([this](float deltaTime) {
        GE_UNREFERENCED_PARAMETER(deltaTime);
      });
      onRender.connect([this]() {});
    }
  };
} //geEngineSDK

using namespace geEngineSDK;

int main() {
  SampleGameApp app;
  return app.run();
}
