#include <catch_amalgamated.hpp>

#include "geVector4.h"

using namespace geEngineSDK;
using Catch::Approx;

namespace {
  inline void requireNear(float a, float b, float eps = 1e-6f) {
    REQUIRE(std::fabs(a - b) <= eps);
  }

  inline void requireVecNear(const Vector4& a, const Vector4& b, float eps = 1e-5f) {
    REQUIRE(std::fabs(a.x - b.x) <= eps);
    REQUIRE(std::fabs(a.y - b.y) <= eps);
    REQUIRE(std::fabs(a.z - b.z) <= eps);
    REQUIRE(std::fabs(a.w - b.w) <= eps);
  }
}

TEST_CASE("Vector4: constructors and indexing", "[Math][Vector4]") {
  Vector4 a(1, 2, 3, 4);
  requireVecNear(a, Vector4(1, 2, 3, 4));

  Vector4 z(FORCE_INIT::kForceInit);
  requireVecNear(z, Vector4(0, 0, 0, 0));

  REQUIRE(a[0] == Approx(1));
  REQUIRE(a[1] == Approx(2));
  REQUIRE(a[2] == Approx(3));
  REQUIRE(a[3] == Approx(4));
}

TEST_CASE("Vector4: arithmetic and dot helpers", "[Math][Vector4]")
{
  Vector4 a(1, 2, 3, 4);
  Vector4 b(5, 6, 7, 8);

  requireVecNear(a + b, Vector4(6, 8, 10, 12));
  requireVecNear(b - a, Vector4(4, 4, 4, 4));
  requireVecNear(a * 2.0f, Vector4(2, 4, 6, 8));

  // dot3/dot4 (están como static inline en el header)
  requireNear(dot3(a, b), 1 * 5 + 2 * 6 + 3 * 7);
  requireNear(dot4(a, b), 1 * 5 + 2 * 6 + 3 * 7 + 4 * 8);
}

TEST_CASE("Vector4: size and isNearlyZero3", "[Math][Vector4]")
{
  Vector4 a(3, 4, 0, 123);
  requireNear(a.size(), std::sqrt(3 * 3 + 4 * 4 + 0 * 0 + 123 * 123), 1e-5f);

  Vector4 z(FORCE_INIT::kForceInit);
  REQUIRE(z.isNearlyZero3());
}
