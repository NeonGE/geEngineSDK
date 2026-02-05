#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_get_random_seed.hpp>
#include <catch2/catch_approx.hpp>

#include <random>

#include "geColor.h"
#include "geVector3.h"
#include "geVector4.h"

using namespace geEngineSDK;

namespace
{
  constexpr float kEps = 1e-6f;

  static std::mt19937&
    rng() {
    static auto seed = Catch::getSeed();
    static std::mt19937 rng(seed);
    return rng;
  }

  inline float
  randFloat(float lo, float hi) {
    std::uniform_real_distribution<float> dist(lo, hi);
    return dist(rng());
  }

  inline int32
  randInt(int32 lo, int32 hi) {
    std::uniform_int_distribution<int32> dist(lo, hi);
    return dist(rng());
  }

  static void
  requireLinearNear(const LinearColor& a, const LinearColor& b, float eps = 1e-6f) {
    REQUIRE(a.r == Catch::Approx(b.r).epsilon(eps));
    REQUIRE(a.g == Catch::Approx(b.g).epsilon(eps));
    REQUIRE(a.b == Catch::Approx(b.b).epsilon(eps));
    REQUIRE(a.a == Catch::Approx(b.a).epsilon(eps));
  }

  inline float
  srgbEncode(float x) {
    if (x < 0.0f) x = 0.0f;
    if (x > 1.0f) x = 1.0f;

    if (x <= 0.0031308f) return x * 12.92f;
    return std::pow(x, 1.0f / 2.4f) * 1.055f - 0.055f;
  }

  inline float
  srgbDecode(float x) {
    if (x <= 0.04045f) return x / 12.92f;
    return std::pow((x + 0.055f) / 1.055f, 2.4f);
  }

  inline uint8_t
  toByteFloor255_999(float x01) {
    if (x01 < 0.0f) x01 = 0.0f;
    if (x01 > 1.0f) x01 = 1.0f;
    const float v = std::floor(x01 * 255.999f);
    int32_t iv = (int32_t)v;
    if (iv < 0) iv = 0;
    if (iv > 255) iv = 255;
    return (uint8_t)iv;
  }
}

TEST_CASE("LinearColor: default / FORCE_INIT", "[Math][Color][LinearColor]") {
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

TEST_CASE("LinearColor: named constants sanity", "[Math][Color][LinearColor]") {
  REQUIRE(LinearColor::Transparent.equals(LinearColor(0, 0, 0, 0)));
  REQUIRE(LinearColor::White.equals(LinearColor(1, 1, 1, 1)));
  REQUIRE(LinearColor::Black.equals(LinearColor(0, 0, 0, 1)));
  REQUIRE(LinearColor::Red.equals(LinearColor(1, 0, 0, 1)));
  REQUIRE(LinearColor::Green.equals(LinearColor(0, 1, 0, 1)));
  REQUIRE(LinearColor::Blue.equals(LinearColor(0, 0, 1, 1)));
}

TEST_CASE("Color: named constants sanity", "[Math][Color][Color]") {
  REQUIRE(Color::Transparent == Color(0, 0, 0, 0));
  REQUIRE(Color::White == Color(255, 255, 255, 255));
  REQUIRE(Color::Black == Color(0, 0, 0, 255));
  REQUIRE(Color::Red == Color(255, 0, 0, 255));
  REQUIRE(Color::Green == Color(0, 255, 0, 255));
  REQUIRE(Color::Blue == Color(0, 0, 255, 255));
}

TEST_CASE("LinearColor: construct from Vector3/Vector4", "[Math][Color][LinearColor]") {
  Vector3 v3(1.0f, 2.0f, 3.0f);
  LinearColor c3(v3);
  requireLinearNear(c3, LinearColor(1, 2, 3, 1), 0.0f);

  Vector4 v4(4.0f, 5.0f, 6.0f, 7.0f);
  LinearColor c4(v4);
  requireLinearNear(c4, LinearColor(4, 5, 6, 7), 0.0f);
}

TEST_CASE("Color: reinterpretAsLinear maps bytes to [0..1]", "[Math][Color][Color]") {
  Color c(255, 128, 0, 64);
  LinearColor lc = c.reinterpretAsLinear();
  REQUIRE(lc.r == Catch::Approx(1.0f).epsilon(kEps));
  REQUIRE(lc.g == Catch::Approx(128.0f / 255.0f).epsilon(kEps));
  REQUIRE(lc.b == Catch::Approx(0.0f).epsilon(kEps));
  REQUIRE(lc.a == Catch::Approx(64.0f / 255.0f).epsilon(kEps));
}

TEST_CASE("LinearColor: quantize truncates (not rounds) and clamps",
          "[Math][Color][LinearColor]") {
  LinearColor c(0.0f, 0.5f, 1.0f, 1.2f);
  Color q = c.quantize();

  // trunc(0*255)=0
  REQUIRE(q.r == 0);

  // trunc(0.5*255)=trunc(127.5)=127
  REQUIRE(q.g == 127);

  // trunc(1*255)=255
  REQUIRE(q.b == 255);

  // 1.2 -> 306 -> clamp 255
  REQUIRE(q.a == 255);

  //negatives also clamp
  LinearColor c2(-0.1f, -100.0f, 0.1f, -1.0f);
  Color q2 = c2.quantize();
  REQUIRE(q2.r == 0);
  REQUIRE(q2.g == 0);
  REQUIRE(q2.b == (uint8_t)std::trunc(0.1f * 255.f));
  REQUIRE(q2.a == 0);
}

TEST_CASE("LinearColor: toColor(false) uses floor(x*255.999) after clamp",
          "[Math][Color][LinearColor]") {
  LinearColor c(0.0f, 0.5f, 1.0f, 1.0f);
  Color out = c.toColor(false);

  REQUIRE(out.r == toByteFloor255_999(0.0f));
  REQUIRE(out.g == toByteFloor255_999(0.5f));
  REQUIRE(out.b == toByteFloor255_999(1.0f));
  REQUIRE(out.a == toByteFloor255_999(1.0f));

  //clamp
  LinearColor c2(-10.0f, 2.0f, 0.25f, -1.0f);
  Color o2 = c2.toColor(false);
  REQUIRE(o2.r == 0);
  REQUIRE(o2.g == 255);
  REQUIRE(o2.b == toByteFloor255_999(0.25f));
  REQUIRE(o2.a == 0);
}

TEST_CASE("LinearColor: toColor(true) matches reference sRGB encode (piecewise)",
          "[Math][Color][LinearColor][sRGB]") {
  const float vals[] = { 0.0f, 0.001f, 0.0031308f, 0.01f, 0.25f, 0.5f, 1.0f };

  for (float v : vals) {
    LinearColor c(v, v, v, 1.0f);
    Color out = c.toColor(true);

    const float enc = srgbEncode(v);
    const uint8_t expected = toByteFloor255_999(enc);

    REQUIRE(out.r == expected);
    REQUIRE(out.g == expected);
    REQUIRE(out.b == expected);
    REQUIRE(out.a == 255);
  }
}

TEST_CASE("LinearColor: fromSRGBColor approx inverts toColor(true) for many samples",
          "[Math][Color][LinearColor][sRGB][Property]") {
  for (int i = 0; i < 2000; ++i) {
    LinearColor lin(randFloat(0.0f, 1.0f),
                    randFloat(0.0f, 1.0f),
                    randFloat(0.0f, 1.0f),
                    randFloat(0.0f, 1.0f));

    Color srgb = lin.toColor(true);
    LinearColor back = LinearColor::fromSRGBColor(srgb);

    REQUIRE(back.r == Catch::Approx(lin.r).margin(0.01f));
    REQUIRE(back.g == Catch::Approx(lin.g).margin(0.01f));
    REQUIRE(back.b == Catch::Approx(lin.b).margin(0.01f));
    REQUIRE(back.a == Catch::Approx(lin.a).margin(1.0f / 255.0f + 1e-3f));
  }
}

TEST_CASE("LinearColor: fromPow22Color matches LinearColor(Color ctor) for RGB",
          "[Math][Color][LinearColor][pow22]") {
  Color c(10, 50, 200, 128);

  LinearColor a(c);
  LinearColor b = LinearColor::fromPow22Color(c);

  REQUIRE(a.equals(b));
}

TEST_CASE("LinearColor: computeLuminance/getLuminance exact weights",
          "[Math][Color][LinearColor]") {
  LinearColor c(1.0f, 2.0f, 3.0f, 0.0f);
  const float expected = 1.0f * 0.3f + 2.0f * 0.59f + 3.0f * 0.11f;

  REQUIRE(c.computeLuminance() == Catch::Approx(expected).epsilon(1e-6f));
  REQUIRE(c.getLuminance() == Catch::Approx(expected).epsilon(1e-6f));
}

TEST_CASE("LinearColor: desaturate endpoints and alpha behavior",
          "[Math][Color][LinearColor]") {
  LinearColor c(0.2f, 0.6f, 0.1f, 0.8f);
  const float lum = c.computeLuminance();

  // Desaturation=0 -> original
  requireLinearNear(c.desaturate(0.0f), c, 1e-6f);

  // Desaturation=1 -> lerp to (lum,lum,lum,0) (ojo alpha!)
  LinearColor d1 = c.desaturate(1.0f);
  REQUIRE(d1.r == Catch::Approx(lum).epsilon(1e-6f));
  REQUIRE(d1.g == Catch::Approx(lum).epsilon(1e-6f));
  REQUIRE(d1.b == Catch::Approx(lum).epsilon(1e-6f));
  REQUIRE(d1.a == Catch::Approx(0.0f).epsilon(1e-6f));
}

TEST_CASE("LinearColor: dist symmetry and zero", "[Math][Color][LinearColor]")
{
  LinearColor a(1, 2, 3, 4);
  LinearColor b(5, 6, 7, 8);

  REQUIRE(LinearColor::dist(a, a) == Catch::Approx(0.0f).epsilon(1e-6f));
  REQUIRE(LinearColor::dist(a, b) == Catch::Approx(LinearColor::dist(b, a)).epsilon(1e-6f));
}

TEST_CASE("LinearColor: toRGBE / Color::fromRGBE roundtrip-ish", "[Math][Color][RGBE]")
{
  // Para valores “HDR” moderados la ida/vuelta debe ser proporcionalmente cercana.
  LinearColor hdr(3.0f, 1.0f, 0.5f, 1.0f);
  Color rgbe = hdr.toRGBE();
  LinearColor back = rgbe.fromRGBE();

  // Alpha vuelve como 1
  REQUIRE(back.a == Catch::Approx(1.0f).epsilon(1e-6f));

  // No es exacto (cuantización + exponent), pero debe conservar proporciones cerca.
  REQUIRE(back.r == Catch::Approx(hdr.r).margin(0.05f));
  REQUIRE(back.g == Catch::Approx(hdr.g).margin(0.05f));
  REQUIRE(back.b == Catch::Approx(hdr.b).margin(0.05f));

  SECTION("Primary very small -> (0,0,0,0)")
  {
    LinearColor tiny(0.0f, 0.0f, 0.0f, 1.0f);
    Color z = tiny.toRGBE();
    REQUIRE(z == Color(0, 0, 0, 0));

    LinearColor bz = z.fromRGBE();
    REQUIRE(bz.equals(LinearColor::Black));
  }
}

TEST_CASE("LinearColor: linearRGBToHSV and hsvToLinearRGB roundtrip (non-edge)",
          "[Math][Color][HSV]") {
  LinearColor rgb(0.25f, 0.6f, 0.4f, 0.7f);

  LinearColor hsv = rgb.linearRGBToHSV();
  REQUIRE(hsv.r >= 0.0f);
  REQUIRE(hsv.r < 360.0f);
  REQUIRE(hsv.g >= 0.0f);
  REQUIRE(hsv.g <= 1.0f);
  REQUIRE(hsv.b >= 0.0f);
  REQUIRE(hsv.b <= 1.0f);
  REQUIRE(hsv.a == Catch::Approx(rgb.a).epsilon(1e-6f));

  LinearColor rgb2 = hsv.hsvToLinearRGB();
  REQUIRE(rgb2.r == Catch::Approx(rgb.r).margin(1e-4f));
  REQUIRE(rgb2.g == Catch::Approx(rgb.g).margin(1e-4f));
  REQUIRE(rgb2.b == Catch::Approx(rgb.b).margin(1e-4f));
  REQUIRE(rgb2.a == Catch::Approx(rgb.a).epsilon(1e-6f));
}

TEST_CASE("LinearColor: lerpUsingHSV endpoints + shortest hue path sanity",
          "[Math][Color][HSV][Lerp]") {
  // Endpoints
  LinearColor A(1, 0, 0, 0.2f);
  LinearColor B(0, 1, 0, 0.8f);

  requireLinearNear(LinearColor::lerpUsingHSV(A, B, 0.0f), A, 1e-6f);
  requireLinearNear(LinearColor::lerpUsingHSV(A, B, 1.0f), B, 1e-6f);

  // Shortest path test:
  LinearColor hsv1(350.0f, 1.0f, 1.0f, 1.0f);
  LinearColor hsv2(10.0f, 1.0f, 1.0f, 1.0f);
  LinearColor rgb1 = hsv1.hsvToLinearRGB();
  LinearColor rgb2 = hsv2.hsvToLinearRGB();

  LinearColor mid = LinearColor::lerpUsingHSV(rgb1, rgb2, 0.5f);
  LinearColor midHSV = mid.linearRGBToHSV();

  const float h = midHSV.r;
  const bool nearZero = (h < 60.0f) || (h > 300.0f);
  REQUIRE(nearZero);
}

TEST_CASE("LinearColor: evaluateBezier basic properties", "[Math][Color][Bezier]")
{
  LinearColor cps[4] = {
    LinearColor(0,0,0,0),
    LinearColor(1,0,0,0),
    LinearColor(1,1,0,0),
    LinearColor(0,1,0,0)
  };

  Vector<LinearColor> pts;
  const int32 N = 32;
  float len = LinearColor::evaluateBezier(cps, N, pts);

  REQUIRE((int32)pts.size() == N);
  REQUIRE(pts.front().equals(cps[0]));
  REQUIRE(pts.back().equals(cps[3], 1e-3f));

  REQUIRE(len >= LinearColor::dist(cps[0], cps[3]));
}

TEST_CASE("LinearColor: makeFromColorTemperature clamps input range", "[Math][Color][Temp]")
{
  LinearColor lo = LinearColor::makeFromColorTemperature(1000.0f);
  LinearColor lo2 = LinearColor::makeFromColorTemperature(1.0f);
  REQUIRE(lo.equals(lo2, 1e-6f));

  LinearColor hi = LinearColor::makeFromColorTemperature(15000.0f);
  LinearColor hi2 = LinearColor::makeFromColorTemperature(999999.0f);
  REQUIRE(hi.equals(hi2, 1e-6f));
}

TEST_CASE("Color: packing helpers", "[Math][Color][Pack]") {
  Color c(0x11, 0x22, 0x33, 0x44);

  REQUIRE(c.toPackedARGB() == uint32((0x44 << 24) | (0x11 << 16) | (0x22 << 8) | 0x33));
  REQUIRE(c.toPackedABGR() == uint32((0x44 << 24) | (0x33 << 16) | (0x22 << 8) | 0x11));
  REQUIRE(c.toPackedRGBA() == uint32((0x11 << 24) | (0x22 << 16) | (0x33 << 8) | 0x44));
  REQUIRE(c.toPackedBGRA() == uint32((0x33 << 24) | (0x22 << 16) | (0x11 << 8) | 0x44));
}

TEST_CASE("Color: dwColor roundtrip (layout-stable within build)", "[Math][Color][Layout]") {
  Color c(1, 2, 3, 4);
  uint32 raw = c.dwColor();

  Color d(raw);
  REQUIRE(d.dwColor() == raw);
  REQUIRE(d == c);
}

TEST_CASE("computeAndFixedColorAndIntensity: normalizes and outputs intensity",
          "[Math][Color][Intensity]") {
  LinearColor in(2.0f, 1.0f, 0.5f, 1.0f);
  Color out;
  float intensity = 0.0f;

  computeAndFixedColorAndIntensity(in, out, intensity);

  REQUIRE(intensity == Catch::Approx(2.0f).epsilon(1e-6f));

  LinearColor norm = in / intensity;
  Color expected = norm.toColor(true);

  REQUIRE(out == expected);
}

TEST_CASE("Color: operator+= saturates to 255 without sign wrap", "[Math][Color][Bug]") {
  Color a(250, 250, 250, 250);
  Color b(20, 10, 1, 100);

  a += b;

  REQUIRE(a.r == 255);
  REQUIRE(a.g == 255);
  REQUIRE(a.b == 251);
  REQUIRE(a.a == 255);
}
