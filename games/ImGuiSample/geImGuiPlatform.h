#pragma once

#include <geAppInputEvents.h>

namespace geEngineSDK {
  class ImGuiPlatform
  {
   public:
    ImGuiPlatform() = default;

    bool
    init();

    void
    registerEvents(const WeakSPtr<AppInputEvents>& inputEvents);

    void
    newFrame(float dtSeconds);

    void
    shutdown();

   private:
    //Clipboard handlers
    static const char*
    getClipboardText(void* userData);

    static void
    setClipboardText(void* userData, const char* text);
  };
}
