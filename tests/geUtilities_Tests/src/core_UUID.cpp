#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <thread>
#include <unordered_set>
#include <vector>

#include "geUUID.h"
#include "gePath.h"

using namespace geEngineSDK;

static bool isLowerHexChar(char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

static void requireUuidStringFormat(const String& s) {
  REQUIRE(s.size() == 36);

  // Hyphens at 8,13,18,23
  REQUIRE(s[8] == '-');
  REQUIRE(s[13] == '-');
  REQUIRE(s[18] == '-');
  REQUIRE(s[23] == '-');

  // All other chars are lower hex
  for (size_t i = 0; i < s.size(); ++i) {
    if (i == 8 || i == 13 || i == 18 || i == 23) continue;
    REQUIRE(isLowerHexChar(s[i]));
  }
}

static String makeKnownUuidString() {
  // full lowercase, canonical hyphen positions
  return "00112233-4455-6677-8899-aabbccddeeff";
}

TEST_CASE("UUID: default constructor produces EMPTY (all zeros)", "[UUID]") {
  UUID u;
  REQUIRE(u.empty());
  REQUIRE(u == UUID::EMPTY);
  REQUIRE_FALSE(u != UUID::EMPTY);

  // toString of empty should be all zeros with hyphens
  const String s = u.toString();
  requireUuidStringFormat(s);
  REQUIRE(s == "00000000-0000-0000-0000-000000000000");
}

TEST_CASE("UUID: ctor(uint32,uint32,uint32,uint32) + equality/inequality", "[UUID]") {
  UUID a(1, 2, 3, 4);
  UUID b(1, 2, 3, 4);
  UUID c(1, 2, 3, 5);

  REQUIRE(a == b);
  REQUIRE_FALSE(a != b);

  REQUIRE(a != c);
  REQUIRE_FALSE(a == c);
}

TEST_CASE("UUID: operator< implements lexicographical compare across 4 uint32 words", "[UUID]") {
  UUID a(0, 0, 0, 0);
  UUID b(0, 0, 0, 1);
  UUID c(0, 0, 1, 0);
  UUID d(0, 1, 0, 0);
  UUID e(1, 0, 0, 0);

  REQUIRE(a < b);
  REQUIRE(b < c);
  REQUIRE(c < d);
  REQUIRE(d < e);

  // Equal is not less-than
  UUID x(7, 8, 9, 10);
  UUID y(7, 8, 9, 10);
  REQUIRE_FALSE(x < y);
  REQUIRE_FALSE(y < x);
}

TEST_CASE("UUID: string ctor parses canonical format and round-trips via toString()", "[UUID]") {
  const String src = makeKnownUuidString();
  UUID u(src);

  REQUIRE_FALSE(u.empty());

  const String s = u.toString();
  requireUuidStringFormat(s);

  // Round-trip
  UUID u2(s);
  REQUIRE(u == u2);
  REQUIRE(u2.toString() == s);
}

TEST_CASE("UUID: string ctor with size < 36 yields empty UUID", "[UUID]") {
  UUID u1(String(""));                 REQUIRE(u1.empty());
  UUID u2(String("123"));              REQUIRE(u2.empty());
  UUID u3(String("00000000-0000"));    REQUIRE(u3.empty());
}

TEST_CASE("UUID: toString always uses lowercase hex digits", "[UUID]") {
  // Use some values that exercise letters a-f in multiple nibbles
  UUID u(0xabcdef01u, 0x2345fedcu, 0x0badc0deu, 0x13579bdfu);

  const String s = u.toString();
  requireUuidStringFormat(s);

  // Ensure no uppercase anywhere
  for (char c : s) {
    REQUIRE(!(c >= 'A' && c <= 'F'));
  }

  // Parsing back yields same UUID
  UUID u2(s);
  REQUIRE(u == u2);
}

TEST_CASE("UUID: std::hash works (unordered_set)", "[UUID]") {
  std::unordered_set<UUID> set;
  UUID a(1, 2, 3, 4);
  UUID b(1, 2, 3, 4);
  UUID c(1, 2, 3, 5);

  set.insert(a);
  REQUIRE(set.find(b) != set.end());
  REQUIRE(set.find(c) == set.end());
}

TEST_CASE("UUID: UUIDGenerator::generateRandom returns non-empty and (very likely) unique values", "[UUID]") {
  UUID a = UUIDGenerator::generateRandom();
  UUID b = UUIDGenerator::generateRandom();

  REQUIRE_FALSE(a.empty());
  REQUIRE_FALSE(b.empty());

  // Extremely unlikely collision; if this ever flakes, we can relax it.
  REQUIRE(a != b);

  requireUuidStringFormat(a.toString());
  requireUuidStringFormat(b.toString());
}

TEST_CASE("UUID: ctor(Path) is deterministic for same file content/mtime and changes when file mtime changes", "[UUID][File]") {
  // Build a temp file path under current working dir (tests typically run in build dir).
  const char* fname = "geUUID_test_temp_file.txt";

  // Create/overwrite file
  {
    std::ofstream f(fname, std::ios::binary | std::ios::trunc);
    f << "hello";
  }

  Path p(fname);

  UUID u1(p);
  UUID u2(p);
  REQUIRE_FALSE(u1.empty());
  REQUIRE(u1 == u2); // same file, same mtime -> same UUID

  // Modify file to bump last-modified time.
  // Your implementation salts with FileSystem::getLastModifiedTime(filePath).
  // Many platforms have 1-second resolution for mtime, so sleep >= 2s.
  std::this_thread::sleep_for(std::chrono::seconds(2));

  {
    std::ofstream f(fname, std::ios::binary | std::ios::app);
    f << "!";
  }

  UUID u3(p);
  REQUIRE_FALSE(u3.empty());
  REQUIRE(u3 != u1);

  // Cleanup best-effort (ignore failure on locked files)
  std::remove(fname);
}
