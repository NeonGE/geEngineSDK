/*****************************************************************************/
/**
 * @file    geCoreBaseApp.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2023/03/04
 * @brief   Base Logic for all Apps created with the geEngine.
 *
 * Base Logic for all Apps created with the geEngine.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geCoreBaseApp.h"

#include <gePlatformUtility.h>
#include <geException.h>
#include <geDebug.h>
#include <geMath.h>

#include <geStackAlloc.h>
#include <geMessageHandler.h>
#include <geThreadPool.h>
#include <geTaskScheduler.h>
#include <geTime.h>
#include <geDynLibManager.h>
#include <geRenderAPI.h>
#include <geGameConfig.h>

#include <geMountManager.h>
#include <geCodecManager.h>
#include <geTextureManager.h>

#if USING(GE_FILE_TRACKER)
#include <geFileTracker.h>
#endif

#if USING(GE_PLATFORM_WINDOWS)
#include <excpt.h>
#endif

namespace geEngineSDK {
  using sf::VideoMode;

  GE_COREBASE_CLASS::GE_COREBASE_CLASS() {
    //First thing, we will initialize the CrashHandler and GameConfig
    CrashHandler::startUp();
    GameConfig::startUp();

    //Initialize the MountManager
    startMountManager();

    //Create the AppInputEvents object
    m_pInputEvents = ge_shared_ptr_new<AppInputEvents>();
    if (!m_pInputEvents) {
      GE_EXCEPT(geEngineSDK::InternalErrorException,
        "Failed to create AppInputEvents");
    }
  }

  GE_COREBASE_CLASS::~GE_COREBASE_CLASS() {
    GameConfig::shutDown();
    CrashHandler::shutDown();
  }

  int32
  GE_COREBASE_CLASS::run() {
#if USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
    __try {
      initSystems();
      registerInputEvents();
      gameLoop();
      destroySystems();
    }
    __except (g_crashHandler().reportCrash(GetExceptionInformation())) {
      PlatformUtility::terminate(true);
    }

#else
    g_crashHandler().installPosixSignalHandlers();

    try {
      initSystems();
      registerInputEvents();
      gameLoop();
      destroySystems();
    }
    catch (const std::exception& e) {
      g_crashHandler().reportCrash("C++Exception", e.what());
      PlatformUtility::terminate(true);
    }
    catch (...) {
      g_crashHandler().reportCrash("C++Exception", "Unknown exception");
      PlatformUtility::terminate(true);
    }
#endif

    return 0; //Return with no errors
  }

  void
  geCoreBaseApp::startMountManager()
  {
    //Check that the GameConfig is initialized
    GE_ASSERT(GameConfig::isStarted() &&
              "GameConfig is not started. Cannot start MountManager.");

    //Start the MountManager
    MountManager::startUp();

    //Mount the main file systems
    auto& mountManager = MountManager::instance();

    //Mount the Engine file system
    auto wkPath = FileSystem::getWorkingDirectoryPath();
    mountManager.mount(ge_shared_ptr_new<DiskFileSystem>(wkPath));

    //Mount the Disk file system for the user data directory
    auto confDir = FileSystem::getUserDataDirectoryPath().append("geEngine/");
    mountManager.mount(ge_shared_ptr_new<DiskFileSystem>(confDir));

    auto& gameConfig = GameConfig::instance();
    gameConfig.load("Config/EngineConfig.ini");

    //Check on the config for the engine path, and plugin path
    String strWkPath = FileSystem::getWorkingDirectoryPath().toString();
    FileSystem::setEnginePath(gameConfig.get<String>("ENGINE", "MainPath", strWkPath));
    FileSystem::setPluginsPath(gameConfig.get<String>("ENGINE", "PluginsPath", strWkPath));
    FileSystem::setAppPath(gameConfig.get<String>("ENGINE", "AppPath", strWkPath));

    //Clear the MountManager and once again mount the file systems with the new paths
    mountManager.clear();
    mountManager.mount(ge_shared_ptr_new<DiskFileSystem>(FileSystem::getEnginePath()));
    mountManager.mount(ge_shared_ptr_new<DiskFileSystem>(FileSystem::getPluginsPath()));
    mountManager.mount(ge_shared_ptr_new<DiskFileSystem>(FileSystem::getAppPath()));
    mountManager.mount(ge_shared_ptr_new<DiskFileSystem>(confDir));

    auto baseEnginePack = FileSystem::getEnginePath().append("BaseEngine.zip");
    if (baseEnginePack.isEmpty()) {
      GE_EXCEPT(geEngineSDK::InternalErrorException,
        "BaseEngine.zip not found in the mounted file systems.");
    }

    //Mount the BaseEngine.zip file system
    auto baseEngineZip = ge_shared_ptr_new<ZipFileSystem>(baseEnginePack);

    //TODO: Here we need to get all the packages names from the listing file and mount them
  }

  void
  GE_COREBASE_CLASS::gameLoop() {
    //Create a window for the App
    createWindow();

    //Create the RenderAPI
    createRenderAPI();

    //Sends the onCreate Message
    onCreate();

    if(!m_window || !m_window->isOpen()) {
      return;
    }

    //Main App Loop
    bool shouldClose = false;
    while (m_window->isOpen()) {
      while (const auto wndEvent = m_window->pollEvent()) {
        if (!wndEvent.has_value()) {
          break;
        }
        if (wndEvent.value().is<sf::Event::Closed>()) {
          m_window->close();
          shouldClose = true;
          break;
        }

        //This function handles all the events of the App
        handleWindowEvent(wndEvent.value());
      }

      if (shouldClose) {
        break;
      }

      //Update the game timer
      g_time()._update();

      //Update the Debug callbacks
      g_debug()._triggerCallbacks();

      //Update game logic
      update(g_time().getFrameDelta());

      //Render game frame
      render();
    }

    //Sends the message right before the systems are destroyed
    onDestroy();
  }

  void
  GE_COREBASE_CLASS::createWindow() {
    auto& gameConfig = GameConfig::instance();
    if (!gameConfig.get<bool>("WINDOW", "CREATEWINDOW", true)) {
      return;
    }

    if(!m_window) {
      m_window = ge_new<WindowBase>();
      sf::Vector2u wndSize(gameConfig.get<uint32>("WINDOW", "WIDTH", 1280),
                           gameConfig.get<uint32>("WINDOW", "HEIGHT", 720));
      m_clientSize.x = wndSize.x;
      m_clientSize.y = wndSize.y;

      m_window->create(VideoMode(wndSize),
        gameConfig.get<String>("WINDOW", "TITLE", "geEngine App").c_str(),
                              sf::Style::Default,
                              gameConfig.get<bool>("WINDOW", "FULLSCREEN", false) ?
                              sf::State::Fullscreen :
                              sf::State::Windowed);
    }

    if (m_window->isOpen()) {
      return;
    }

    sf::Vector2i wndPosition(gameConfig.get<int32>("WINDOW", "POSITIONX", -1),
                             gameConfig.get<int32>("WINDOW", "POSITIONY", -1));

    if (-1 != wndPosition.x && -1 != wndPosition.y) {
      m_window->setPosition(wndPosition);
    }

    m_windowHasFocus = m_window->hasFocus();
  }

  void
  GE_COREBASE_CLASS::initSystems() {
    //Number of cores while excluding current thread.
    uint32 numWorkerThreads = GE_THREAD_HARDWARE_CONCURRENCY - 1;

    //Initilize the Engine Systems
    MemStack::beginThread();
    MessageHandler::startUp();
    ThreadPool::startUp<TThreadPool<ThreadDefaultPolicy>>((numWorkerThreads));
    TaskScheduler::startUp();
    Time::startUp();
    DynLibManager::startUp();
    CodecManager::startUp();

    //Create the optional systems
#if USING(GE_FILE_TRACKER)
    FileTracker::startUp();
#endif
  }

  void
  GE_COREBASE_CLASS::createRenderAPI() {
    //Check if we have a valid window
    if (!m_window || !m_window->isOpen()) {
      return; //No window to create the RenderAPI with
    }

    auto& gameConfig = GameConfig::instance();

    String renderApiDllName = "geRenderAPI";
    renderApiDllName += gameConfig.get<String>("RENDERAPI", "DRIVER", "DX11");
#if USING(GE_DEBUG_MODE)
    renderApiDllName += "d"; //Use the debug version of the RenderAPI DLL
#endif

    auto renderAPIDll = g_dynLibManager().load(renderApiDllName);
    if (!renderAPIDll) {
      GE_EXCEPT(geEngineSDK::InternalErrorException,
        "Failed to load RenderAPI DLL: " + renderApiDllName);
    }
    typedef void (*InitFnPtr)(void);
    auto initPluginFn = reinterpret_cast<InitFnPtr>(renderAPIDll->getSymbol("InitPlugin"));
    initPluginFn();

    if (!RenderAPI::isStarted()) {
      GE_EXCEPT(geEngineSDK::InternalErrorException,
        "Failed to start RenderAPI");
    }

    //Initialize the RenderAPI
    auto& renderApi = RenderAPI::instance();
    renderApi.initRenderAPI(m_window->getNativeHandle(),
                            gameConfig.get<bool>("WINDOW", "FULLSCREEN", false));

    //Initialize the Graphics managers
    TextureManager::startUp();

  }

  void
  GE_COREBASE_CLASS::registerInputEvents() {
    m_pInputEvents->onResize.connect([this](int32 width, int32 height) {
      resize(width, height);
    });

    onSuscribeInputs(m_pInputEvents);
  }

  void
  GE_COREBASE_CLASS::destroySystems() {
    //Destroy the Graphics Managers before the RenderAPI
    if(TextureManager::isStarted()) {
      TextureManager::shutDown();
    }

    //Destroy the Graphics Managers
    if (RenderAPI::isStarted()) {
      RenderAPI::shutDown();
    }

    //Destroy the Resource Importer
    CodecManager::shutDown();

    //Destroy the rest of the systems
    DynLibManager::shutDown();
    Time::shutDown();
    TaskScheduler::shutDown();
    ThreadPool::shutDown();
    MessageHandler::shutDown();
    MemStack::endThread();

    auto& gameConfig = GameConfig::instance();
    if (gameConfig.get<bool>("WINDOW", "CREATEWINDOW", true)) {
      if (m_window) {
        m_window->close();
        ge_delete(m_window);
        m_window = nullptr;
      }
    }
  }

  void
  GE_COREBASE_CLASS::resize(int32 width, int32 height) {
    GE_UNREFERENCED_PARAMETER(width);
    GE_UNREFERENCED_PARAMETER(height);

    auto clientSize = m_window->getSize();
    m_clientSize.x = clientSize.x;
    m_clientSize.y = clientSize.y;

    auto& graphman = RenderAPI::instance();
    graphman.resizeSwapChain(clientSize.x, clientSize.y);
  }

  void
  GE_COREBASE_CLASS::handleWindowEvent(const sf::Event& wndEvent) {
    //WindowHandle handle = window->getNativeHandle();

    /*
    if (nullptr == ImGui::GetCurrentContext()) {
      //TODO: This possibly should be handled different
      return;
    }
    */

    if (m_windowHasFocus) {
      //ImGuiIO& io = ImGui::GetIO();

      if (const auto* Resized = wndEvent.getIf<sf::Event::Resized>()) {
        m_pInputEvents->onResize(Resized->size.x, Resized->size.y);
        return;
      }

      if (const auto* TextEntered = wndEvent.getIf<sf::Event::TextEntered>()) {
        //io.AddInputCharacterUTF16(static_cast<uint16>(TextEntered->unicode));
        m_pInputEvents->onTextEntered(static_cast<UNICHAR>(TextEntered->unicode));
        return;
      }

      if (const auto* KeyPressed = wndEvent.getIf<sf::Event::KeyPressed>()) {
        if (KeyPressed->code != sf::Keyboard::Key::Unknown) {
          //io.KeysDown[static_cast<int32>(KeyPressed->code)] = true;
        }

        m_pInputEvents->onKeyPressed(static_cast<int32>(KeyPressed->code),
                                     KeyPressed->alt,
                                     KeyPressed->control,
                                     KeyPressed->shift,
                                     KeyPressed->system);
        return;
      }

      if (const auto* KeyReleased = wndEvent.getIf<sf::Event::KeyReleased>()) {
        if (KeyReleased->code != sf::Keyboard::Key::Unknown) {
          //io.KeysDown[static_cast<int32>(KeyReleased->code)] = false;
        }

        m_pInputEvents->onKeyReleased(static_cast<int32>(KeyReleased->code),
                                      KeyReleased->alt,
                                      KeyReleased->control,
                                      KeyReleased->shift,
                                      KeyReleased->system);
        return;
      }

      if (const auto* MouseWheelScrolled = wndEvent.getIf<sf::Event::MouseWheelScrolled>()) {
        //io.MouseWheel += MouseWheelScrolled->delta;

        m_pInputEvents->onMouseWheelScrolled(static_cast<int32>(MouseWheelScrolled->wheel),
                                             MouseWheelScrolled->delta,
                                             MouseWheelScrolled->position.x,
                                             MouseWheelScrolled->position.y);
        return;
      }

      if (const auto* MouseButtonPressed = wndEvent.getIf<sf::Event::MouseButtonPressed>()) {
        /*
        if (!ImGui::IsAnyMouseDown() && ::GetCapture() == nullptr) {
          ::SetCapture(handle);
        }
        io.MouseDown[static_cast<int32>(MouseButtonPressed->button)] = true;
        */

        m_pInputEvents->onMouseButtonPressed(static_cast<int32>(MouseButtonPressed->button),
                                             MouseButtonPressed->position.x,
                                             MouseButtonPressed->position.y);
        return;
      }

      if (const auto* MouseButtonReleased = wndEvent.getIf<sf::Event::MouseButtonReleased>()) {
        /*
        io.MouseDown[static_cast<int32>(MouseButtonReleased->button)] = false;
        if (!ImGui::IsAnyMouseDown() && ::GetCapture() == handle) {
          ::ReleaseCapture();
        }
        */

        m_pInputEvents->onMouseButtonReleased(static_cast<int32>(MouseButtonReleased->button),
                                              MouseButtonReleased->position.x,
                                              MouseButtonReleased->position.y);
        return;
      }

      if (const auto* MouseMoved = wndEvent.getIf<sf::Event::MouseMoved>()) {
        m_pInputEvents->onMouseMoved(MouseMoved->position.x, MouseMoved->position.y);
        return;
      }

      if (wndEvent.getIf<sf::Event::MouseEntered>()) {
        m_pInputEvents->onMouseEntered();
        return;
      }

      if (wndEvent.getIf<sf::Event::MouseLeft>()) {
        m_pInputEvents->onMouseLeft();
        return;
      }

      if (const auto* JoystickBtnPrssd = wndEvent.getIf<sf::Event::JoystickButtonPressed>()) {
        m_pInputEvents->onJoystickButtonPressed(JoystickBtnPrssd->joystickId,
                                                JoystickBtnPrssd->button);
        return;
      }

      if (const auto* JoystickBtnRlsd = wndEvent.getIf<sf::Event::JoystickButtonReleased>()) {
        m_pInputEvents->onJoystickButtonReleased(JoystickBtnRlsd->joystickId,
                                                 JoystickBtnRlsd->button);
        return;
      }

      if (const auto* JoystickMoved = wndEvent.getIf<sf::Event::JoystickMoved>()) {
        m_pInputEvents->onJoystickMoved(JoystickMoved->joystickId,
                                        static_cast<int32>(JoystickMoved->axis),
                                        JoystickMoved->position);
        return;
      }

      if (const auto* JoystickConnected = wndEvent.getIf<sf::Event::JoystickConnected>()) {
        m_pInputEvents->onJoystickConnected(JoystickConnected->joystickId);
        return;
      }

      if (const auto* JoystickDscnnctd = wndEvent.getIf<sf::Event::JoystickDisconnected>()) {
        m_pInputEvents->onJoystickDisconnected(JoystickDscnnctd->joystickId);
        return;
      }

      if (const auto* TouchBegan = wndEvent.getIf<sf::Event::TouchBegan>()) {
        GE_UNREFERENCED_PARAMETER(TouchBegan);
        //wndEvent.touch
        return;
      }

      if (const auto* TouchMoved = wndEvent.getIf<sf::Event::TouchMoved>()) {
        GE_UNREFERENCED_PARAMETER(TouchMoved);
        //wndEvent.touch
        return;
      }

      if (const auto* TouchEnded = wndEvent.getIf<sf::Event::TouchEnded>()) {
        GE_UNREFERENCED_PARAMETER(TouchEnded);
        //wndEvent.touch
        return;
      }

      if (const auto* SensorChanged = wndEvent.getIf<sf::Event::SensorChanged>()) {
        GE_UNREFERENCED_PARAMETER(SensorChanged);
        //wndEvent.sensor
        return;
      }
    }

    if (const auto* FocusLost = wndEvent.getIf<sf::Event::FocusLost>()) {
      GE_UNREFERENCED_PARAMETER(FocusLost);
      m_windowHasFocus = false;
      m_pInputEvents->onFocusLost();
      return;
    }

    if (const auto* FocusGained = wndEvent.getIf<sf::Event::FocusGained>()) {
      GE_UNREFERENCED_PARAMETER(FocusGained);
      m_windowHasFocus = true;
      m_pInputEvents->onFocusGained();
      return;
    }
  }

  void
  GE_COREBASE_CLASS::update(float deltaTime) {
    onUpdate(deltaTime);
  }

  void
  GE_COREBASE_CLASS::render() {
    auto& graphMan = RenderAPI::instance();
    
    Vector<RenderTarget> defatultRT = { {graphMan.getBackBuffer(), 0} };

    //TODO: Change this for the RendererManager calls
    graphMan.setRenderTargets(defatultRT, WeakSPtr<Texture>());
    onRender();
    graphMan.present();
  }

  void
  GE_COREBASE_CLASS::setWindow(WindowBase* window) {
    auto& gameConfig = GameConfig::instance();
    bool bCreateWindow = gameConfig.get<bool>("WINDOW", "CREATEWINDOW", true);
    if (bCreateWindow) {
      return;
    }
    m_window = window;
    m_windowHasFocus = m_window->hasFocus();
  }
}
