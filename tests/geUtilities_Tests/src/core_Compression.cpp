#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <cstdint>
#include <random>
#include <vector>

#include "geCompression.h"
#include "geDataStream.h" // MemoryDataStream, DataStream

using namespace geEngineSDK;

static SPtr<DataStream> makeStreamFromBytes(const std::vector<uint8_t>& bytes) {
  auto ms = ge_shared_ptr_new<MemoryDataStream>(bytes.size());
  if (!bytes.empty())
    ms->write(bytes.data(), bytes.size());
  ms->seek(0);
  return std::static_pointer_cast<DataStream>(ms);
}

static std::vector<uint8_t> readAllBytes(SPtr<MemoryDataStream>& ms) {
  std::vector<uint8_t> out(ms->size());
  if (!out.empty())
    ms->read(out.data(), out.size());
  ms->seek(0);
  return out;
}

TEST_CASE("Compression: roundtrip small buffer", "[Compression]") {
  const char* text = "Hello LZ4 world";
  std::vector<uint8_t> srcBytes(text, text + std::strlen(text));

  auto src = makeStreamFromBytes(srcBytes);

  auto comp = Compression::compress(src);
  REQUIRE(comp != nullptr);

  auto compAsDS = std::static_pointer_cast<DataStream>(comp);
  auto decomp = Compression::decompress(compAsDS);
  REQUIRE(decomp != nullptr);

  auto outBytes = readAllBytes(decomp);
  REQUIRE(outBytes == srcBytes);
}

TEST_CASE("Compression: roundtrip random sizes", "[Compression]") {
  std::mt19937 rng(1337);
  std::uniform_int_distribution<int> distByte(0, 255);

  for (size_t sz : {1u, 2u, 3u, 7u, 16u, 31u, 64u, 257u, 1024u, 4096u, 32768u}) {
    std::vector<uint8_t> srcBytes(sz);
    for (auto& b : srcBytes) b = static_cast<uint8_t>(distByte(rng));

    auto src = makeStreamFromBytes(srcBytes);

    auto comp = Compression::compress(src);
    REQUIRE(comp != nullptr);

    auto compAsDS = std::static_pointer_cast<DataStream>(comp);
    auto decomp = Compression::decompress(compAsDS);
    REQUIRE(decomp != nullptr);

    auto outBytes = readAllBytes(decomp);
    REQUIRE(outBytes == srcBytes);
  }
}

TEST_CASE("Compression: progress callback called with 1.0 on success", "[Compression]") {
  std::vector<uint8_t> srcBytes(1024, 0xAB);
  auto src = makeStreamFromBytes(srcBytes);

  std::atomic<int> calls{ 0 };
  std::atomic<float> last{ 0.0f };

  auto comp = Compression::compress(
    src,
    [&](float p) {
      calls.fetch_add(1, std::memory_order_relaxed);
      last.store(p, std::memory_order_relaxed);
    }
  );

  REQUIRE(comp != nullptr);
  REQUIRE(calls.load(std::memory_order_relaxed) >= 1);
  REQUIRE(last.load(std::memory_order_relaxed) == 1.0f);

  calls.store(0, std::memory_order_relaxed);
  last.store(0.0f, std::memory_order_relaxed);

  auto compAsDS = std::static_pointer_cast<DataStream>(comp);
  auto decomp = Compression::decompress(
    compAsDS,
    [&](float p) {
      calls.fetch_add(1, std::memory_order_relaxed);
      last.store(p, std::memory_order_relaxed);
    }
  );

  REQUIRE(decomp != nullptr);
  REQUIRE(calls.load(std::memory_order_relaxed) >= 1);
  REQUIRE(last.load(std::memory_order_relaxed) == 1.0f);
}

TEST_CASE("Compression: decompress accepts header with original size = 0 (empty stream)", "[Compression]") {
  // header uint64 = 0, sin payload
  auto s = ge_shared_ptr_new<MemoryDataStream>(sizeof(uint64));
  uint64 zero = 0;
  s->write(&zero, sizeof(uint64));
  s->seek(0);

  auto sAsDS = std::static_pointer_cast<DataStream>(s);
  auto out = Compression::decompress(sAsDS);
  REQUIRE(out != nullptr);
  REQUIRE(out->size() == 0);
}

TEST_CASE("Compression: decompress rejects stream smaller than header", "[Compression]") {
  auto s = ge_shared_ptr_new<MemoryDataStream>(4); // < sizeof(uint64)
  uint32 junk = 0x12345678;
  s->write(&junk, sizeof(junk));
  s->seek(0);

  auto sAsDS = std::static_pointer_cast<DataStream>(s);
  auto out = Compression::decompress(sAsDS);
  REQUIRE(out == nullptr);
}

TEST_CASE("Compression: decompress fails on corrupted payload", "[Compression]") {
  auto s = ge_shared_ptr_new<MemoryDataStream>(sizeof(uint64) + 8);
  uint64 original = 64;
  uint8 payload[8] = { 1,2,3,4,5,6,7,8 }; // basura
  s->write(&original, sizeof(uint64));
  s->write(payload, sizeof(payload));
  s->seek(0);

  auto sAsDS = std::static_pointer_cast<DataStream>(s);
  auto out = Compression::decompress(sAsDS);
  REQUIRE(out == nullptr);
}


TEST_CASE("Compression: decompress fails on truncated payload", "[Compression]") {
  // header dice que original size = 16, pero payload es basura/truncado
  auto bad = ge_shared_ptr_new<MemoryDataStream>(sizeof(uint64) + 4);
  uint64 original = 16;
  uint8_t payload[4] = { 1,2,3,4 };

  bad->write(&original, sizeof(uint64));
  bad->write(payload, sizeof(payload));
  bad->seek(0);

  auto badAsDS = std::static_pointer_cast<DataStream>(bad);
  auto out = Compression::decompress(badAsDS);

  REQUIRE(out == nullptr);
}

TEST_CASE("Compression: empty roundtrip", "[Compression]") {
  auto empty = makeStreamFromBytes({});

  auto comp = Compression::compress(empty);
  REQUIRE(comp != nullptr);

  auto compAsDS = std::static_pointer_cast<DataStream>(comp);
  auto decomp = Compression::decompress(compAsDS);
  REQUIRE(decomp != nullptr);
  REQUIRE(decomp->size() == 0);
}

