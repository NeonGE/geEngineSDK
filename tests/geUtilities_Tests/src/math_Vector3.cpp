#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "geVector3.h"

using namespace geEngineSDK;
using Catch::Approx;

namespace {
  inline void near(float a, float b, float eps = 1e-6f) {
    REQUIRE(std::fabs(a - b) <= eps);
  }
  inline void vecNear(const Vector3& a, const Vector3& b, float eps = 1e-5f) {
    REQUIRE(std::fabs(a.x - b.x) <= eps);
    REQUIRE(std::fabs(a.y - b.y) <= eps);
    REQUIRE(std::fabs(a.z - b.z) <= eps);
  }
}

TEST_CASE("Vector3: component access, set, equals, []/component()", "[Math][Vector3]")
{
  Vector3 v(1, 2, 3);

  REQUIRE(v[0] == Approx(1));
  REQUIRE(v[1] == Approx(2));
  REQUIRE(v[2] == Approx(3));

  v[0] = 10;
  REQUIRE(v.x == Approx(10));

  v.component(1) = 20;
  REQUIRE(v.y == Approx(20));

  v.set(-1, -2, -3);
  vecNear(v, Vector3(-1, -2, -3));

  REQUIRE(v == Vector3(-1, -2, -3));
  REQUIRE(v != Vector3(-1, -2, -4));
  REQUIRE(v.equals(Vector3(-1.000001f, -2.0f, -3.0f), 1e-3f));
  REQUIRE_FALSE(v.equals(Vector3(-1.01f, -2.0f, -3.0f), 1e-3f));
}

TEST_CASE("Vector3: min/max/abs/componentMinMax", "[Math][Vector3]")
{
  Vector3 v(-2, 5, -3);

  near(v.getMax(), 5.0f);
  near(v.getMin(), -3.0f);

  near(v.getAbsMax(), 5.0f);
  near(v.getAbsMin(), 2.0f);

  vecNear(v.getAbs(), Vector3(2, 5, 3));

  Vector3 a(1, 10, 3);
  Vector3 b(2, 5, 7);

  vecNear(a.componentMin(b), Vector3(1, 5, 3));
  vecNear(a.componentMax(b), Vector3(2, 10, 7));
}

TEST_CASE("Vector3: size, size2D, normalize, unit/normalized", "[Math][Vector3]")
{
  Vector3 v(3, 4, 12);

  near(v.sizeSquared(), 3 * 3 + 4 * 4 + 12 * 12);
  near(v.size(), std::sqrt(v.sizeSquared()));
  near(v.size2D(), 5.0f);
  near(v.sizeSquared2D(), 25.0f);

  Vector3 dir; float len = 0;
  v.toDirectionAndLength(dir, len);
  near(len, v.size(), 1e-5f);
  REQUIRE(dir.isNormalized());

  Vector3 n = v.getSafeNormal();
  REQUIRE(n.isNormalized());

  Vector3 u = v.getUnsafeNormal();
  REQUIRE(u.isNormalized());

  Vector3 z(FORCE_INIT::kForceInit);
  REQUIRE(z.isZero());
  REQUIRE(z.isNearlyZero());
  REQUIRE_FALSE(z.normalize());
  REQUIRE(z.getSafeNormal().isZero());
  REQUIRE(z.getSafeNormal2D().isZero());

  REQUIRE(v.isUnit(1e-3f) == false);
  Vector3 one(1, 0, 0);
  REQUIRE(one.isUnit());
}

TEST_CASE("Vector3: dot, cross, triple, projections", "[Math][Vector3]")
{
  Vector3 a(1, 2, 3);
  Vector3 b(4, 5, 6);

  near(a | b, 32.0f);

  Vector3 c = a ^ b; // (-3,6,-3)
  vecNear(c, Vector3(-3, 6, -3));

  near(Vector3::triple(a, b, Vector3(7, 8, 9)), a | (b ^ Vector3(7, 8, 9)));

  // projectOnTo: no asume normalizado
  Vector3 p = a.projectOnTo(b);
  // p debe ser paralelo a b: cross ~ 0
  REQUIRE((p ^ b).isNearlyZero(1e-4f));

  // projectOnToNormal asume normalizado
  Vector3 bn = b.getSafeNormal();
  Vector3 pn = a.projectOnToNormal(bn);
  REQUIRE((pn ^ bn).isNearlyZero(1e-4f));
}

TEST_CASE("Vector3: sign vector, uniform, bounded/clamped, grid", "[Math][Vector3]")
{
  Vector3 v(-2, 0, 3);
  Vector3 s = v.getSignVector();
  // doc: cero tratado como +1
  vecNear(s, Vector3(-1, 1, 1));

  REQUIRE(Vector3(5, 5, 5).isUniform());
  REQUIRE_FALSE(Vector3(5, 5, 5.1f).isUniform(1e-3f));

  vecNear(Vector3(11, -12, 3).boundToCube(10), Vector3(10, -10, 3));
  vecNear(Vector3(12, 5, 0).gridSnap(10.0f), Vector3(10, 10, 0));

  // clamped to size
  Vector3 a(10, 0, 0);
  vecNear(a.getClampedToMaxSize(5.0f), Vector3(5, 0, 0));

  Vector3 tst;
  Vector3 b(3, 4, 0);
  tst = b.getClampedToSize(2.0f, 4.0f);
  REQUIRE(tst.size() >= 2.0f - 1e-5f);
  REQUIRE(tst.size() <= 4.0f + 1e-5f);

  Vector3 c(10, 0, 0);
  tst = c.getClampedToSize(20.0f, 30.0f);
  REQUIRE(tst.size() >= 20.0f - 1e-5f);
  REQUIRE(tst.size() <= 30.0f + 1e-5f);

  Vector3 d(3, 4, 7);
  tst = d.getClampedToMaxSize2D(2.0f);
  REQUIRE(tst.size2D() <= 20.0f + 1e-5f);
}

TEST_CASE("Vector3: angle helpers and rotations", "[Math][Vector3]")
{
  // cosineAngle2D
  Vector3 a(1, 0, 0);
  Vector3 b(0, 1, 0);
  near(a.cosineAngle2D(b), 0.0f);

  // rotateAngleAxis: rota (1,0,0) 90° alrededor de Z => (0,1,0)
  Vector3 r = Vector3(1, 0, 0).rotateAngleAxis(90.0f, Vector3(0, 0, 1));
  vecNear(r, Vector3(0, 1, 0), 1e-4f);

  // mirrorByVector: espejo sobre normal (1,0,0): refleja X
  vecNear(Vector3(2, 3, 4).mirrorByVector(Vector3(1, 0, 0)), Vector3(-2, 3, 4), 1e-4f);
}

TEST_CASE("Vector3: basis, near/same, plane distances (when available)", "[Math][Vector3]")
{
  // createOrthonormalBasis invariantes: ortogonal + normalizado
  Vector3 X(1, 0, 0), Y(0, 1, 0), Z(0, 0, 1);
  Vector3::createOrthonormalBasis(X, Y, Z);
  REQUIRE(X.isNormalized());
  REQUIRE(Y.isNormalized());
  REQUIRE(Z.isNormalized());
  REQUIRE(std::fabs((X | Y)) < 1e-4f);
  REQUIRE(std::fabs((X | Z)) < 1e-4f);
  REQUIRE(std::fabs((Y | Z)) < 1e-4f);

  REQUIRE(Vector3::pointsAreSame(Vector3(1, 2, 3), Vector3(1, 2, 3)));
  REQUIRE(Vector3::pointsAreNear(Vector3(1, 2, 3), Vector3(1.1f, 2, 3), 0.2f));
  REQUIRE_FALSE(Vector3::pointsAreNear(Vector3(1, 2, 3), Vector3(2, 2, 3), 0.2f));

  // pointPlaneDist: plano X=0 con normal +X
  near(Vector3::pointPlaneDist(Vector3(5, 0, 0), Vector3(0, 0, 0), Vector3(1, 0, 0)), 5.0f);
  near(Vector3::pointPlaneDist(Vector3(-2, 0, 0), Vector3(0, 0, 0), Vector3(1, 0, 0)), -2.0f);
}

TEST_CASE("Vector3: containsNaN/unwindEuler/headingAngle", "[Math][Vector3]")
{
  Vector3 v(0, 0, 0);
  REQUIRE_FALSE(v.containsNaN());

  Vector3 n(std::numeric_limits<float>::quiet_NaN(), 0, 0);
  REQUIRE(n.containsNaN());

  // unwindEuler: deja en [-180,180]
  Vector3 e(540, -540, 181);
  e.unwindEuler();
  REQUIRE(e.x <= 180.0f); REQUIRE(e.x >= -180.0f);
  REQUIRE(e.y <= 180.0f); REQUIRE(e.y >= -180.0f);
  REQUIRE(e.z <= 180.0f); REQUIRE(e.z >= -180.0f);

  // headingAngle: 0 apunta +X
  near(Vector3(1, 0, 0).headingAngle(), 0.0f, 1e-6f);
}
