#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_get_random_seed.hpp>
#include <catch2/catch_approx.hpp>
#include <random>

#include "geSphere.h"
#include "geBox.h"
#include "geBox2D.h"
#include "geBox2DI.h"
#include "geBoxSphereBounds.h"
#include "geCapsuleShape.h"

using namespace geEngineSDK;

namespace
{
  constexpr float kEps = 1e-5f;

  inline void
  requireVec3Near(const Vector3& a, const Vector3& b, float eps = kEps) {
    REQUIRE(a.equals(b, eps));
  }

  inline void
  requireVec2Near(const Vector2& a, const Vector2& b, float eps = kEps) {
    REQUIRE(a.equals(b, eps));
  }

  static std::mt19937&
  rng() {
    static auto seed = Catch::getSeed();
    static std::mt19937 rng(seed);
    return rng;
  }

  static float
  randRange(float a, float b) {
    std::uniform_real_distribution<float> dist(a, b);
    return dist(rng());
  }

  static Vector3
  randVec3(float minv = -1.0f, float maxv = 1.0f) {
    return Vector3{ randRange(minv,maxv), randRange(minv,maxv), randRange(minv,maxv) };
  }
}

TEST_CASE("Sphere: equals / inside / intersects", "[Math][Bounds][Sphere]") {
  Sphere s0(Vector3(0, 0, 0), 1.0f);
  Sphere s1(Vector3(0, 0, 0), 1.0f);
  Sphere s2(Vector3(0, 0, 0), 1.00001f);

  REQUIRE(s0.equals(s1));
  REQUIRE(s0.equals(s2, 1e-3f));
  REQUIRE_FALSE(s0.equals(s2, 1e-7f));

  SECTION("Point inside / on / outside (tolerance)")
  {
    REQUIRE(s0.isInside(Vector3(0, 0, 0)));
    REQUIRE(s0.isInside(Vector3(1, 0, 0), 0.0f));
    REQUIRE_FALSE(s0.isInside(Vector3(1.0f + 1e-4f, 0, 0), 0.0f));
    REQUIRE(s0.isInside(Vector3(1.0f + 1e-4f, 0, 0), 1e-3f));
  }

  SECTION("Sphere inside sphere")
  {
    Sphere big(Vector3(0, 0, 0), 5.0f);
    Sphere small(Vector3(1, 0, 0), 1.0f);

    REQUIRE(small.isInside(big));
    REQUIRE_FALSE(big.isInside(small));
  }

  SECTION("Sphere intersects sphere")
  {
    Sphere a(Vector3(0, 0, 0), 1.0f);
    Sphere b(Vector3(1.9f, 0, 0), 1.0f);
    Sphere c(Vector3(2.1f, 0, 0), 1.0f);

    REQUIRE(a.intersects(b));
    REQUIRE_FALSE(a.intersects(c));
  }
}

TEST_CASE("Sphere: operator+= point expands correctly", "[Math][Bounds][Sphere]") {
  Sphere s(Vector3(0, 0, 0), 1.0f);
  s += Vector3(3, 0, 0);

  //dist=3, newR=2, shift=(2-1)/3 = 1/3 => center=(1,0,0), r=2
  requireVec3Near(s.m_center, Vector3(1, 0, 0), 1e-6f);
  REQUIRE(s.m_radius == Catch::Approx(2.0f).epsilon(1e-6f));
}

TEST_CASE("AABox: init/validity and adding points", "[Math][Bounds][AABox]") {
  AABox b(FORCE_INIT::kForceInit);
  REQUIRE(b.m_isValid == 0);
  requireVec3Near(b.m_min, Vector3::ZERO);
  requireVec3Near(b.m_max, Vector3::ZERO);

  b += Vector3(10, 20, 30);
  REQUIRE(b.m_isValid == 1);
  requireVec3Near(b.m_min, Vector3(10, 20, 30));
  requireVec3Near(b.m_max, Vector3(10, 20, 30));

  b += Vector3(-5, 25, 0);
  requireVec3Near(b.m_min, Vector3(-5, 20, 0));
  requireVec3Near(b.m_max, Vector3(10, 25, 30));
}

TEST_CASE("AABox: center/extent/size/volume", "[Math][Bounds][AABox]") {
  AABox b(Vector3(-2, -4, -6), Vector3(2, 4, 6));

  requireVec3Near(b.getCenter(), Vector3(0, 0, 0));
  requireVec3Near(b.getExtent(), Vector3(2, 4, 6));
  requireVec3Near(b.getSize(), Vector3(4, 8, 12));
  REQUIRE(b.getVolume() == Catch::Approx(4.0f * 8.0f * 12.0f).epsilon(1e-6f));
}

TEST_CASE("AABox: isInside vs isInsideOrOn", "[Math][Bounds][AABox]") {
  AABox b(Vector3(-1, -1, -1), Vector3(1, 1, 1));

  REQUIRE(b.isInside(Vector3(0, 0, 0)));
  REQUIRE_FALSE(b.isInside(Vector3(1, 0, 0)));      // strict
  REQUIRE(b.isInsideOrOn(Vector3(1, 0, 0)));        // inclusive
  REQUIRE_FALSE(b.isInside(Vector3(2, 0, 0)));
  REQUIRE_FALSE(b.isInsideOrOn(Vector3(2, 0, 0)));
}

TEST_CASE("AABox: intersect / overlap basics", "[Math][Bounds][AABox]") {
  AABox a(Vector3(0, 0, 0), Vector3(2, 2, 2));
  AABox b(Vector3(1, 1, 1), Vector3(3, 3, 3));
  AABox c(Vector3(3.1f, 0, 0), Vector3(4, 1, 1));

  REQUIRE(a.intersect(b));
  REQUIRE_FALSE(a.intersect(c));

  SECTION("overlap() returns intersection region")
  {
    AABox o = a.overlap(b);
    requireVec3Near(o.m_min, Vector3(1, 1, 1));
    requireVec3Near(o.m_max, Vector3(2, 2, 2));
  }
}

TEST_CASE("AABox: closest point and squared distance", "[Math][Bounds][AABox]") {
  AABox b(Vector3(-1, -1, -1), Vector3(1, 1, 1));

  SECTION("Closest point clamps")
  {
    Vector3 p(5, 0.5f, -3);
    Vector3 c = b.getClosestPointTo(p);
    requireVec3Near(c, Vector3(1, 0.5f, -1));
  }

  SECTION("Squared distance matches expected")
  {
    //p = (5,0.5,-3) -> closest = (1,0.5,-1)
    //delta = (4,0,2) => dist2=16+0+4=20
    Vector3 p(5, 0.5f, -3);
    float d2 = b.computeSquaredDistanceToPoint(p);
    REQUIRE(d2 == Catch::Approx(20.0f).epsilon(1e-6f));
  }
}

TEST_CASE("Box2D: init/adding points/area", "[Math][Bounds][Box2D]") {
  Box2D b(FORCE_INIT::kForceInit);
  REQUIRE_FALSE(b.m_bIsValid);

  b += Vector2(10, 20);
  REQUIRE(b.m_bIsValid);
  requireVec2Near(b.m_min, Vector2(10, 20));
  requireVec2Near(b.m_max, Vector2(10, 20));

  b += Vector2(-5, 25);
  requireVec2Near(b.m_min, Vector2(-5, 20));
  requireVec2Near(b.m_max, Vector2(10, 25));

  REQUIRE(b.getArea() == Catch::Approx(15.0f * 5.0f).epsilon(1e-6f));
}

TEST_CASE("Box2D: inside/intersect/closest point", "[Math][Bounds][Box2D]") {
  Box2D a(Vector2(-1, -1), Vector2(1, 1));
  Box2D b(Vector2(0, 0), Vector2(2, 2));
  Box2D c(Vector2(3, 0), Vector2(4, 1));

  REQUIRE(a.isInside(Vector2(0, 0)));
  REQUIRE_FALSE(a.isInside(Vector2(1, 0)));
  REQUIRE(a.intersect(b));
  REQUIRE_FALSE(a.intersect(c));

  Vector2 p(5, -3);
  Vector2 cp = a.getClosestPointTo(p);
  requireVec2Near(cp, Vector2(1, -1));
}

TEST_CASE("Box2DI: init/adding points/area", "[Math][Bounds][Box2DI]") {
  Box2DI b(FORCE_INIT::kForceInit);
  REQUIRE_FALSE(b.m_bIsValid);

  b += Vector2I(10, 20);
  REQUIRE(b.m_bIsValid);
  REQUIRE(b.m_min == Vector2I(10, 20));
  REQUIRE(b.m_max == Vector2I(10, 20));

  b += Vector2I(-5, 25);
  REQUIRE(b.m_min == Vector2I(-5, 20));
  REQUIRE(b.m_max == Vector2I(10, 25));

  // area int
  REQUIRE(b.getArea() == 15 * 5);
}

TEST_CASE("BoxSphereBounds: from box/sphere getters + intersections",
          "[Math][Bounds][BoxSphereBounds]") {
  AABox box(Vector3(-2, -4, -6), Vector3(2, 4, 6));
  BoxSphereBounds b(box);

  Vector3 origin, ext;
  box.getCenterAndExtents(origin, ext);

  requireVec3Near(b.m_origin, origin);
  requireVec3Near(b.m_boxExtent, ext);
  REQUIRE(b.m_sphereRadius == Catch::Approx(ext.size()).epsilon(1e-6f));

  SECTION("getBox reproduces original")
  {
    AABox bb = b.getBox();
    requireVec3Near(bb.m_min, box.m_min);
    requireVec3Near(bb.m_max, box.m_max);
  }

  SECTION("getSphere center/radius")
  {
    Sphere s = b.getSphere();
    requireVec3Near(s.m_center, b.m_origin);
    REQUIRE(s.m_radius == Catch::Approx(b.m_sphereRadius).epsilon(1e-6f));
  }

  SECTION("spheresIntersect / boxesIntersect")
  {
    BoxSphereBounds a(Vector3(0, 0, 0), Vector3(1, 1, 1), 1.0f);
    BoxSphereBounds c(Vector3(3.1f, 0, 0), Vector3(1, 1, 1), 1.0f);

    REQUIRE_FALSE(BoxSphereBounds::spheresIntersect(a, c));
    REQUIRE_FALSE(BoxSphereBounds::boxesIntersect(a, c));

    BoxSphereBounds d(Vector3(1.5f, 0, 0), Vector3(1, 1, 1), 1.0f);
    REQUIRE(BoxSphereBounds::spheresIntersect(a, d));
    REQUIRE(BoxSphereBounds::boxesIntersect(a, d));
  }

  SECTION("expandBy increases extents and radius")
  {
    BoxSphereBounds e = b.expandBy(2.0f);
    requireVec3Near(e.m_boxExtent, b.m_boxExtent + 2.0f);
    REQUIRE(e.m_sphereRadius == Catch::Approx(b.m_sphereRadius + 2.0f).epsilon(1e-6f));
  }

  SECTION("boundsUnion contains both boxes")
  {
    BoxSphereBounds a(Vector3(0, 0, 0), Vector3(1, 1, 1), 1.0f);
    BoxSphereBounds d(Vector3(5, 0, 0), Vector3(1, 2, 1), 2.0f);

    BoxSphereBounds u = boundsUnion(a, d);
    AABox ub = u.getBox();

    REQUIRE(ub.isInsideOrOn(a.getBoxExtrema(0)));
    REQUIRE(ub.isInsideOrOn(a.getBoxExtrema(1)));
    REQUIRE(ub.isInsideOrOn(d.getBoxExtrema(0)));
    REQUIRE(ub.isInsideOrOn(d.getBoxExtrema(1)));
  }
}

TEST_CASE("CapsuleShape: ctor sets fields", "[Math][Bounds][CapsuleShape]") {
  CapsuleShape c(Vector3(1, 2, 3), 0.5f, Vector3(0, 1, 0), 10.0f);
  requireVec3Near(c.center, Vector3(1, 2, 3));
  REQUIRE(c.radius == Catch::Approx(0.5f).epsilon(1e-6f));
  requireVec3Near(c.orientation, Vector3(0, 1, 0));
  REQUIRE(c.length == Catch::Approx(10.0f).epsilon(1e-6f));
}

TEST_CASE("Vector3: length / normalization invariants", "[Math][Vector3][Property]") {
  for (int i = 0; i < 2000; ++i) {
    Vector3 v = randVec3(-1000.0f, 1000.0f);

    if (v.sizeSquared() < 1e-6f) {
      continue;
    }

    Vector3 n = v.getSafeNormal();
    REQUIRE(n.sizeSquared() == Catch::Approx(1.0f).epsilon(1e-4f));

    float len = v.size();
    REQUIRE((n * len).equals(v, 1e-3f));
  }
}

TEST_CASE("Vector3: dot/projection consistency", "[Math][Vector3][Property]") {
  for (int i = 0; i < 2000; ++i) {
    Vector3 a = randVec3(-1000.0f, 1000.0f);
    Vector3 b = randVec3(-1000.0f, 1000.0f);

    float dot = a | b;

    if (b.sizeSquared() < 1e-6f) {
      continue;
    }

    Vector3 proj = b * (dot / b.sizeSquared());

    REQUIRE((proj | b) == Catch::Approx(dot).epsilon(1e-3f));
  }
}

TEST_CASE("AABox: adding points must contain them", "[Math][AABox][Property]") {
  for (int i = 0; i < 1000; ++i) {
    AABox box(FORCE_INIT::kForceInit);

    Vector<Vector3> pts;
    for (int p = 0; p < 50; ++p) {
      Vector3 v = randVec3(-1000.0f, 1000.0f);
      pts.push_back(v);
      box += v;
    }

    for (auto& v : pts) {
      REQUIRE(box.isInsideOrOn(v));
    }
  }
}

TEST_CASE("AABox: closest point always inside", "[Math][AABox][Property]") {
  for (int i = 0; i < 2000; ++i) {
    Vector3 a = randVec3(-1000.0f, 1000.0f);
    Vector3 b = randVec3(-1000.0f, 1000.0f);

    AABox box(a, b);

    Vector3 p = randVec3(-1000.0f, 1000.0f);
    Vector3 c = box.getClosestPointTo(p);

    REQUIRE(box.isInsideOrOn(c));
  }
}

TEST_CASE("Sphere: adding points must contain them", "[Math][Sphere][Property]") {
  for (int i = 0; i < 1000; ++i) {
    Sphere s(Vector3(0, 0, 0), 0.1f);

    Vector<Vector3> pts;

    for (int p = 0; p < 30; ++p) {
      Vector3 v = randVec3(-1000.0f, 1000.0f);
      pts.push_back(v);
      s += v;
    }

    for (auto& v : pts) {
      REQUIRE(s.isInside(v, 1e-3f));
    }
  }
}

TEST_CASE("Sphere vs AABox geometric coherence", "[Math][Bounds][Property]") {
  for (int i = 0; i < 1000; ++i) {
    AABox box(FORCE_INIT::kForceInit);

    for (int p = 0; p < 20; ++p) {
      box += randVec3(-1000.0f, 1000.0f);
    }

    Sphere s(box.getCenter(), box.getExtent().size());

    Vector3 test = randVec3(-1000.0f, 1000.0f);

    if (s.isInside(test)) {
      AABox expanded = box.expandBy(s.m_radius);
      REQUIRE(expanded.isInsideOrOn(test));
    }
  }
}

TEST_CASE("Box2DI: integer containment property", "[Math][Box2DI][Property]") {
  Box2DI box(FORCE_INIT::kForceInit);

  for (int i = 0; i < 100; ++i) {
    Vector2I p(i - 50, 50 - i);
    box += p;
  }

  for (int i = 0; i < 100; ++i) {
    Vector2I p(i - 50, 50 - i);
    REQUIRE(box.isInsideOrOn(p));
  }
}

TEST_CASE("Box2DI: isInsideOrOn inclusive", "[Math][Bounds][Box2DI]") {
  Box2DI b(Vector2I(-1, -2), Vector2I(3, 4));

  REQUIRE(b.isInsideOrOn(Vector2I(-1, -2)));
  REQUIRE(b.isInsideOrOn(Vector2I(3, 4)));
  REQUIRE(b.isInsideOrOn(Vector2I(0, 0)));

  REQUIRE_FALSE(b.isInsideOrOn(Vector2I(-2, 0)));
  REQUIRE_FALSE(b.isInsideOrOn(Vector2I(0, 5)));
}
