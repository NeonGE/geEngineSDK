/*****************************************************************************/
/**
 * @file    gePlatformUtility.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/10
 * @brief   Provides access to various operating system specific utility
 *          functions.
 *
 * Provides access to various operating system specific utility functions.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

#include "gePlatformDefines.h"
#include "geString.h"
#include "gePlatformTypes.h"
#include "geUUID.h"

namespace geEngineSDK {

  /**
   * @brief Represents a MAC (Ethernet) address.
   */
  struct MACAddress
  {
    uint8 value[6];
  };

  /**
   * @brief Contains information about available GPUs on the system.
   */
  struct GPUInfo
  {
    String names[5];
    uint32 numGPUs;
  };

  /**
   * @brief Contains information about the system hardware and operating system.
   */
  struct SystemInfo
  {
    String cpuManufacturer;
    String cpuModel;
    uint32 cpuClockSpeedMhz;
    uint32 cpuNumCores;
    uint32 memoryAmountMb;
    String osName;
    bool osIs64Bit;
    GPUInfo gpuInfo;
  };

  /**
   * @brief Possible type of platform file dialogs.
   */
  namespace FILE_DIALOG_TYPE {
    enum E {
      kOpenFile = 0x0,
      kOpenFolder = 0x1,
      kSave = 0x2,
      kMultiselect = 0x10000,
      kTypeMask = 0xFFFF
    };
  }

  /**
   * @brief Provides access to various operating system specific utility functions.
   */
  class GE_UTILITIES_EXPORT PlatformUtility
  {
   public:
    /**
     * @brief Terminates the current process.
     * @param[in] force True if the process should be forcefully terminated with no cleanup.
     */
    [[noreturn]] static void
    terminate(bool force = false);

    /**
     * @brief Converts a keyboard key-code to a Unicode character.
     * @note  Normally this will output a single character, but it can happen
     *        it outputs multiple in case a accent/diacritic character could
     *        not be combined with the virtual key into a single character.
     */
    static WString
    keyCodeToUnicode(uint32 keyCode);

    /**
     * @brief Populates the provided buffer with a MAC address of the first available
     *        adapter, if one exists. If no adapters exist, returns false.
     */
    static bool
    getMACAddress(MACAddress& address);

    /**
     * @brief Creates a new universally unique identifier and returns it as a UUID.
     */
    static UUID
    generateUUID();

    /**
     * @brief Opens the provided file or folder using the default application
     *        for that file type, as specified by the operating system.
     * @param[in] path  Absolute path to the file or folder to open.
     */
    static void
    open(const Path& path);

    /**
     * @brief Returns information about the underlying hardware.
     */
    static SystemInfo
    getSystemInfo();

    /**
     * @brief Converts a UTF8 encoded string into uppercase or lowercase.
     * @param[in]  input    String to convert.
     * @param[in]  toUpper  If true, converts the character to uppercase.
     *                      Otherwise convert to lowercase.
     * @return     Converted string.
     */
    static String
    convertCaseUTF8(const String& input, bool toUpper);

    /**
     * @brief Assigns information about GPU hardware.
     *        This data will be returned by getSystemInfo() when requested.
     *        This is expeced to be called by the render API backend when initialized.
     */
    static void
    _setGPUInfo(GPUInfo gpuInfo) {
      s_gpuInfo = gpuInfo;
    }

   private:
    static GPUInfo s_gpuInfo;
  };
}
