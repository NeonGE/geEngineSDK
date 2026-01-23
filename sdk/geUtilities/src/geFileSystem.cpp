/*****************************************************************************/
/**
 * @file    geFileSystem.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/09/19
 * @brief   Utility class for dealing with files.
 *
 * Shared functions among different operating systems
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geFileSystem.h"
#include "geException.h"
#include "geDataStream.h"
#include "geUnicode.h"
#include "geDebug.h"

#if USING(GE_USE_GENERIC_FILESYSTEM)
#   include <filesystem>
#include "geFileSystem.h"
namespace fileSys = std::filesystem;
  using std::visit;
  using std::error_code;
#endif

#if USING(GE_PLATFORM_WINDOWS)
#include <Win32/geMinWindows.h>
#include <shlobj.h>
#elif USING(GE_PLATFORM_OSX)
#include <cstdlib>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#elif USING(GE_PLATFORM_LINUX)
#include <cstdlib>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include "geFileSystem.h"
#endif

namespace geEngineSDK {
  using std::tuple;
  using std::make_tuple;
  using std::get;
  using namespace std::chrono;
  
  Mutex FileScheduler::m_mutex;

#if USING(GE_USE_GENERIC_FILESYSTEM)
  bool
  sys_isDevice(const WString& path) {
#if USING(GE_PLATFORM_WINDOWS) || USING(GE_PLATFORM_XBOX)
    //Windows and Xbox-specific device check
    WString ucPath = path;
    StringUtil::toUpperCase(ucPath);
    return ucPath.compare(0, 4, L"\\\\.\\") == 0 ||  // Device prefix for Windows
           ucPath == L"CON" ||
           ucPath == L"PRN" ||
           ucPath == L"AUX" ||
           ucPath == L"NUL" ||
           (ucPath.length() == 4 && ucPath.compare(0, 3, L"LPT") == 0 &&
            ucPath[3] >= L'1' && ucPath[3] <= L'9') ||
           (ucPath.length() == 4 && ucPath.compare(0, 3, L"COM") == 0 &&
            ucPath[3] >= L'1' && ucPath[3] <= L'9');

#elif USING(GE_PLATFORM_ANDROID)
    //Android-specific check: often '/dev' but sometimes other directories
    fileSys::path p = path;
    return p.has_parent_path() &&
           (p.parent_path() == L"/dev" || p.parent_path() == L"/dev/block");

#elif USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_OSX) || USING(GE_PLATFORM_IOS) || \
      USING(GE_PLATFORM_PS4) || USING(GE_PLATFORM_PS5)
    //General UNIX-based check for Linux, macOS, iOS, PS4, PS5
    fileSys::path p = path;
    return p.has_parent_path() && p.parent_path() == L"/dev";

#else
#   error "Unsupported platform for device detection"
#endif
  }

  bool
  sys_pathExists(const WString& path) {
    fileSys::file_status status = fileSys::status(fileSys::path(path));
    return fileSys::exists(status);
  }

  bool
  sys_isDirectory(const WString& path) {
    fileSys::file_status status = fileSys::status(fileSys::path(path));
    return fileSys::is_directory(status);
  }

  bool
  sys_isFile(const WString& path) {
    return !sys_isDirectory(path) && !sys_isDevice(path);
  }

  uint64
  sys_getFileSize(const WString& path) {
    fileSys::path filePath = path;

    //Check if the path exists and is a regular file
    fileSys::file_status status = fileSys::status(filePath);
    if (fileSys::exists(status) && fileSys::is_regular_file(status)) {
      return static_cast<uint64>(fileSys::file_size(filePath));
    }

    return 0;
  }

  time_t
  sys_getLastModifiedTime(const WString& path) {
    fileSys::path filePath = path;

    //Check if the path exists and is a regular file
    fileSys::file_status status = fileSys::status(filePath);
    if (fileSys::exists(status) && fileSys::is_regular_file(status)) {
      //Get last write time
      auto ftime = fileSys::last_write_time(filePath);

      //Convert to system time (time_t)
      auto sctp = time_point_cast<system_clock::duration>
      (
        ftime - fileSys::file_time_type::clock::now() + system_clock::now()
      );
      return system_clock::to_time_t(sctp);
    }

    return 0;
  }

  SPtr<DataStream>
  FileSystem::openFile(const Path& fullPath, bool readOnly) {
    WString pathWString = UTF8::toWide(fullPath.toString());
    auto pathString = pathWString.c_str();

    if (!sys_pathExists(pathString) || !sys_isFile(pathString)) {
      GE_LOG(kWarning,
             Platform,
             "Attempting to open a file that doesn't exist: {0}",
             fullPath);
      return nullptr;
    }

    ACCESS_MODE::E accessMode = ACCESS_MODE::kREAD;
    if (!readOnly) {
      accessMode = static_cast<ACCESS_MODE::E>(accessMode |
                     static_cast<uint32>(ACCESS_MODE::kWRITE));
    }

    return ge_shared_ptr_new<FileDataStream>(fullPath, accessMode, true);
  }

  bool
  FileSystem::exists(const Path& fullPath) {
    return sys_pathExists(UTF8::toWide(fullPath.toString()));
  }

  bool
  FileSystem::isFile(const Path& fullPath) {
    WString pathStr = UTF8::toWide(fullPath.toString());
    return sys_pathExists(pathStr) && sys_isFile(pathStr);
  }

  bool
  FileSystem::isDirectory(const Path& fullPath) {
    WString pathStr = UTF8::toWide(fullPath.toString());
    return sys_pathExists(pathStr) && sys_isDirectory(pathStr);
  }

  Path
  FileSystem::getWorkingDirectoryPath() {
    //Check if we can access the current path and it exists
    fileSys::path currentDir = fileSys::current_path();
    if (!fileSys::exists(currentDir)) {
      return Path::BLANK;
    }

    WString result(currentDir.wstring());

    //Append backslash if not present
    if (!result.empty() && result.back() != L'\\') {
      result.append(L"\\");
    }

    return Path(UTF8::fromWide(result));
  }

  Path
  FileSystem::getTempDirectoryPath() {
    fileSys::path tempDir = fileSys::temp_directory_path();
    WString result(tempDir.wstring());

    //Append backslash if not present
    if (!result.empty() && result.back() != L'\\') {
      result.append(L"\\");
    }

    return Path(UTF8::fromWide(result));
  }

  void
  FileSystem::createDir(const Path& fullPath) {
    fileSys::path dirPath = UTF8::toWide(fullPath.toString());

    if (fullPath.isFile()) {
      //If `fullPath` is a file, create all directories up to the parent directory
      fileSys::create_directories(dirPath.parent_path());
    }
    else {
      //If `fullPath` is a directory, create all directories up to the directory
      fileSys::create_directories(dirPath);
    }
  }

  void
  FileSystem::getChildren(const Path& dirPath,
                          Vector<Path>& files,
                          Vector<Path>& directories) {
    WString findPath = UTF8::toWide(dirPath.toString());

    //Check if the path is a directory; if it's a file, return immediately
    if (!fileSys::exists(findPath) || !fileSys::is_directory(findPath)) {
      return;
    }

    //Iterate over the directory entries
    for (const auto& entry : fileSys::directory_iterator(findPath)) {
      String fullPath = UTF8::fromWide(WString(entry.path().wstring()));

      if (entry.is_directory()) {
        directories.push_back(fullPath);
      }
      else if (entry.is_regular_file()) {
        files.push_back(fullPath);
      }
    }
  }

  bool
  FileSystem::iterate(const Path& dirPath,
                      const function<bool(const Path&)>& fileCallback,
                      const function<bool(const Path&)>& dirCallback,
                      bool recursive) {
    WString findPath = UTF8::toWide(dirPath.toString());

    //Check if the path is a directory
    if (!fileSys::exists(findPath) || !fileSys::is_directory(findPath)) {
      return false;
    }

    // Use std::variant to hold either type of iterator
    using DirIter = Variant<fileSys::directory_iterator,
                            fileSys::recursive_directory_iterator>;
    DirIter iterator = recursive ? DirIter(fileSys::recursive_directory_iterator(findPath))
                                 : DirIter(fileSys::directory_iterator(findPath));

    //Iterate over each entry in the directory
    auto visitEntry = [&](auto& it)
    {
      for (const auto& entry : it) {
        Path fullPath = UTF8::fromWide(WString(entry.path().wstring()));

        if (entry.is_directory()) {
          //Handle directories
          if (dirCallback) {
            if (!dirCallback(fullPath)) {
              return false; //Stop iteration if callback returns false
            }
          }
        }
        else if (entry.is_regular_file()) {
          //Handle files
          if (fileCallback) {
            if (!fileCallback(fullPath)) {
              return false;  //Stop iteration if callback returns false
            }
          }
        }
      }
      return true;
    };

    return visit(visitEntry, iterator);
  }

  time_t
  FileSystem::getLastModifiedTime(const Path& fullPath) {
    return sys_getLastModifiedTime(UTF8::toWide(fullPath.toString()));
  }

  void
  FileSystem::copyFile(const Path& from, const Path& to) {
    WString fromPath = UTF8::toWide(from.toString());
    WString toPath = UTF8::toWide(to.toString());
    error_code ec;

    fileSys::copy(fromPath, toPath, fileSys::copy_options::overwrite_existing, ec);
    if (ec) {
      GE_LOG(kWarning,
             FileSystem,
             "Failed to copy file from \"{0}\" to \"{1}\". Error: {2}",
             from.toString(), to.toString(), String(ec.message()));
    }
  }

  void
  FileSystem::removeFile(const Path& path) {
    WString fromPath = UTF8::toWide(path.toString());

    //Check if the path exists and whether it is a directory or a file
    if (fileSys::exists(fromPath)) {
      if (fileSys::is_directory(fromPath)) {
        fileSys::remove_all(fromPath);  //Removes directory and its contents
      }
      else {
        fileSys::remove(fromPath);  //Removes a single file
      }
    }
  }

  void
  FileSystem::moveFile(const Path& oldPath, const Path& newPath) {
    WString fromPath = UTF8::toWide(oldPath.toString());
    WString toPath = UTF8::toWide(newPath.toString());

    //Check if the source path exists before attempting to move it
    if (fileSys::exists(fromPath)) {
      fileSys::rename(fromPath, toPath);  //Moves or renames the file
    }
  }

#endif // USING(GE_USE_GENERIC_FILESYSTEM)

  void
  FileSystem::copy(const Path& oldPath, const Path& newPath, bool overwriteExisting) {
    Stack<tuple<Path, Path>> todo;
    todo.push(make_tuple(oldPath, newPath));

    while (!todo.empty()) {
      auto current = todo.top();
      todo.pop();

      Path sourcePath = get<0>(current);
      if (!FileSystem::exists(sourcePath)) {
        continue;
      }

      bool srcIsFile = FileSystem::isFile(sourcePath);
      Path destinationPath = get<1>(current);
      bool destExists = FileSystem::exists(destinationPath);

      if (destExists) {
        if (FileSystem::isFile(destinationPath)) {
          if (overwriteExisting) {
            FileSystem::remove(destinationPath);
          }
          else {
             GE_LOG(kWarning, FileSystem, "Copy operation failed because "
                    "another file already exists at the new path: \"{0}\"",
                    destinationPath);
            return;
          }
        }
      }

      if (srcIsFile) {
        FileSystem::copyFile(sourcePath, destinationPath);
      }
      else {
        if (!destExists) {
          FileSystem::createDir(destinationPath);
        }

        Vector<Path> files;
        Vector<Path> directories;
        getChildren(destinationPath, files, directories);

        for (auto& file : files) {
          Path fileDestPath = destinationPath;
          fileDestPath.append(Path(file.getTail()));
          todo.push(make_tuple(file, fileDestPath));
        }

        for (auto& dir : directories) {
          Path dirDestPath = destinationPath;
          dirDestPath.append(Path(dir.getTail()));
          todo.push(make_tuple(dir, dirDestPath));
        }
      }
    }
  }

  void
  FileSystem::remove(const Path& fullPath, bool recursively) {
    if (!FileSystem::exists(fullPath)) {
      return;
    }

    if (recursively) {
      Vector<Path> files;
      Vector<Path> directories;

      getChildren(fullPath, files, directories);

      for (auto& file : files) {
        remove(file, false);
      }

      for (auto& dir : directories) {
        remove(dir, true);
      }
    }

    FileSystem::removeFile(fullPath);
  }

  void
  FileSystem::move(const Path& oldPath, const Path& newPath, bool overwriteExisting) {
    if (FileSystem::exists(newPath)) {
      if (overwriteExisting) {
        FileSystem::remove(newPath);
      }
      else {
        GE_LOG(kWarning, FileSystem, "Move operation failed because another "
               "file already exists at the new path: \"{0}\"", newPath);
        return;
      }
    }

    FileSystem::moveFile(oldPath, newPath);
  }

  SPtr<DataStream>
  FileSystem::createAndOpenFile(const Path& fullPath) {
    return ge_shared_ptr_new<FileDataStream>(fullPath, ACCESS_MODE::kWRITE, true);
  }
  
  Path
  FileSystem::getUserDataDirectoryPath() {
#if USING(GE_PLATFORM_WINDOWS)
    ANSICHAR path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path))) {
      Path retPath(path);
      return retPath;
    }
    return Path::BLANK;

#elif USING(GE_PLATFORM_OSX) || USING(GE_PLATFORM_LINUX)
    const char* homeDir = getenv("HOME");
    if (!homeDir) {
      homeDir = getpwuid(getuid())->pw_dir;
    }

    Path baseDir;
#if USING(GE_PLATFORM_OSX)
    baseDir.append(String(homeDir) + "/Library/Application Support/");
#else
    baseDir.append(String(homeDir) + "/.config/");
#endif
    return baseDir;
#else
    return Path::BLANK;
#endif
  }

  static Path s_enginePath;
  static Path s_pluginsPath;
  static Path s_appPath;

  void
  FileSystem::setEnginePath(const Path& path) {
    s_enginePath = path;
  }

  Path
  FileSystem::getEnginePath() {
    GE_ASSERT(!s_enginePath.isEmpty());
    return s_enginePath;
  }

  void
  FileSystem::setPluginsPath(const Path& path) {
    s_pluginsPath = path;
  }

  Path
  FileSystem::getPluginsPath() {
    GE_ASSERT(!s_pluginsPath.isEmpty());
    return s_pluginsPath;
  }

  void
  FileSystem::setAppPath(const Path& path) {
    s_appPath = path;
  }

  Path
  FileSystem::getAppPath() {
    GE_ASSERT(!s_appPath.isEmpty());
    return s_appPath;
  }

}
