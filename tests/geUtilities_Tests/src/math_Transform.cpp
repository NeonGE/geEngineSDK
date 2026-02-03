#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_get_random_seed.hpp>
#include <random>

#include "geTransform.h"

using namespace geEngineSDK;

static inline bool
nearFloat(float a, float b, float eps = 1e-4f) {
  return std::fabs(a - b) <= eps;
}

static inline void
requireNearFloat(float a, float b, float eps = 1e-4f) {
  REQUIRE(nearFloat(a, b, eps));
}

static inline void
requireNearVec3(const Vector3& a, const Vector3& b, float eps = 1e-4f) {
  REQUIRE((a - b).size() <= eps);
}

static inline void
requireNearQuatSameRotation(const Quaternion& a, const Quaternion& b, float eps = 1e-4f) {
  Quaternion qa = a.getNormalized();
  Quaternion qb = b.getNormalized();

  float d = std::fabs(qa | qb);
  REQUIRE(d >= 1.0f - eps);
}

static inline float
quatAngleErrorDeg(const Quaternion& a, const Quaternion& b) {
  Quaternion qa = a.getNormalized();
  Quaternion qb = b.getNormalized();
  Quaternion qe = qa.conjugate() * qb;
  qe = qe.getNormalized();

  float w = Math::clamp(qe.w, -1.0f, 1.0f);
  float ang = 2.0f * Math::acos(w).valueRadians();
  return Math::RAD2DEG * ang;
}


TEST_CASE("Transform: identity leaves point unchanged", "[Math][Transform]") {
  Transform T;
  Vector3 p(1, 2, 3);
  requireNearVec3(T.transformPosition(p), p);
}

TEST_CASE("Transform: translation only", "[Math][Transform]") {
  Transform T(Quaternion::IDENTITY, Vector3(10, 20, 30), Vector3::UNIT);
  Vector3 p(1, 2, 3);
  requireNearVec3(T.transformPosition(p), p + Vector3(10, 20, 30));
}

TEST_CASE("Transform: scale only", "[Math][Transform]") {
  Transform T(Quaternion::IDENTITY, Vector3::ZERO, Vector3(2, 3, 4));
  Vector3 p(1, 2, 3);
  requireNearVec3(T.transformPosition(p), Vector3(2, 6, 12));
}

TEST_CASE("Transform::inverse matches inverseTransformPosition (uniform scale)",
          "[Math][Transform]") {
  Transform T(Rotator(10, 20, 30).toQuaternion().getNormalized(),
              Vector3(3, 4, 5),
              Vector3(2, 2, 2)); //Uniform scale

  Vector3 P(7, 8, 9);
  Vector3 A = T.inverseTransformPosition(T.transformPosition(P));

  Transform Ti = T.inverse();
  Vector3 B = Ti.transformPosition(T.transformPosition(P));

  REQUIRE((A - P).size() < 1e-3f);
  REQUIRE((B - P).size() < 1e-3f);
}

TEST_CASE("Transform multiply matches matrices", "[Math][Transform]") {
  Transform A(Rotator(10, 20, 30).toQuaternion(), Vector3(1, 2, 3), Vector3(2, 3, 4));
  Transform B(Rotator(-5, 40, 15).toQuaternion(), Vector3(4, 5, 6), Vector3(1, 2, 1));

  Transform C;
  Transform::multiply(&C, &A, &B);

  Matrix4 Am = A.toMatrixWithScale();
  Matrix4 Bm = B.toMatrixWithScale();
  Matrix4 Cm = C.toMatrixWithScale();

  // dependiendo de tu operador de Matrix4, aquí puede ser Am*Bm o Bm*Am
  // Lo importante: ajusta UNA vez y ya.
}

TEST_CASE("Transform: rotation only matches quaternion rotateVector", "[Math][Transform]")
{
  Quaternion q = Rotator(10, 20, 30).toQuaternion().getNormalized();
  Transform T(q, Vector3::ZERO, Vector3::UNIT);

  Vector3 p(1, 2, 3);
  Vector3 a = T.transformPosition(p);
  Vector3 b = q.rotateVector(p);

  requireNearVec3(a, b, 1e-4f);
}

TEST_CASE("Transform: inverseTransformPosition undoes transformPosition", "[Math][Transform]") {
  Transform T(Rotator(10, 20, 30).toQuaternion().getNormalized(),
    Vector3(3, 4, 5),
    Vector3(2, 3, 4)); // non-uniform

  Vector3 p(7, 8, 9);
  Vector3 t = T.transformPosition(p);
  Vector3 back = T.inverseTransformPosition(t);

  requireNearVec3(back, p, 1e-3f);
}

TEST_CASE("Transform: inverse() matches inverseTransformPosition for points", "[Math][Transform]") {
  Transform T(Rotator(10, 20, 30).toQuaternion().getNormalized(),
              Vector3(3, 4, 5),
              Vector3(2, 2, 2)); //uniform scale

  Transform Ti = T.inverse();

  Vector3 p(7, 8, 9);
  Vector3 back = Ti.transformPosition(T.transformPosition(p));

  requireNearVec3(back, p, 1e-3f);
}

TEST_CASE("Transform: multiply composes like sequential application", "[Math][Transform]") {
  Transform A(Rotator(10, 20, 30).toQuaternion().getNormalized(),
              Vector3(1, 2, 3),
              Vector3(2, 2, 2));

  Transform B(Rotator(-5, 40, 15).toQuaternion().getNormalized(),
              Vector3(4, 5, 6),
              Vector3(3, 3, 3));

  Transform C;
  Transform::multiply(&C, &A, &B);

  Vector3 p(0.3f, -1.2f, 5.0f);

  Vector3 expected = B.transformPosition(A.transformPosition(p));
  Vector3 got = C.transformPosition(p);

  requireNearVec3(got, expected, 1e-3f);
}

TEST_CASE("Transform: toMatrixWithScale matches transformPosition", "[Math][Transform][Matrix]") {
  Transform T(Rotator(10, 20, 30).toQuaternion().getNormalized(),
    Vector3(3, 4, 5),
    Vector3(2, 3, 4));

  Matrix4 M = T.toMatrixWithScale();

  Vector3 p(7, 8, 9);

  Vector3 a = T.transformPosition(p);

  Vector4 hp(p.x, p.y, p.z, 1.0f);
  Vector4 r = M.transformVector4(hp);
  Vector3 b(r.x, r.y, r.z);

  requireNearVec3(a, b, 1e-3f);
}

TEST_CASE("Transform: parent-child composition sanity (bone-like)", "[Math][Transform][Skeleton]") {
  Transform parent(Rotator(0, 45, 0).toQuaternion().getNormalized(),
    Vector3(10, 0, 0),
    Vector3(1, 1, 1));

  Transform childLocal(Quaternion::IDENTITY,
    Vector3(0, 0, 5), // el hueso está a +Z del parent
    Vector3(1, 1, 1));

  Transform childGlobal;
  Transform::multiply(&childGlobal, &childLocal, &parent); // asumiendo "aplica local luego parent"

  // La punta del hueso child en local:
  Vector3 tipLocal(0, 0, 2);

  // Aplicación secuencial equivalente:
  Vector3 expected = parent.transformPosition(childLocal.transformPosition(tipLocal));
  Vector3 got = childGlobal.transformPosition(tipLocal);

  requireNearVec3(got, expected, 1e-3f);
}

TEST_CASE("Transform: fuzz roundtrip (no negative scale)", "[Math][Transform][Fuzz]") {
  auto seed = Catch::getSeed();
  std::mt19937 rng(seed);
  std::uniform_real_distribution<float> ang(-179.0f, 179.0f);
  std::uniform_real_distribution<float> pos(-100.0f, 100.0f);
  std::uniform_real_distribution<float> sca(0.1f, 10.0f);

  for (int i = 0; i < 500; ++i) {
    float s = sca(rng);
    Rotator r(ang(rng), ang(rng), ang(rng));
    Transform T(r.toQuaternion().getNormalized(),
                Vector3(pos(rng), pos(rng), pos(rng)),
                Vector3(s, s, s));

    Transform Ti = T.inverse();

    Vector3 p(pos(rng), pos(rng), pos(rng));
    Vector3 back = Ti.transformPosition(T.transformPosition(p));

    REQUIRE((back - p).size() < 2e-3f);
  }
}
