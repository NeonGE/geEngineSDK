#include <iostream>
#include <catch_amalgamated.hpp>

#include <gePrerequisitesUtilities.h>
#include <geMath.h>

using namespace geEngineSDK;

TEST_CASE("Basic_Type_Size", "[Data Types]") {
  REQUIRE(sizeof(unsigned char) == 1U);
  REQUIRE(sizeof(uint8) == 1U);
  REQUIRE(sizeof(uint16) == 2U);
  REQUIRE(sizeof(uint32) == 4U);
  REQUIRE(sizeof(uint64) == 8U);
  REQUIRE(sizeof(uint128) == 16U);

  REQUIRE(sizeof(char) == 1);
  REQUIRE(sizeof(int8) == 1);
  REQUIRE(sizeof(int16) == 2);
  REQUIRE(sizeof(int32) == 4);
  REQUIRE(sizeof(int64) == 8);
  REQUIRE(sizeof(int128) == 16);

  REQUIRE(sizeof(float) == 4);
  REQUIRE(sizeof(double) == 8);

  REQUIRE(static_cast<uint8>(-1) == NumLimit::MAX_UINT8);
  REQUIRE(static_cast<uint16>(-1) == NumLimit::MAX_UINT16);
  REQUIRE(static_cast<uint32>(-1) == NumLimit::MAX_UINT32);
  REQUIRE(static_cast<uint64>(-1) == NumLimit::MAX_UINT64);

  REQUIRE_THAT(Math::PI, Catch::Matchers::WithinRel(3.141592f, 0.0001f));
}