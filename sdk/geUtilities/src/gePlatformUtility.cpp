/*****************************************************************************/
/**
 * @file    gePlatformUtility.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/02/24
 * @brief   Platform Utilities implementation for multi-platform
 *
 * Implementation of multi-platform versions of some older system specific
 * implementations that were previously only available on Windows.
 *
 * @bug	    No known bugs.
 *
 * @update  2026/01/22 Samuel Prince - Added POSIX implementation.
 * @update  2024/06/12 Samuel Prince - Added terminate implementation.
 * @update  2024/06/24 Samuel Prince - Added necessary includes for PS4/PS5.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
 /*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "gePlatformUtility.h"
#include "geUUID.h"
#include "geUnicode.h"
#include "geDebug.h"
#include "geColor.h"

#if USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
# include "win32/geMinWindows.h"
# include <shellapi.h>
# include <iphlpapi.h>
# include <rpc.h>
# include <intrin.h>
# include <VersionHelpers.h>
#endif

#if USING(GE_PLATFORM_PS4) || USING(GE_PLATFORM_PS5)
# include <kernel.h>
#endif

//POSIX family (Linux/mac/iOS/Android)
#if USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_OSX) || \
    USING(GE_PLATFORM_ANDROID) || USING(GE_PLATFORM_IOS)

# include <unistd.h>
# include <sys/utsname.h>

# include <ifaddrs.h>
# include <net/if.h>

# if USING(GE_PLATFORM_OSX) || USING(GE_PLATFORM_IOS)
#   include <sys/sysctl.h>
#   include <net/if_dl.h>

    //For keyCodeToUnicode on macOS (no iOS)
#   if USING(GE_PLATFORM_OSX)
#     include <Carbon/Carbon.h>
#   endif
# else
    //Linux/Android
#   include <netpacket/packet.h>
#   if USING(GE_PLATFORM_LINUX)
#     include <sys/sysinfo.h>
#   endif
# endif

# if __has_include(<uuid/uuid.h>)
#   include <uuid/uuid.h>
#   define GE_HAS_LIBUUID 1
# else
#   define GE_HAS_LIBUUID 0
# endif

  //Optional ICU for proper Unicode case conversion (recommended)
# if __has_include(<unicode/unistr.h>) && __has_include(<unicode/locid.h>)
#   include <unicode/unistr.h>
#   include <unicode/locid.h>
#   define GE_HAS_ICU 1
# else
#   define GE_HAS_ICU 0
# endif

  //x86 cpuid (Linux/mac) - optional, only for x86/x64 clang/gcc
# if (defined(__i386__) || defined(__x86_64__)) && \
     (defined(__GNUC__) || defined(__clang__))
#   include <cpuid.h>
#   define GE_HAS_X86_CPUID 1
# else
#   define GE_HAS_X86_CPUID 0
# endif

#endif

namespace geEngineSDK {
  GPUInfo PlatformUtility::s_gpuInfo;

  GE_NORETURN void
  PlatformUtility::terminate(bool force) {
    if (!force) {
#if USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
      PostQuitMessage(0);
#elif USING(GE_PLATFORM_PS4) || USING(GE_PLATFORM_PS5)
      _exit(0);
#elif USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_OSX) || \
      USING(GE_PLATFORM_ANDROID) || USING(GE_PLATFORM_IOS)
      _exit(0);
#endif
    }
    else {
#if USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
      TerminateProcess(GetCurrentProcess(), 0);
#elif USING(GE_PLATFORM_PS4) || USING(GE_PLATFORM_PS5)
      _exit(0);
#elif USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_OSX) || \
      USING(GE_PLATFORM_ANDROID) || USING(GE_PLATFORM_IOS)
      _exit(0);
#endif
    }
  }

  SystemInfo
  PlatformUtility::getSystemInfo() {
    SystemInfo output;

#if USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
    int32 CPUInfo[4] = { -1 };

    __cpuid(CPUInfo, 0);
    output.cpuManufacturer = String(12, ' ');

#if USING(GE_CPP17_OR_LATER)
    auto pCPUManuf = output.cpuManufacturer.data();
#else
    auto pCPUManuf = &output.cpuManufacturer[0];
#endif
    memcpy(cast::re<char*>(pCPUManuf) + 0, &CPUInfo[1], 4);
    memcpy(cast::re<char*>(pCPUManuf) + 4, &CPUInfo[3], 4);
    memcpy(cast::re<char*>(pCPUManuf) + 8, &CPUInfo[2], 4);

    String brandString;
    brandString.resize(48);

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

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    output.cpuNumCores = static_cast<uint32>(sysInfo.dwNumberOfProcessors);

    //CPU clock (Windows: registry)
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                  R"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)",
                                  0,
                                  KEY_READ,
                                  &hKey);
    if (ERROR_SUCCESS == status) {
      DWORD mhz;
      DWORD bufferSize = 4;
      RegQueryValueEx(hKey,
                      "~MHz",
                      nullptr,
                      nullptr,
                      reinterpret_cast<LPBYTE>(&mhz),
                      &bufferSize);
      output.cpuClockSpeedMhz = static_cast<uint32>(mhz);
    }
    else {
      output.cpuClockSpeedMhz = 0;
    }

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
    output.osName = "Windows";
    output.gpuInfo = s_gpuInfo;
    return output;

#elif USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_OSX) || \
      USING(GE_PLATFORM_ANDROID) || USING(GE_PLATFORM_IOS)

    //Cores
    uint32 cores = cast::st<uint32>(GE_THREAD_HARDWARE_CONCURRENCY);
    output.cpuNumCores = cores > 0 ? cores : 1;

    //CPU vendor/model (x86 only)
#if GE_HAS_X86_CPUID
    {
      String brand, vendor;
      brand.clear(); vendor.clear();

      uint32 eax = 0, ebx = 0, ecx = 0, edx = 0;
      if (__get_cpuid(0, &eax, &ebx, &ecx, &edx)) {
        char v[13] = {};
        std::memcpy(v + 0, &ebx, 4);
        std::memcpy(v + 4, &edx, 4);
        std::memcpy(v + 8, &ecx, 4);
        vendor = String(v);
      }

      uint32 maxExt = 0;
      __get_cpuid(0x80000000, &maxExt, &ebx, &ecx, &edx);
      if (maxExt >= 0x80000004) {
        char b[49] = {};
        uint32 regs[4];

        __get_cpuid(0x80000002, &regs[0], &regs[1], &regs[2], &regs[3]);
        std::memcpy(b + 0, regs, sizeof(regs));
        __get_cpuid(0x80000003, &regs[0], &regs[1], &regs[2], &regs[3]);
        std::memcpy(b + 16, regs, sizeof(regs));
        __get_cpuid(0x80000004, &regs[0], &regs[1], &regs[2], &regs[3]);
        std::memcpy(b + 32, regs, sizeof(regs));

        brand = String(b);
      }

      output.cpuManufacturer = vendor.empty() ? "Unknown" : vendor;
      output.cpuModel = brand.empty() ? "Unknown" : brand;
    }
#else
    output.cpuManufacturer = "Unknown";
    output.cpuModel = "Unknown";
#endif

    // CPU clock (best effort)
    output.cpuClockSpeedMhz = 0;
#if USING(GE_PLATFORM_OSX) || USING(GE_PLATFORM_IOS)
    {
      uint64_t freqHz = 0;
      size_t sz = sizeof(freqHz);
      if (0 == sysctlbyname("hw.cpufrequency", &freqHz, &sz, nullptr, 0) && freqHz > 0) {
        output.cpuClockSpeedMhz = (uint32)(freqHz / 1000000ULL);
      }
    }
#elif USING(GE_PLATFORM_LINUX)
    {
      FILE* f = fopen("/proc/cpuinfo", "r");
      if (f) {
        char line[512];
        while (fgets(line, sizeof(line), f)) {
          if (0 == std::strncmp(line, "cpu MHz", 7)) {
            const char* colon = std::strchr(line, ':');
            if (colon) {
              double mhz = std::atof(colon + 1);
              if (mhz > 0.0) {
                output.cpuClockSpeedMhz = static_cast<uint32>(mhz);
                break;
              }
            }
          }
        }
        fclose(f);
      }
    }
#endif

    // Memory (best effort)
    output.memoryAmountMb = 0;
#if USING(GE_PLATFORM_OSX) || USING(GE_PLATFORM_IOS)
    {
      uint64_t memBytes = 0;
      size_t memSize = sizeof(memBytes);
      if (0 == sysctlbyname("hw.memsize", &memBytes, &memSize, nullptr, 0) && memBytes > 0) {
        output.memoryAmountMb = static_cast<uint32>(memBytes / (1024ULL * 1024ULL));
      }
    }
#elif USING(GE_PLATFORM_LINUX)
    {
      struct sysinfo info {};
      if (0 == sysinfo(&info)) {
        uint64_t totalBytes = uint64_t(info.totalram) * uint64_t(info.mem_unit);
        output.memoryAmountMb = static_cast<uint32>(totalBytes / (1024ULL * 1024ULL));
      }
    }
#endif

    // 64-bit
#if USING(GE_ARCHITECTURE_x86_64) || USING(GE_ARCHITECTURE_ARM64)
    output.osIs64Bit = true;
#else
    output.osIs64Bit = (sizeof(void*) == 8);
#endif

    // OS name
    {
      struct utsname u {};
      if (0 == uname(&u)) {
#if USING(GE_PLATFORM_OSX)
        output.osName = String("macOS ") + u.sysname + " " + u.release;
#elif USING(GE_PLATFORM_IOS)
        output.osName = String("iOS ") + u.sysname + " " + u.release;
#elif USING(GE_PLATFORM_ANDROID)
        output.osName = String("Android ") + u.sysname + " " + u.release;
#else
        output.osName = String("Linux ") + u.sysname + " " + u.release;
#endif
      }
      else {
        output.osName = "Unknown";
      }
    }

    output.gpuInfo = s_gpuInfo;
    return output;

#else
    // Other platforms: return minimal info
    output.cpuManufacturer = "Unknown";
    output.cpuModel = "Unknown";
    output.cpuNumCores = 1;
    output.cpuClockSpeedMhz = 0;
    output.memoryAmountMb = 0;
    output.osIs64Bit = (sizeof(void*) == 8);
    output.osName = "Unknown";
    output.gpuInfo = s_gpuInfo;
    return output;
#endif
  }

  //===========================================================================
  // keyCodeToUnicode (Win + macOS; Linux generic fallback)
  //===========================================================================
  WString
  PlatformUtility::keyCodeToUnicode(uint32 keyCode) {
#if USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
    static HKL keyboardLayout = GetKeyboardLayout(0);
    static uint8 keyboarState[256];

    if (FALSE == GetKeyboardState(keyboarState)) {
      return WString();
    }

    uint32 virtualKey = MapVirtualKeyExW(keyCode, 1, keyboardLayout);

    UNICHAR output[2];
    int32 count = ToUnicodeEx(virtualKey,
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

#elif USING(GE_PLATFORM_OSX)
    //macOS: must be a "virtual keycode" like kVK_* 
    TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardLayoutInputSource();
    if (!currentKeyboard) {
      return WString();
    }

    CFDataRef layoutData = (CFDataRef)TISGetInputSourceProperty(currentKeyboard,
                             kTISPropertyUnicodeKeyLayoutData);

    if (!layoutData) {
      CFRelease(currentKeyboard);
      return WString();
    }

    const UCKeyboardLayout* keyboardLayout =
      (const UCKeyboardLayout*)CFDataGetBytePtr(layoutData);

    UInt32 deadKeyState = 0;
    UniCharCount maxLen = 4;
    UniCharCount outLen = 0;
    UniChar chars[4] = {};

    UInt32 modifiers = 0;
    OSStatus status = UCKeyTranslate(keyboardLayout,
                                     (UInt16)keyCode,
                                     kUCKeyActionDown,
                                     (modifiers >> 8) & 0xFF,
                                     LMGetKbdType(),
                                     kUCKeyTranslateNoDeadKeysBit,
                                     &deadKeyState,
                                     maxLen,
                                     &outLen,
                                     chars);

    CFRelease(currentKeyboard);

    if (status == noErr && outLen > 0) {
      return WString((wchar_t*)chars, (size_t)outLen);
    }
    return WString();

#else
    (void)keyCode;
    return WString(); //Linux/iOS/Android: requires per-desktop-environment implementation
#endif
  }

  //===========================================================================
  // getMACAddress (Win + Linux/mac)
  //===========================================================================
  bool
  PlatformUtility::getMACAddress(MACAddress& address) {
#if USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
    memset(&address, 0, sizeof(address));

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
          memcpy(&address, curAdapter->Address, curAdapter->AddressLength);
          ge_free(adapterInfo);
          return true;
        }
        curAdapter = curAdapter->Next;
      }
    }

    ge_free(adapterInfo);
    return false;

#elif USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_OSX) || \
      USING(GE_PLATFORM_ANDROID) || USING(GE_PLATFORM_IOS)

    std::memset(&address, 0, sizeof(address));

    struct ifaddrs* ifa = nullptr;
    if (0 != getifaddrs(&ifa) || !ifa) {
      return false;
    }

    bool found = false;
    for (struct ifaddrs* it = ifa; it != nullptr; it = it->ifa_next) {
      if (!it->ifa_addr) continue;
      if ((it->ifa_flags & IFF_LOOPBACK) != 0) continue;
      if ((it->ifa_flags & IFF_UP) == 0) continue;

#if USING(GE_PLATFORM_OSX) || USING(GE_PLATFORM_IOS)
      if (it->ifa_addr->sa_family == AF_LINK) {
        const struct sockaddr_dl* sdl = (const struct sockaddr_dl*)it->ifa_addr;
        if (sdl->sdl_alen == sizeof(MACAddress)) {
          const unsigned char* mac = (const unsigned char*)LLADDR(sdl);
          std::memcpy(&address, mac, sizeof(MACAddress));
          found = true;
          break;
        }
      }
#else
      if (it->ifa_addr->sa_family == AF_PACKET) {
        const auto* s = reinterpret_cast<const struct sockaddr_ll*>(it->ifa_addr);
        if (s->sll_halen == sizeof(MACAddress)) {
          std::memcpy(&address, s->sll_addr, sizeof(MACAddress));
          found = true;
          break;
        }
      }
#endif
    }

    freeifaddrs(ifa);
    return found;

#else
    std::memset(&address, 0, sizeof(address));
    return false;
#endif
  }

  //===========================================================================
  // generateUUID (Win + Linux/mac)
  //===========================================================================
  UUID
  PlatformUtility::generateUUID() {
#if USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
    ::UUID uuid;
    if (RPC_S_OK != UuidCreate(&uuid)) {
      GE_LOG(kError, Generic, "Error creating UUID");
    }

    uint32 data1 = uuid.Data1;
    uint32 data2 = uuid.Data2 | (uuid.Data3 << 16);
    uint32 data3 = uuid.Data3 | (uuid.Data4[0] << 16) | (uuid.Data4[1] << 24);
    uint32 data4 = uuid.Data4[2] |
                  (uuid.Data4[3] << 8) |
                  (uuid.Data4[4] << 16) |
                  (uuid.Data4[5] << 24);

    return UUID(data1, data2, data3, data4);

#elif USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_OSX) || \
      USING(GE_PLATFORM_ANDROID) || USING(GE_PLATFORM_IOS)

#if GE_HAS_LIBUUID
    uuid_t u;
    uuid_generate(u);

    uint32 data1 = 0, data2 = 0, data3 = 0, data4 = 0;
    std::memcpy(&data1, &u[0], 4);
    std::memcpy(&data2, &u[4], 4);
    std::memcpy(&data3, &u[8], 4);
    std::memcpy(&data4, &u[12], 4);
    return UUID(data1, data2, data3, data4);
#else
    //fallback: no libuuid
    uint32 data1 = static_cast<uint32>(std::rand());
    uint32 data2 = static_cast<uint32>(std::rand());
    uint32 data3 = static_cast<uint32>(std::rand());
    uint32 data4 = static_cast<uint32>(std::rand());
    return UUID(data1, data2, data3, data4);
#endif

#else
    return UUID();
#endif
  }

  //===========================================================================
  // convertCaseUTF8 (Win + POSIX)
  //===========================================================================
  String
  PlatformUtility::convertCaseUTF8(const String& input, bool toUpper) {
#if USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
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

#elif USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_OSX) || \
      USING(GE_PLATFORM_ANDROID) || USING(GE_PLATFORM_IOS)

    if (input.empty()) return "";

#if GE_HAS_ICU
    icu::UnicodeString u = icu::UnicodeString::fromUTF8(input.c_str());
    if (toUpper) u.toUpper();
    else         u.toLower();

    String out;
    u.toUTF8String(out);
    return out;
#else
    //Simple fallback (not proper Unicode handling)
    WString wide = UTF8::toWide(input);
    if (wide.empty()) return "";

    for (auto& ch : wide) {
      ch = (wchar_t)(toUpper ? std::towupper(ch) : std::towlower(ch));
    }
    return UTF8::fromWide(wide);
#endif

#else
    return input;
#endif
  }

  //===========================================================================
  // open (Win + POSIX)
  //===========================================================================
  void
  PlatformUtility::open(const Path& path) {
#if USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
    ShellExecute(nullptr,
                 "open",
                 path.toString().c_str(),
                 nullptr,
                 nullptr,
                 SW_SHOWNORMAL);

#elif USING(GE_PLATFORM_OSX)
    const String s = path.toString();
    pid_t pid = fork();
    if (pid == 0) {
      execlp("open", "open", s.c_str(), nullptr);
      _exit(127);
    }

#elif USING(GE_PLATFORM_LINUX)
    const String s = path.toString();
    pid_t pid = fork();
    if (pid == 0) {
      execlp("xdg-open", "xdg-open", s.c_str(), nullptr);
      _exit(127);
    }
#else
    //iOS/Android/Consoles: not supported
    (void)path;
#endif
  }
} // geEngineSDK
