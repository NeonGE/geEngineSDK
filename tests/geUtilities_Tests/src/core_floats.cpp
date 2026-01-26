#include <catch_amalgamated.hpp>

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

TEST_CASE("Float16: sign, inf and NaN", "[Float][Float16]")
{
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

TEST_CASE("Float10/Float11: monotonic and finite", "[Float][Float10][Float11]")
{
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

TEST_CASE("Cross conversion: Float32 -> Float16 -> Float10/11 stability", "[Float][Cross]")
{
  const float vals[] = { 0.0f, 0.1f, 0.5f, 1.0f, 5.0f, 10.0f, 100.0f };

  for (float v : vals)
  {
    Float16 h(v);
    float hv = (float)h;

    Float10 f10(hv);
    Float11 f11(hv);

    float b10 = (float)f10;
    float b11 = (float)f11;

    REQUIRE(std::isfinite(b10));
    REQUIRE(std::isfinite(b11));

    // No deberían explotar fuera de rango grotesco
    REQUIRE(b10 >= 0.0f);
    REQUIRE(b11 >= 0.0f);

    // Error razonable acumulado
    REQUIRE(std::fabs(b10 - v) < std::max(0.5f, v * 0.1f));
    REQUIRE(std::fabs(b11 - v) < std::max(0.5f, v * 0.1f));
  }
}

TEST_CASE("Float16Color: RGB roundtrip", "[Float][Float16Color]")
{
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
