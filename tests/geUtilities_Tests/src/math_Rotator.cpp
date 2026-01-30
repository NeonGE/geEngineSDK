#include <catch_amalgamated.hpp>
#include <random>

#include "geQuaternion.h"
#include "geMatrix4.h"
#include "geRotator.h"
#include "geVector3.h"
#include "geMath.h"

using namespace geEngineSDK;
using Catch::Approx;

static float
dot(const Vector3& a, const Vector3& b) {
  return (a | b);
}

static bool
near(float a, float b, float eps) {
  return std::fabs(a - b) <= eps;
}

static bool
vec3Near(const Vector3& a, const Vector3& b, float eps) {
  return near(a.x, b.x, eps) && near(a.y, b.y, eps) && near(a.z, b.z, eps);
}

static std::mt19937&
rng() {
  static std::mt19937 rng(0xC0FFEEu);
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

static Vector3
randVec3Wide() {
  return randVec3(-25.f, 25.f);
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

TEST_CASE("Rotator: identity looks forward +Z", "[Rotator][Contract]") {
  Rotator r(0, 0, 0);
  Quaternion q = r.toQuaternion();

  Vector3 f = q.rotateVector(Vector3(0, 0, 1));
  REQUIRE(vec3Near(f, Vector3(0, 0, 1), 1e-4f));
}

TEST_CASE("Rotator: yaw +90 turns right (forward +Z -> +X)", "[Rotator][Yaw]") {
  Rotator r(0, 90, 0);
  Quaternion q = r.toQuaternion();

  Vector3 f = q.rotateVector(Vector3(0, 0, 1));
  REQUIRE(vec3Near(f, Vector3(1, 0, 0), 2e-4f));
}

TEST_CASE("Rotator: pitch +90 looks up (forward +Z -> +Y)", "[Rotator][Pitch]") {
  Rotator r(90, 0, 0);
  Quaternion q = r.toQuaternion();

  Vector3 f = q.rotateVector(Vector3(0, 0, 1));
  REQUIRE(vec3Near(f, Vector3(0, 1, 0), 2e-4f));
}

TEST_CASE("Rotator: roll +90 is clockwise (right +X -> down -Y)", "[Rotator][Roll]") {
  Rotator r(0, 0, 90);
  Quaternion q = r.toQuaternion();

  Vector3 right = q.rotateVector(Vector3(1, 0, 0));
  REQUIRE(vec3Near(right, Vector3(0, -1, 0), 2e-4f));
}

TEST_CASE("Rotator: matches manual axis-angle composition (yaw->pitch->roll)", "[Rotator][Compose]") {
  Rotator r(30.f, 45.f, 10.f);
  Quaternion qR = r.toQuaternion();

  Quaternion qYaw(Vector3(0, 1, 0), Degree(r.yaw));
  Quaternion qPitch(Vector3(1, 0, 0), Degree(-r.pitch));
  Quaternion qRoll(Vector3(0, 0, 1), Degree(-r.roll));

  Quaternion qM = (qYaw * qPitch * qRoll).getNormalized();
  REQUIRE(quatNearSameRotation(qR, qM, 2e-4f));
}
/*
TEST_CASE("Rotator: Matrix4::rotator round-trip preserves rotation (by effect)", "[Rotator][Matrix][RoundTrip]") {
  constexpr float eps = 6e-4f;

  for (int i = 0; i < 12000; ++i) {
    //Rotator r(randRange(-179.f, 179.f), randRange(-179.f, 179.f), randRange(-179.f, 179.f));
    Rotator r(45, 45, 45);

    Quaternion q0 = r.toQuaternion();
    Matrix4 M = q0.toMatrix();

    Rotator r2 = M.rotator();
    Quaternion q1 = r2.toQuaternion();

    Vector3 v = randVec3(-10, 10);
    REQUIRE(vec3Near(q0.rotateVector(v), q1.rotateVector(v), eps));
  }
}
*/
TEST_CASE("Rot", "DetRot") {
  Rotator r(45.f, 45.f, 45.f);
  Quaternion qR = r.toQuaternion().getNormalized();

  Quaternion qYaw(Vector3(0, 1, 0), Degree(r.yaw));
  Quaternion qPitch(Vector3(1, 0, 0), Degree(-r.pitch));
  Quaternion qRoll(Vector3(0, 0, 1), Degree(-r.roll));

  Quaternion q_engine_YXZ = (qYaw * qPitch * qRoll).getNormalized();
  Quaternion q_calc_YXZ = (qRoll * qPitch * qYaw).getNormalized();

  CAPTURE(Math::abs(dotQ(qR, q_engine_YXZ)));
  CAPTURE(Math::abs(dotQ(qR, q_calc_YXZ)));
}

/*
TEST_CASE("Rotator: getInverse cancels rotation (by effect)", "[Rotator][Inverse]") {
  constexpr float eps = 6e-4f;

  for (int i = 0; i < 10000; ++i) {
    Rotator r(randRange(-179.f, 179.f), randRange(-179.f, 179.f), randRange(-179.f, 179.f));
    Rotator ri = r.getInverse();

    Quaternion q = r.toQuaternion();
    Quaternion qi = ri.toQuaternion();

    Vector3 v = randVec3Wide();

    Vector3 out = qi.rotateVector(q.rotateVector(v));
    REQUIRE(vec3Near(out, v, eps));
  }
}

TEST_CASE("Rotator: r -> q -> r2 -> q2 preserves rotation (by effect)", "[Rotator][RoundTrip][Diag]") {
  constexpr float eps = 6e-4f;

  for (int i = 0; i < 12000; ++i) {
    Rotator r(randRange(-179.f, 179.f), randRange(-179.f, 179.f), randRange(-179.f, 179.f));
    Quaternion q0 = r.toQuaternion();

    Rotator r2 = q0.rotator();
    Quaternion q1 = r2.toQuaternion();

    Vector3 v = randVec3(-10, 10);
    REQUIRE(vec3Near(q0.rotateVector(v), q1.rotateVector(v), eps));
  }
}

TEST_CASE("Rotator: getInverse matches quaternion inverse (by effect)", "[Rotator][Inverse][Diag]") {
  constexpr float eps = 6e-4f;

  for (int i = 0; i < 12000; ++i) {
    Rotator r(randRange(-179.f, 179.f), randRange(-179.f, 179.f), randRange(-179.f, 179.f));
    Quaternion q = r.toQuaternion().getNormalized();

    Rotator ri = r.getInverse();
    Quaternion qi = ri.toQuaternion().getNormalized();

    Vector3 v = randVec3(-10, 10);
    REQUIRE(vec3Near(q.inverse().rotateVector(v), qi.rotateVector(v), eps));
  }
}

TEST_CASE("AxisAngle: +Yaw around +Y turns forward to right", "[Quat][AxisAngle][Diag]") {
  Quaternion q(Vector3(0, 1, 0), Degree(90.f));
  Vector3 f = q.rotateVector(Vector3(0, 0, 1));
  REQUIRE(vec3Near(f, Vector3(1, 0, 0), 2e-4f));
}
*/