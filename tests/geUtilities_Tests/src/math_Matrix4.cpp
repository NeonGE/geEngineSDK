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


  static bool nearly(float a, float b, float eps = 1e-5f) {
    return std::fabs(a - b) <= eps;
  }

  static bool vecNearly(const Vector3& a, const Vector3& b, float eps = 1e-5f) {
    return nearly(a.x, b.x, eps) && nearly(a.y, b.y, eps) && nearly(a.z, b.z, eps);
  }

  static void requireMatrixNearlyEqual(const Matrix4& A, const Matrix4& B, float eps = 1e-5f) {
    for (int r = 0; r < 4; ++r) {
      for (int c = 0; c < 4; ++c) {
        REQUIRE(nearly(A.m[r][c], B.m[r][c], eps));
      }
    }
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

TEST_CASE("TranslationMatrix: transforms positions by delta and leaves vectors unchanged", "[Matrix][Derived]") {
  const Vector3 delta(10, -5, 2);
  TranslationMatrix T(delta);

  const Vector3 p(1, 2, 3);
  const Vector3 v(1, 2, 3);

  const Vector3 p2 = T.transformPosition(p);
  REQUIRE(vecNearly(p2, p + delta));

  const Vector4 v2 = T.transformVector(v);
  REQUIRE(vecNearly(Vector3(v2.x, v2.y, v2.z), v));
}

TEST_CASE("ScaleMatrix: uniform scale scales position and vector", "[Matrix][Derived]") {
  ScaleMatrix S(2.0f);

  const Vector3 p(1, 2, 3);
  const Vector3 v(1, 2, 3);

  REQUIRE(vecNearly(S.transformPosition(p), Vector3(2, 4, 6)));

  const Vector4 v2 = S.transformVector(v);
  REQUIRE(vecNearly(Vector3(v2.x, v2.y, v2.z), Vector3(2, 4, 6)));
}

TEST_CASE("ScaleMatrix: non-uniform scale scales per axis", "[Matrix][Derived]") {
  ScaleMatrix S(Vector3(2, 3, 4));

  const Vector3 p(1, 2, 3);
  REQUIRE(vecNearly(S.transformPosition(p), Vector3(2, 6, 12)));

  const Vector4 v2 = S.transformVector(Vector3(1, 2, 3));
  REQUIRE(vecNearly(Vector3(v2.x, v2.y, v2.z), Vector3(2, 6, 12)));
}

TEST_CASE("BasisVectorMatrix: builds a basis + origin correctly", "[Matrix][Derived]") {
  const Vector3 X(1, 0, 0);
  const Vector3 Y(0, 1, 0);
  const Vector3 Z(0, 0, 1);
  const Vector3 O(10, 20, 30);

  BasisVectorMatrix B(X, Y, Z, O);

  // Transform origin should map to origin + translation (identity basis)
  const Vector3 p(1, 2, 3);
  REQUIRE(vecNearly(B.transformPosition(p), p + O));

  // Vector should be unchanged (identity basis)
  const Vector4 v2 = B.transformVector(Vector3(4, 5, 6));
  REQUIRE(vecNearly(Vector3(v2.x, v2.y, v2.z), Vector3(4, 5, 6)));
}

TEST_CASE("RotationMatrix: equals RotationTranslationMatrix with ZERO translation", "[Matrix][Derived]") {
  // Usamos una rotación sencilla.
  Rotator r;
  r.pitch = 0.0f;
  r.yaw = 90.0f;
  r.roll = 0.0f;

  RotationMatrix RM(r);
  RotationTranslationMatrix RT(r, Vector3::ZERO);

  requireMatrixNearlyEqual((const Matrix4&)RM, (const Matrix4&)RT, 1e-4f);

  // Además: la traslación debe ser cero
  REQUIRE(nearly(((const Matrix4&)RM).m[3][0], 0.0f));
  REQUIRE(nearly(((const Matrix4&)RM).m[3][1], 0.0f));
  REQUIRE(nearly(((const Matrix4&)RM).m[3][2], 0.0f));
}

TEST_CASE("RotationTranslationMatrix: translation component moves position", "[Matrix][Derived]") {
  Rotator r;
  r.pitch = 0.0f;
  r.yaw = 0.0f;
  r.roll = 0.0f;

  const Vector3 t(5, 6, 7);
  RotationTranslationMatrix M(r, t);

  const Vector3 p(1, 2, 3);
  REQUIRE(vecNearly(M.transformPosition(p), p + t));
}

TEST_CASE("ScaleRotationTranslationMatrix: pure translation/scale cases behave", "[Matrix][Derived]") {
  // Caso 1: rot=0, scale=1 -> igual a TranslationMatrix
  Rotator r0; r0.pitch = r0.yaw = r0.roll = 0.0f;
  const Vector3 scale1(1, 1, 1);
  const Vector3 trans(3, 4, 5);

  ScaleRotationTranslationMatrix SRT(scale1, r0, trans);
  TranslationMatrix T(trans);

  REQUIRE(vecNearly(SRT.transformPosition(Vector3(1, 2, 3)), T.transformPosition(Vector3(1, 2, 3))));

  // Caso 2: trans=0, rot=0 -> igual a ScaleMatrix
  const Vector3 scale(2, 3, 4);
  ScaleRotationTranslationMatrix SRT2(scale, r0, Vector3::ZERO);
  ScaleMatrix SM(scale);

  REQUIRE(vecNearly(SRT2.transformPosition(Vector3(1, 2, 3)), SM.transformPosition(Vector3(1, 2, 3))));
}

TEST_CASE("RotationAboutPointMatrix: rotating about point leaves that point invariant", "[Matrix][Derived]") {
  Rotator r;
  r.pitch = 0.0f;
  r.yaw = 90.0f;
  r.roll = 0.0f;

  const Vector3 origin(10, 20, 30);
  RotationAboutPointMatrix M(r, origin);

  // Punto de pivote debe quedarse igual
  REQUIRE(vecNearly(M.transformPosition(origin), origin, 1e-3f));
}

TEST_CASE("InverseRotationMatrix: is inverse of RotationMatrix (3x3)", "[Matrix][Derived]") {
  Rotator r(10.f, 20.f, 30.f);

  RotationMatrix R(r);
  InverseRotationMatrix IR(r);

  const Matrix4& A = (const Matrix4&)R;
  const Matrix4& B = (const Matrix4&)IR;

  // Check A * B ~= Identity in 3x3 (pero OJO: tu operator* no es estándar).
  // Mejor validar por aplicación a vectores:

  Vector3 v(3, 4, 5);
  Vector4 vR = A.transformVector4(Vector4(v.x, v.y, v.z, 0.f));
  Vector4 vRR = B.transformVector4(Vector4(vR.x, vR.y, vR.z, 0.f));

  REQUIRE(std::fabs(vRR.x - v.x) < 1e-3f);
  REQUIRE(std::fabs(vRR.y - v.y) < 1e-3f);
  REQUIRE(std::fabs(vRR.z - v.z) < 1e-3f);
}

TEST_CASE("QuatRotationMatrix: equals QuatRotationTranslationMatrix with ZERO translation", "[Matrix][Derived]") {
  // Quaternion identity: debe ser identity (sin traslación)
  Quaternion q = Quaternion::IDENTITY;

  QuatRotationMatrix QM(q);
  QuatRotationTranslationMatrix QMT(q, Vector3::ZERO);

  requireMatrixNearlyEqual((const Matrix4&)QM, (const Matrix4&)QMT, 1e-5f);

  // Identidad: diagonal 1
  const Matrix4& M = (const Matrix4&)QM;
  REQUIRE(nearly(M.m[0][0], 1.0f));
  REQUIRE(nearly(M.m[1][1], 1.0f));
  REQUIRE(nearly(M.m[2][2], 1.0f));
  REQUIRE(nearly(M.m[3][3], 1.0f));
}

TEST_CASE("QuatRotationTranslationMatrix: translation moves positions", "[Matrix][Derived]") {
  Quaternion q = Quaternion::IDENTITY;
  const Vector3 t(7, 8, 9);

  QuatRotationTranslationMatrix M(q, t);

  const Vector3 p(1, 2, 3);
  REQUIRE(vecNearly(M.transformPosition(p), p + t));
}

TEST_CASE("MirrorMatrix: mirroring across Y=0 flips Y and keeps X/Z for points on axes", "[Matrix][Derived]") {
  // Tu comentario dice: Plane uses Ax+By+Cz=D (asumido normalizado).
  // Plano Y=0: normal (0,1,0), D=0
  Plane plane(0, 1, 0, 0);

  MirrorMatrix M(plane);

  // Punto cualquiera: Y se invierte al espejar contra Y=0
  Vector3 p(3, 5, -2);
  Vector3 pm = M.transformPosition(p);

  REQUIRE(nearly(pm.x, p.x, 1e-4f));
  REQUIRE(nearly(pm.y, -p.y, 1e-4f));
  REQUIRE(nearly(pm.z, p.z, 1e-4f));
}

TEST_CASE("LookAtMatrix: eye transforms near origin (view matrix sanity)", "[Matrix][Derived]") {
  // LH look-at style (como D3DXMatrixLookAtLH).
  // Up = +Y (tu convención).
  const Vector3 eye(0, 0, -10);
  const Vector3 at(0, 0, 0);
  const Vector3 up(0, 1, 0);

  LookAtMatrix V(eye, at, up);

  // En una view matrix estándar, Eye debe mapear a origen o muy cercano
  const Vector3 eyeCam = V.transformPosition(eye);
  REQUIRE(nearly(eyeCam.x, 0.0f, 1e-3f));
  REQUIRE(nearly(eyeCam.y, 0.0f, 1e-3f));
  REQUIRE(nearly(eyeCam.z, 0.0f, 1e-3f));
}

TEST_CASE("OrthoMatrix: basic invariants (m[3][3] == 1)", "[Matrix][Derived]") {
  OrthoMatrix O(640.0f, 480.0f, 1.0f, 0.0f);

  const Matrix4& M = (const Matrix4&)O;
  REQUIRE(nearly(M.m[3][3], 1.0f));
}

TEST_CASE("PerspectiveMatrix: basic invariants (projection-style row/col)", "[Matrix][Derived]") {
  // No validamos fórmula exacta aquí (depende de tu convención), solo invariantes típicos
  // y que sea consistente (no NaNs).
  PerspectiveMatrix P(
    /*HalfFOVX*/ 45.0f,
    /*HalfFOVY*/ 45.0f,
    /*MultFOVX*/ 1.0f,
    /*MultFOVY*/ 1.0f,
    /*MinZ*/     0.1f,
    /*MaxZ*/     1000.0f
  );

  const Matrix4& M = (const Matrix4&)P;

  for (int r = 0; r < 4; ++r) {
    for (int c = 0; c < 4; ++c) {
      REQUIRE(std::isfinite(M.m[r][c]));
    }
  }

  // Proyección típica: m[3][3] suele ser 0
  REQUIRE(nearly(M.m[3][3], 0.0f, 1e-3f));
}

TEST_CASE("ReversedZPerspectiveMatrix: finite values + projection-ish m[3][3]==0", "[Matrix][Derived]") {
  ReversedZPerspectiveMatrix P(
    45.0f, 45.0f,
    1.0f, 1.0f,
    0.1f, 1000.0f
  );

  const Matrix4& M = (const Matrix4&)P;
  for (int r = 0; r < 4; ++r) {
    for (int c = 0; c < 4; ++c) {
      REQUIRE(std::isfinite(M.m[r][c]));
    }
  }
  REQUIRE(nearly(M.m[3][3], 0.0f, 1e-3f));
}

TEST_CASE("ReversedZOrthoMatrix: basic invariants (m[3][3] == 1)", "[Matrix][Derived]") {
  ReversedZOrthoMatrix O(640.0f, 480.0f, 1.0f, 0.0f);
  const Matrix4& M = (const Matrix4&)O;
  REQUIRE(nearly(M.m[3][3], 1.0f));
}

TEST_CASE("ClipProjectionMatrix: produces finite matrix (Perspective)", "[Matrix][Derived]") {
  PerspectiveMatrix P(45.f, 45.f, 1.f, 1.f, 0.1f, 1000.f);

  // plane in camera space: z = 1  (Ax+By+Cz=D)
  Plane clipPlane(0.f, 0.f, 1.f, 1.f);

  ClipProjectionMatrix C((const Matrix4&)P, clipPlane);
  const Matrix4& M = (const Matrix4&)C;

  for (int r = 0; r < 4; ++r) {
    for (int c = 0; c < 4; ++c) {
      REQUIRE(std::isfinite(M.m[r][c]));
    }
  }
}
