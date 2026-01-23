/*****************************************************************************/
/**
 * @file    gePlane.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/19
 * @brief   Structure for three dimensional planes.
 *
 * Structure for three dimensional planes.
 * Stores the coefficients as Xx+Yy+Zz=W. Note that this is different from many
 * other Plane classes that use Xx+Yy+Zz+W=0.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geVector3.h"
#include "geVector4.h"

namespace geEngineSDK {
  class ALIGN_AS(16) Plane : public Vector3
  {
   public:
    /**
     * @brief Default constructor (no initialization).
     */
    FORCEINLINE Plane() = default;

    /**
     * @brief Constructor.
     * @param V 4D vector to set up plane.
     */
    FORCEINLINE Plane(const Vector4& V);

    /**
     * @brief Constructor.
     * @param InX X-coefficient.
     * @param InY Y-coefficient.
     * @param InZ Z-coefficient.
     * @param InW W-coefficient.
     */
    FORCEINLINE Plane(float InX, float InY, float InZ, float InW);

    /**
     * @brief Constructor.
     * @param InNormal Plane Normal Vector.
     * @param InW Plane W-coefficient.
     */
    FORCEINLINE Plane(const Vector3& InNormal, float InW);

    /**
     * @brief Constructor.
     * @param InBase Base point in plane.
     * @param InNormal Plane Normal Vector.
     */
    FORCEINLINE Plane(const Vector3& InBase, const Vector3 &InNormal);

    /**
     * @brief Constructor.
     * @param A First point in the plane.
     * @param B Second point in the plane.
     * @param C Third point in the plane.
     */
    Plane(const Vector3& A, const Vector3& B, const Vector3& C);

    /**
     * @brief Constructor
     * @param EForceInit Force Init Enum.
     */
    explicit FORCEINLINE Plane(FORCE_INIT::E);

    /*************************************************************************/
    /**
     * Functions.
     */
    /*************************************************************************/

    /**
     * @brief Calculates distance between plane and a point.
     * @param P The other point.
     * @return >0: point is in front of the plane, <0: behind, =0: on the plane.
     */
    FORCEINLINE float
    planeDot(const Vector3 &P) const;

    /**
     * @brief Get a flipped version of the plane.
     * @return A flipped version of the plane.
     */
    Plane
    flip() const;

    /**
     * @brief Get the result of transforming the plane by a Matrix.
     * @param M The matrix to transform plane with.
     * @return The result of transform.
     */
    Plane
    transformBy(const Matrix4& M) const;

    /**
     * @brief You can optionally pass in the matrices transpose-adjoint, which
     *        save it recalculating it.
     * MSM: If we are going to save the transpose-adjoint we should also save
     *      the more expensive determinant.
     * @param M The Matrix to transform plane with.
     * @param DetM Determinant of Matrix.
     * @param TA Transpose-adjoint of Matrix.
     * @return The result of transform.
     */
    Plane
    transformByUsingAdjointT(const Matrix4& M, float DetM, const Matrix4& TA) const;

    /**
     * @brief Check if two planes are identical.
     * @param V The other plane.
     * @return true if planes are identical, otherwise false.
     */
    bool
    operator==(const Plane& V) const;

    /**
     * @brief Check if two planes are different.
     * @param V The other plane.
     * @return true if planes are different, otherwise false.
     */
    bool
    operator!=(const Plane& V) const;

    /**
     * @brief Checks whether two planes are equal within specified tolerance.
     * @param V The other plane.
     * @param Tolerance Error Tolerance.
     * @return true if the two planes are equal within specified tolerance, otherwise false.
     */
    bool
    equals(const Plane& V, float Tolerance = Math::KINDA_SMALL_NUMBER) const;

    /**
     * @brief Calculates dot product of two planes.
     * @param V The other plane.
     * @return The dot product.
     */
    FORCEINLINE float
    operator|(const Plane& V) const;

    /**
     * @brief Gets result of adding a plane to this.
     * @param V The other plane.
     * @return The result of adding a plane to this.
     */
    Plane
    operator+(const Plane& V) const;

    /**
     * @brief Gets result of subtracting a plane from this.
     * @param V The other plane.
     * @return The result of subtracting a plane from this.
     */
    Plane
    operator-(const Plane& V) const;

    /**
     * @brief Gets result of dividing a plane.
     * @param Scale What to divide by.
     * @return The result of division.
     */
    Plane
    operator/(float Scale) const;

    /**
     * @brief Gets result of scaling a plane.
     * @param Scale The scaling factor.
     * @return The result of scaling.
     */
    Plane
    operator*(float Scale) const;

    /**
     * @brief Gets result of multiplying a plane with this.
     * @param V The other plane.
     * @return The result of multiplying a plane with this.
     */
    Plane
    operator*(const Plane& V);

    /**
     * @brief Add another plane to this.
     * @param V The other plane.
     * @return Copy of plane after addition.
     */
    Plane&
    operator+=(const Plane& V);

    /**
     * @brief Subtract another plane from this.
     * @param V The other plane.
     * @return Copy of plane after subtraction.
     */
    Plane&
    operator-=(const Plane& V);

    /**
     * @brief Scale this plane.
     * @param Scale The scaling factor.
     * @return Copy of plane after scaling.
     */
    Plane&
    operator*=(float Scale);

    /**
     * @brief Multiply another plane with this.
     * @param V The other plane.
     * @return Copy of plane after multiplication.
     */
    Plane&
    operator*=(const Plane& V);

    /**
     * @brief Divide this plane.
     * @param V What to divide by.
     * @return Copy of plane after division.
     */
    Plane&
    operator/=(float V);

   public:
    /**
     * @brief The w-component.
     */
    float w;
  };
}

namespace geEngineSDK {
  /***************************************************************************/
  /**
   * Math Functions.
   */
  /***************************************************************************/

  inline Vector3
  Math::linePlaneIntersection(const Vector3& Point1,
                              const Vector3& Point2,
                              const Plane& plane) {
    return Point1 + (Point2 - Point1) *	
           ((plane.w - (Point1 | plane)) / ((Point2 - Point1) | plane));
  }

  inline bool
  Math::intersectPlanes3(Vector3& I, const Plane& P1, const Plane& P2, const Plane& P3) {
    //Compute determinant, the triple product P1|(P2^P3)==(P1^P2)|P3.
    const float Det = (P1 ^ P2) | P3;
    if (square(Det) < square(0.001f)) {
      //Degenerate.
      I = Vector3::ZERO;
      return false;
    }
    else {
      //Compute the intersection point, guaranteed valid if determinant is nonzero.
      I = (P1.w*(P2^P3) + P2.w*(P3^P1) + P3.w*(P1^P2)) / Det;
    }
    return true;
  }

  inline bool
  Math::intersectPlanes2(Vector3& I, Vector3& D, const Plane& P1, const Plane& P2) {
    //Compute line direction, perpendicular to both plane normals.
    D = P1 ^ P2;
    const float DD = D.sizeSquared();
    if (DD < square(0.001f)) {
      //Parallel or nearly parallel planes.
      D = I = Vector3::ZERO;
      return false;
    }

    //Compute intersection.
    I = (P1.w*(P2^D) + P2.w*(D^P1)) / DD;
    D.normalize();
    return true;
  }

  /***************************************************************************/
  /**
   * Vector3 inline functions
   */
  /***************************************************************************/

  inline Vector3
  Vector3::mirrorByPlane(const Plane& plane) const {
    return *this - plane * (2.f * plane.planeDot(*this));
  }

  inline Vector3
  Vector3::pointPlaneProject(const Vector3& point, const Plane& plane) {
    //Find the distance of x from the plane
    //Add the distance back along the normal from the point
    return point - plane.planeDot(point) * plane;
  }

  inline Vector3
  Vector3::pointPlaneProject(const Vector3& point,
                             const Vector3& a,
                             const Vector3& b,
                             const Vector3& c) {
    //Compute the plane normal from ABC
    Plane plane(a, b, c);

    //Find the distance of x from the plane
    //Add the distance back along the normal from the point
    return point - plane.planeDot(point) * plane;
  }

  /***************************************************************************/
  /**
   * Plane inline functions
   */
  /***************************************************************************/

  FORCEINLINE Plane::Plane(const Vector4& V) : Vector3(V), w(V.w) {}

  FORCEINLINE Plane::Plane(float InX, float InY, float InZ, float InW)
    : Vector3(InX, InY, InZ),
      w(InW)
  {}

  FORCEINLINE Plane::Plane(const Vector3& InNormal, float InW)
    : Vector3(InNormal),
      w(InW)
  {}

  FORCEINLINE Plane::Plane(const Vector3& InBase, const Vector3& InNormal)
    : Vector3(InNormal),
      w(InBase | InNormal)
  {}

  FORCEINLINE Plane::Plane(const Vector3& A, const Vector3& B, const Vector3& C)
    : Vector3(((B - A) ^ (C - A)).getSafeNormal()) {
    w = A | *this;
  }

  FORCEINLINE Plane::Plane(FORCE_INIT::E)
    : Vector3(FORCE_INIT::kForceInitToZero), w(0.f)
  {}

  FORCEINLINE float
  Plane::planeDot(const Vector3 &P) const {
    return x * P.x + y * P.y + z * P.z - w;
  }

  FORCEINLINE Plane
  Plane::flip() const {
    return Plane(-x, -y, -z, -w);
  }

  FORCEINLINE bool
  Plane::operator==(const Plane& V) const {
    return (x == V.x) && (y == V.y) && (z == V.z) && (w == V.w);
  }
  
  FORCEINLINE bool
  Plane::operator!=(const Plane& V) const {
    return (x != V.x) || (y != V.y) || (z != V.z) || (w != V.w);
  }

  FORCEINLINE bool
  Plane::equals(const Plane& V, float Tolerance) const {
    return (Math::abs(x - V.x) < Tolerance) &&
           (Math::abs(y - V.y) < Tolerance) &&
           (Math::abs(z - V.z) < Tolerance) &&
           (Math::abs(w - V.w) < Tolerance);
  }

  FORCEINLINE float
  Plane::operator|(const Plane& V) const {
    return x * V.x + y * V.y + z * V.z + w * V.w;
  }

  FORCEINLINE Plane
  Plane::operator+(const Plane& V) const {
    return Plane(x + V.x, y + V.y, z + V.z, w + V.w);
  }

  FORCEINLINE Plane
  Plane::operator-(const Plane& V) const {
    return Plane(x - V.x, y - V.y, z - V.z, w - V.w);
  }

  FORCEINLINE Plane
  Plane::operator/(float Scale) const {
    const float RScale = 1.f / Scale;
    return Plane(x * RScale, y * RScale, z * RScale, w * RScale);
  }

  FORCEINLINE Plane
  Plane::operator*(float Scale) const {
    return Plane(x * Scale, y * Scale, z * Scale, w * Scale);
  }

  FORCEINLINE Plane
  Plane::operator*(const Plane& V) {
    return Plane(x * V.x, y * V.y, z * V.z, w * V.w);
  }

  FORCEINLINE Plane&
  Plane::operator+=(const Plane& V) {
    x += V.x; y += V.y; z += V.z; w += V.w;
    return *this;
  }

  FORCEINLINE Plane&
  Plane::operator-=(const Plane& V) {
    x -= V.x; y -= V.y; z -= V.z; w -= V.w;
    return *this;
  }


  FORCEINLINE Plane&
  Plane::operator*=(float Scale) {
    x *= Scale; y *= Scale; z *= Scale; w *= Scale;
    return *this;
  }

  FORCEINLINE Plane&
  Plane::operator*=(const Plane& V) {
    x *= V.x; y *= V.y; z *= V.z; w *= V.w;
    return *this;
  }

  FORCEINLINE Plane&
  Plane::operator/=(float V) {
    const float RV = 1.f / V;
    x *= RV; y *= RV; z *= RV; w *= RV;
    return *this;
  }
}
