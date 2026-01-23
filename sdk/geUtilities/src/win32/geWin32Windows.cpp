/*****************************************************************************/
/**
 * @file    geWin32Windows.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/18
 * @brief   Objects used to represents a Windows native window.
 *
 * Objects used to represents a Windows native window.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#ifdef _WIN32
/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "Win32/geWin32Windows.h"
#include "Win32/geWin32PlatformUtility.h"
#include "geVector2I.h"

namespace geEngineSDK {
  Vector<Win32Window*> Win32Window::s_allWindows;
  Vector<Win32Window*> Win32Window::s_modalWindowStack;
  Mutex Win32Window::s_windowsMutex;

  struct Win32Window::Pimpl
  {
    HWND hWnd = nullptr;
    INT32 left = 0;
    INT32 top = 0;
    UINT32 width = 0;
    UINT32 height = 0;
    bool isExternal = false;
    bool isModal = false;
    bool isHidden = false;
    DWORD style = 0;
    DWORD styleEx = 0;
  };

  Win32Window::Win32Window(const WINDOW_DESC& desc) {
    m_windowData = ge_new<Pimpl>();
    m_windowData->isModal = desc.modal;
    m_windowData->isHidden = desc.hidden;
    bool shouldFocus = true;

    HMONITOR hMonitor = desc.monitor;
    if (!desc.external) {
      m_windowData->style = WS_CLIPCHILDREN;

      int32 left = desc.left;
      int32 top = desc.top;

      //If we didn't specified the adapter index, or if we didn't find it
      if (nullptr == hMonitor) {
        POINT windowAnchorPoint;

        //Fill in anchor point.
        windowAnchorPoint.x = left;
        windowAnchorPoint.y = top;

        //Get the nearest monitor to this window.
        hMonitor = ::MonitorFromPoint(windowAnchorPoint, MONITOR_DEFAULTTOPRIMARY);
      }

      //Get the target monitor info
      MONITORINFO monitorInfo;
      memset(&monitorInfo, 0, sizeof(MONITORINFO));
      monitorInfo.cbSize = sizeof(MONITORINFO);
      GetMonitorInfo(hMonitor, &monitorInfo);

      auto width = static_cast<int32>(desc.width);
      auto height = static_cast<int32>(desc.height);

      if (-1 == left || -1 == top) {  //No specified top left
        //Center the window in the middle of the monitor
        int32 screenw = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
        int32 screenh = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

        //Clamp window dimensions to screen size
        int32 outerw = (width  < screenw) ? width  : screenw;
        int32 outerh = (height < screenh) ? height : screenh;

        if (-1 == left) {
          left = monitorInfo.rcWork.left + ((screenw - outerw) >> 1);
        }
        else if (nullptr != hMonitor) {
          left += monitorInfo.rcWork.left;
        }

        if (-1 == top) {
          top = monitorInfo.rcWork.top + ((screenh - outerh) >> 1);
        }
        else if (nullptr != hMonitor) {
          top += monitorInfo.rcWork.top;
        }
      }
      else if (nullptr != hMonitor) {
        left += monitorInfo.rcWork.left;
        top += monitorInfo.rcWork.top;
      }

      if (!desc.fullscreen) {
        if (desc.parent) {
          if (desc.toolWindow) {
            m_windowData->styleEx = WS_EX_TOOLWINDOW;
          }
          else {
            m_windowData->style |= WS_CHILD;
          }
        }
        else {
          if (desc.toolWindow) {
            m_windowData->styleEx = WS_EX_TOOLWINDOW;
          }
        }

        if (!desc.parent || desc.toolWindow) {
          if (desc.showTitleBar) {
            if (desc.showBorder || desc.allowResize) {
              m_windowData->style |= WS_OVERLAPPEDWINDOW;
            }
            else {
              m_windowData->style |= WS_OVERLAPPED
                                  | WS_CAPTION
                                  | WS_SYSMENU
                                  | WS_MINIMIZEBOX
                                  | WS_MAXIMIZEBOX;
            }
          }
          else {
            if (desc.showBorder || desc.allowResize) {
              m_windowData->style |= WS_POPUP | WS_BORDER;
            }
            else {
              m_windowData->style |= WS_POPUP;
            }
          }
        }

        if (!desc.outerDimensions) {
          //Calculate window dimensions required to get the requested client area
          RECT rect;
          SetRect(&rect, 0, 0, width, height);
          AdjustWindowRect(&rect, m_windowData->style, false);
          width = rect.right - rect.left;
          height = rect.bottom - rect.top;

          //Clamp width and height to the desktop dimensions
          int32 screenw = GetSystemMetrics(SM_CXSCREEN);
          int32 screenh = GetSystemMetrics(SM_CYSCREEN);

          if (width > screenw) {
            width = screenw;
          }

          if (height > screenh) {
            height = screenh;
          }

          if (0 > left) {
            left = (screenw - width) >> 1;
          }

          if (0 > top) {
            top = (screenh - height) >> 1;
          }
        }

        if (nullptr != desc.backgroundPixels) {
          m_windowData->styleEx |= WS_EX_LAYERED;
        }
      }
      else {
        m_windowData->style |= WS_POPUP;
        top = 0;
        left = 0;
      }

      uint32 classStyle = 0;
      if (desc.enableDoubleClick) {
        classStyle |= CS_DBLCLKS;
      }

      //Register the window class
      WNDCLASSA wc = { classStyle,
                       desc.wndProc,
                       0,
                       0,
                       desc.module,
                       LoadIcon(nullptr, IDI_APPLICATION),
                       LoadCursor(nullptr, IDC_ARROW),
                       static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)),
                       nullptr,
                       "GE_WIN32_WINDOW" };

      RegisterClassA(&wc);

      //Create main window
      m_windowData->hWnd = CreateWindowExA(m_windowData->styleEx,
                                          "GE_WIN32_WINDOW",
                                          desc.title.c_str(),
                                          m_windowData->style,
                                          left,
                                          top,
                                          width,
                                          height,
                                          desc.parent,
                                          nullptr,
                                          desc.module,
                                          desc.creationParams);

      GE_ASSERT(nullptr != m_windowData->hWnd);

      m_windowData->isExternal = false;
    }
    else {
      m_windowData->hWnd = desc.external;
      m_windowData->isExternal = true;
    }

    RECT rect;
    GetWindowRect(m_windowData->hWnd, &rect);
    m_windowData->top = rect.top;
    m_windowData->left = rect.left;

    GetClientRect(m_windowData->hWnd, &rect);
    m_windowData->width = rect.right;
    m_windowData->height = rect.bottom;

    //Set background, if any
    if (nullptr != desc.backgroundPixels) {
      HBITMAP backgroundBitmap = Win32PlatformUtility::createBitmap(desc.backgroundPixels,
                                                                    desc.backgroundWidth,
                                                                    desc.backgroundHeight,
                                                                    true);

      HDC hdcScreen = GetDC(nullptr);
      HDC hdcMem = CreateCompatibleDC(hdcScreen);
      auto hOldBitmap = static_cast<HBITMAP>(SelectObject(hdcMem, backgroundBitmap));

      BLENDFUNCTION blend;
      ge_zero_out(blend);
      blend.BlendOp = AC_SRC_OVER;
      blend.SourceConstantAlpha = 255;
      blend.AlphaFormat = AC_SRC_ALPHA;

      POINT origin;
      origin.x = m_windowData->left;
      origin.y = m_windowData->top;

      SIZE size;
      size.cx = m_windowData->width;
      size.cy = m_windowData->height;

      POINT zero = {0, 0};

      ::UpdateLayeredWindow(m_windowData->hWnd,
                            hdcScreen,
                            &origin,
                            &size,
                            hdcMem,
                            &zero,
                            RGB(0, 0, 0),
                            &blend,
                            desc.alphaBlending ? ULW_ALPHA : ULW_OPAQUE);

      SelectObject(hdcMem, hOldBitmap);
      DeleteDC(hdcMem);
      ReleaseDC(nullptr, hdcScreen);
    }

    //Handle modal windows
    ge_frame_mark();

    {
      FrameVector<HWND> windowsToDisable;
      FrameVector<HWND> windowsToBringToFront;
      {
        Lock lock(s_windowsMutex);

        if (m_windowData->isModal) {
          if (!s_modalWindowStack.empty()) {
            Win32Window* curModalWindow = s_modalWindowStack.back();
            windowsToDisable.push_back(curModalWindow->m_windowData->hWnd);
          }
          else {
            for (auto& window : s_allWindows) {
              windowsToDisable.push_back(window->m_windowData->hWnd);
            }
          }

          s_modalWindowStack.push_back(this);
        }
        else {
          //A non-modal window was opened while another modal one is open,
          //immediately deactivate it and make sure the modal windows stay on top.
          if (!s_modalWindowStack.empty()) {
            shouldFocus = false;
            windowsToDisable.push_back(m_windowData->hWnd);

            for (auto window : s_modalWindowStack) {
              windowsToBringToFront.push_back(window->m_windowData->hWnd);
            }
          }
        }

        s_allWindows.push_back(this);
      }

      for (auto& entry : windowsToDisable) {
        EnableWindow(entry, FALSE);
      }

      for (auto& entry : windowsToBringToFront) {
        BringWindowToTop(entry);
      }

      if (shouldFocus) {
        SetFocus(m_windowData->hWnd);
      }
    }

    if (desc.hidden) {
      setHidden(true);
    }

    ge_frame_clear();
  }

  Win32Window::~Win32Window() {
    if (m_windowData->hWnd && !m_windowData->isExternal) {
      //Handle modal windows
      ge_frame_mark();

      {
        FrameVector<HWND> windowsToEnable;
        {
          Lock lock(s_windowsMutex);

          //Hidden dependency: All windows must be re-enabled before a window
          //is destroyed, otherwise the incorrect window in the z order will
          //be activated.
          bool reenableWindows = false;
          if (!s_modalWindowStack.empty()) {
            //Start from back because the most common case is closing the
            //top-most modal window
            for (auto iter = s_modalWindowStack.rbegin();
                 iter != s_modalWindowStack.rend(); ++iter) {
              if (this == *iter) {
                //erase doesn't accept reverse iter, so convert
                auto iterFwd = std::next(iter).base();
                s_modalWindowStack.erase(iterFwd);
                break;
              }
            }

            if (!s_modalWindowStack.empty()) { //Enable next modal window
              Win32Window* curModalWindow = s_modalWindowStack.back();
              windowsToEnable.push_back(curModalWindow->m_windowData->hWnd);
            }
            else {
              reenableWindows = true; //No more modal windows, re-enable any remaining window
            }
          }

          if (reenableWindows)
          {
            for (auto& window : s_allWindows) {
              windowsToEnable.push_back(window->m_windowData->hWnd);
            }
          }
        }

        for (auto& entry : windowsToEnable) {
          EnableWindow(entry, TRUE);
        }
      }
      ge_frame_clear();
      DestroyWindow(m_windowData->hWnd);
    }

    {
      Lock lock(s_windowsMutex);

      auto iterFind = std::find(s_allWindows.begin(), s_allWindows.end(), this);
      s_allWindows.erase(iterFind);
    }

    ge_delete(m_windowData);
  }

  void
  Win32Window::move(int32 left, int32 top) {
    if (m_windowData->hWnd) {
      m_windowData->top = top;
      m_windowData->left = left;
      SetWindowPos(m_windowData->hWnd,
                   HWND_TOP,
                   left,
                   top,
                   m_windowData->width,
                   m_windowData->height,
                   SWP_NOSIZE);
    }
  }

  void
  Win32Window::resize(uint32 width, uint32 height) {
    if (m_windowData->hWnd) {
      RECT rc = {0, 0, (LONG)width, (LONG)height};
      AdjustWindowRect(&rc, GetWindowLong(m_windowData->hWnd, GWL_STYLE), false);
      width = rc.right - rc.left;
      height = rc.bottom - rc.top;
      m_windowData->width = width;
      m_windowData->height = height;

      SetWindowPos(m_windowData->hWnd,
                   HWND_TOP,
                   m_windowData->left,
                   m_windowData->top,
                   width,
                   height,
                   SWP_NOMOVE);
    }
  }

  void
  Win32Window::setActive(bool state) {
    if (m_windowData->hWnd) {
      if (state) {
        ShowWindow(m_windowData->hWnd, SW_RESTORE);
      }
      else {
        ShowWindow(m_windowData->hWnd, SW_SHOWMINNOACTIVE);
      }
    }
  }

  void
  Win32Window::setHidden(bool hidden) {
    if (hidden) {
      ShowWindow(m_windowData->hWnd, SW_HIDE);
    }
    else {
      ShowWindow(m_windowData->hWnd, SW_SHOW);
    }
    
    m_windowData->isHidden = hidden;
  }

  void
  Win32Window::minimize() {
    GE_ASSERT(nullptr != m_windowData && nullptr != m_windowData->hWnd);

    if (m_windowData->hWnd) {
      ShowWindow(m_windowData->hWnd, SW_MINIMIZE);
    }

    if (m_windowData->isHidden) {
      ShowWindow(m_windowData->hWnd, SW_HIDE);
    }
  }

  void
  Win32Window::maximize() {
    GE_ASSERT(nullptr != m_windowData && nullptr != m_windowData->hWnd);

    if (m_windowData->hWnd) {
      ShowWindow(m_windowData->hWnd, SW_MAXIMIZE);
    }

    if (m_windowData->isHidden) {
      ShowWindow(m_windowData->hWnd, SW_HIDE);
    }
  }

  void
  Win32Window::restore() {
    GE_ASSERT(nullptr != m_windowData && nullptr != m_windowData->hWnd);

    if (m_windowData->hWnd) {
      ShowWindow(m_windowData->hWnd, SW_RESTORE);
    }

    if (m_windowData->isHidden) {
      ShowWindow(m_windowData->hWnd, SW_HIDE);
    }
  }

  void
  Win32Window::_windowMovedOrResized() {
    GE_ASSERT(nullptr != m_windowData && nullptr != m_windowData->hWnd);

    if (IsIconic(m_windowData->hWnd)) {
      return;
    }

    RECT rc;
    GetWindowRect(m_windowData->hWnd, &rc);
    m_windowData->top = rc.top;
    m_windowData->left = rc.left;

    GetClientRect(m_windowData->hWnd, &rc);
    m_windowData->width = rc.right - rc.left;
    m_windowData->height = rc.bottom - rc.top;
  }

  Vector2I
  Win32Window::screenToWindowPos(const Vector2I& screenPos) const {
    POINT pos;
    pos.x = screenPos.x;
    pos.y = screenPos.y;

    ScreenToClient(m_windowData->hWnd, &pos);
    return Vector2I(pos.x, pos.y);
  }

  Vector2I
  Win32Window::windowToScreenPos(const Vector2I& windowPos) const {
    POINT pos;
    pos.x = windowPos.x;
    pos.y = windowPos.y;

    ClientToScreen(m_windowData->hWnd, &pos);
    return Vector2I(pos.x, pos.y);
  }

  int32
  Win32Window::getLeft() const {
    return m_windowData->left;
  }

  int32
  Win32Window::getTop() const {
    return m_windowData->top;
  }

  uint32
  Win32Window::getWidth() const {
    return m_windowData->width;
  }

  uint32
  Win32Window::getHeight() const {
    return m_windowData->height;
  }

  HWND
  Win32Window::getHWnd() const {
    return m_windowData->hWnd;
  }

  uint32
  Win32Window::getStyle() const {
    return m_windowData->style;
  }

  uint32
  Win32Window::getStyleEx() const {
    return m_windowData->styleEx;
  }

  void
  Win32Window::_enableAllWindows() {
    Vector<HWND> windowsToEnable;

    {
      Lock lock(s_windowsMutex);
      for (auto& window : s_allWindows) {
        windowsToEnable.push_back(window->m_windowData->hWnd);
      }
    }

    for (auto& entry : windowsToEnable) {
      EnableWindow(entry, TRUE);
    }
  }

  void
  Win32Window::_restoreModalWindows() {
    FrameVector<HWND> windowsToDisable;
    HWND bringToFrontHwnd = nullptr;

    {
      Lock lock(s_windowsMutex);

      if (!s_modalWindowStack.empty()) {
        Win32Window* curModalWindow = s_modalWindowStack.back();
        bringToFrontHwnd = curModalWindow->m_windowData->hWnd;

        for (auto& window : s_allWindows) {
          if (window != curModalWindow) {
            windowsToDisable.push_back(window->m_windowData->hWnd);
          }
        }
      }
    }

    for (auto& entry : windowsToDisable) {
      EnableWindow(entry, FALSE);
    }

    if (nullptr != bringToFrontHwnd) {
      BringWindowToTop(bringToFrontHwnd);
    }
  }
}
#endif // _WIN32
