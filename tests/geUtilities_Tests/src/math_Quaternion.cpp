#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_get_random_seed.hpp>
#include <random>

#include "geQuaternion.h"
#include "geVector3.h"
#include "geMatrix4.h"

using namespace geEngineSDK;

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

static bool
near(float a, float b, float eps) {
  return std::fabs(a - b) <= eps;
}

static bool
vec3Near(const Vector3& a, const Vector3& b, float eps) {
  return near(a.x, b.x, eps) && near(a.y, b.y, eps) && near(a.z, b.z, eps);
}

static Vector3
randVec3(float minv = -1.0f, float maxv = 1.0f) {
  return Vector3{ randRange(minv,maxv), randRange(minv,maxv), randRange(minv,maxv) };
}

static Vector3
randUnitVec3() {
  Vector3 v;
  do {
    v = randVec3(-1, 1);
  }while (v.sizeSquared() < 1e-8f);

  return v.getUnsafeNormal();
}

static Quaternion
randUnitQuat() {
  Vector3 axis = randUnitVec3();
  float angle = randRange(-3.14159265f, 3.14159265f);

  Quaternion q(axis, Radian(angle));
  return q.getNormalized();
}

static Vector3
transformDir(const Matrix4& M, const Vector3& v) {
  return M.transformVector(v);
}

static float
dotQ(const Quaternion& a, const Quaternion& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static bool
quatNearSameRotation(const Quaternion& a, const Quaternion& b, float eps) {
  Quaternion an = a.getNormalized();
  Quaternion bn = b.getNormalized();
  float d = std::fabs(dotQ(an, bn));
  return (1.0f - d) <= eps;
}

TEST_CASE("Quat: normalization produces unit length", "[Quat][Portable][Normalize]") {
  constexpr float eps = 2e-5f;

  for (int i = 0; i < 8000; ++i) {
    Quaternion q;
    q.x = randRange(-2, 2);
    q.y = randRange(-2, 2);
    q.z = randRange(-2, 2);
    q.w = randRange(-2, 2);

    Quaternion n = q.getNormalized();
    float len2 = n.x * n.x + n.y * n.y + n.z * n.z + n.w * n.w;

    REQUIRE(std::fabs(len2 - 1.0f) <= eps);
  }
}

TEST_CASE("Quat: inverse cancels (unit quats)", "[Quat][Portable][Inverse]") {
  constexpr float eps = 2e-4f;

  Quaternion I; I.x = 0; I.y = 0; I.z = 0; I.w = 1;

  for (int i = 0; i < 10000; ++i) {
    Quaternion q = randUnitQuat();
    Quaternion qi = q.inverse().getNormalized();

    Quaternion prod = (q * qi).getNormalized();
    REQUIRE(quatNearSameRotation(prod, I, eps));
  }
}

TEST_CASE("Quat: rotateVector preserves length", "[Quat][Portable][Rotate]") {
  constexpr float eps = 5e-4f;

  for (int i = 0; i < 12000; ++i) {
    Quaternion q = randUnitQuat();
    Vector3 v = randVec3(-10, 10);

    Vector3 r = q.rotateVector(v);

    REQUIRE(std::fabs(r.size() - v.size()) <= eps);
  }
}

TEST_CASE("Quat: unrotateVector is inverse of rotateVector", "[Quat][Portable][Unrotate]") {
  constexpr float eps = 4e-4f;

  for (int i = 0; i < 12000; ++i) {
    Quaternion q = randUnitQuat();
    Vector3 v = randVec3(-10, 10);

    Vector3 a = q.rotateVector(v);
    Vector3 b = q.unrotateVector(a);

    REQUIRE(vec3Near(v, b, eps));
  }
}

TEST_CASE("Quat: multiplication composes rotations (by effect)",
          "[Quat][Portable][Multiply]") {
  constexpr float eps = 6e-4f;

  for (int i = 0; i < 9000; ++i) {
    Quaternion a = randUnitQuat();
    Quaternion b = randUnitQuat();
    Vector3 v = randVec3(-5, 5);

    Vector3 direct = (a * b).rotateVector(v);
    Vector3 step = a.rotateVector(b.rotateVector(v));

    REQUIRE(vec3Near(direct, step, eps));
  }
}

TEST_CASE("Quat/Matrix: rotateVector equals transformDir(q.toMatrix, v)",
          "[Quat][Mat][Portable]")
{
  constexpr float eps = 6e-4f;

  for (int i = 0; i < 10000; ++i)
  {
    Quaternion q = randUnitQuat();
    Matrix4 M = q.toMatrix();

    Vector3 v = randVec3(-10, 10);

    Vector3 a = q.rotateVector(v);
    Vector3 b = transformDir(M, v);

    REQUIRE(vec3Near(a, b, eps));
  }
}

TEST_CASE("Quat/Matrix: round-trip q -> M -> q preserves rotation",
          "[Quat][Mat][Portable][RoundTrip]")
{
  constexpr float eps = 2e-4f;

  for (int i = 0; i < 10000; ++i)
  {
    Quaternion q0 = randUnitQuat();
    Matrix4 M = q0.toMatrix();

    Quaternion q1(M);
    REQUIRE(quatNearSameRotation(q0, q1, eps));
  }
}

TEST_CASE("Quat/Matrix: round-trip M -> q -> M preserves rotation (by basis)",
          "[Quat][Mat][Portable][RoundTrip]") {
  constexpr float eps = 8e-4f;

  for (int i = 0; i < 8000; ++i)
  {
    Quaternion q0 = randUnitQuat();
    Matrix4 M0 = q0.toMatrix();

    Quaternion q1(M0);
    Matrix4 M1 = q1.toMatrix();

    Vector3 ex{ 1,0,0 }, ey{ 0,1,0 }, ez{ 0,0,1 };
    REQUIRE(vec3Near(transformDir(M0, ex), transformDir(M1, ex), eps));
    REQUIRE(vec3Near(transformDir(M0, ey), transformDir(M1, ey), eps));
    REQUIRE(vec3Near(transformDir(M0, ez), transformDir(M1, ez), eps));
  }
}

TEST_CASE("Quat/Matrix: edge cases near 0 and 180 degrees",
          "[Quat][Mat][Portable][EdgeCases]") {
  constexpr float epsQ = 4e-4f;
  constexpr float epsV = 1e-3f;

  Vector3 axes[] = { Vector3{1,0,0}, Vector3{0,1,0}, Vector3{0,0,1}, randUnitVec3() };
  float angles[] = { 0.0f, 1e-6f, 1e-3f, 3.14159265f - 1e-6f, 3.14159265f - 1e-3f, 3.14159265f };

  for (auto axis : axes) {
    axis = axis.getSafeNormal();
    for (float a : angles) {
      Quaternion q0(axis, Radian(a));
      Matrix4 M = q0.toMatrix();
      Quaternion q1(M);

      REQUIRE(quatNearSameRotation(q0, q1, epsQ));

      Vector3 v = randVec3(-3, 3);
      REQUIRE(vec3Near(q0.rotateVector(v), q1.rotateVector(v), epsV));
    }
  }
}
