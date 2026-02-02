#include <catch_amalgamated.hpp>
#include <random>

#include "geFloat32.h"
#include "geFloat16.h"
#include "geFloat10.h"
#include "geFloat11.h"
#include "geFloat16Color.h"

using namespace geEngineSDK;

namespace{
  inline void
  requireNear(float a, float b, float absTol) {
    REQUIRE(std::fabs(a - b) <= absTol);
  }

  inline void
  requireFinite(float v) {
    REQUIRE(std::isfinite(v));
  }

  inline float
  ldexpf_safe(float x, int e) {
    return std::ldexp(x, e);
  }

  inline std::mt19937&
  rng() {
    static auto seed = Catch::getSeed();
    static std::mt19937 r(seed);
    return r;
  }

  inline float
  randFloat(float lo, float hi) {
    std::uniform_real_distribution<float> d(lo, hi);
    return d(rng());
  }

  inline void
  requireNearRelAbs(float a, float b, float relTol, float absTol) {
    float diff = std::fabs(a - b);
    float bound = std::max(absTol, relTol * std::max(std::fabs(a), std::fabs(b)));
    REQUIRE(diff <= bound);
  }
}

TEST_CASE("Float32: IEEE layout sanity", "[Float][Float32]") {
  Float32 f1(1.0f);
  REQUIRE(f1.sign() == 0u);
  REQUIRE(f1.exponent() == 127u);
  REQUIRE(f1.mantissa() == 0u);

  Float32 fn(-0.0f);
  REQUIRE(fn.sign() == 1u);
  REQUIRE(fn.exponent() == 0u);
  REQUIRE(fn.mantissa() == 0u);

  Float32 inf(std::numeric_limits<float>::infinity());
  REQUIRE(inf.exponent() == 255u);
  REQUIRE(inf.mantissa() == 0u);

  Float32 nan(std::numeric_limits<float>::quiet_NaN());
  REQUIRE(nan.exponent() == 255u);
  REQUIRE(nan.mantissa() != 0u);
}

TEST_CASE("Float16: roundtrip accuracy", "[Float][Float16]")
{
  const float vals[] = {
    0.0f, -0.0f, 1.0f, -1.0f, 0.5f, 2.0f, 10.0f, -10.0f, 100.0f, -100.0f
  };

  for (float v : vals)
  {
    Float16 h(v);
    float back = (float)h;

    float tol = (std::fabs(v) <= 10.0f) ? 1e-3f : 1e-2f;
    requireNear(back, v, tol);
  }
}

TEST_CASE("Float16: sign, inf and NaN", "[Float][Float16]") {
  Float16 pz(0.0f), nz(-0.0f);
  REQUIRE(Float32((float)pz).sign() == 0u);
  REQUIRE(Float32((float)nz).sign() == 1u);

  Float16 pinf(std::numeric_limits<float>::infinity());
  REQUIRE(std::isinf((float)pinf));

  Float16 ninf(-std::numeric_limits<float>::infinity());
  REQUIRE(std::isinf((float)ninf));

  Float16 qnan(std::numeric_limits<float>::quiet_NaN());
  REQUIRE(std::isnan((float)qnan));
}

TEST_CASE("Float10/Float11: monotonic and finite", "[Float][Float10][Float11]") {
  float prev10 = 0.0f;
  float prev11 = 0.0f;

  for (int i = 0; i <= 1000; ++i)
  {
    float v = i * 0.01f; // 0 .. 10

    Float10 f10(v);
    Float11 f11(v);

    float b10 = (float)f10;
    float b11 = (float)f11;

    requireFinite(b10);
    requireFinite(b11);

    REQUIRE(b10 >= prev10 - 1e-6f);
    REQUIRE(b11 >= prev11 - 1e-6f);

    prev10 = b10;
    prev11 = b11;
  }
}

TEST_CASE("Cross conversion: Float32 -> Float16 -> Float10/11 stability", "[Float][Cross]") {
  const float vals[] = { 0.0f, 0.1f, 0.5f, 1.0f, 5.0f, 10.0f, 100.0f };

  for (float v : vals) {
    Float16 h(v);
    float hv = (float)h;

    Float10 f10(hv);
    Float11 f11(hv);

    float b10 = (float)f10;
    float b11 = (float)f11;

    REQUIRE(std::isfinite(b10));
    REQUIRE(std::isfinite(b11));

    //Shouldn't explore out of degenerate range
    REQUIRE(b10 >= 0.0f);
    REQUIRE(b11 >= 0.0f);

    REQUIRE(std::fabs(b10 - v) < std::max(0.5f, v * 0.1f));
    REQUIRE(std::fabs(b11 - v) < std::max(0.5f, v * 0.1f));
  }
}

TEST_CASE("Float16Color: RGB roundtrip", "[Float][Float16Color]") {
  struct C { float r, g, b; };
  const C colors[] = {
    {0,0,0}, {1,1,1}, {0.5f, 0.25f, 0.75f}, {10,20,30}
  };

  for (auto& c : colors) {
    Float16Color fc(c.r, c.g, c.b);

    float r = fc.r;
    float g = fc.g;
    float b = fc.b;

    requireFinite(r);
    requireFinite(g);
    requireFinite(b);

    REQUIRE(std::fabs(r - c.r) < std::max(0.01f, c.r * 0.05f));
    REQUIRE(std::fabs(g - c.g) < std::max(0.01f, c.g * 0.05f));
    REQUIRE(std::fabs(b - c.b) < std::max(0.01f, c.b * 0.05f));
  }
}

TEST_CASE("Float16: exact bit patterns for common values", "[Float][Float16][Bits]")
{
  // Exact encodings for IEEE-754 half:
  // +0: 0x0000, -0: 0x8000, 1:0x3C00, 2:0x4000, -2:0xC000, +inf:0x7C00, qNaN:0x7E00-ish
  Float16 pz(0x0000u);
  Float16 nz(0x8000u);
  Float16 one(0x3C00u);
  Float16 two(0x4000u);
  Float16 ntwo(0xC000u);
  Float16 pinf(0x7C00u);

  REQUIRE(Float32((float)pz).sign() == 0u);
  REQUIRE(Float32((float)nz).sign() == 1u);

  REQUIRE((float)one == 1.0f);
  REQUIRE((float)two == 2.0f);
  REQUIRE((float)ntwo == -2.0f);

  REQUIRE(std::isinf((float)pinf));
  REQUIRE(Float16(pinf).exponent() == 0x1Fu);
  REQUIRE(Float16(pinf).mantissa() == 0u);
}

TEST_CASE("Float16: boundary values (min subnormal, min normal, max finite)", "[Float][Float16][Bounds]")
{
  // Smallest positive subnormal half = 2^-24
  const float minSub = ldexpf_safe(1.0f, -24);
  Float16 hMinSub(0x0001u);
  requireNear((float)hMinSub, minSub, minSub * 0.001f);

  // Largest subnormal = (1023/1024) * 2^-14
  Float16 hMaxSub(0x03FFu);
  float maxSub = (1023.0f / 1024.0f) * ldexpf_safe(1.0f, -14);
  requireNearRelAbs((float)hMaxSub, maxSub, 0.0f, 1e-10f);

  // Smallest normal half = 2^-14
  Float16 hMinNorm(0x0400u);
  float minNorm = ldexpf_safe(1.0f, -14);
  requireNearRelAbs((float)hMinNorm, minNorm, 0.0f, 1e-10f);

  // Max finite half = 65504
  Float16 hMaxFinite(0x7BFFu);
  requireNearRelAbs((float)hMaxFinite, 65504.0f, 0.0f, 1e-3f);
}

TEST_CASE("Float16: overflow clamps to max finite (not inf)", "[Float][Float16][Clamp]")
{
  Float16 hBig(1e20f);
  float b = (float)hBig;

  REQUIRE(std::isfinite(b));
  // Your set() clamps (fp32Exp >= 143) to exp=30 mant=1023 (max finite)
  requireNearRelAbs(b, 65504.0f, 0.0f, 1e-3f);
}

TEST_CASE("Float16: underflow to zero for tiny values", "[Float][Float16][Underflow]")
{
  // Much smaller than min subnormal half
  Float16 hTiny(ldexpf_safe(1.0f, -40));
  float b = (float)hTiny;

  REQUIRE(b == 0.0f);
  // sign should be preserved for negative tiny
  Float16 hTinyNeg(-ldexpf_safe(1.0f, -40));
  float bn = (float)hTinyNeg;
  REQUIRE(bn == 0.0f);
  REQUIRE(Float32(bn).sign() == 1u); // should become -0 if sign preserved
}

TEST_CASE("Float16: NaN payload stays NaN and mantissa non-zero", "[Float][Float16][NaN]")
{
  // Build a NaN with a payload via Float32 bits
  // (quiet NaN: exp all ones, mantissa non-zero)
  Float32 nanBits(0x7FC12345u);
  Float16 hnan((float)nanBits);

  float b = (float)hnan;
  REQUIRE(std::isnan(b));
  REQUIRE(hnan.exponent() == 0x1Fu);
  REQUIRE(hnan.mantissa() != 0u);
}

TEST_CASE("Float16: roundtrip random finite values (tolerances)", "[Float][Float16][Random]")
{
  // Half has ~10 mantissa bits; relative precision around 2^-10 (~0.000976) for normals
  for (int i = 0; i < 5000; ++i)
  {
    float v = randFloat(-70000.0f, 70000.0f);
    // skip NaN/inf generation
    if (!std::isfinite(v)) continue;

    Float16 h(v);
    float b = (float)h;

    // If |v| > 65504 it clamps; otherwise expect reasonable relative error
    if (std::fabs(v) > 65504.0f)
    {
      REQUIRE(std::isfinite(b));
      REQUIRE(std::fabs(b) <= 65504.0f + 1e-3f);
    }
    else
    {
      // conservative: 0.2% relative or 1e-3 absolute
      requireNearRelAbs(b, v, 0.002f, 1e-3f);
    }
  }
}

TEST_CASE("Float10: basic layout invariants and special cases", "[Float][Float10][Bits]")
{
  // Negative values clamp to 0 (including -0 and -inf)
  Float10 n1(-1.0f);
  REQUIRE((float)n1 == 0.0f);

  Float10 nz(-0.0f);
  REQUIRE((float)nz == 0.0f);
  REQUIRE(Float32((float)nz).sign() == 0u);

  Float10 ninf(-std::numeric_limits<float>::infinity());
  REQUIRE((float)ninf == 0.0f);

  // +inf -> exp=31 mant=0 pattern (encoded = 992 in your code)
  Float10 pinf(std::numeric_limits<float>::infinity());
  REQUIRE(pinf.exponent() == 31u);
  REQUIRE(pinf.mantissa() == 0u);
  REQUIRE(std::isinf((float)pinf));

  // NaN -> exp=31 mant!=0
  Float10 qnan(std::numeric_limits<float>::quiet_NaN());
  REQUIRE(qnan.exponent() == 31u);
  REQUIRE(qnan.mantissa() != 0u);
  REQUIRE(std::isnan((float)qnan));

  // Max finite encoded used by your clamp path is 991
  Float10 maxEnc(991u);
  REQUIRE(maxEnc.exponent() == 30u);
  REQUIRE(maxEnc.mantissa() == 31u);
  REQUIRE(std::isfinite((float)maxEnc));
}

TEST_CASE("Float11: basic layout invariants and special cases", "[Float][Float11][Bits]")
{
  // Negative values clamp to 0
  Float11 n1(-1.0f);
  REQUIRE((float)n1 == 0.0f);

  Float11 nz(-0.0f);
  REQUIRE((float)nz == 0.0f);
  REQUIRE(Float32((float)nz).sign() == 0u);

  Float11 ninf(-std::numeric_limits<float>::infinity());
  REQUIRE((float)ninf == 0.0f);

  // +inf -> exp=31 mant=0 pattern (encoded = 1984 in your code)
  Float11 pinf(std::numeric_limits<float>::infinity());
  REQUIRE(pinf.exponent() == 31u);
  REQUIRE(pinf.mantissa() == 0u);
  REQUIRE(std::isinf((float)pinf));

  // NaN -> exp=31 mant!=0
  Float11 qnan(std::numeric_limits<float>::quiet_NaN());
  REQUIRE(qnan.exponent() == 31u);
  REQUIRE(qnan.mantissa() != 0u);
  REQUIRE(std::isnan((float)qnan));

  // Max finite encoded used by your clamp path is 1983
  Float11 maxEnc(1983u);
  REQUIRE(maxEnc.exponent() == 30u);
  REQUIRE(maxEnc.mantissa() == 63u);
  REQUIRE(std::isfinite((float)maxEnc));
}

TEST_CASE("Float10/Float11: clamps large finite (not inf) and stays non-negative", "[Float][Float10][Float11][Clamp]")
{
  Float10 f10(1e20f);
  Float11 f11(1e20f);

  float b10 = (float)f10;
  float b11 = (float)f11;

  REQUIRE(std::isfinite(b10));
  REQUIRE(std::isfinite(b11));
  REQUIRE(b10 >= 0.0f);
  REQUIRE(b11 >= 0.0f);

  // Should clamp to their max finite encodings
  REQUIRE(f10.integerValue() == 991u);
  REQUIRE(f11.integerValue() == 1983u);
}

TEST_CASE("Float10/Float11: subnormal behavior (encoded smallest non-zero > 0)", "[Float][Float10][Float11][Subnormal]")
{
  Float10 s10(1u);
  Float11 s11(1u);

  float b10 = (float)s10;
  float b11 = (float)s11;

  REQUIRE(b10 > 0.0f);
  REQUIRE(b11 > 0.0f);
  REQUIRE(std::isfinite(b10));
  REQUIRE(std::isfinite(b11));
}

TEST_CASE("Float10/Float11: monotonic over wide range", "[Float][Float10][Float11][Monotonic]")
{
  // Use their computed max finite values as upper bounds
  const float max10 = (float)Float10(991u);
  const float max11 = (float)Float11(1983u);

  float prev10 = 0.0f;
  float prev11 = 0.0f;

  // Scan 0..max with a step that triggers rounding and exponent changes
  for (int i = 0; i <= 20000; ++i)
  {
    float t = i / 20000.0f;
    float v10 = t * max10;
    float v11 = t * max11;

    Float10 f10(v10);
    Float11 f11(v11);

    float b10 = (float)f10;
    float b11 = (float)f11;

    requireFinite(b10);
    requireFinite(b11);

    REQUIRE(b10 >= prev10 - 1e-6f);
    REQUIRE(b11 >= prev11 - 1e-6f);

    prev10 = b10;
    prev11 = b11;
  }
}

TEST_CASE("Float10/Float11: random roundtrip stays finite and within coarse relative error", "[Float][Float10][Float11][Random]")
{
  // Relative precision: Float10 ~ 5 mantissa bits => ~3% step; Float11 ~ 6 bits => ~1.5% step
  const float max10 = (float)Float10(991u);
  const float max11 = (float)Float11(1983u);

  for (int i = 0; i < 8000; ++i)
  {
    float v10 = randFloat(0.0f, max10);
    float v11 = randFloat(0.0f, max11);

    Float10 f10(v10);
    Float11 f11(v11);

    float b10 = (float)f10;
    float b11 = (float)f11;

    requireFinite(b10);
    requireFinite(b11);

    REQUIRE(b10 >= 0.0f);
    REQUIRE(b11 >= 0.0f);

    // Very conservative error bounds (these formats are super low precision)
    requireNearRelAbs(b10, v10, 0.06f, 1e-6f); // 6% rel
    requireNearRelAbs(b11, v11, 0.03f, 1e-6f); // 3% rel
  }
}

TEST_CASE("Float10/Float11: integerValue masks upper bits", "[Float][Float10][Float11][Mask]")
{
  // Constructors take uint32 encoded; integerValue should mask to 10/11 bits.
  Float10 f10(0xFFFFFFFFu);
  Float11 f11(0xFFFFFFFFu);

  REQUIRE(f10.integerValue() == 0x3FFu);
  REQUIRE(f11.integerValue() == 0x7FFu);
}
