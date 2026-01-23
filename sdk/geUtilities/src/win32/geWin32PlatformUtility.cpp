/*****************************************************************************/
/**
 * @file    geWin32PlatformUtility.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/11
 * @brief   Provides access to Windows specific utility functions
 *
 * Provides access to various Windows specific utility functions
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#ifdef _WIN32
/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "Win32/geMinWindows.h"
#include "gePrerequisitesUtilities.h"
#include "Win32/geWin32PlatformUtility.h"
#include "geColor.h"
#include "geUUID.h"
#include "geUnicode.h"
#include "geDebug.h"

#include <shellapi.h>
#include <iphlpapi.h>
#include <rpc.h>
#include <VersionHelpers.h>
#include <intrin.h>

namespace geEngineSDK {
  GPUInfo PlatformUtility::s_gpuInfo;

  typedef LONG NTSTATUS, *PNTSTATUS;
  using RtlGetVersionPtr = NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW);

  RTL_OSVERSIONINFOW getRealOSVersion() {
    HMODULE handle = GetModuleHandleW(L"ntdll.dll");
    if (handle) {
      auto rtlGetVersionFunc = reinterpret_cast<RtlGetVersionPtr>(
                                 GetProcAddress(handle, "RtlGetVersion"));
      if (nullptr != rtlGetVersionFunc) {
        RTL_OSVERSIONINFOW rovi;
        ge_zero_out(rovi);
        rovi.dwOSVersionInfoSize = sizeof(rovi);
        if (0 == rtlGetVersionFunc(&rovi)) {
          return rovi;
        }
      }
    }

    RTL_OSVERSIONINFOW rovi;
    ge_zero_out(rovi);
    return rovi;
  }

  SystemInfo
  PlatformUtility::getSystemInfo() {
    SystemInfo output;

    int32 CPUInfo[4] = { -1 };

    //Get CPU manufacturer
    __cpuid(CPUInfo, 0);
    output.cpuManufacturer = String(12, ' ');
    memcpy((char*)output.cpuManufacturer.data() + 0, &CPUInfo[1], 4);
    memcpy((char*)output.cpuManufacturer.data() + 4, &CPUInfo[3], 4);
    memcpy((char*)output.cpuManufacturer.data() + 8, &CPUInfo[2], 4);

    //Get CPU brand string
    String brandString;
    brandString.resize(48);

    //Get the information associated with each extended ID.
    __cpuid(CPUInfo, 0x80000000);
    uint32 numExtensionIds = static_cast<uint32>(CPUInfo[0]);
    for (uint32 i = 0x80000000; i <= numExtensionIds; ++i) {
      __cpuid(CPUInfo, i);

      if (0x80000002 == i) {
        memcpy(&brandString[0], CPUInfo, sizeof(CPUInfo));
      }
      else if (0x80000003 == i) {
        memcpy(&brandString[16], CPUInfo, sizeof(CPUInfo));
      }
      else if (0x80000004 == i) {
        memcpy(&brandString[32], CPUInfo, sizeof(CPUInfo));
      }
    }

    output.cpuModel = brandString;

    //Get number of CPU cores
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    output.cpuNumCores = static_cast<uint32>(sysInfo.dwNumberOfProcessors);

    //Get CPU clock speed
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                  R"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)",
                                  0,
                                  KEY_READ,
                                  &hKey);
    if (ERROR_SUCCESS == status) {
      DWORD mhz;
      DWORD bufferSize = 4;
      RegQueryValueEx(hKey, "~MHz", nullptr, nullptr, reinterpret_cast<LPBYTE>(&mhz), &bufferSize);
      output.cpuClockSpeedMhz = static_cast<uint32>(mhz);
    }
    else {
      output.cpuClockSpeedMhz = 0;
    }

    //Get amount of system memory
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    output.memoryAmountMb = static_cast<uint32>(statex.ullTotalPhys / (1024 * 1024));

#if USING(GE_ARCHITECTURE_x86_64)
      output.osIs64Bit = true;
#else
      HANDLE process = GetCurrentProcess();
      BOOL is64Bit = false;
      IsWow64Process(process, reinterpret_cast<PBOOL>(&is64Bit));
      output.osIs64Bit = is64Bit > 0;
#endif

    //Get OS version
    output.osName = "Windows " +
                    toString(static_cast<uint32>(getRealOSVersion().dwMajorVersion));

    //Get GPU info
    output.gpuInfo = s_gpuInfo;

    return output;
  }

  WString
  PlatformUtility::keyCodeToUnicode(uint32 keyCode) {
    static HKL keyboardLayout = GetKeyboardLayout(0);
    static uint8 keyboarState[256];

    if (FALSE == GetKeyboardState(keyboarState)) {
      return WString();
    }

    uint32 virtualKey = MapVirtualKeyExW(keyCode, 1, keyboardLayout);

    UNICHAR output[2];
    int count = ToUnicodeEx(virtualKey,
                            keyCode,
                            keyboarState,
                            output,
                            2,
                            0,
                            keyboardLayout);
    if (0 < count) {
      return WString(output, count);
    }

    return StringUtil::WBLANK;
  }

  bool
  PlatformUtility::getMACAddress(MACAddress& address) {
    std::memset(&address, 0, sizeof(address));

    PIP_ADAPTER_INFO adapterInfo = ge_alloc<IP_ADAPTER_INFO>();
    ULONG len = sizeof(IP_ADAPTER_INFO);
    DWORD rc = GetAdaptersInfo(adapterInfo, &len);

    if (ERROR_BUFFER_OVERFLOW == rc) {
      ge_free(adapterInfo);
      adapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(ge_alloc(len));
    }
    else if (ERROR_SUCCESS != rc) {
      ge_free(adapterInfo);
      return false;
    }

    if (NO_ERROR == GetAdaptersInfo(adapterInfo, &len)) {
      PIP_ADAPTER_INFO curAdapter = adapterInfo;
      while (curAdapter) {
        if (MIB_IF_TYPE_ETHERNET == curAdapter->Type && 
            sizeof(address) == curAdapter->AddressLength) {
          std::memcpy(&address, curAdapter->Address, curAdapter->AddressLength);
          return true;
        }

        curAdapter = curAdapter->Next;
      }
    }

    ge_free(adapterInfo);
    return false;
  }
  
  UUID
  PlatformUtility::generateUUID() {
    ::UUID uuid;
    if (RPC_S_OK != UuidCreate(&uuid)) {
      GE_LOG(kError, Generic, "Error creating UUID");
    }

    //Endianess might not be correct, but it shouldn't matter
    uint32 data1 = uuid.Data1;
    uint32 data2 = uuid.Data2 | (uuid.Data3 << 16);
    uint32 data3 = uuid.Data3 | (uuid.Data4[0] << 16) | (uuid.Data4[1] << 24);
    uint32 data4 = uuid.Data4[2] |
                  (uuid.Data4[3] << 8) |
                  (uuid.Data4[4] << 16) |
                  (uuid.Data4[5] << 24);

    return UUID(data1, data2, data3, data4);
  }

  String
  PlatformUtility::convertCaseUTF8(const String& input, bool toUpper) {
    if (input.empty()) {
      return "";
    }

    WString wideString = UTF8::toWide(input);

    DWORD flags = LCMAP_LINGUISTIC_CASING;
    flags |= toUpper ? LCMAP_UPPERCASE : LCMAP_LOWERCASE;

    uint32 reqNumChars = LCMapStringEx(LOCALE_NAME_USER_DEFAULT,
                                       flags,
                                       wideString.data(),
                                       static_cast<int>(wideString.length()),
                                       nullptr,
                                       0,
                                       nullptr,
                                       nullptr,
                                       0);

    WString outputWideString(reqNumChars, ' ');

    LCMapStringEx(LOCALE_NAME_USER_DEFAULT,
                  flags,
                  wideString.data(),
                  static_cast<int>(wideString.length()),
                  &outputWideString[0],
                  static_cast<int>(outputWideString.length()),
                  nullptr,
                  nullptr,
                  0);

    return UTF8::fromWide(outputWideString);
  }

  void
  PlatformUtility::open(const Path& path) {
    ShellExecute(nullptr, "open", path.toString().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
  }

  HBITMAP
  Win32PlatformUtility::createBitmap(const LinearColor* pixels,
                                     uint32 width,
                                     uint32 height,
                                     bool premultiplyAlpha) {
    BITMAPINFO bi;

    memset(&bi, 0, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC hDC = GetDC(nullptr);

    void* data = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hDC,
                                       &bi,
                                       DIB_RGB_COLORS,
                                       reinterpret_cast<void**>(&data),
                                       nullptr,
                                       0);
    HDC hBitmapDC = CreateCompatibleDC(hDC);
    ReleaseDC(nullptr, hDC);

    //Select the bitmaps to DC
    auto hOldBitmap = static_cast<HBITMAP>(SelectObject(hBitmapDC, hBitmap));

    //Scan each pixel of the source bitmap and create the masks
    LinearColor pixel;
    auto* dst = reinterpret_cast<DWORD*>(data);
    for (uint32 y = 0; y < height; ++y) {
      for (uint32 x = 0; x < width; ++x) {
        uint32 revY = height - y - 1;
        pixel = pixels[revY * width + x];

        if (premultiplyAlpha) {
          pixel.r *= pixel.a;
          pixel.g *= pixel.a;
          pixel.b *= pixel.a;
        }

        *dst = pixel.toColor(false).dwColor();
        ++dst;
      }
    }

    SelectObject(hBitmapDC, hOldBitmap);
    DeleteDC(hBitmapDC);

    return hBitmap;
  }
}

#endif
