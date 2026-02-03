#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "geDegree.h"
#include "geRadian.h"
#include "geMath.h"

using namespace geEngineSDK;
using Catch::Approx;

namespace {
  inline void requireNear(float a, float b, float eps = 1e-6f) {
    REQUIRE(std::fabs(a - b) <= eps);
  }
}

TEST_CASE("Angles: basic construction and accessors", "[Math][Angle]") {
  Degree d(90.0f);
  REQUIRE(d.valueDegrees() == Approx(90.0f));

  Radian r(Math::PI);
  REQUIRE(r.valueRadians() == Approx(Math::PI));

  //Construct from other type
  Radian r2(d);
  requireNear(r2.valueRadians(), Math::PI * 0.5f, 1e-6f);

  Degree d2(r);
  requireNear(d2.valueDegrees(), 180.0f, 1e-5f);
}

TEST_CASE("Angles: conversion invariants (360 <-> 2PI)", "[Math][Angle]") {
  Degree d360(360.0f);
  Radian r360(d360);
  requireNear(r360.valueRadians(), Math::TWO_PI, 1e-5f);

  Radian r2pi(Math::TWO_PI);
  Degree d2pi(r2pi);
  requireNear(d2pi.valueDegrees(), 360.0f, 1e-4f);
}

TEST_CASE("Angles: arithmetic operators", "[Math][Angle]") {
  Degree a(30.0f), b(15.0f);

  Degree c = a + b;
  requireNear(c.valueDegrees(), 45.0f);

  c = a - b;
  requireNear(c.valueDegrees(), 15.0f);

  c = a * 2.0f;
  requireNear(c.valueDegrees(), 60.0f);

  c = a / 2.0f;
  requireNear(c.valueDegrees(), 15.0f);

  a += Degree(10.0f);
  requireNear(a.valueDegrees(), 40.0f);

  a -= Degree(5.0f);
  requireNear(a.valueDegrees(), 35.0f);

  a *= 2.0f;
  requireNear(a.valueDegrees(), 70.0f);

  a /= 2.0f;
  requireNear(a.valueDegrees(), 35.0f);
}

TEST_CASE("Angles: comparisons", "[Math][Angle]")
{
  REQUIRE(Degree(10.0f) < Degree(11.0f));
  REQUIRE(Degree(10.0f) <= Degree(10.0f));
  REQUIRE(Degree(10.0f) == Degree(10.0f));
  REQUIRE(Degree(10.0f) != Degree(9.0f));
  REQUIRE(Degree(11.0f) > Degree(10.0f));
  REQUIRE(Degree(11.0f) >= Degree(11.0f));

  REQUIRE(Radian(1.0f) < Radian(2.0f));
  REQUIRE(Radian(2.0f) > Radian(1.0f));
}

TEST_CASE("Angles: cross-type operations sanity (Degree + Radian via conversion)",\
          "[Math][Angle]") {
  Degree d(90.0f);
  Radian r = Radian(d);

  //90° == PI/2
  requireNear(r.valueRadians(), Math::PI * 0.5f, 1e-6f);

  //Add 90° + 90° -> 180°
  Degree d2 = Degree(Radian(d) + Radian(d));
  requireNear(d2.valueDegrees(), 180.0f, 1e-4f);
}

TEST_CASE("Angles: unwindDegrees range [-180, 180]", "[Math][Angle][Unwind]") {
  const float samples[] = {
    -1080.0f, -720.0f, -540.0f, -360.0f, -270.0f, -181.0f,
    -180.0f, -90.0f, 0.0f, 90.0f, 180.0f, 181.0f, 270.0f, 360.0f, 540.0f, 720.0f
  };

  for (float d : samples) {
    float u = Math::unwindDegrees(d);
    REQUIRE(u >= -180.0f);
    REQUIRE(u <= 180.0f);
  }
}

TEST_CASE("Angles: unwindDegrees modulo 360 equivalence", "[Math][Angle][Unwind]") {
  const float base = 37.5f;

  for (int k = -5; k <= 5; ++k) {
    float d = base + 360.0f * k;
    float u = Math::unwindDegrees(d);
    float ref = Math::unwindDegrees(base);

    REQUIRE(std::fabs(u - ref) < 1e-5f);
  }
}

TEST_CASE("Angles: unwindDegrees boundary cases", "[Math][Angle][Unwind]") {
  REQUIRE(Math::unwindDegrees(180.0f) == Approx(180.0f));
  REQUIRE(Math::unwindDegrees(-180.0f) == Approx(-180.0f));

  REQUIRE(Math::unwindDegrees(540.0f) == Approx(180.0f));
  REQUIRE(Math::unwindDegrees(-540.0f) == Approx(-180.0f));

  REQUIRE(Math::unwindDegrees(360.0f) == Approx(0.0f));
  REQUIRE(Math::unwindDegrees(-360.0f) == Approx(0.0f));
}

TEST_CASE("Angles: unwindRadians range [-PI, PI]", "[Math][Angle][Unwind]") {
  const float PI = Math::PI;

  const float samples[] = {
    -10 * PI, -5 * PI, -3 * PI, -2 * PI, -1.5f * PI, -PI, -0.5f * PI,
     0.0f,
     0.5f * PI, PI, 1.5f * PI, 2 * PI, 3 * PI, 5 * PI, 10 * PI
  };

  for (float r : samples) {
    float u = Math::unwindRadians(r);
    REQUIRE(u >= -PI);
    REQUIRE(u <= PI);
  }
}

TEST_CASE("Angles: unwindRadians modulo 2PI equivalence", "[Math][Angle][Unwind]") {
  const float base = 0.7f;
  const float TWO_PI = Math::TWO_PI;

  float ref = Math::unwindRadians(base);

  auto sameAngleTrig = [](float a, float b) {
    float da = std::fabs(std::cos(a) - std::cos(b));
    float db = std::fabs(std::sin(a) - std::sin(b));
    return da < 1e-4f && db < 1e-4f;
    };

  for (int k = -5; k <= 5; ++k) {
    float r = base + TWO_PI * k;
    float u = Math::unwindRadians(r);
    REQUIRE(sameAngleTrig(u, ref));
  }
}

TEST_CASE("Angles: unwindRadians boundary cases", "[Math][Angle][Unwind]") {
  const float PI = Math::PI;

  REQUIRE(Math::unwindRadians(PI) == Approx(PI));
  REQUIRE(Math::unwindRadians(-PI) == Approx(-PI));

  REQUIRE(Math::unwindRadians(3 * PI) == Approx(PI));
  REQUIRE(Math::unwindRadians(-3 * PI) == Approx(-PI));

  REQUIRE(Math::unwindRadians(2 * PI) == Approx(0.0f));
  REQUIRE(Math::unwindRadians(-2 * PI) == Approx(0.0f));
}
