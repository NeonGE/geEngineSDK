#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <limits>

#include "geRandom.h"
#include "geDegree.h"

using namespace geEngineSDK;

static bool nearly(float a, float b, float eps = 1e-4f) {
  return std::fabs(a - b) <= eps;
}

TEST_CASE("Random: same seed produces identical sequence", "[Random]") {
  Random a(12345);
  Random b(12345);

  for (int i = 0; i < 1000; ++i) {
    REQUIRE(a.get() == b.get());
  }
}

TEST_CASE("Random: different seeds produce different sequence (very likely)", "[Random]") {
  Random a(1);
  Random b(2);

  // Extremely unlikely to match for many draws.
  bool anyDiff = false;
  for (int i = 0; i < 64; ++i) {
    if (a.get() != b.get()) { anyDiff = true; break; }
  }
  REQUIRE(anyDiff);
}

TEST_CASE("Random: getUNorm range is [0, 1]", "[Random]") {
  Random r(7);

  float minV = 1.0f;
  float maxV = 0.0f;

  for (int i = 0; i < 10000; ++i) {
    const float v = r.getUNorm();
    REQUIRE(v >= 0.0f);
    REQUIRE(v <= 1.0f);
    minV = std::min(minV, v);
    maxV = std::max(maxV, v);
  }

  // Not strict randomness tests, but should not be constant.
  REQUIRE(maxV > minV);
}

TEST_CASE("Random: getSNorm range is [-1, 1]", "[Random]") {
  Random r(8);

  float minV = 1.0f;
  float maxV = -1.0f;

  for (int i = 0; i < 10000; ++i) {
    const float v = r.getSNorm();
    REQUIRE(v >= -1.0f);
    REQUIRE(v <= 1.0f);
    minV = std::min(minV, v);
    maxV = std::max(maxV, v);
  }

  REQUIRE(maxV > minV);
}

TEST_CASE("Random: getRange produces values in [min,max] inclusive", "[Random]") {
  Random r(999);

  const int32 min = -3;
  const int32 max = 5;

  bool sawMin = false;
  bool sawMax = false;

  for (int i = 0; i < 20000; ++i) {
    const int32 v = r.getRange(min, max);
    REQUIRE(v >= min);
    REQUIRE(v <= max);
    if (v == min) sawMin = true;
    if (v == max) sawMax = true;
  }

  // Not guaranteed, but with 20k draws on small range it's a safe expectation.
  REQUIRE(sawMin);
  REQUIRE(sawMax);
}

TEST_CASE("Random: getUnitVector returns unit length and non-degenerate", "[Random]") {
  Random r(42);

  for (int i = 0; i < 5000; ++i) {
    const Vector3 v = r.getUnitVector();
    const float lenSq = v.sizeSquared();
    REQUIRE(std::isfinite(lenSq));
    REQUIRE(lenSq > 0.999f);
    REQUIRE(lenSq < 1.001f);
  }
}

TEST_CASE("Random: getUnitVector2D returns unit length and non-degenerate", "[Random]") {
  Random r(43);

  for (int i = 0; i < 5000; ++i) {
    const Vector2 v = r.getUnitVector2D();
    const float lenSq = v.sizeSquared();
    REQUIRE(std::isfinite(lenSq));
    REQUIRE(lenSq > 0.999f);
    REQUIRE(lenSq < 1.001f);
  }
}

TEST_CASE("Random: getPointInSphere returns point inside unit sphere", "[Random]") {
  Random r(44);

  for (int i = 0; i < 10000; ++i) {
    const Vector3 p = r.getPointInSphere();
    const float d2 = p.sizeSquared();
    REQUIRE(std::isfinite(d2));
    REQUIRE(d2 <= 1.0f + 1e-4f); // tolerance
  }
}

TEST_CASE("Random: getPointInSphereShell thickness=0 returns on surface (radius ~= 1)", "[Random]") {
  Random r(45);

  for (int i = 0; i < 2000; ++i) {
    const Vector3 p = r.getPointInSphereShell(0.0f);
    const float d = std::sqrt(p.sizeSquared());
    REQUIRE(d > 0.999f);
    REQUIRE(d < 1.001f);
  }
}

TEST_CASE("Random: getPointInSphereShell thickness=1 returns inside sphere", "[Random]") {
  Random r(46);

  for (int i = 0; i < 10000; ++i) {
    const Vector3 p = r.getPointInSphereShell(1.0f);
    const float d2 = p.sizeSquared();
    REQUIRE(d2 <= 1.0f + 1e-4f);
  }
}

TEST_CASE("Random: getPointInSphereShell thickness in (0,1) stays within shell radii", "[Random]") {
  Random r(47);

  const float thickness = 0.25f;
  const float minR = 1.0f - thickness; // 0.75

  for (int i = 0; i < 10000; ++i) {
    const Vector3 p = r.getPointInSphereShell(thickness);
    const float d = std::sqrt(p.sizeSquared());
    REQUIRE(d >= minR - 1e-3f);
    REQUIRE(d <= 1.0f + 1e-3f);
  }
}

TEST_CASE("Random: getPointInCircle returns point inside unit circle", "[Random]") {
  Random r(48);

  for (int i = 0; i < 10000; ++i) {
    const Vector2 p = r.getPointInCircle();
    const float d2 = p.sizeSquared();
    REQUIRE(std::isfinite(d2));
    REQUIRE(d2 <= 1.0f + 1e-4f);
  }
}

TEST_CASE("Random: getPointInCircleShell thickness=0 returns on edge (radius ~= 1)", "[Random]") {
  Random r(49);

  for (int i = 0; i < 2000; ++i) {
    const Vector2 p = r.getPointInCircleShell(0.0f);
    const float d = std::sqrt(p.sizeSquared());
    REQUIRE(d > 0.999f);
    REQUIRE(d < 1.001f);
  }
}

TEST_CASE("Random: getPointInCircleShell thickness in (0,1) stays within shell radii", "[Random]") {
  Random r(50);

  const float thickness = 0.4f;
  const float minR = 1.0f - thickness;

  for (int i = 0; i < 10000; ++i) {
    const Vector2 p = r.getPointInCircleShell(thickness);
    const float d = std::sqrt(p.sizeSquared());
    REQUIRE(d >= minR - 1e-3f);
    REQUIRE(d <= 1.0f + 1e-3f);
  }
}

TEST_CASE("Random: getPointInArc angle=0 always returns (1,0)", "[Random]") {
  Random r(51);

  for (int i = 0; i < 50; ++i) {
    const Vector2 p = r.getPointInArc(Degree(0.0f));
    REQUIRE(nearly(p.x, 1.0f, 1e-5f));
    REQUIRE(nearly(p.y, 0.0f, 1e-5f));
    REQUIRE(nearly(p.sizeSquared(), 1.0f, 1e-4f));
  }
}

TEST_CASE("Random: getPointInArc produces points on unit circle within angle range", "[Random]") {
  Random r(52);

  const Degree ang(90.0f);
  for (int i = 0; i < 5000; ++i) {
    const Vector2 p = r.getPointInArc(ang);

    // On unit circle
    const float d2 = p.sizeSquared();
    REQUIRE(d2 > 0.999f);
    REQUIRE(d2 < 1.001f);

    // Angle should be within [0, angle] relative to +X axis in the XY plane:
    // atan2(y, x) in radians in [0, pi/2] (with some tolerance for wrap due to numeric).
    const float theta = std::atan2(p.y, p.x);
    REQUIRE(theta >= -1e-4f);
    REQUIRE(theta <= ang.valueRadians() + 1e-4f);
  }
}

TEST_CASE("Random: getPointInArcShell thickness=0 returns on edge of arc (unit radius)", "[Random]") {
  Random r(53);

  const Degree ang(180.0f);
  for (int i = 0; i < 2000; ++i) {
    const Vector2 p = r.getPointInArcShell(ang, 0.0f);
    const float d = std::sqrt(p.sizeSquared());
    REQUIRE(d > 0.999f);
    REQUIRE(d < 1.001f);
  }
}

TEST_CASE("Random: getPointInArcShell thickness in (0,1) stays within [minR,1]", "[Random]") {
  Random r(54);

  const Degree ang(180.0f);
  const float thickness = 0.3f;
  const float minR = 1.0f - thickness;

  for (int i = 0; i < 10000; ++i) {
    const Vector2 p = r.getPointInArcShell(ang, thickness);

    const float d = std::sqrt(p.sizeSquared());
    REQUIRE(d >= minR - 1e-3f);
    REQUIRE(d <= 1.0f + 1e-3f);

    const float theta = std::atan2(p.y, p.x);
    REQUIRE(theta >= -1e-4f);
    REQUIRE(theta <= ang.valueRadians() + 1e-4f);
  }
}

TEST_CASE("Random: getBarycentric produces u,v,w in [0,1] with sum 1", "[Random]") {
  Random r(55);

  for (int i = 0; i < 20000; ++i) {
    const Vector3 b = r.getBarycentric();
    REQUIRE(b.x >= 0.0f);
    REQUIRE(b.y >= 0.0f);
    REQUIRE(b.z >= 0.0f);
    REQUIRE(b.x <= 1.0f);
    REQUIRE(b.y <= 1.0f);
    REQUIRE(b.z <= 1.0f);

    const float sum = b.x + b.y + b.z;
    REQUIRE(nearly(sum, 1.0f, 1e-4f));
  }
}

TEST_CASE("Random: setSeed resets sequence", "[Random]") {
  Random r(123);

  const uint32 a0 = r.get();
  const uint32 a1 = r.get();

  r.setSeed(123);

  const uint32 b0 = r.get();
  const uint32 b1 = r.get();

  REQUIRE(a0 == b0);
  REQUIRE(a1 == b1);
}
