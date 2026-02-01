#include <catch_amalgamated.hpp>

#include "geColor.h"

using namespace geEngineSDK;

namespace
{
  constexpr float kEps = 1e-6f;

  static void
  requireLinearNear(const LinearColor& a, const LinearColor& b, float eps = 1e-6f) {
    REQUIRE(a.r == Catch::Approx(b.r).epsilon(eps));
    REQUIRE(a.g == Catch::Approx(b.g).epsilon(eps));
    REQUIRE(a.b == Catch::Approx(b.b).epsilon(eps));
    REQUIRE(a.a == Catch::Approx(b.a).epsilon(eps));
  }
}

TEST_CASE("LinearColor: default / FORCE_INIT", "[Math][Color][LinearColor]") {
  LinearColor d;
  LinearColor z(FORCE_INIT::kForceInit);
  REQUIRE(z.r == 0.0f);
  REQUIRE(z.g == 0.0f);
  REQUIRE(z.b == 0.0f);
  REQUIRE(z.a == 0.0f);
}

TEST_CASE("LinearColor: equals tolerance", "[Math][Color][LinearColor]") {
  LinearColor a(0.1f, 0.2f, 0.3f, 0.4f);
  LinearColor b(0.1f + 1e-5f, 0.2f - 1e-5f, 0.3f, 0.4f);

  REQUIRE(a.equals(b, 1e-4f));
  REQUIRE_FALSE(a.equals(b, 1e-7f));
}

TEST_CASE("LinearColor: operator[] indexes RGBA", "[Math][Color][LinearColor]") {
  LinearColor c(1.0f, 2.0f, 3.0f, 4.0f);

  REQUIRE(c[0] == 1.0f);
  REQUIRE(c[1] == 2.0f);
  REQUIRE(c[2] == 3.0f);
  REQUIRE(c[3] == 4.0f);

  c[0] = 10.0f;
  c[3] = 40.0f;

  REQUIRE(c.r == 10.0f);
  REQUIRE(c.a == 40.0f);
}

TEST_CASE("LinearColor: arithmetic operators", "[Math][Color][LinearColor]") {
  LinearColor a(1, 2, 3, 4);
  LinearColor b(10, 20, 30, 40);

  requireLinearNear(a + b, LinearColor(11, 22, 33, 44));
  requireLinearNear(b - a, LinearColor(9, 18, 27, 36));

  //*= and /= are present in header
  LinearColor m(1, 2, 3, 4);
  m *= 2.0f;
  requireLinearNear(m, LinearColor(2, 4, 6, 8));

  LinearColor d(2, 4, 6, 8);
  d /= 2.0f;
  requireLinearNear(d, LinearColor(1, 2, 3, 4));
}

TEST_CASE("LinearColor: getClamped clamps per component", "[Math][Color][LinearColor]") {
  LinearColor a(-1.0f, 0.5f, 2.0f, 999.0f);

  LinearColor c01 = a.getClamped(); // default [0..1]
  requireLinearNear(c01, LinearColor(0.0f, 0.5f, 1.0f, 1.0f));

  LinearColor ccustom = a.getClamped(-2.0f, 2.0f);
  requireLinearNear(ccustom, LinearColor(-1.0f, 0.5f, 2.0f, 2.0f));
}

TEST_CASE("LinearColor: saturateCopy is getClamped()", "[Math][Color][LinearColor]"){
  LinearColor a(-10.0f, 0.25f, 100.0f, 1.5f);

  LinearColor s = a.saturateCopy();
  LinearColor c = a.getClamped(0.0f, 1.0f);

  REQUIRE(s.equals(c));
}

TEST_CASE("Color: FORCE_INIT / ctor bytes / dwColor roundtrip", "[Math][Color][Color]") {
  Color z(FORCE_INIT::kForceInit);
  REQUIRE(z.r == 0);
  REQUIRE(z.g == 0);
  REQUIRE(z.b == 0);
  REQUIRE(z.a == 0);

  Color c(1, 2, 3, 4);
  REQUIRE(c.r == 1);
  REQUIRE(c.g == 2);
  REQUIRE(c.b == 3);
  REQUIRE(c.a == 4);

  uint32 packed = c.dwColor();
  Color c2(packed);
  REQUIRE(c2.dwColor() == packed);
  REQUIRE(c2 == c);
}

TEST_CASE("Color: withAlpha keeps RGB and overrides alpha", "[Math][Color][Color]") {
  Color c(10, 20, 30, 40);
  Color d = c.withAlpha(128);

  REQUIRE(d.r == 10);
  REQUIRE(d.g == 20);
  REQUIRE(d.b == 30);
  REQUIRE(d.a == 128);
}

TEST_CASE("Color: operator+= saturates to 255 per channel", "[Math][Color][Color]") {
  Color a(250, 250, 250, 250);
  Color b(20, 10, 1, 100);

  a += b;

  REQUIRE(a.r == 255);
  REQUIRE(a.g == 255);
  REQUIRE(a.b == 251);  // 250 + 1
  REQUIRE(a.a == 255);  // 250 + 100 -> saturated
}

TEST_CASE("Color: reinterpretAsLinear maps to [0..1] floats", "[Math][Color][Color]") {
  Color c(255, 128, 0, 64);
  LinearColor lc = c.reinterpretAsLinear();

  REQUIRE(lc.r == Catch::Approx(1.0f).epsilon(kEps));
  REQUIRE(lc.g == Catch::Approx(128.0f / 255.0f).epsilon(kEps));
  REQUIRE(lc.b == Catch::Approx(0.0f).epsilon(kEps));
  REQUIRE(lc.a == Catch::Approx(64.0f / 255.0f).epsilon(kEps));
}
