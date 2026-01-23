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
    memcpy(cast::re<char*>(output.cpuManufacturer.data()) + 0, &CPUInfo[1], 4);
    memcpy(cast::re<char*>(output.cpuManufacturer.data()) + 4, &CPUInfo[3], 4);
    memcpy(cast::re<char*>(output.cpuManufacturer.data()) + 8, &CPUInfo[2], 4);

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
              if (mhz > 0.0) { output.cpuClockSpeedMhz = (uint32)mhz; break; }
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
        output.memoryAmountMb = (uint32)(memBytes / (1024ULL * 1024ULL));
      }
    }
#elif USING(GE_PLATFORM_LINUX)
    {
      struct sysinfo info {};
      if (0 == sysinfo(&info)) {
        uint64_t totalBytes = (uint64_t)info.totalram * (uint64_t)info.mem_unit;
        output.memoryAmountMb = (uint32)(totalBytes / (1024ULL * 1024ULL));
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

} // geEngineSDK
