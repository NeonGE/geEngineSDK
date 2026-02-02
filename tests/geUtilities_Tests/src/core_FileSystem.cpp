#include <catch_amalgamated.hpp>

#include "geFileSystem.h"
#include "geDataStream.h"

#include <chrono>

using namespace geEngineSDK;

namespace
{
  String uniqueSuffix()
  {
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    const auto us = std::chrono::duration_cast<std::chrono::microseconds>(now).count();
    auto usStr = std::to_string(static_cast<long long>(us));
    return String("u").append(usStr.c_str());
  }

  Path makeTempRoot(const char* folderName)
  {
    Path base = FileSystem::getTempDirectoryPath();
    Path root = base;
    root.append(String(folderName) + "_" + uniqueSuffix() + "/");
    FileSystem::createDir(root);
    return root;
  }

  void writeExactFile(const Path& p, const void* data, SIZE_T bytes)
  {
    auto s = FileSystem::createAndOpenFile(p);
    REQUIRE(s);
    REQUIRE(s->write(data, bytes) == bytes);
    s->close();
  }

  String readAllBytesAsString(const Path& p)
  {
    auto s = FileSystem::openFile(p, true);
    REQUIRE(s);

    String out;
    out.resize(static_cast<size_t>(s->size()));
    if (!out.empty())
      REQUIRE(s->read(out.data(), out.size()) == out.size());

    s->close();
    return out;
  }
}

TEST_CASE("FileSystem: exists/isFile/isDirectory/createDir", "[FileSystem]")
{
  const Path root = makeTempRoot("geFileSystemTests_root");
  REQUIRE(FileSystem::exists(root));
  REQUIRE(FileSystem::isDirectory(root));
  REQUIRE_FALSE(FileSystem::isFile(root));

  // createDir with a file path should create parent dirs (per implementation)
  Path nestedFile = root;
  nestedFile.append("a/b/c/hello.bin");
  FileSystem::createDir(nestedFile);

  Path nestedDir = root;
  nestedDir.append("a/b/c/");
  REQUIRE(FileSystem::exists(nestedDir));
  REQUIRE(FileSystem::isDirectory(nestedDir));

  FileSystem::remove(root, true);
  REQUIRE_FALSE(FileSystem::exists(root));
}

TEST_CASE("FileSystem: openFile returns nullptr for missing file", "[FileSystem][openFile]")
{
  const Path root = makeTempRoot("geFileSystemTests_missing");
  Path missing = root;
  missing.append("does_not_exist.bin");

  auto s = FileSystem::openFile(missing, true);
  REQUIRE(s == nullptr);

  FileSystem::remove(root, true);
}

TEST_CASE("FileSystem: createAndOpenFile + openFile + getFileSize", "[FileSystem][IO]")
{
  const Path root = makeTempRoot("geFileSystemTests_io");

  Path file = root;
  file.append("payload.bin");

  const char payload[] = "ABCDEF";
  writeExactFile(file, payload, sizeof(payload) - 1);

  REQUIRE(FileSystem::exists(file));
  REQUIRE(FileSystem::isFile(file));
  REQUIRE(FileSystem::getFileSize(file) == (sizeof(payload) - 1));

  const String roundtrip = readAllBytesAsString(file);
  REQUIRE(roundtrip == "ABCDEF");

  FileSystem::remove(root, true);
}

TEST_CASE("FileSystem: copyFile / moveFile / copy / move", "[FileSystem][CopyMove]")
{
  const Path root = makeTempRoot("geFileSystemTests_copy_move");

  Path src = root;
  src.append("src.txt");
  const char payload[] = "Hello";
  writeExactFile(src, payload, sizeof(payload) - 1);

  // copyFile
  Path copied = root;
  copied.append("copied.txt");
  FileSystem::copy(src, copied);
  REQUIRE(FileSystem::exists(copied));
  REQUIRE(FileSystem::getFileSize(copied) == 5);

  // moveFile
  Path moved = root;
  moved.append("moved.txt");
  FileSystem::move(copied, moved);
  REQUIRE_FALSE(FileSystem::exists(copied));
  REQUIRE(FileSystem::exists(moved));

  FileSystem::remove(root, true);
}

TEST_CASE("FileSystem: getChildren returns files and dirs", "[FileSystem][Children]")
{
  const Path root = makeTempRoot("geFileSystemTests_children");

  Path d1 = root; d1.append("d1/");
  Path d2 = root; d2.append("d2/");
  FileSystem::createDir(d1);
  FileSystem::createDir(d2);

  Path f1 = root; f1.append("f1.txt");
  Path f2 = root; f2.append("f2.txt");
  const char x = 'x';
  writeExactFile(f1, &x, 1);
  writeExactFile(f2, &x, 1);

  Vector<Path> files;
  Vector<Path> dirs;
  FileSystem::getChildren(root, files, dirs);

  // We don't assume order, but we assume at least what we created exists.
  auto containsPathEnding = [](const Vector<Path>& arr, const String& suffix)
    {
      for (auto& p : arr)
        if (p.toString().find(suffix) != String::npos) return true;
      return false;
    };

  REQUIRE(containsPathEnding(dirs, "d1"));
  REQUIRE(containsPathEnding(dirs, "d2"));
  REQUIRE(containsPathEnding(files, "f1.txt"));
  REQUIRE(containsPathEnding(files, "f2.txt"));

  FileSystem::remove(root, true);
}

TEST_CASE("FileSystem: iterate recursive collects nodes", "[FileSystem][Iterate]")
{
  const Path root = makeTempRoot("geFileSystemTests_iterate");

  Path sub = root; sub.append("sub/");
  FileSystem::createDir(sub);

  Path f1 = root; f1.append("a.txt");
  Path f2 = sub;  f2.append("b.txt");
  const char c = 'c';
  writeExactFile(f1, &c, 1);
  writeExactFile(f2, &c, 1);

  int fileCount = 0;
  int dirCount = 0;

  const bool ok = FileSystem::iterate(
    root,
    [&](const Path& p) {
      ++fileCount;
      return true; // continue
    },
    [&](const Path& p) {
      ++dirCount;
      return true; // continue
    },
    true // recursive
  );

  REQUIRE(ok);
  REQUIRE(fileCount >= 2);
  REQUIRE(dirCount >= 1);

  FileSystem::remove(root, true);
}

TEST_CASE("FileSystem: getLastModifiedTime non-zero for real file", "[FileSystem][Time]")
{
  const Path root = makeTempRoot("geFileSystemTests_time");

  Path f = root; f.append("time.bin");
  const uint8 b = 7;
  writeExactFile(f, &b, 1);

  const time_t t = FileSystem::getLastModifiedTime(f);
  REQUIRE(t != 0);

  FileSystem::remove(root, true);
}

TEST_CASE("FileSystem: working/temp/user data directories are sane", "[FileSystem][Paths]")
{
  const Path wd = FileSystem::getWorkingDirectoryPath();
  REQUIRE_FALSE(wd.isEmpty());
  REQUIRE(FileSystem::exists(wd));
  REQUIRE(FileSystem::isDirectory(wd));

  const Path td = FileSystem::getTempDirectoryPath();
  REQUIRE_FALSE(td.isEmpty());
  REQUIRE(FileSystem::exists(td));
  REQUIRE(FileSystem::isDirectory(td));

  const Path ud = FileSystem::getUserDataDirectoryPath();
  // On some platforms it might not exist yet, but it should not be blank.
  REQUIRE_FALSE(ud.isEmpty());
}

TEST_CASE("FileSystem: set/get Engine/Plugins/App paths", "[FileSystem][Config]")
{
  const Path root = makeTempRoot("geFileSystemTests_config");

  Path engine = root;  engine.append("Engine/");
  Path plugins = root; plugins.append("Plugins/");
  Path app = root;     app.append("App/");
  FileSystem::createDir(engine);
  FileSystem::createDir(plugins);
  FileSystem::createDir(app);

  FileSystem::setEnginePath(engine);
  FileSystem::setPluginsPath(plugins);
  FileSystem::setAppPath(app);

  REQUIRE(FileSystem::getEnginePath().toString() == engine.toString());
  REQUIRE(FileSystem::getPluginsPath().toString() == plugins.toString());
  REQUIRE(FileSystem::getAppPath().toString() == app.toString());

  FileSystem::remove(root, true);
}
