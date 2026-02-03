#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_get_random_seed.hpp>
#include <catch2/catch_approx.hpp>

#include <random>
#include <limits>
#include <locale>

#include "geString.h"
#include "geStringID.h"
#include "geUnicode.h"

//#include "geStringFormat.h"

using namespace geEngineSDK;

static std::mt19937&
rng() {
  static auto seed = Catch::getSeed();
  static std::mt19937 r(seed);
  return r;
}

static int32
randInt(int32 lo, int32 hi) {
  std::uniform_int_distribution<int32> dist(lo, hi);
  return dist(rng());
}

static String
randAsciiString(int32 minLen, int32 maxLen) {
  const int32 len = randInt(minLen, maxLen);
  String s;
  s.reserve((SIZE_T)len);

  //Visible ASCII excluding control; incluye espacio.
  std::uniform_int_distribution<int32> chDist(32, 126);
  for (int32 i = 0; i < len; ++i) {
    s.push_back((ANSICHAR)chDist(rng()));
  }

  return s;
}

//Inserts random delimiters into the input string at random positions.
static String
sprinkleDelims(const String& in, const String& delims, int32 extra = 5) {
  String out = in;
  for (int32 i = 0; i < extra; ++i) {
    if (out.empty()) break;
    const int32 pos = randInt(0, (int32)out.size() - 1);
    out.insert((SIZE_T)pos, 1, delims[(SIZE_T)randInt(0, (int32)delims.size() - 1)]);
  }
  return out;
}

// -----------------------------------------------------------------------------
// toString / toWString basics
// -----------------------------------------------------------------------------
TEST_CASE("toString/toWString: basic narrow/wide roundtrip ASCII", "[Text][String][Convert]") {
  String a = "Hello 123 !?";
  WString w = toWString(a);
  String b = toString(w);

  REQUIRE(a == b);

  //empty
  REQUIRE(toString(toWString(String(""))) == "");
  REQUIRE(toWString(toString(WString(L""))) == L"");
}

TEST_CASE("toString(bool): true/false and yes/no", "[Text][String][Convert]") {
  REQUIRE(toString(true, false) == "true");
  REQUIRE(toString(false, false) == "false");

  REQUIRE(toString(true, true) == "yes");
  REQUIRE(toString(false, true) == "no");

  REQUIRE(toWString(true, false) == L"true");
  REQUIRE(toWString(false, false) == L"false");

  REQUIRE(toWString(true, true) == L"yes");
  REQUIRE(toWString(false, true) == L"no");
}

// -----------------------------------------------------------------------------
// parse* + isNumber (narrow & wide) + edge cases
// -----------------------------------------------------------------------------
TEST_CASE("parseInt/parseUnsignedInt/parseInt64: valid and invalid cases", "[Text][Parse]") {
  REQUIRE(parseInt(String("0")) == 0);
  REQUIRE(parseInt(String("-123")) == -123);
  REQUIRE(parseUnsignedInt(String("123")) == 123u);
  REQUIRE(parseInt64(String("-922337203685477580")) == (int64)-922337203685477580LL);
  REQUIRE(parseUnsignedInt64(String("1844674407370955161")) == (uint64)1844674407370955161ULL);

  //leading/trailing whitespace
  REQUIRE(parseInt(String("   42   ")) == 42);
  REQUIRE(parseFloat(String("   3.5   ")) == Catch::Approx(3.5f));

  //invalid: returns defaultValue
  REQUIRE(parseInt(String("abc"), 7) == 7);
  REQUIRE(parseFloat(String("nope"), 1.25f) == Catch::Approx(1.25f));

  //unsigned negative should fail and return default
  REQUIRE(parseUnsignedInt(String("-1"), 9u) == 9u);
}

TEST_CASE("parseInt strict parsing", "[Text][Parse]") {
  REQUIRE(parseInt("123", 7) == 123);
  REQUIRE(parseInt("  10  ", 7) == 10);

  REQUIRE(parseInt("abc", 7) == 7);
  REQUIRE(parseInt("123abc", 7) == 7);
  REQUIRE(parseInt("abc123", 7) == 7);
  REQUIRE(parseInt("12 3", 7) == 7);
}


TEST_CASE("parseBool: prefixes and case-insensitive", "[Text][Parse][Bool]") {
  REQUIRE(parseBool(String("true")) == true);
  REQUIRE(parseBool(String("TRUE")) == true);
  REQUIRE(parseBool(String("TrUe")) == true);

  REQUIRE(parseBool(String("yes")) == true);
  REQUIRE(parseBool(String("YES")) == true);

  REQUIRE(parseBool(String("1")) == true);

  REQUIRE(parseBool(String("trueblah")) == true);
  REQUIRE(parseBool(String("yes please")) == true);

  //false cases
  REQUIRE(parseBool(String("false")) == false);
  REQUIRE(parseBool(String("0")) == false);
  REQUIRE(parseBool(String("no")) == false);
  REQUIRE(parseBool(String("nope")) == false);

  //default
  REQUIRE(parseBool(String("???"), true) == true);
  REQUIRE(parseBool(String("???"), false) == false);
}

TEST_CASE("isNumber: basic forms", "[Text][Parse][isNumber]") {
  //Integers
  REQUIRE(isNumber(String("0")));
  REQUIRE(isNumber(String("-1")));
  REQUIRE(isNumber(String("123456")));

  //Floats
  REQUIRE(isNumber(String("1.0")));
  REQUIRE(isNumber(String("-0.25")));
  REQUIRE(isNumber(String("3.14159")));

  //Leading/trailing spaces
  //isNumber should be strict here and return false
  REQUIRE_FALSE(isNumber(String("  10  ")));

  //Invalid
  REQUIRE_FALSE(isNumber(String("")));
  REQUIRE_FALSE(isNumber(String("abc")));
  REQUIRE_FALSE(isNumber(String("1.2.3")));
  REQUIRE_FALSE(isNumber(String("--1")));
}

//Wide versions (same set)
TEST_CASE("parse*/isNumber for WString", "[Text][Parse][Wide]") {
  REQUIRE(parseInt(WString(L" -10 ")) == -10);
  REQUIRE(parseUnsignedInt(WString(L"10")) == 10u);
  REQUIRE(parseFloat(WString(L"2.5")) == Catch::Approx(2.5f));
  REQUIRE(parseBool(WString(L"YES")) == true);

  REQUIRE(isNumber(WString(L"123")));
  REQUIRE(isNumber(WString(L"-0.5")));
  REQUIRE_FALSE(isNumber(WString(L"wat")));
}

// -----------------------------------------------------------------------------
// StringUtil: trim (whitespace and custom delims), split, tokenise
// -----------------------------------------------------------------------------
TEST_CASE("StringUtil::trim: whitespace variations", "[Text][StringUtil][Trim]")
{
  String s = "\t  hello world \n\r";
  StringUtil::trim(s);
  REQUIRE(s == "hello world");

  // left only
  String l = "   abc  ";
  StringUtil::trim(l, true, false);
  REQUIRE(l == "abc  ");

  // right only
  String r = "   abc  ";
  StringUtil::trim(r, false, true);
  REQUIRE(r == "   abc");

  // empty stays empty
  String e = "";
  StringUtil::trim(e);
  REQUIRE(e.empty());
}

TEST_CASE("StringUtil::trim: custom delims", "[Text][StringUtil][Trim]")
{
  String s = "---==hello==---";
  StringUtil::trim(s, String("-="));
  REQUIRE(s == "hello");

  // only right
  String s2 = "###hi###";
  StringUtil::trim(s2, String("#"), false, true);
  REQUIRE(s2 == "###hi");
}

TEST_CASE("StringUtil::split: multiple delims, collapse runs, maxSplits", "[Text][StringUtil][Split]")
{
  String s = "  a\tb\nc   d  ";
  auto toks = StringUtil::split(s); // default delims whitespace
  REQUIRE(toks.size() == 4);
  REQUIRE(toks[0] == "a");
  REQUIRE(toks[1] == "b");
  REQUIRE(toks[2] == "c");
  REQUIRE(toks[3] == "d");

  // custom delims
  String s2 = "one,two;three,,four";
  auto t2 = StringUtil::split(s2, ",;");
  // Nota: splitInternal salta runs de delims y no genera tokens vacíos.
  REQUIRE(t2.size() == 4);
  REQUIRE(t2[0] == "one");
  REQUIRE(t2[1] == "two");
  REQUIRE(t2[2] == "three");
  REQUIRE(t2[3] == "four");

  // maxSplits
  String s3 = "a b c d";
  auto t3 = StringUtil::split(s3, " ", 2);
  REQUIRE(t3.size() == 3);
  REQUIRE(t3[0] == "a");
  REQUIRE(t3[1] == "b");
  REQUIRE(t3[2] == "c d");
}

TEST_CASE("StringUtil::tokenise: quoted tokens include spaces/delims", "[Text][StringUtil][Tokenise]")
{
  String s = "one \"two three\" four";
  auto t = StringUtil::tokenise(s);
  REQUIRE(t.size() == 3);
  REQUIRE(t[0] == "one");
  REQUIRE(t[1] == "two three");
  REQUIRE(t[2] == "four");

  // Delims inside quotes should be kept
  String s2 = "a \"b,c;d\" e";
  auto t2 = StringUtil::tokenise(s2, " ,;", "\"");
  REQUIRE(t2.size() == 3);
  REQUIRE(t2[0] == "a");
  REQUIRE(t2[1] == "b,c;d");
  REQUIRE(t2[2] == "e");

  // maxSplits with tokenise
  String s3 = "a \"b c\" d e";
  auto t3 = StringUtil::tokenise(s3, " ", "\"", 2);
  REQUIRE(t3.size() == 3);
  REQUIRE(t3[0] == "a");
  REQUIRE(t3[1] == "b c");
  REQUIRE(t3[2] == "d e");
}

// -----------------------------------------------------------------------------
// StringUtil: case conversion, startsWith/endsWith, compare
// -----------------------------------------------------------------------------
TEST_CASE("StringUtil::toLowerCase/toUpperCase", "[Text][StringUtil][Case]")
{
  String s = "AbC_123!";
  StringUtil::toLowerCase(s);
  REQUIRE(s == "abc_123!");

  StringUtil::toUpperCase(s);
  REQUIRE(s == "ABC_123!");
}

TEST_CASE("StringUtil::startsWith/endsWith: case options and edge cases", "[Text][StringUtil][PrefixSuffix]")
{
  REQUIRE(StringUtil::startsWith("HelloWorld", "hello", true));   // lowerCase=true -> compare lowercased
  REQUIRE_FALSE(StringUtil::startsWith("HelloWorld", "hello", false)); // case sensitive if false

  REQUIRE(StringUtil::endsWith("HelloWorld", "WORLD", true));
  REQUIRE_FALSE(StringUtil::endsWith("HelloWorld", "WORLD", false));

  // Empty pattern behavior: should be true typically
  REQUIRE(StringUtil::startsWith("abc", "", true));
  REQUIRE(StringUtil::endsWith("abc", "", true));

  // Pattern longer than string
  REQUIRE_FALSE(StringUtil::startsWith("ab", "abc", true));
  REQUIRE_FALSE(StringUtil::endsWith("ab", "abc", true));
}

TEST_CASE("StringUtil::compare: caseSensitive false", "[Text][StringUtil][Compare]")
{
  REQUIRE(StringUtil::compare(String("abc"), String("abc"), true) == 0);
  REQUIRE(StringUtil::compare(String("abc"), String("AbC"), true) != 0);
  REQUIRE(StringUtil::compare(String("abc"), String("AbC"), false) == 0);

  REQUIRE(StringUtil::compare(String("abc"), String("abd"), false) < 0);
  REQUIRE(StringUtil::compare(String("abd"), String("abc"), false) > 0);

  // prefix compare
  REQUIRE(StringUtil::compare(String("ab"), String("abc"), false) < 0);
  REQUIRE(StringUtil::compare(String("abc"), String("ab"), false) > 0);
}

// -----------------------------------------------------------------------------
// StringUtil: match (wildcards), replaceAll
// -----------------------------------------------------------------------------
TEST_CASE("StringUtil::match: basic wildcard patterns", "[Text][StringUtil][Match]")
{
  REQUIRE(StringUtil::match("hello", "hello", true));
  REQUIRE(StringUtil::match("hello", "h*o", true));
  REQUIRE(StringUtil::match("hello", "*", true));
  REQUIRE(StringUtil::match("hello", "he*", true));
  REQUIRE(StringUtil::match("hello", "*lo", true));

  REQUIRE_FALSE(StringUtil::match("hello", "he*z", true));
  REQUIRE_FALSE(StringUtil::match("hello", "HELLO", true));
  REQUIRE(StringUtil::match("hello", "HELLO", false)); // case-insensitive

  // edge cases
  REQUIRE(StringUtil::match("", "", true));
  REQUIRE(StringUtil::match("", "*", true));
  REQUIRE_FALSE(StringUtil::match("", "a*", true));
}

TEST_CASE("StringUtil::replaceAll: multiple occurrences and no-op", "[Text][StringUtil][ReplaceAll]")
{
  REQUIRE(StringUtil::replaceAll("aaaa", "aa", "b") == "bb");
  REQUIRE(StringUtil::replaceAll("one two one", "one", "1") == "1 two 1");

  // replaceWhat not found
  REQUIRE(StringUtil::replaceAll("abc", "x", "y") == "abc");

  // replaceWhat empty: depende implementación (puede loop infinito si no se protege)
  // Si tu implementación no maneja esto, este test te lo va a revelar.
  // Yo espero que se comporte como no-op:
  REQUIRE(StringUtil::replaceAll("abc", "", "x") == "abc");
}

// -----------------------------------------------------------------------------
// StringFormat / StringUtil::format: IDs, escapes, bounds
// -----------------------------------------------------------------------------
TEST_CASE("StringFormat: basic formatting and repeated IDs", "[Text][Format]")
{
  REQUIRE(StringFormat::format("a{0}b", 123) == "a123b");
  REQUIRE(StringFormat::format("{0}-{0}-{1}", "x", 7) == "x-x-7");

  // Unknown/too high IDs should remain literal
  // (por doc: max ID 19; {20} no se reconoce como ID)
  REQUIRE(StringFormat::format("hi{20}", 1) == "hi{20}");
}

TEST_CASE("StringFormat: escaping brackets with double brackets", "[Text][Format][Escape]") {
  REQUIRE(StringFormat::format("{{0}}", 5) == "{0}");
  REQUIRE(StringFormat::format("x{{0}}y", 5) == "x{0}y");
}

TEST_CASE("StringUtil::format wrapper", "[Text][Format][StringUtil]")
{
  REQUIRE(StringUtil::format(String("val={0}"), 10) == "val=10");
  REQUIRE(StringUtil::format("sum {0}+{1}={2}", 2, 3, 5) == "sum 2+3=5");
}

// -----------------------------------------------------------------------------
// UTF8: fromWide/toWide, UTF16/UTF32 roundtrips, count, char indexing
// -----------------------------------------------------------------------------
TEST_CASE("UTF8: wide <-> utf8 roundtrip with non-ASCII", "[Text][Unicode][UTF8]")
{
  // Usamos caracteres comunes:
  // Nota: dependiendo de plataforma (Windows UTF-16 / Unix UTF-32), esto debe funcionar igual por tu implementación.
  WString w = L"Espa\u00f1a \u00e1rbol \u20ac \u4e2d";
  String u8 = UTF8::fromWide(w);
  WString w2 = UTF8::toWide(u8);

  REQUIRE(w == w2);

  // count debe contar chars, no bytes
  REQUIRE(UTF8::count(u8) == w.size());
}

TEST_CASE("UTF8: UTF16/UTF32 conversions roundtrip", "[Text][Unicode][UTF8]")
{
  // Construimos desde wide -> utf8 -> utf16/utf32 -> utf8
  WString w = L"abc \u00f1 \u4e2d \U0001F600"; // incluye
  String u8 = UTF8::fromWide(w);

  U16String u16 = UTF8::toUTF16(u8);
  String u8a = UTF8::fromUTF16(u16);
  REQUIRE(u8a == u8);

  U32String u32 = UTF8::toUTF32(u8);
  String u8b = UTF8::fromUTF32(u32);
  REQUIRE(u8b == u8);
}

TEST_CASE("UTF8: charToByteIndex and charByteCount basic sanity", "[Text][Unicode][UTF8][Indexing]")
{
  String u8 = UTF8::fromWide(L"A\u00f1\u4e2d"); //
  // chars: 3
  REQUIRE(UTF8::count(u8) == 3);

  // idx 0 -> 0
  REQUIRE(UTF8::charToByteIndex(u8, 0) == 0);

  
  REQUIRE(UTF8::charByteCount(u8, 0) == 1);
  REQUIRE(UTF8::charByteCount(u8, 1) == 2);
  REQUIRE(UTF8::charByteCount(u8, 2) == 3);

  // idx out of range -> returns input.size() (doc)
  REQUIRE(UTF8::charToByteIndex(u8, 999) == u8.size());
}

// -----------------------------------------------------------------------------
// StringID: identity, stability, NONE/empty, hash support
// -----------------------------------------------------------------------------
TEST_CASE("StringID: same string => same id, different => different (typically)", "[Text][StringID]")
{
  StringID a("hello");
  StringID b("hello");
  StringID c("world");

  REQUIRE(a == b);
  REQUIRE(a.id() == b.id());
  REQUIRE(std::string(a.c_str()) == "hello");

  // c should be different most of the time; in the impossible case of collision of internal pointer
  // (no debería ocurrir por tu interning), esto fallaría y te avisa.
  REQUIRE(a != c);
  REQUIRE(a.id() != c.id());
}

TEST_CASE("StringID: NONE and empty()", "[Text][StringID]")
{
  REQUIRE(StringID::NONE.empty());

  StringID x;
  REQUIRE(x.empty());
  REQUIRE(std::string(x.c_str()).empty());
  // id() returns -1 cast to uint32 per tu código
  REQUIRE(x.id() == (uint32)-1);
}

TEST_CASE("StringID: works with std::hash (unordered_map key)", "[Text][StringID][Hash]")
{
  std::unordered_map<StringID, int> m;
  m[StringID("a")] = 1;
  m[StringID("b")] = 2;

  REQUIRE(m[StringID("a")] == 1);
  REQUIRE(m[StringID("b")] == 2);
}

// -----------------------------------------------------------------------------
// Property-based: split/join-ish invariants, trim idempotence, format stability
// -----------------------------------------------------------------------------
TEST_CASE("Property: trim is idempotent", "[Text][Property][Trim]")
{
  for (int i = 0; i < 2000; ++i)
  {
    String s = randAsciiString(0, 40);
    s = sprinkleDelims(s, "\t\n \r", 8); // mete whitespace

    String a = s;
    StringUtil::trim(a);

    String b = a;
    StringUtil::trim(b);

    REQUIRE(a == b);
  }
}

TEST_CASE("Property: split tokens do not contain delimiters (for split)",
          "[Text][Property][Split]") {
  const String delims = ",; \t";
  for (int i = 0; i < 2000; ++i) {
    String s = randAsciiString(0, 60);
    s = sprinkleDelims(s, delims, 10);

    auto toks = StringUtil::split(s, delims, 0); // sin maxSplits

    for (auto& t : toks) {
      REQUIRE(t.find_first_of(delims) == String::npos);
    }
  }
}

TEST_CASE("StringUtil::split: empty tokens behavior is consistent",
          "[Text][StringUtil][Split]") {
  auto t = StringUtil::split("a,,b", ",", 0);
  REQUIRE(t.size() == 2);
  REQUIRE(t[0] == "a"); REQUIRE(t[1] == "b");
}

TEST_CASE("Property: format preserves literal when no valid identifiers present",
          "[Text][Property][Format]") {
  for (int i = 0; i < 2000; ++i) {
    String s = randAsciiString(0, 80);
    for (auto& ch : s) {
      if (ch == '{') ch = '(';
      if (ch == '}') ch = ')';
    }

    REQUIRE(StringFormat::format(s.c_str(), 1, 2, 3) == s);
  }
}

// -----------------------------------------------------------------------------
// toString(time_t): ISO 8601 shape checks
// -----------------------------------------------------------------------------
TEST_CASE("toString(time_t): ISO 8601 shapes and type variants", "[Text][Time][Format]") {
  const time_t t = 0; //epoch
  String d = toString(t, true, true, TIME_TO_STRING_CONVERSION_TYPE::kDate);
  String tm = toString(t, true, true, TIME_TO_STRING_CONVERSION_TYPE::kTime);
  String utcFull = toString(t, true, true, TIME_TO_STRING_CONVERSION_TYPE::kFull);
  String localFull = toString(t, false, true, TIME_TO_STRING_CONVERSION_TYPE::kFull);

  //ISO date: "YYYY-MM-DD" => length 10, '-' at 4 and 7
  REQUIRE(d.size() == 10);
  REQUIRE(d[4] == '-');
  REQUIRE(d[7] == '-');

  //ISO time: "HH::MM::SS"
  REQUIRE(tm.size() == 8);
  REQUIRE(tm[2] == ':');
  REQUIRE(tm[5] == ':');

  REQUIRE(utcFull.size() == 20);
  REQUIRE(utcFull[10] == 'T');
  REQUIRE(utcFull.back() == 'Z');

  REQUIRE(localFull.size() == 19);
  REQUIRE(localFull[10] == 'T');
  REQUIRE(localFull.back() != 'Z');
}

TEST_CASE("StringFormat: backslash is literal unless escaping braces/backslash",
          "[Text][Format][Backslash]") {
  // 1) Backslash literal no debe desaparecer
  REQUIRE(StringFormat::format("C:\\Temp\\file.txt") == "C:\\Temp\\file.txt");

  // 2) Formateo en una ruta con placeholders
  REQUIRE(StringFormat::format("C:\\Temp\\file_{0}.txt", 7) == "C:\\Temp\\file_7.txt");

  // 3) UNC path
  REQUIRE(StringFormat::format("\\\\server\\share\\{0}", "data") == "\\\\server\\share\\data");

  // 4) Escape de '{' con backslash: "\{0}" debe dejar "{0}" literal (no formatear)
  REQUIRE(StringFormat::format("x{{0}}y", 9) == "x{0}y");
}
