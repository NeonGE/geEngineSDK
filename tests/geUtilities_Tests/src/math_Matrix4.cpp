#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "geMatrix4.h"
#include "geVector3.h"
#include "geQuaternion.h"
#include "geRotator.h"
#include "geMath.h"

using namespace geEngineSDK;
using Catch::Approx;

namespace {
  inline void
  near(float a, float b, float eps = 1e-5f) {
    REQUIRE(std::fabs(a - b) <= eps);
  }

  inline void
  vecNear(const Vector3& a, const Vector3& b, float eps = 1e-4f) {
    REQUIRE(std::fabs(a.x - b.x) <= eps);
    REQUIRE(std::fabs(a.y - b.y) <= eps);
    REQUIRE(std::fabs(a.z - b.z) <= eps);
  }

  inline void
  matNear(const Matrix4& A, const Matrix4& B, float eps = 1e-4f) {
    for (int r = 0; r < 4; ++r) {
      for (int c = 0; c < 4; ++c) {
        REQUIRE(std::fabs(A.m[r][c] - B.m[r][c]) <= eps);
      }
    }
  }

  inline bool
  matIsFinite(const Matrix4& M) {
    for (int r = 0; r < 4; ++r) {
      for (int c = 0; c < 4; ++c) {
        if (!std::isfinite(M.m[r][c])) {
          return false;
        }
      }
    }
    return true;
  }
}

TEST_CASE("Matrix4: constants ZERO/IDENTITY", "[Math][Matrix4]") {
  matNear(Matrix4::IDENTITY * Matrix4::IDENTITY, Matrix4::IDENTITY);
  matNear(Matrix4::ZERO * Matrix4::IDENTITY, Matrix4::ZERO);

  //I * v = v
  Vector3 v(1, 2, 3);
  vecNear(Matrix4::IDENTITY.transformVector(v), v);
  vecNear(Matrix4::IDENTITY.transformPosition(v), v);

  REQUIRE(matIsFinite(Matrix4::ZERO));
}

TEST_CASE("Matrix4: transformVector vs transformPosition handles translation",\
          "[Math][Matrix4]") {
  Matrix4 T = Matrix4::IDENTITY;

  T.m[3][0] = 10.0f;
  T.m[3][1] = 20.0f;
  T.m[3][2] = 30.0f;

  Vector3 p(1, 2, 3);
  Vector3 v(1, 2, 3);

  vecNear(T.transformPosition(p), Vector3(11, 22, 33));
  vecNear(T.transformVector(v), v);
}

TEST_CASE("Matrix4: transpose properties", "[Math][Matrix4]") {
  Matrix4 M = Matrix4::IDENTITY;
  M.m[0][1] = 2.0f;
  M.m[1][2] = 3.0f;
  M.m[2][3] = 4.0f;

  Matrix4 Mt = M.getTransposed();
  REQUIRE(Mt.m[1][0] == Approx(2.0f));
  REQUIRE(Mt.m[2][1] == Approx(3.0f));
  REQUIRE(Mt.m[3][2] == Approx(4.0f));

  matNear(Mt.getTransposed(), M);
}

TEST_CASE("Matrix4: determinant sanity", "[Math][Matrix4]") {
  near(Matrix4::IDENTITY.determinant(), 1.0f);

  Matrix4 S = Matrix4::IDENTITY;
  S.m[0][0] = 2.0f;
  S.m[1][1] = 2.0f;
  S.m[2][2] = 2.0f;

  near(S.determinant(), 8.0f, 1e-3f);
}

TEST_CASE("Matrix4: inverse correctness on simple TRS", "[Math][Matrix4]") {
  Matrix4 M = Matrix4::IDENTITY;
  M.m[0][0] = 2.0f;
  M.m[1][1] = 3.0f;
  M.m[2][2] = 4.0f;
  M.m[3][0] = 10.0f;
  M.m[3][1] = 20.0f;
  M.m[3][2] = 30.0f;

  Matrix4 Inv = M.inverse();
  Matrix4 I = M * Inv;

  matNear(I, Matrix4::IDENTITY, 2e-3f);

  Vector3 p(5, 6, 7);
  vecNear(Inv.transformPosition(M.transformPosition(p)), p, 2e-3f);
}

TEST_CASE("Matrix4: axis getters are consistent with basis", "[Math][Matrix4]") {
  vecNear(Matrix4::IDENTITY.getScaledAxis(AXIS::kX), Vector3(1, 0, 0));
  vecNear(Matrix4::IDENTITY.getScaledAxis(AXIS::kY), Vector3(0, 1, 0));
  vecNear(Matrix4::IDENTITY.getScaledAxis(AXIS::kZ), Vector3(0, 0, 1));
}

TEST_CASE("LinkSmoke: Matrix4 touches exported/non-inline members", "[Link][Matrix4]") {
  Matrix4 M = Matrix4::IDENTITY;
  (void)M.determinant();
  (void)M.inverse();
  (void)M.inverseFast();
  (void)M.transpose();
  (void)M.transposeAdjoint();
  (void)M.rotDeterminant();
  (void)M.rotator();
  (void)M.toQuaternion();
  SUCCEED();
}
