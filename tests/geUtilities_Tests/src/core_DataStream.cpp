#include <catch2/catch_test_macros.hpp>

#include "geDataStream.h"
#include "geFileSystem.h"
#include "geUnicode.h"

#include <cstring>
#include <chrono>

using namespace geEngineSDK;

namespace
{
  // Small helper to write/read POD safely
  template<class T>
  void writePod(DataStream& s, const T& v)
  {
    const SIZE_T written = s.write(&v, sizeof(T));
    REQUIRE(written == sizeof(T));
  }

  template<class T>
  T readPod(DataStream& s)
  {
    T out{};
    const SIZE_T read = s.read(&out, sizeof(T));
    REQUIRE(read == sizeof(T));
    return out;
  }

  // MemoryDataStream has a fixed size; for string tests we must allocate
  // exactly the bytes we intend to write (otherwise getAsString() would
  // read trailing zeroes too).
  static SIZE_T utf8BytesForWriteString(const String& s)
  {
    // BOM (3) + payload
    return 3 + static_cast<SIZE_T>(s.size());
  }

  static SIZE_T utf16BytesForWriteString(const String& utf8)
  {
    // BOM (2) + UTF-16 payload bytes
    const U16String u16 = UTF8::toUTF16(utf8);
    return 2 + static_cast<SIZE_T>(u16.size() * sizeof(char16_t));
  }
}

TEST_CASE("MemoryDataStream: basic write/read/seek/tell/eof", "[DataStream][MemoryDataStream]")
{
  const uint8 payload[] = { 1, 2, 3, 4, 5, 6, 7 };
  MemoryDataStream s(sizeof(payload));

  REQUIRE(s.isReadable());
  REQUIRE(s.isWriteable());
  REQUIRE_FALSE(s.isFile());
  REQUIRE(s.size() == sizeof(payload));
  REQUIRE(s.tell() == 0);

  REQUIRE(s.write(payload, sizeof(payload)) == sizeof(payload));
  REQUIRE(s.tell() == sizeof(payload));
  REQUIRE(s.isEOF()); // at end

  s.seek(0);
  REQUIRE(s.tell() == 0);
  REQUIRE_FALSE(s.isEOF());

  uint8 out[sizeof(payload)]{};
  REQUIRE(s.read(out, sizeof(out)) == sizeof(out));
  REQUIRE(std::memcmp(out, payload, sizeof(payload)) == 0);
  REQUIRE(s.isEOF());
}

TEST_CASE("MemoryDataStream: partial read clamps at end", "[DataStream][MemoryDataStream]")
{
  const uint8 payload[] = { 9, 8, 7, 6 };
  MemoryDataStream s(sizeof(payload));
  REQUIRE(s.write(payload, sizeof(payload)) == sizeof(payload));

  s.seek(2);
  uint8 out[16]{};
  const SIZE_T read = s.read(out, 16);
  REQUIRE(read == 2);
  REQUIRE(out[0] == 7);
  REQUIRE(out[1] == 6);
  REQUIRE(s.isEOF());
}

TEST_CASE("MemoryDataStream: skip and seek boundaries", "[DataStream][MemoryDataStream]")
{
  MemoryDataStream s(10);

  REQUIRE(s.tell() == 0);
  s.skip(3);
  REQUIRE(s.tell() == 3);

  s.seek(10);
  REQUIRE(s.tell() == 10);
  REQUIRE(s.isEOF());

  s.seek(0);
  REQUIRE_FALSE(s.isEOF());
}

TEST_CASE("DataStream operator>> reads POD", "[DataStream][MemoryDataStream]")
{
  const int32 v = 0x12345678;
  MemoryDataStream s(sizeof(v));

  writePod(s, v);

  s.seek(0);
  int32 out = 0;
  s >> out;
  REQUIRE(out == v);
}

TEST_CASE("DataStream: writeString/getAsString UTF-8 (BOM)", "[DataStream][String][UTF8]")
{
  const String msg = U8STRING("Hello ñáéíóú ✓");
  MemoryDataStream s(utf8BytesForWriteString(msg));

  s.writeString(msg, STRING_ENCODING::kUTF8);

  // getAsString() seeks internally
  const String roundtrip = s.getAsString();
  REQUIRE(roundtrip == msg);
}

TEST_CASE("UTF8: toUTF16/fromUTF16 roundtrip", "[Unicode][UTF16]")
{
  const String msg =
    "Hello "
    "\xC3\xB1" "\xC3\xA1" "\xC3\xA9" "\xC3\xAD" "\xC3\xB3" "\xC3\xBA"
    " "
    "\xE2\x9C\x93";

  const U16String u16 = UTF8::toUTF16(msg);

  REQUIRE(std::find(u16.begin(), u16.end(), static_cast<char16_t>(0x2713)) != u16.end());
  REQUIRE(UTF8::fromUTF16(u16) == msg);
}

static String hexBytes(const String& s)
{
  String out;
  for (unsigned char c : s)
  {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "%02X ", (unsigned)c);
    out += buf;
  }
  return out;
}

TEST_CASE("DataStream: writeString/getAsString UTF-16LE (BOM)", "[DataStream][String][UTF16]")
{
  const String msg =
    "Hello "
    "\xC3\xB1" "\xC3\xA1" "\xC3\xA9" "\xC3\xAD" "\xC3\xB3" "\xC3\xBA"
    " "
    "\xE2\x9C\x93";

  const U16String u16 = UTF8::toUTF16(msg);
  const SIZE_T bytesNeeded = 2 + static_cast<SIZE_T>(u16.size() * sizeof(char16_t));

  MemoryDataStream s(bytesNeeded);

  s.writeString(msg, STRING_ENCODING::kUTF16);

  // IMPORTANTE: valida que sí escribió todo
  REQUIRE(s.tell() == bytesNeeded);

  const String roundtrip = s.getAsString();
  REQUIRE(roundtrip == msg);
}

TEST_CASE("DataStream: writeString(WString) roundtrips UTF-8 and UTF-16", "[DataStream][WString]")
{
  const WString wmsg = UTF8::toWide(reinterpret_cast<const char*>("Wide ✓ ñ"));
  {
    // UTF-8: BOM(3) + UTF-8 payload
    const String u8 = UTF8::fromWide(wmsg);
    MemoryDataStream s(utf8BytesForWriteString(u8));

    s.writeString(wmsg, STRING_ENCODING::kUTF8);
    REQUIRE(s.getAsString() == u8);
    REQUIRE(s.getAsWString() == wmsg);
  }

  {
    const String u8 = UTF8::fromWide(wmsg);
    MemoryDataStream s(utf16BytesForWriteString(u8));

    s.writeString(wmsg, STRING_ENCODING::kUTF16);
    REQUIRE(s.getAsString() == u8);
    REQUIRE(s.getAsWString() == wmsg);
  }
}

TEST_CASE("MemoryDataStream: clone(copyData=true) deep-copies", "[DataStream][Clone]")
{
  const uint8 payload[] = { 1, 2, 3, 4 };
  MemoryDataStream s(sizeof(payload));
  REQUIRE(s.write(payload, sizeof(payload)) == sizeof(payload));

  s.seek(0);
  auto clone = s.clone(true);
  REQUIRE(clone);
  REQUIRE_FALSE(clone->isFile());
  REQUIRE(clone->size() == s.size());

  // Modify clone, original should remain
  clone->seek(0);
  const uint8 nine = 9;
  REQUIRE(clone->write(&nine, 1) == 1);

  s.seek(0);
  uint8 origFirst = 0;
  REQUIRE(s.read(&origFirst, 1) == 1);
  REQUIRE(origFirst == 1);
}

TEST_CASE("MemoryDataStream: clone(copyData=false) shares buffer", "[DataStream][Clone]")
{
  const uint8 payload[] = { 1, 2, 3, 4 };
  MemoryDataStream s(sizeof(payload));
  REQUIRE(s.write(payload, sizeof(payload)) == sizeof(payload));

  auto clone = s.clone(false);
  REQUIRE(clone);
  REQUIRE_FALSE(clone->isFile());
  REQUIRE(clone->size() == s.size());

  // Overwrite first byte in clone, should reflect in original
  clone->seek(0);
  const uint8 nine = 9;
  REQUIRE(clone->write(&nine, 1) == 1);

  s.seek(0);
  uint8 origFirst = 0;
  REQUIRE(s.read(&origFirst, 1) == 1);
  REQUIRE(origFirst == 9);
}

TEST_CASE("MemoryDataStream: close nulls pointer", "[DataStream][Close]")
{
  MemoryDataStream s(8);
  REQUIRE(s.getPtr() != nullptr);

  s.close();
  REQUIRE(s.getPtr() == nullptr);
}

TEST_CASE("FileDataStream: write then read (reopen), skip/seek/tell/eof", "[DataStream][FileDataStream]")
{
  // Arrange paths in temp directory
  const Path temp = FileSystem::getTempDirectoryPath();
  Path dir = temp;
  dir.append("geDataStream_FileDataStream_Tests/");
  FileSystem::createDir(dir);

  Path filePath = dir;
  filePath.append("file_stream_test.bin");

  // Write
  {
    auto out = FileSystem::createAndOpenFile(filePath);
    REQUIRE(out);
    REQUIRE(out->isFile());
    REQUIRE_FALSE(out->isReadable()); // write-only by contract here
    REQUIRE(out->isWriteable());

    const uint8 payload[] = { 10, 11, 12, 13, 14 };
    REQUIRE(out->write(payload, sizeof(payload)) == sizeof(payload));
    out->close();
  }

  // Read back
  {
    auto in = FileSystem::openFile(filePath, true);
    REQUIRE(in);
    REQUIRE(in->isFile());
    REQUIRE(in->isReadable());
    REQUIRE_FALSE(in->isWriteable());

    REQUIRE(in->tell() == 0);
    uint8 b = 0;
    REQUIRE(in->read(&b, 1) == 1);
    REQUIRE(b == 10);

    in->skip(2); // now at 3
    REQUIRE(in->tell() == 3);

    uint8 out[2]{};
    REQUIRE(in->read(out, 2) == 2);
    REQUIRE(out[0] == 13);
    REQUIRE(out[1] == 14);
    REQUIRE(in->isEOF());

    in->close();
  }

  // Cleanup
  FileSystem::remove(dir, true);
}
