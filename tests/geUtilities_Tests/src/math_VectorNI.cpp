#include <catch_amalgamated.hpp>

#include "gePrerequisitesUtilities.h"
#include "geVectorNI.h"

using namespace geEngineSDK;

TEST_CASE("VectorNI: default zero + copy/assign + equality", "[Math][VectorNI]") {
  VectorNI<3> a;
  REQUIRE(a[0] == 0);
  REQUIRE(a[1] == 0);
  REQUIRE(a[2] == 0);

  int32 vals[3] = { 1, 2, 3 };
  VectorNI<3> b(vals);
  REQUIRE(b[0] == 1);
  REQUIRE(b[1] == 2);
  REQUIRE(b[2] == 3);

  VectorNI<3> c(b);
  REQUIRE(c == b);
  REQUIRE_FALSE(c != b);

  int32 vals2[3] = { 9, 8, 7 };
  c = vals2;
  REQUIRE(c[0] == 9);
  REQUIRE(c[1] == 8);
  REQUIRE(c[2] == 7);
  REQUIRE(c != b);
}
