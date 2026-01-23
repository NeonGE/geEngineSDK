/*****************************************************************************/
/**
 * @file    geWin32CrashHandler.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/10
 * @brief   Saves crash data and notifies when a crash occurs
 *
 * Saves crash data and notifies the user when a crash occurs
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
#include <psapi.h>

//Disable warning in VS2015 that's not under my control
#pragma warning(disable : 4091)
#	include <DbgHelp.h>
# include "geUnicode.h"
#pragma warning(default : 4091)

#include "gePrerequisitesUtilities.h"
#include "geDebug.h"
#include "geDynLib.h"
#include "geMath.h"

namespace geEngineSDK {
  using std::endl;
  using std::ios;

  static const ANSICHAR* s_MiniDumpName = "MiniDump.dmp";

  /**
   * @brief Returns the raw stack trace using the provided context.
   *        Raw stack trace contains only function addresses.
   * @param context Processor context from which to start the stack trace.
   * @param stackTrace  Output parameter that will contain the function addresses.
   *        First address is the deepest called function and following address is
   *        its caller and so on.
   * @returns	Number of functions in the call stack.
   */
  uint32
  win32_getRawStackTrace(CONTEXT context, uint64 stackTrace[GE_MAX_STACKTRACE_DEPTH]) {
    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();
    uint32 machineType;

    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(stackFrame));

    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrStack.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Mode = AddrModeFlat;

#if USING(GE_ARCHITECTURE_x86_64)
    stackFrame.AddrPC.Offset = context.Rip;
    stackFrame.AddrStack.Offset = context.Rsp;
    stackFrame.AddrFrame.Offset = context.Rbp;
    machineType = IMAGE_FILE_MACHINE_AMD64;
#else
    stackFrame.AddrPC.Offset = context.Eip;
    stackFrame.AddrStack.Offset = context.Esp;
    stackFrame.AddrFrame.Offset = context.Ebp;
    machineType = IMAGE_FILE_MACHINE_I386;
#endif

    uint32 numEntries = 0;
    while (true) {
      if (!StackWalk64(machineType,
                       hProcess,
                       hThread,
                       &stackFrame,
                       &context,
                       nullptr,
                       SymFunctionTableAccess64,
                       SymGetModuleBase64,
                       nullptr)) {
        break;
      }

      if (GE_MAX_STACKTRACE_DEPTH > numEntries) {
        stackTrace[numEntries] = stackFrame.AddrPC.Offset;
      }

      ++numEntries;

      if (0 == stackFrame.AddrPC.Offset || 0 == stackFrame.AddrFrame.Offset) {
        break;
      }
    }

    return numEntries;
  }

  /**
   * @brief Returns a string containing a stack trace using the provided context.
   *        If function can be found in the symbol table its readable name will be
   *        present in the stack trace, otherwise just its address.
   * @param context Processor context from which to start the stack trace.
   * @param skip Number of bottom-most call stack entries to skip.
   * @returns String containing the call stack with each function on its own line.
   */
  String
  win32_getStackTrace(CONTEXT context, uint32 skip = 0) {
    uint64 rawStackTrace[GE_MAX_STACKTRACE_DEPTH];
    uint32 numEntries = win32_getRawStackTrace(context, rawStackTrace);

    numEntries = Math::min(static_cast<uint32>(GE_MAX_STACKTRACE_DEPTH), numEntries);

    uint32 bufferSize = sizeof(PIMAGEHLP_SYMBOL64) + GE_MAX_STACKTRACE_NAME_BYTES;
    auto buffer = static_cast<uint8*>(ge_alloc(bufferSize));

    auto symbol = reinterpret_cast<PIMAGEHLP_SYMBOL64>(buffer);
    symbol->SizeOfStruct = bufferSize;
    symbol->MaxNameLength = GE_MAX_STACKTRACE_NAME_BYTES;

    HANDLE hProcess = GetCurrentProcess();

    StringStream outputStream;
    for (uint32 i = skip; i<numEntries; ++i) {
      if (i > skip) {
        outputStream << endl;
      }

      DWORD64 funcAddress = rawStackTrace[i];

      //Output function name
      DWORD64 dummy;
      if (SymGetSymFromAddr64(hProcess, funcAddress, &dummy, symbol)) {
        outputStream << StringUtil::format("{0}() - ", symbol->Name);
      }

      //Output file name and line
      IMAGEHLP_LINE64	lineData;
      lineData.SizeOfStruct = sizeof(lineData);

      String addressString = toString(funcAddress, 0, ' ', ios::hex);

      DWORD column;
      if (SymGetLineFromAddr64(hProcess, funcAddress, &column, &lineData)) {
        Path filePath(lineData.FileName);
        outputStream << StringUtil::format("0x{0} File[{1}:{2} ({3})]",
                                           addressString,
                                           filePath.getFilename(),
                                           static_cast<uint32>(lineData.LineNumber),
                                           static_cast<uint32>(column));
      }
      else {
        outputStream << StringUtil::format("0x{0}", addressString);
      }

      //Output module name
      IMAGEHLP_MODULE64 moduleData;
      moduleData.SizeOfStruct = sizeof(moduleData);

      if (SymGetModuleInfo64(hProcess, funcAddress, &moduleData)) {
        Path filePath(moduleData.ImageName);
        outputStream << StringUtil::format(" Module[{0}]", filePath.getFilename());
      }
    }

    ge_free(buffer);

    return outputStream.str();
  }

  using EnumProcessModulesType = bool(WINAPI*)(HANDLE hProcess,
                                               HMODULE* lphModule,
                                               DWORD cb,
                                               LPDWORD lpcbNeeded);

  using GetModuleBaseNameType = DWORD(WINAPI*)(HANDLE hProcess,
                                               HMODULE hModule,
                                               LPSTR lpBaseName,
                                               DWORD nSize);

  using GetModuleFileNameExType = DWORD(WINAPI*)(HANDLE hProcess,
                                                 HMODULE hModule,
                                                 LPSTR lpFilename,
                                                 DWORD nSize);

  using GetModuleInformationType = bool(WINAPI*)(HANDLE hProcess,
                                                 HMODULE hModule,
                                                 LPMODULEINFO lpmodinfo,
                                                 DWORD cb);

  static DynLib* g_PSAPILib = nullptr;

  static EnumProcessModulesType g_enumProcessModules;
  static GetModuleBaseNameType g_getModuleBaseName;
  static GetModuleFileNameExType g_getModuleFileNameEx;
  static GetModuleInformationType g_getModuleInformation;

  /**
   * @brief Dynamically load the PSAPI.dll and the required symbols, if not already loaded.
   */
  void win32_initPSAPI() {
    if (nullptr != g_PSAPILib) {
      return;
    }

    g_PSAPILib = ge_new<DynLib>("PSAPI.dll");
    g_enumProcessModules = reinterpret_cast<EnumProcessModulesType>
                            (g_PSAPILib->getSymbol("EnumProcessModules"));

    g_getModuleBaseName = reinterpret_cast<GetModuleBaseNameType>
                            (g_PSAPILib->getSymbol("GetModuleFileNameExA"));

    g_getModuleFileNameEx = reinterpret_cast<GetModuleFileNameExType>
                              (g_PSAPILib->getSymbol("GetModuleBaseNameA"));

    g_getModuleInformation = reinterpret_cast<GetModuleInformationType>
                                (g_PSAPILib->getSymbol("GetModuleInformation"));
  }

  /**
   * @brief Unloads the PSAPI.dll if is loaded.
   */
  void
  win32_unloadPSAPI() {
    if (nullptr == g_PSAPILib) {
      return;
    }

    g_PSAPILib->unload();
    ge_delete(g_PSAPILib);
    g_PSAPILib = nullptr;
  }

  static bool g_symbolsLoaded = false;

  /**
   * @brief Loads symbols for all modules in the current process.
   *        Loaded symbols allow the stack walker to retrieve human readable
   *        method, file, module names and other information.
   */
  void
  win32_loadSymbols() {
    if (g_symbolsLoaded) {
      return;
    }

    HANDLE hProcess = GetCurrentProcess();
    uint32 options = SymGetOptions();

    options |= SYMOPT_LOAD_LINES;
    options |= SYMOPT_EXACT_SYMBOLS;
    options |= SYMOPT_UNDNAME;
    options |= SYMOPT_FAIL_CRITICAL_ERRORS;
    options |= SYMOPT_NO_PROMPTS;

    SymSetOptions(options);
    if (!SymInitialize(hProcess, nullptr, false)) {
      GE_LOG(kError, Generic, "SymInitialize failed. Error code: {0}",
             static_cast<uint32>(GetLastError()));
      return;
    }

    DWORD bufferSize;
    g_enumProcessModules(hProcess, nullptr, 0, &bufferSize);

    auto* modules = static_cast<HMODULE*>(ge_alloc(bufferSize));
    g_enumProcessModules(hProcess, modules, bufferSize, &bufferSize);

    uint32 numModules = bufferSize / sizeof(HMODULE);
    for (uint32 i = 0; i<numModules; ++i) {
      MODULEINFO moduleInfo;

      char moduleName[GE_MAX_STACKTRACE_NAME_BYTES];
      char imageName[GE_MAX_STACKTRACE_NAME_BYTES];

      g_getModuleInformation(hProcess, modules[i], &moduleInfo, sizeof(moduleInfo));
      g_getModuleFileNameEx(hProcess, modules[i], imageName, GE_MAX_STACKTRACE_NAME_BYTES);
      g_getModuleBaseName(hProcess, modules[i], moduleName, GE_MAX_STACKTRACE_NAME_BYTES);

      char pdbSearchPath[GE_MAX_STACKTRACE_NAME_BYTES];
      char* fileName = nullptr;
      GetFullPathNameA(moduleName, GE_MAX_STACKTRACE_NAME_BYTES, pdbSearchPath, &fileName);
      *fileName = '\0';

      SymSetSearchPath(GetCurrentProcess(), pdbSearchPath);

      DWORD64 moduleAddress = SymLoadModule64(hProcess,
                                              modules[i],
                                              imageName,
                                              moduleName,
                                              (DWORD64)moduleInfo.lpBaseOfDll,
                                              static_cast<DWORD>(moduleInfo.SizeOfImage));

      if (0 != moduleAddress) {
        IMAGEHLP_MODULE64 imageInfo;
        memset(&imageInfo, 0, sizeof(imageInfo));
        imageInfo.SizeOfStruct = sizeof(imageInfo);

        if (!SymGetModuleInfo64(GetCurrentProcess(), moduleAddress, &imageInfo)) {
          GE_LOG(kWarning, Platform, "Failed retrieving module info for module: {0}. "
                                     "Error code: {1}",
                 moduleName, static_cast<uint32>(GetLastError()));
        }
        else {
#if USING(GE_DEBUG_DETAILED_SYMBOLS)
          if (SymNone == imageInfo.SymType) {
            GE_LOG(kWarning, Platform, "Failed loading symbols for module: {0}", moduleName);
          }
#endif
        }
      }
      else {
        GE_LOG(kWarning, Platform, "Failed loading module {0}. "
                                   "Error code: {1}. Search path: {2}. "
                                   "Image name: {3}",
               moduleName, static_cast<uint32>(GetLastError()),
               pdbSearchPath, imageName);;
      }
    }

    ge_free(modules);
    g_symbolsLoaded = true;
  }

  /**
   * @brief Converts an exception record into a human readable error message.
   */
  String
  win32_getExceptionMessage(EXCEPTION_RECORD* record) {
    String exceptionAddress = toString((uint64)record->ExceptionAddress,
                                       0,
                                       ' ',
                                       ios::hex);

    String format;
    switch (record->ExceptionCode)
    {
      case EXCEPTION_ACCESS_VIOLATION:
      {
        DWORD_PTR violatedAddress = 0;
        if (2 == record->NumberParameters) {
          if (0 == record->ExceptionInformation[0]) {
            format = "Unhandled exception at 0x{0}."\
                     "Access violation reading 0x{1}.";
          }
          else if (8 == record->ExceptionInformation[0]) {
            format = "Unhandled exception at 0x{0}."\
                     "Access violation DEP 0x{1}.";
          }
          else {
            format = "Unhandled exception at 0x{0}."\
                     "Access violation writing 0x{1}.";
          }

          violatedAddress = record->ExceptionInformation[1];
        }
        else {
          format = "Unhandled exception at 0x{0}."\
                   "Access violation.";
        }

        String violatedAddressStr = toString((uint64)violatedAddress, 0, ' ', ios::hex);
        return StringUtil::format(format, exceptionAddress, violatedAddressStr);
      }
      case EXCEPTION_IN_PAGE_ERROR:
      {
        DWORD_PTR violatedAddress = 0;
        DWORD_PTR code = 0;
        if (3 == record->NumberParameters) {
          if (0 == record->ExceptionInformation[0]) {
            format = "Unhandled exception at 0x{0}."\
                     "Page fault reading 0x{1} with code 0x{2}.";
          }
          else if (8 == record->ExceptionInformation[0]) {
            format = "Unhandled exception at 0x{0}."\
                     "Page fault DEP 0x{1} with code 0x{2}.";
          }
          else {
            format = "Unhandled exception at 0x{0}."\
                     "Page fault writing 0x{1} with code 0x{2}.";
          }

          violatedAddress = record->ExceptionInformation[1];
          code = record->ExceptionInformation[3];
        }
        else {
          format = "Unhandled exception at 0x{0}."\
                   "Page fault.";
        }

        String violatedAddressStr = toString((uint64)violatedAddress, 0, ' ', ios::hex);
        String codeStr = toString((uint64)code, 0, ' ', ios::hex);
        return StringUtil::format(format, exceptionAddress, violatedAddressStr, codeStr);
      }
      case STATUS_ARRAY_BOUNDS_EXCEEDED:
      {
        format = "Unhandled exception at 0x{0}."\
                 "Attempting to access an out of range array element.";
        return StringUtil::format(format, exceptionAddress);
      }
      case EXCEPTION_DATATYPE_MISALIGNMENT:
      {
        format = "Unhandled exception at 0x{0}."\
                 "Attempting to access missaligned data.";
        return StringUtil::format(format, exceptionAddress);
      }
      case EXCEPTION_FLT_DENORMAL_OPERAND:
      {
        format = "Unhandled exception at 0x{0}."\
                 "Floating point operand too small.";
        return StringUtil::format(format, exceptionAddress);
      }
      case EXCEPTION_FLT_DIVIDE_BY_ZERO:
      {
        format = "Unhandled exception at 0x{0}."\
                 "Floating point operation attempted to divide by zero.";
        return StringUtil::format(format, exceptionAddress);
      }
      case EXCEPTION_FLT_INVALID_OPERATION:
      {
        format = "Unhandled exception at 0x{0}."\
                 "Floating point invalid operation.";
        return StringUtil::format(format, exceptionAddress);
      }
      case EXCEPTION_FLT_OVERFLOW:
      {
        format = "Unhandled exception at 0x{0}."\
                 "Floating point overflow.";
        return StringUtil::format(format, exceptionAddress);
      }
      case EXCEPTION_FLT_UNDERFLOW:
      {
        format = "Unhandled exception at 0x{0}."\
                 "Floating point underflow.";
        return StringUtil::format(format, exceptionAddress);
      }
      case EXCEPTION_FLT_STACK_CHECK:
      {
        format = "Unhandled exception at 0x{0}."\
                 "Floating point stack overflow/underflow.";
        return StringUtil::format(format, exceptionAddress);
      }
      case EXCEPTION_ILLEGAL_INSTRUCTION:
      {
        format = "Unhandled exception at 0x{0}."\
                 "Attempting to execute an illegal instruction.";
        return StringUtil::format(format, exceptionAddress);
      }
      case EXCEPTION_PRIV_INSTRUCTION:
      {
        format = "Unhandled exception at 0x{0}."\
                 "Attempting to execute a private instruction.";
        return StringUtil::format(format, exceptionAddress);
      }
      case EXCEPTION_INT_DIVIDE_BY_ZERO:
      {
        format = "Unhandled exception at 0x{0}."\
                 "Integer operation attempted to divide by zero.";
        return StringUtil::format(format, exceptionAddress);
      }
      case EXCEPTION_INT_OVERFLOW:
      {
        format = "Unhandled exception at 0x{0}."\
                 "Integer operation result has overflown.";
        return StringUtil::format(format, exceptionAddress);
      }
      case EXCEPTION_STACK_OVERFLOW:
      {
        format = "Unhandled exception at 0x{0}."\
                 "Stack overflow.";
        return StringUtil::format(format, exceptionAddress);
      }
      default:
      {
        format = "Unhandled exception at 0x{0}."\
                 "Code 0x{1}.";

        String exceptionCode = toString((uint32)record->ExceptionCode, 0, ' ', ios::hex);
        return StringUtil::format(format, exceptionAddress, exceptionCode);
      }

    } //switch
  }

  struct MiniDumpParams
  {
    Path filePath;
    EXCEPTION_POINTERS* exceptionData;
  };

  DWORD
  CALLBACK win32_writeMiniDumpWorker(void* data) {
    MiniDumpParams* params = static_cast<MiniDumpParams*>(data);

    WString pathString = UTF8::toWide(params->filePath.toString());
    HANDLE hFile = CreateFileW(pathString.c_str(),
                               GENERIC_WRITE,
                               0,
                               nullptr,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               nullptr);

    if (INVALID_HANDLE_VALUE != hFile) {
      MINIDUMP_EXCEPTION_INFORMATION DumpExceptionInfo;
      DumpExceptionInfo.ThreadId = GetCurrentThreadId();
      DumpExceptionInfo.ExceptionPointers = params->exceptionData;
      DumpExceptionInfo.ClientPointers = false;

      MiniDumpWriteDump(GetCurrentProcess(),
                        GetCurrentProcessId(),
                        hFile,
                        MiniDumpNormal,
                        &DumpExceptionInfo,
                        nullptr,
                        nullptr);
      CloseHandle(hFile);
    }

    return 0;
  }

  void
  win32_writeMiniDump(const Path& filePath, EXCEPTION_POINTERS* exceptionData) {
    MiniDumpParams param = {filePath, exceptionData};

    //Write mini dump on a second thread in order to preserve the current thread's call stack
    DWORD threadId = 0;
    HANDLE hThread = CreateThread(nullptr,
                                  0,
                                  &win32_writeMiniDumpWorker,
                                  &param,
                                  0,
                                  &threadId);

    if (nullptr != hThread) {
      WaitForSingleObject(hThread, INFINITE);
      CloseHandle(hThread);
    }
  }

  void
  win32_popupErrorMessageBox(const WString& msg, const Path& folder) {
    WString simpleErrorMessage = msg +
            L"\n\nFor more information check the crash report located at:\n " +
            UTF8::toWide(folder.toString());
    MessageBoxW(nullptr, simpleErrorMessage.c_str(), L"geEngineSDK fatal error!", MB_OK);

  }

  struct CrashHandler::Data
  {
    Mutex mutex;
  };

  CrashHandler::CrashHandler() {
    m_crashData = ge_new<Data>();
  }

  CrashHandler::~CrashHandler() {
    ge_delete(m_crashData);
  }

  void
  CrashHandler::reportCrash(const String& type,
                            const String& strDescription,
                            const String& strFunction,
                            const String& strFile,
                            uint32 nLine) const {
    //Win32 debug methods are not thread safe
    Lock lock(m_crashData->mutex);

    logErrorAndStackTrace(type, strDescription, strFunction, strFile, nLine);
    saveCrashLog();

    win32_writeMiniDump(getCrashFolder() + Path(s_MiniDumpName), nullptr);
    win32_popupErrorMessageBox(toWString(s_fatalErrorMsg), getCrashFolder());
    
    //Note: Potentially also log Windows Error Report and/or send crash data to server
  }

  int
  CrashHandler::reportCrash(void* exceptionDataPtr) const {
    EXCEPTION_POINTERS* exceptionData = static_cast<EXCEPTION_POINTERS*>(exceptionDataPtr);

    //Win32 debug methods are not thread safe
    Lock lock(m_crashData->mutex);

    win32_initPSAPI();
    win32_loadSymbols();

    logErrorAndStackTrace(win32_getExceptionMessage(exceptionData->ExceptionRecord),
                          win32_getStackTrace(*exceptionData->ContextRecord, 0));
    saveCrashLog();

    win32_writeMiniDump(getCrashFolder() + Path(s_MiniDumpName), exceptionData);
    win32_popupErrorMessageBox(toWString(s_fatalErrorMsg), getCrashFolder());

    //Note: Potentially also log Windows Error Report and/or send crash data to server
    return EXCEPTION_EXECUTE_HANDLER;
  }

  String
  CrashHandler::getCrashTimestamp() {
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);

    String timeStamp = "{0}{1}{2}_{3}{4}";
    String strYear = toString(systemTime.wYear, 4, '0');
    String strMonth = toString(systemTime.wMonth, 2, '0');
    String strDay = toString(systemTime.wDay, 2, '0');
    String strHour = toString(systemTime.wHour, 2, '0');
    String strMinute = toString(systemTime.wMinute, 2, '0');
    return StringUtil::format(timeStamp, strYear, strMonth, strDay, strHour, strMinute);
  }

  String
  CrashHandler::getStackTrace() {
    CONTEXT context;
    RtlCaptureContext(&context);

    win32_initPSAPI();
    win32_loadSymbols();
    return win32_getStackTrace(context, 2);
  }
}

#endif // _WIN32
