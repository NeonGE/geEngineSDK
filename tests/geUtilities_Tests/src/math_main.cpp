#include <catch2/catch_test_macros.hpp>

#include "geMath.h"
using namespace geEngineSDK;

namespace {
  inline void
  requireNear(float a, float b, float rel = 1e-5f, float abs = 1e-6f) {
    REQUIRE(std::fabs(a - b) <= abs + rel * std::fabs(b));
  }

  inline void
  requireNearRad(const Radian& a, float bRadians, float absTol) {
    REQUIRE(std::fabs(a.valueRadians() - bRadians) <= absTol);
  }
}

TEST_CASE("Math: basic rounding functions", "[Math]") {
  REQUIRE(Math::trunc(1.9f) == 1);
  REQUIRE(Math::trunc(-1.9f) == -1);

  REQUIRE(Math::floor(1.9f) == 1);
  REQUIRE(Math::floor(-1.1f) == -2);

  REQUIRE(Math::ceil(1.1f) == 2);
  REQUIRE(Math::ceil(-1.9f) == -1);

  REQUIRE(Math::round(1.4f) == 1);
  REQUIRE(Math::round(1.5f) == 2);
  REQUIRE(Math::round(-1.5f) == -2);  //std::round: halfway away from zero
}

TEST_CASE("Math: fractional", "[Math]") {
  requireNear(Math::fractional(3.25f), 0.25f);
  requireNear(Math::fractional(-3.25f), -0.25f);  //sign is preserved
}

TEST_CASE("Math: fmod/pow/exp/log", "[Math]") {
  requireNear(Math::fmod(5.5f, 2.0f), std::fmod(5.5f, 2.0f));

  requireNear(Math::pow(2.0f, 3.0f), 8.0f);
  requireNear(Math::exp(1.0f), std::exp(1.0f), 1e-5f, 1e-6f);

  requireNear(Math::logE(10.0f), std::log(10.0f), 1e-5f, 1e-6f);
  requireNear(Math::logX(10.0f, 1000.0f), 3.0f, 1e-5f, 1e-6f);
}

TEST_CASE("Math: sqrt/invSqrt/carmackInvSqrt", "[Math]") {
  requireNear(Math::sqrt(9.0f), 3.0f);

  //invSqrt exact (1/sqrt)
  requireNear(Math::invSqrt(4.0f), 0.5f);

  //carmackInvSqrt its an approximation
  float v = 10.0f;
  float ref = 1.0f / std::sqrt(v);
  float approx = Math::carmackInvSqrt(v);
  REQUIRE(std::fabs(approx - ref) < 0.01f); // ~1% tolerancia
}

TEST_CASE("Math: isNaN/isFinite", "[Math]") {
  float nanv = std::numeric_limits<float>::quiet_NaN();
  float infv = std::numeric_limits<float>::infinity();
  float ninf = -std::numeric_limits<float>::infinity();

  REQUIRE(Math::isNaN(nanv));
  REQUIRE_FALSE(Math::isNaN(1.0f));

  REQUIRE_FALSE(Math::isFinite(infv));
  REQUIRE_FALSE(Math::isFinite(ninf));
  REQUIRE(Math::isFinite(123.0f));
}

TEST_CASE("Math: sign/abs", "[Math]") {
  REQUIRE(Math::sign(-3) == -1);
  REQUIRE(Math::sign(0) == 0);
  REQUIRE(Math::sign(7) == 1);

  REQUIRE(Math::abs(-5) == 5);
  requireNear(Math::abs(-5.25f), 5.25f);
}

TEST_CASE("Math: min/max (2 args, 3 args, variadic common_type)", "[Math]")
{
  REQUIRE(Math::min(3, 7) == 3);
  REQUIRE(Math::max(3, 7) == 7);

  REQUIRE(Math::min3(3, 7, -2) == -2);
  REQUIRE(Math::max3(3, 7, -2) == 7);

  //Variadic with common_type
  auto m = Math::min(3, 7.0f, -2);
  auto M = Math::max(3, 7.0f, -2);
  requireNear((float)m, -2.0f);
  requireNear((float)M, 7.0f);
}

TEST_CASE("Math: clamp/clamp01", "[Math]") {
  REQUIRE(Math::clamp(5, 0, 10) == 5);
  REQUIRE(Math::clamp(-1, 0, 10) == 0);
  REQUIRE(Math::clamp(11, 0, 10) == 10);

  requireNear(Math::clamp01(-0.2f), 0.0f);
  requireNear(Math::clamp01(0.3f), 0.3f);
  requireNear(Math::clamp01(1.2f), 1.0f);
}

TEST_CASE("Math: lerp/invLerp", "[Math]") {
  requireNear(Math::lerp(0.0f, 10.0f, 0.0f), 0.0f);
  requireNear(Math::lerp(0.0f, 10.0f, 1.0f), 10.0f);
  requireNear(Math::lerp(0.0f, 10.0f, 0.25f), 2.5f);

  //invLerp clamp01
  requireNear(Math::invLerp(5.0f, 0.0f, 10.0f), 0.5f);
  requireNear(Math::invLerp(-5.0f, 0.0f, 10.0f), 0.0f);
  requireNear(Math::invLerp(15.0f, 0.0f, 10.0f), 1.0f);

  //degenerate range: use max(_max-_min, 0.0001)
  float t = Math::invLerp(10.0f, 1.0f, 1.0f);
  REQUIRE(t >= 0.0f);
  REQUIRE(t <= 1.0f);
}

TEST_CASE("Math: cubicInterp (scalar)", "[Math]") {
  //Basic property: on A=0 returns P0, on A=1 returns P1
  float P0 = 2.0f, P1 = 10.0f;
  float T0 = 0.0f, T1 = 0.0f;

  requireNear(Math::cubicInterp(P0, T0, P1, T1, 0.0f), P0);
  requireNear(Math::cubicInterp(P0, T0, P1, T1, 1.0f), P1);
}

TEST_CASE("Math: divideAndRoundUp/divideAndRoundDown", "[Math]") {
  REQUIRE(Math::divideAndRoundDown(10u, 3u) == 3u);
  REQUIRE(Math::divideAndRoundUp(10u, 3u) == 4u);

  REQUIRE(Math::divideAndRoundUp(9u, 3u) == 3u);
  REQUIRE(Math::divideAndRoundDown(9u, 3u) == 3u);
}

TEST_CASE("Math: repeat/pingPong", "[Math]") {
  //Repeat: [0, length)
  requireNear(Math::repeat(0.0f, 2.0f), 0.0f);
  requireNear(Math::repeat(1.0f, 2.0f), 1.0f);
  requireNear(Math::repeat(2.0f, 2.0f), 0.0f);
  requireNear(Math::repeat(3.5f, 2.0f), 1.5f);

  //PingPong: up and down between [0, length]
  requireNear(Math::pingPong(0.0f, 2.0f), 0.0f);
  requireNear(Math::pingPong(1.0f, 2.0f), 1.0f);
  requireNear(Math::pingPong(2.0f, 2.0f), 2.0f);
  requireNear(Math::pingPong(3.0f, 2.0f), 1.0f);
  requireNear(Math::pingPong(4.0f, 2.0f), 0.0f);
}

TEST_CASE("Math: isPowerOfTwo", "[Math]") {
  REQUIRE(Math::isPowerOfTwo(1));
  REQUIRE(Math::isPowerOfTwo(2));
  REQUIRE(Math::isPowerOfTwo(4));
  REQUIRE(Math::isPowerOfTwo(1024));

  REQUIRE_FALSE(Math::isPowerOfTwo(3));
  REQUIRE_FALSE(Math::isPowerOfTwo(6));
  REQUIRE_FALSE(Math::isPowerOfTwo(0));
}

TEST_CASE("Math: gridSnap", "[Math]") {
  requireNear(Math::gridSnap(0.0f, 10.0f), 0.0f);
  requireNear(Math::gridSnap(4.9f, 10.0f), 0.0f);
  requireNear(Math::gridSnap(5.1f, 10.0f), 10.0f);
  requireNear(Math::gridSnap(15.0f, 10.0f), 20.0f);

  //grid 0 => retorna igual
  requireNear(Math::gridSnap(12.34f, 0.0f), 12.34f);
}

TEST_CASE("Math: sin_cos matches std::sin/cos", "[Math]") {
  float angles[] = {
    0.0f, 0.1f, 0.5f, 1.0f,
    Math::PI * 0.25f,
    Math::PI * 0.5f,
    Math::PI,
    -0.25f * Math::PI,
    2.0f * Math::PI + 0.3f
  };

  for (float a : angles) {
    float s = 0.0f, c = 0.0f;
    Math::sin_cos(&s, &c, a);

    REQUIRE(std::fabs(s - std::sin(a)) < 1e-3f);
    REQUIRE(std::fabs(c - std::cos(a)) < 1e-3f);

    REQUIRE(std::fabs((s * s + c * c) - 1.0f) < 2e-3f);
  }
}

TEST_CASE("Math: unwindDegrees/unwindRadians ranges", "[Math]") {
  //Degrees in [-180, 180]
  float d = Math::unwindDegrees(540.0f);  // 540 -> 180
  REQUIRE(d <= 180.0f);
  REQUIRE(d >= -180.0f);
  requireNear(d, 180.0f);

  d = Math::unwindDegrees(-540.0f); // -540 -> -180
  REQUIRE(d <= 180.0f);
  REQUIRE(d >= -180.0f);
  requireNear(d, -180.0f);

  //Radians in [-PI, PI]
  float r = Math::unwindRadians(3.0f * Math::PI); // 3pi -> pi
  REQUIRE(r <= Math::PI);
  REQUIRE(r >= -Math::PI);
  requireNear(r, Math::PI, 1e-6f, 1e-6f);

  r = Math::unwindRadians(-3.0f * Math::PI); // -3pi -> -pi
  REQUIRE(r <= Math::PI);
  REQUIRE(r >= -Math::PI);
  requireNear(r, -Math::PI, 1e-6f, 1e-6f);
}

TEST_CASE("Math: solveLinear/solveQuadratic (real roots)", "[Math]") {
  {
    float roots[2] = {};
    auto n = Math::solveLinear(2.0f, -4.0f, roots); // 2x - 4 = 0 => x=2
    REQUIRE(n == 1);
    requireNear(roots[0], 2.0f);
  }

  {
    float roots[2] = {};
    auto n = Math::solveQuadratic(1.0f, 0.0f, -4.0f, roots); // x^2 - 4=0 => -2, 2
    REQUIRE(n == 2);
    
    //Orden may vary, validate as a set
    bool ok = ((std::fabs(roots[0] - 2.0f) < 1e-4f &&
                std::fabs(roots[1] + 2.0f) < 1e-4f) ||
               (std::fabs(roots[1] - 2.0f) < 1e-4f &&
                std::fabs(roots[0] + 2.0f) < 1e-4f));
    REQUIRE(ok);
  }
}

TEST_CASE("Math: floorLog2", "[Math][BitOps]") {
  REQUIRE(Math::floorLog2(1) == 0);
  REQUIRE(Math::floorLog2(2) == 1);
  REQUIRE(Math::floorLog2(3) == 1);
  REQUIRE(Math::floorLog2(4) == 2);
  REQUIRE(Math::floorLog2(7) == 2);
  REQUIRE(Math::floorLog2(8) == 3);
  REQUIRE(Math::floorLog2(1024) == 10);
}

TEST_CASE("Math: countLeadingZeros", "[Math][BitOps]") {
  REQUIRE(Math::countLeadingZeros(uint32(0x80000000)) == 0);
  REQUIRE(Math::countLeadingZeros(uint32(0x40000000)) == 1);
  REQUIRE(Math::countLeadingZeros(uint32(0x00000001)) == 31);
}

TEST_CASE("Math: countTrailingZeros", "[Math][BitOps]") {
  REQUIRE(Math::countTrailingZeros(uint32(1)) == 0);
  REQUIRE(Math::countTrailingZeros(uint32(2)) == 1);
  REQUIRE(Math::countTrailingZeros(uint32(4)) == 2);
  REQUIRE(Math::countTrailingZeros(uint32(8)) == 3);
  REQUIRE(Math::countTrailingZeros(uint32(0x80000000)) == 31);
}

TEST_CASE("Math: roundUpToPowerOfTwo", "[Math][BitOps]") {
  REQUIRE(Math::roundUpToPowerOfTwo(1) == 1);
  REQUIRE(Math::roundUpToPowerOfTwo(2) == 2);
  REQUIRE(Math::roundUpToPowerOfTwo(3) == 4);
  REQUIRE(Math::roundUpToPowerOfTwo(5) == 8);
  REQUIRE(Math::roundUpToPowerOfTwo(17) == 32);
}

TEST_CASE("Math trig: sin/cos/tan wrappers match std (spot checks)", "[Math][Trig]") {
  const float angles[] = {
    0.0f,
    0.1f,
    0.5f,
    1.0f,
    Math::PI * 0.25f,
    Math::PI * 0.5f,
    -Math::PI * 0.25f
  };

  for (float a : angles) {
    requireNear(Math::sin(a), std::sin(a), 1e-6f);
    requireNear(Math::cos(a), std::cos(a), 1e-6f);

    //Tan becomes too sensitive near PI/2
    if (std::fabs(std::cos(a)) > 0.2f) {
      requireNear(Math::tan(a), std::tan(a), 1e-6f);
    }
  }
}

TEST_CASE("Math trig: acos clamp behavior", "[Math][Trig]") {
  //Value >= 1 => 0
  //Value <= -1 => PI
  //else => std::acos(Value)

  requireNearRad(Math::acos(1.0f), 0.0f, 1e-7f);
  requireNearRad(Math::acos(2.0f), 0.0f, 1e-7f);

  requireNearRad(Math::acos(-1.0f), Math::PI, 1e-7f);
  requireNearRad(Math::acos(-2.0f), Math::PI, 1e-7f);

  //Value in range
  requireNearRad(Math::acos(0.5f), std::acos(0.5f), 1e-6f);
  requireNearRad(Math::acos(0.0f), std::acos(0.0f), 1e-6f);
}

TEST_CASE("Math trig: asin clamp behavior", "[Math][Trig]") {
  //Value >= 1 => +HALF_PI
  //Value <= -1 => -HALF_PI
  //else => std::asin(Value)

  requireNearRad(Math::asin(1.0f), Math::HALF_PI, 1e-7f);
  requireNearRad(Math::asin(2.0f), Math::HALF_PI, 1e-7f);

  requireNearRad(Math::asin(-1.0f), -Math::HALF_PI, 1e-7f);
  requireNearRad(Math::asin(-2.0f), -Math::HALF_PI, 1e-7f);

  // valor dentro de rango
  requireNearRad(Math::asin(0.5f), std::asin(0.5f), 1e-6f);
  requireNearRad(Math::asin(0.0f), std::asin(0.0f), 1e-6f);
}

TEST_CASE("Math trig: atan/atan2 return radians consistent with std", "[Math][Trig]") {
  requireNearRad(Math::atan(1.0f), std::atan(1.0f), 1e-6f);
  requireNearRad(Math::atan(-1.0f), std::atan(-1.0f), 1e-6f);

  requireNearRad(Math::atan2(1.0f, 1.0f), std::atan2(1.0f, 1.0f), 1e-6f);
  requireNearRad(Math::atan2(-1.0f, 1.0f), std::atan2(-1.0f, 1.0f), 1e-6f);
  requireNearRad(Math::atan2(1.0f, -1.0f), std::atan2(1.0f, -1.0f), 1e-6f);
}

TEST_CASE("Math trig: fastSin0/fastSin1 accuracy in range [0, PI/2]", "[Math][Trig][Fast]") {
  const float samples[] = {
    0.0f,
    Math::PI * 0.125f,
    Math::PI * 0.25f,
    Math::PI * 0.375f,
    Math::PI * 0.5f
  };

  for (float a : samples) {
    float ref = std::sin(a);

    //fastSin0: wider tolerancia
    REQUIRE(std::fabs(Math::fastSin0(a) - ref) < 2e-3f);

    //fastSin1: must be better or similar to fastSin0
    REQUIRE(std::fabs(Math::fastSin1(a) - ref) < 1e-3f);
  }
}

TEST_CASE("Math trig: fastCos0/fastCos1 accuracy in range [0, PI/2]", "[Math][Trig][Fast]") {
  const float samples[] = {
    0.0f,
    Math::PI * 0.125f,
    Math::PI * 0.25f,
    Math::PI * 0.375f,
    Math::PI * 0.5f
  };

  for (float a : samples) {
    float ref = std::cos(a);
    REQUIRE(std::fabs(Math::fastCos0(a) - ref) < 2e-3f);
    REQUIRE(std::fabs(Math::fastCos1(a) - ref) < 1e-3f);
  }
}

TEST_CASE("Math trig: fastTan0/fastTan1 accuracy in range [0, PI/4]", "[Math][Trig][Fast]") {
  const float samples[] = {
    0.0f,
    Math::PI * 0.0625f,
    Math::PI * 0.125f,
    Math::PI * 0.25f
  };

  for (float a : samples) {
    float ref = std::tan(a);
    REQUIRE(std::fabs(Math::fastTan0(a) - ref) < 3e-3f);
    REQUIRE(std::fabs(Math::fastTan1(a) - ref) < 2e-3f);
  }
}

TEST_CASE("Math trig: fastAsin (Radian) clamps input and is accurate", "[Math][Trig][Fast]") {
  //Clamp
  REQUIRE(std::fabs(Math::fastAsin(2.0f).valueRadians() - Math::HALF_PI) < 2e-4f);
  REQUIRE(std::fabs(Math::fastAsin(-2.0f).valueRadians() + Math::HALF_PI) < 2e-4f);

  //Accuracy between [-1, 1]
  const float samples[] = { -1.0f, -0.5f, 0.0f, 0.5f, 1.0f };
  for (float x : samples) {
    float ref = std::asin(std::max(-1.0f, std::min(1.0f, x)));
    REQUIRE(std::fabs(Math::fastAsin(x).valueRadians() - ref) < 2e-3f);
  }
}

TEST_CASE("Math trig: fastASin0/1,\
          fastACos0/1,\
          fastATan0/1 basic accuracy in stated ranges",\
          "[Math][Trig][Fast]") {
  //ASin/ACos docs: input [0, 1]
  const float asinSamples[] = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };
  for (float x : asinSamples) {
    REQUIRE(std::fabs(Math::fastASin0(x) - std::asin(x)) < 3e-3f);
    REQUIRE(std::fabs(Math::fastASin1(x) - std::asin(x)) < 2e-3f);

    REQUIRE(std::fabs(Math::fastACos0(x) - std::acos(x)) < 3e-3f);
    REQUIRE(std::fabs(Math::fastACos1(x) - std::acos(x)) < 2e-3f);
  }

  //ATan docs: input [-1, 1]
  const float atanSamples[] = { -1.0f, -0.5f, 0.0f, 0.5f, 1.0f };
  for (float x : atanSamples) {
    REQUIRE(std::fabs(Math::fastATan0(x) - std::atan(x)) < 3e-3f);
    REQUIRE(std::fabs(Math::fastATan1(x) - std::atan(x)) < 2e-3f);
  }
}
