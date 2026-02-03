#include <catch2/catch_test_macros.hpp>

#include "geVector2.h"
#include "geVector3.h" // para ctor Vector2(Vector3)

using namespace geEngineSDK;

namespace {
  inline void requireNear(float a, float b, float eps = 1e-6f) {
    REQUIRE(std::fabs(a - b) <= eps);
  }
}

TEST_CASE("Vector2: constructors and indexing", "[Math][Vector2]") {
  Vector2 a(1.0f, 2.0f);
  requireNear(a.x, 1.0f);
  requireNear(a.y, 2.0f);

  Vector2 z(FORCE_INIT::kForceInit);
  requireNear(z.x, 0.0f);
  requireNear(z.y, 0.0f);

  Vector3 v3(7.0f, 8.0f, 9.0f);
  Vector2 from3(v3);
  requireNear(from3.x, 7.0f);
  requireNear(from3.y, 8.0f);
}

TEST_CASE("Vector2: arithmetic operators", "[Math][Vector2]") {
  Vector2 a(1.0f, 2.0f);
  Vector2 b(3.0f, 4.0f);

  Vector2 c = a + b;
  requireNear(c.x, 4.0f);
  requireNear(c.y, 6.0f);

  c = b - a;
  requireNear(c.x, 2.0f);
  requireNear(c.y, 2.0f);

  c = a * 2.0f;
  requireNear(c.x, 2.0f);
  requireNear(c.y, 4.0f);

  c = a + 5.0f;
  requireNear(c.x, 6.0f);
  requireNear(c.y, 7.0f);

  c = a - 1.0f;
  requireNear(c.x, 0.0f);
  requireNear(c.y, 1.0f);

  c = a * b; //component-wise
  requireNear(c.x, 3.0f);
  requireNear(c.y, 8.0f);

  c = -a;
  requireNear(c.x, -1.0f);
  requireNear(c.y, -2.0f);
}

TEST_CASE("Vector2: dot and cross", "[Math][Vector2]") {
  Vector2 a(1.0f, 2.0f);
  Vector2 b(3.0f, 4.0f);

  //dot: 1*3 + 2*4 = 11
  requireNear((a | b), 11.0f);

  //cross 2D (escalar): x1*y2 - y1*x2 = 1*4 - 2*3 = -2
  requireNear((a ^ b), -2.0f);
}

TEST_CASE("Vector2: size/normalize/safe normal", "[Math][Vector2]") {
  Vector2 a(3.0f, 4.0f);
  requireNear(a.size(), 5.0f);
  requireNear(a.sizeSquared(), 25.0f);

  Vector2 n = a.getSafeNormal();
  requireNear(n.size(), 1.0f, 1e-5f);

  Vector2 z(FORCE_INIT::kForceInit);
  Vector2 nz = z.getSafeNormal();
  REQUIRE(nz.isZero());

  Vector2 b(3.0f, 4.0f);
  b.normalize();
  requireNear(b.size(), 1.0f, 1e-5f);
}

TEST_CASE("Vector2: isZero/isNearlyZero and comparisons", "[Math][Vector2]") {
  Vector2 z(FORCE_INIT::kForceInit);
  REQUIRE(z.isZero());
  REQUIRE(z.isNearlyZero());

  Vector2 a(1.0f, 2.0f);
  Vector2 b(1.0f, 2.0f);
  Vector2 c(1.0f, 2.0001f);

  REQUIRE(a == b);
  REQUIRE(a != c);

  //Comparisons (lexicographical)
  REQUIRE(Vector2(1, 1) < Vector2(2, 2));
  REQUIRE(Vector2(2, 2) > Vector2(1, 1));
  REQUIRE(Vector2(2, 2) >= Vector2(2, 2));
  REQUIRE(Vector2(2, 2) <= Vector2(2, 2));
}
