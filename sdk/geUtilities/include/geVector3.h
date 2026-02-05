/*****************************************************************************/
/**
 * @file    geVector3.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/06/28
 * @brief   A vector in 3-D space composed of components (X, Y, Z).
 *
 * A vector in 3-D space composed of components (X, Y, Z) with floating point
 * precision
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

#ifndef _INC_VECTOR3_H_
# define _INC_VECTOR3_H_
#endif

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geMath.h"

#if USING(GE_REFLECTION)
# include <rttr/type>
#endif

namespace geEngineSDK {
  /**
   * @brief A vector in 3-D space composed of components (x, y, z) with
   *        floating point precision.
   */
  class Vector3
  {
   public:
    /**
     * @brief Default constructor (no initialization).
     */
    FORCEINLINE Vector3() = default;

    /**
     * @brief Constructor initializing all components to a single float value.
     * @param inF Value to set all components to.
     */
    explicit FORCEINLINE Vector3(float inF);

    /**
     * @brief Constructor using initial values for each component.
     * @param inX X Coordinate.
     * @param inY Y Coordinate.
     * @param inZ Z Coordinate.
     */
    FORCEINLINE Vector3(float inX, float inY, float inZ);

    /**
     * @brief Constructs a vector from an Vector32D and Z value.
     * @param v Vector to copy from.
     * @param inZ Z Coordinate.
     */
    GE_UTILITIES_EXPORT Vector3(const Vector2& v, float inZ);

    /**
     * @brief Constructor using the XYZ components from a 4D vector.
     * @param v 4D Vector to copy from.
     */
    FORCEINLINE Vector3(const Vector4& v);

    /**
     * @brief Constructs a vector from an LinearColor.
     * @param inColor Color to copy from.
     */
    GE_UTILITIES_EXPORT explicit Vector3(const LinearColor& inColor);

    /**
     * @brief Constructs a vector from an Vector2I.
     * @param inVector Vector2I to copy from.
     */
    GE_UTILITIES_EXPORT explicit Vector3(Vector2I& inVector);

    /**
     * @brief Constructor which initializes all components to zero.
     * @param Force init enum
     */
    explicit FORCEINLINE Vector3(FORCE_INIT::E);

    /**
     * @brief Calculate cross product between this and another vector.
     * @param v The other vector.
     * @return The cross product.
     */
    FORCEINLINE Vector3
    operator^(const Vector3& v) const;

    /**
     * @brief Calculate the cross product of two vectors.
     * @param a The first vector.
     * @param b The second vector.
     * @return The cross product.
     */
    static FORCEINLINE Vector3
    crossProduct(const Vector3& a, const Vector3& b);

    /**
     * @brief Calculate the dot product between this and another vector.
     * @param v The other vector.
     * @return The dot product.
     */
    FORCEINLINE float
    operator|(const Vector3& v) const;

    /**
     * @brief Calculate the dot product of two vectors.
     * @param a The first vector.
     * @param b The second vector.
     * @return The dot product.
     */
    static FORCEINLINE float
    dot(const Vector3& a, const Vector3& b);

    /**
     * @brief Gets the result of component-wise addition of this and another vector.
     * @param v The vector to add to this.
     * @return The result of vector addition.
     */
    FORCEINLINE Vector3
    operator+(const Vector3& v) const;

    /**
     * @brief Gets the result of component-wise subtraction of this by another vector.
     * @param v The vector to subtract from this.
     * @return The result of vector subtraction.
     */
    FORCEINLINE Vector3
    operator-(const Vector3& v) const;

    /**
     * @brief Gets the result of subtracting from each component of the vector.
     * @param bias How much to subtract from each component.
     * @return The result of subtraction.
     */
    FORCEINLINE Vector3
    operator-(float bias) const;

    /**
     * @brief Gets the result of adding to each component of the vector.
     * @param bias How much to add to each component.
     * @return The result of addition.
     */
    FORCEINLINE Vector3
    operator+(float bias) const;

    /**
     * @brief Gets the result of scaling the vector (multiplying each component by a value).
     * @param scale What to multiply each component by.
     * @return The result of multiplication.
     */
    FORCEINLINE Vector3
    operator*(float scale) const;

    /**
     * @brief Gets the result of dividing each component of the vector by a value.
     * @param scale What to divide each component by.
     * @return The result of division.
     */
    Vector3
    operator/(float scale) const;

    /**
     * @brief Gets the result of component-wise multiplication of this vector by another.
     * @param v The vector to multiply with.
     * @return The result of multiplication.
     */
    FORCEINLINE Vector3
    operator*(const Vector3& v) const;

    /**
     * @brief Gets the result of component-wise division of this vector by another.
     * @param v The vector to divide by.
     * @return The result of division.
     */
    FORCEINLINE Vector3
    operator/(const Vector3& v) const;

    /**
     * @brief Check against another vector for equality.
     * @param v The vector to check against.
     * @return true if the vectors are equal, false otherwise.
     */
    FORCEINLINE bool
    operator==(const Vector3& v) const;

    /**
     * @brief Check against another vector for inequality.
     * @param v The vector to check against.
     * @return true if the vectors are not equal, false otherwise.
     */
    FORCEINLINE bool
    operator!=(const Vector3& v) const;

    /**
     * @brief Check against another vector for equality, within specified error limits.
     * @param v The vector to check against.
     * @param tolerance Error tolerance.
     * @return true if the vectors are equal within tolerance limits, false otherwise.
     */
    bool
    equals(const Vector3& v, float tolerance = Math::KINDA_SMALL_NUMBER) const;

    /**
     * @brief Checks whether all components of this vector are the same, within a tolerance.
     * @param tolerance Error tolerance.
     * @return true if the vectors are equal within tolerance limits, false otherwise.
     */
    bool
    allComponentsEqual(float tolerance = Math::KINDA_SMALL_NUMBER) const;

    /**
     * @brief Get a negated copy of the vector.
     * @return A negated copy of the vector.
     */
    FORCEINLINE Vector3
    operator-() const;

    /**
     * @brief Adds another vector to this.
     *        Uses component-wise addition.
     * @param v Vector to add to this.
     * @return Copy of the vector after addition.
     */
    FORCEINLINE Vector3&
    operator+=(const Vector3& v);

    /**
     * @brief Subtracts another vector from this.
     * Uses component-wise subtraction.
     * @param v Vector to subtract from this.
     * @return Copy of the vector after subtraction.
     */
    FORCEINLINE Vector3&
    operator-=(const Vector3& v);

    /**
     * @brief Scales the vector.
     * @param scale Amount to scale this vector by.
     * @return Copy of the vector after scaling.
     */
    FORCEINLINE Vector3&
    operator*=(float scale);

    /**
     * @brief Divides the vector by a number.
     * @param v What to divide this vector by.
     * @return Copy of the vector after division.
     */
    Vector3&
    operator/=(float v);

    /**
     * @brief Multiplies the vector with another vector, using component-wise multiplication.
     * @param v What to multiply this vector with.
     * @return Copy of the vector after multiplication.
     */
    Vector3&
    operator*=(const Vector3& v);

    /**
     * @brief Divides the vector by another vector, using component-wise division.
     * @param v What to divide vector by.
     * @return Copy of the vector after division.
     */
    Vector3&
    operator/=(const Vector3& v);

    /**
     * @brief Gets specific component of the vector.
     * @param index the index of vector component
     * @return reference to component.
     */
    float&
    operator[](uint32 index);

    /**
     * @brief Gets specific component of the vector.
     * @param index the index of vector component
     * @return Copy of the component.
     */
    float
    operator[](uint32 index)const;

    /**
     * @brief Gets a specific component of the vector.
     * @param index The index of the component required.
     * @return Reference to the specified component.
     */
    float&
    component(uint32 index);

    /**
     * @brief Gets a specific component of the vector.
     * @param index The index of the component required.
     * @return Copy of the specified component.
     */
    float
    component(uint32 index) const;

    /**
     * @brief Set the values of the vector directly.
     * @param inX New X coordinate.
     * @param inY New Y coordinate.
     * @param inZ New Z coordinate.
     */
    void
    set(float inX, float inY, float inZ);

    /**
     * @brief Get the maximum value of the vector's components.
     * @return The maximum value of the vector's components.
     */
    float
    getMax() const;

    /**
     * @brief Get the maximum absolute value of the vector's components.
     * @return The maximum absolute value of the vector's components.
     */
    float
    getAbsMax() const;

    /**
    * @brief Get the minimum value of the vector's components.
    * @return The minimum value of the vector's components.
    */
    float
    getMin() const;

    /**
     * @brief Get the minimum absolute value of the vector's components.
     * @return The minimum absolute value of the vector's components.
     */
    float
    getAbsMin() const;

    /**
     * @brief Gets the component-wise min of two vectors.
     */
    Vector3
    componentMin(const Vector3& other) const;

    /**
     * @brief Gets the component-wise max of two vectors.
     */
    Vector3
    componentMax(const Vector3& other) const;

    /**
     * @brief Get a copy of this vector with absolute value of each component.
     * @return A copy of this vector with absolute value of each component.
     */
    Vector3
    getAbs() const;

    /**
     * @brief Get the length (magnitude) of this vector.
     * @return The length of this vector.
     */
    float
    size() const;

    /**
     * @brief Get the squared length of this vector.
     * @return The squared length of this vector.
     */
    float
    sizeSquared() const;

    /**
     * @brief Get the length of the 2D components of this vector.
     * @return The 2D length of this vector.
     */
    float
    size2D() const;

    /**
     * @brief Get the squared length of the 2D components of this vector.
     * @return The squared 2D length of this vector.
     */
    float
    sizeSquared2D() const;

    /**
     * @brief Checks whether vector is near to zero within a specified tolerance.
     * @param tolerance Error tolerance.
     * @return true if the vector is near to zero, false otherwise.
     */
    bool
    isNearlyZero(float tolerance = Math::KINDA_SMALL_NUMBER) const;

    /**
     * @brief Checks whether all components of the vector are exactly zero.
     * @return true if the vector is exactly zero, false otherwise.
     */
    bool
    isZero() const;

    /**
     * @brief Normalize this vector in-place if it is large enough, set it to
     *        (0,0,0) otherwise.
     * @param tolerance Minimum squared length of vector for normalization.
     * @return true if the vector was normalized correctly, false otherwise.
     */
    bool
    normalize(float Tolerance = Math::SMALL_NUMBER);

    /**
     * @brief Checks whether vector is normalized.
     * @return true if Normalized, false otherwise.
     */
    bool
    isNormalized() const;

    /**
     * @brief Utility to convert this vector into a unit direction vector and
     *        its original length.
     * @param outDir Reference passed in to store unit direction vector.
     * @param outLength Reference passed in to store length of the vector.
     */
    void
    toDirectionAndLength(Vector3& outDir, float& outLength) const;

    /**
     * @brief Get a copy of the vector as sign only.
     * Each component is set to +1 or -1, with the sign of zero treated as +1.
     * @param A copy of the vector with each component set to +1 or -1
     */
    FORCEINLINE Vector3
    getSignVector() const;

    /**
     * @brief Projects 2D components of vector based on Z.
     * @return Projected version of vector based on Z.
     */
    Vector3
    projection() const;

    /**
     * @brief Calculates normalized version of vector without checking for zero length.
     * @return Normalized version of vector.
     * @see getSafeNormal()
     */
    FORCEINLINE Vector3
    getUnsafeNormal() const;

    /**
     * @brief Gets a copy of this vector snapped to a grid.
     * @param gridSz Grid dimension.
     * @return A copy of this vector snapped to a grid.
     * @see FMath::GridSnap()
     */
    Vector3
    gridSnap(const float& gridSz) const;

    /**
     * @brief Get a copy of this vector, clamped inside of a cube.
     * @param radius Half size of the cube.
     * @return A copy of this vector, bound by cube.
     */
    Vector3
    boundToCube(float radius) const;

    /**
     * @brief Create a copy of this vector, with its magnitude clamped between Min and Max.
     */
    Vector3
    getClampedToSize(float Min, float Max) const;

    /**
     * @brief Create a copy of this vector, with the 2D magnitude clamped between
     *        Min and Max. Z is unchanged.
     */
    Vector3
    getClampedToSize2D(float Min, float Max) const;

    /**
     * @brief Create a copy of this vector, with its maximum magnitude clamped to maxSize.
     */
    Vector3
    getClampedToMaxSize(float maxSize) const;

    /**
     * @brief Create a copy of this vector, with the maximum 2D magnitude clamped to maxSize.
     *        Z is unchanged.
     */
    Vector3
    getClampedToMaxSize2D(float maxSize) const;

    /**
     * @brief Add a vector to this and clamp the result in a cube.
     * @param V Vector to add.
     * @param Radius Half size of the cube.
     */
    void
    addBounded(const Vector3& V, float Radius = NumLimit::MAX_INT16);

    /**
     * @brief Gets the reciprocal of this vector, avoiding division by zero.
     * Zero components are set to BIG_NUMBER.
     * @return Reciprocal of this vector.
     */
    Vector3
    reciprocal() const;

    /**
     * @brief Check whether X, Y and Z are nearly equal.
     * @param tolerance Specified Tolerance.
     * @return true if X == Y == Z within the specified tolerance.
     */
    bool
    isUniform(float tolerance = Math::KINDA_SMALL_NUMBER) const;

    /**
     * @brief Mirror a vector about a normal vector.
     * @param mirrorNormal Normal vector to mirror about.
     * @return Mirrored vector.
     */
    Vector3
    mirrorByVector(const Vector3& mirrorNormal) const;

    /**
     * @brief Mirrors a vector about a plane.
     * @param plane Plane to mirror about.
     * @return Mirrored vector.
     */
    Vector3
    mirrorByPlane(const Plane& plane) const;

    /**
     * @brief Rotates around Axis (assumes axis.size() == 1).
     * @param angleDeg angle to rotate (in degrees).
     * @param axis Axis to rotate around.
     * @return Rotated Vector.
     */
    Vector3
    rotateAngleAxis(const float angleDeg, const Vector3& axis) const;

    /**
     * @brief Gets a normalized copy of the vector, checking it is safe to do
     *        so based on the length. Returns zero vector if vector length is
     *        too small to safely normalize.
     * @param tolerance Minimum squared vector length.
     * @return A normalized copy if safe, (0,0,0) otherwise.
     */
    Vector3
    getSafeNormal(float tolerance = Math::SMALL_NUMBER) const;

    /**
     * @brief Gets a normalized copy of the 2D components of the vector,
     *        checking it is safe to do so. Z is set to zero. Returns zero
     *        vector if vector length is too small to normalize.
     * @param Tolerance Minimum squared vector length.
     * @return Normalized copy if safe, otherwise returns zero vector.
     */
    Vector3
    getSafeNormal2D(float Tolerance = Math::SMALL_NUMBER) const;

    /**
     * @brief Returns the cosine of the angle between this vector and another
     *        projected onto the XY plane (no Z).
     * @param B the other vector to find the 2D cosine of the angle with.
     * @return The cosine.
     */
    FORCEINLINE float
    cosineAngle2D(Vector3 B) const;

    /**
     * @brief Gets a copy of this vector projected onto the input vector.
     * @param A	Vector to project onto, does not assume it is normalized.
     * @return Projected vector.
     */
    FORCEINLINE Vector3
    projectOnTo(const Vector3& A) const;

    /**
     * @brief Gets a copy of this vector projected onto the input vector,
     *        which is assumed to be unit length.
     * @param  Normal Vector to project onto (assumed to be unit length).
     * @return Projected vector.
     */
    FORCEINLINE Vector3
    projectOnToNormal(const Vector3& Normal) const;

    /**
     * @brief Return the Rotator orientation corresponding to the direction in
     *        which the vector points. Sets Yaw and Pitch to the proper numbers
     *        and sets Roll to zero because the roll can't be determined from a
     *        vector.
     * @return Rotator from the Vector's direction, without any roll.
     * @see toOrientationQuat()
     */
    GE_UTILITIES_EXPORT Rotator
    toOrientationRotator() const;

    /**
     * @brief Return the Quaternion orientation corresponding to the direction
     *        in which the vector points. Similar to the Rotator version, returns
     *        a result without roll such that it preserves the up vector.
     *
     * @note If you don't care about preserving the up vector and just want the
     *       most direct rotation, you can use the faster:
     *       'Quaternion::findBetweenVectors(FVector::ForwardVector, YourVector)' or
     *       'Quaternion::findBetweenNormals(...)' if you know the vector is of unit length.
     * @return Quaternion from the Vector's direction, without any roll.
     * @see toOrientationRotator(), Quaternion::findBetweenVectors()
     */
    GE_UTILITIES_EXPORT Quaternion
    toOrientationQuat() const;

    /**
     * @brief Return the Rotator corresponding to the direction that the vector
     *        is pointing in. Sets Yaw and Pitch to the proper numbers, and sets
     *        roll to zero because the roll can't be determined from a vector.
     * @return The Rotator from the vector's direction.
     */
    GE_UTILITIES_EXPORT Rotator
    rotation() const;

    /**
     * @brief Find good arbitrary axis vectors to represent U and V axes of a
     *        plane, using this vector as the normal of the plane.
     * @param Axis1 Reference to first axis.
     * @param Axis2 Reference to second axis.
     */
    void
    findBestAxisVectors(Vector3& Axis1, Vector3& Axis2) const;

    /**
     * @brief When this vector contains Euler angles (degrees), ensure that
     *        angles are between +/-180
     */
    GE_UTILITIES_EXPORT void
    unwindEuler();

    /**
     * @brief Utility to check if there are any NaNs in this vector.
     * @return true if there are any NaNs in this vector, false otherwise.
     */
    bool
    containsNaN() const;

    /**
     * @brief Check if the vector is of unit length, with specified tolerance.
     * @param LengthSquaredTolerance Tolerance against squared length.
     * @return true if the vector is a unit vector within the specified tolerance.
     */
    FORCEINLINE bool
    isUnit(float LengthSquaredTolerance = Math::KINDA_SMALL_NUMBER) const;

    /**
     * @brief Converts a Cartesian unit vector into spherical coordinates on the unit sphere.
     * @return  Output Theta will be in the range [0, PI], and output Phi will
     *          be in the range [-PI, PI].
     */
    GE_UTILITIES_EXPORT Vector2
    unitCartesianToSpherical() const;

    /**
     * @brief Convert a direction vector into a 'heading' angle.
     * @return 'Heading' angle between +/-PI. 0 is pointing down +X.
     */
    float
    headingAngle() const;

    /**
     * @brief Create an orthonormal basis from a basis with at least two orthogonal vectors.
     *        It may change the directions of the X and Y axes to make the basis orthogonal,
     *        but it won't change the direction of the Z axis. All axes will be normalized.
     * @param XAxis The input basis' XAxis, and upon return the orthonormal basis' XAxis.
     * @param YAxis The input basis' YAxis, and upon return the orthonormal basis' YAxis.
     * @param ZAxis The input basis' ZAxis, and upon return the orthonormal basis' ZAxis.
     */
    GE_UTILITIES_EXPORT static void
    createOrthonormalBasis(Vector3& XAxis, Vector3& YAxis, Vector3& ZAxis);

    /**
     * @brief Compare two points and see if they're the same, using a threshold.
     * @param p First vector.
     * @param q Second vector.
     * @return  Whether points are the same within a threshold.
     *          Uses fast distance approximation (linear per-component distance).
     */
    static bool
    pointsAreSame(const Vector3& p, const Vector3& q);

    /**
     * @brief Compare two points and see if they're within specified distance.
     * @param point1 First vector.
     * @param point2 Second vector.
     * @param Dist Specified distance.
     * @return  Whether two points are within the specified distance.
     *          Uses fast distance approximation (linear per-component distance).
     */
    static bool
    pointsAreNear(const Vector3& point1, const Vector3& point2, float dist);

    /**
     * @brief Calculate the signed distance (in the direction of the normal)
     *        between a point and a plane.
     * @param point The Point we are checking.
     * @param planeBase The Base Point in the plane.
     * @param planeNormal The Normal of the plane (assumed to be unit length).
     * @return Signed distance between point and plane.
     */
    static float
    pointPlaneDist(const Vector3& point,
                   const Vector3& planeBase,
                   const Vector3& planeNormal);

    /**
     * @brief Calculate the projection of a point on the given plane.
     * @param Point The point to project onto the plane
     * @param Plane The plane
     * @return Projection of Point onto Plane
     */
    static Vector3
    pointPlaneProject(const Vector3& point, const Plane& plane);

    /**
     * @brief Calculate the projection of a point on the plane defined by
     *        counter-clockwise (CCW) points A,B,C.
     * @param Point The point to project onto the plane
     * @param a 1st of three points in CCW order defining the plane
     * @param b 2nd of three points in CCW order defining the plane
     * @param c 3rd of three points in CCW order defining the plane
     * @return Projection of Point onto plane ABC
     */
    static Vector3
    pointPlaneProject(const Vector3& point,
                      const Vector3& a,
                      const Vector3& b,
                      const Vector3& c);

    /**
     * @brief Calculate the projection of a point on the plane defined by
     *        planeBase and planeNormal.
     * @param Point The point to project onto the plane
     * @param planeBase Point on the plane
     * @param planeNorm Normal of the plane (assumed to be unit length).
     * @return Projection of Point onto plane
     */
    static Vector3
    pointPlaneProject(const Vector3& point,
                      const Vector3& planeBase,
                      const Vector3& planeNormal);

    /**
     * @brief Calculate the projection of a vector on the plane defined by PlaneNormal.
     * @param  V The vector to project onto the plane.
     * @param  PlaneNormal Normal of the plane (assumed to be unit length).
     * @return Projection of V onto plane.
     */
    static Vector3
    vectorPlaneProject(const Vector3& v, const Vector3& planeNormal);

    /**
     * @brief Euclidean distance between two points.
     * @param v1 The first point.
     * @param v2 The second point.
     * @return The distance between two points.
     */
    static FORCEINLINE float
    distance(const Vector3& v1, const Vector3& v2);

    /**
     * @brief Squared distance between two points.
     * @param v1 The first point.
     * @param v2 The second point.
     * @return The squared distance between two points.
     */
    static FORCEINLINE float
    distSquared(const Vector3& v1, const Vector3& v2);

    /**
     * @brief Compute pushout of a box from a plane.
     * @param normal The plane normal.
     * @param size The size of the box.
     * @return Pushout required.
     */
    static FORCEINLINE float
    boxPushOut(const Vector3& normal, const Vector3& size);

    /**
     * @brief See if two normal vectors are nearly parallel, meaning the angle
     *        between them is close to 0 degrees.
     * @param normal1 First normalized vector.
     * @param normal1 Second normalized vector.
     * @param parallelCosineThreshold normals are parallel if absolute value
     *        of dot product (cosine of angle between them) is greater than or
     *        equal to this. For example: cos(1.0 degrees).
     * @return true if vectors are nearly parallel, false otherwise.
     */
    static bool
    parallel(const Vector3& normal1,
             const Vector3& normal2,
             float parallelCosineThreshold = Math::THRESH_NORMALS_ARE_PARALLEL);

    /**
     * @brief See if two normal vectors are coincident (nearly parallel and
     *        point in the same direction).
     * @param normal1 First normalized vector.
     * @param normal2 Second normalized vector.
     * @param parallelCosineThreshold Normals are coincident if dot product
     *        (cosine of angle between them) is greater than or equal to this.
     *        For example: cos(1.0 degrees).
     * @return true if vectors are coincident (nearly parallel and point in the
     *        same direction), false otherwise.
     */
    static bool
    coincident(const Vector3& normal1,
               const Vector3& normal2,
               float parallelCosineThreshold = Math::THRESH_NORMALS_ARE_PARALLEL);

    /**
     * @brief See if two normal vectors are nearly orthogonal (perpendicular),
     *        meaning the angle between them is close to 90 degrees.
     * @param normal1 First normalized vector.
     * @param normal2 Second normalized vector.
     * @param orthogonalCosineThreshold Normals are orthogonal if absolute
     *        value of dot product (cosine of angle between them) is less than
     *        or equal to this. For example: cos(89.0 degrees).
     * @return true if vectors are orthogonal (perpendicular), false otherwise.
     */
    static bool
    orthogonal(const Vector3& normal1,
               const Vector3& normal2,
               float orthogonalCosineThreshold = Math::THRESH_NORMALS_ARE_ORTHOGONAL);

    /**
     * @brief See if two planes are coplanar. They are coplanar if the normals
     *        are nearly parallel and the planes include the same set of points.
     * @param base1 The base point in the first plane.
     * @param normal1 The normal of the first plane.
     * @param base2 The base point in the second plane.
     * @param normal2 The normal of the second plane.
     * @param parallelCosineThreshold Normals are parallel if absolute value of
     *        dot product is greater than or equal to this.
     * @return true if the planes are coplanar, false otherwise.
     */
    static bool
    coplanar(const Vector3& base1,
             const Vector3& normal1,
             const Vector3& base2,
             const Vector3& normal2,
             float parallelCosineThreshold = Math::THRESH_NORMALS_ARE_PARALLEL);

    /**
     * @brief Triple product of three vectors: X dot (Y cross Z).
     * @param inX The first vector.
     * @param inY The second vector.
     * @param inZ The third vector.
     * @return The triple product: x dot (y cross z).
     */
    static float
    triple(const Vector3& inX, const Vector3& inY, const Vector3& inZ);

    /**
     * @brief Generates a list of sample points on a Bezier curve defined by 2 points.
     * @param controlPoints	Array of 4 Vector3s (vert1, controlpoint1, controlpoint2, vert2).
     * @param numPoints Number of samples.
     * @param outPoints Receives the output samples.
     * @return The path length.
     */
    static float
    evaluateBezier(const Vector3* controlPoints, int32 numPoints, Vector<Vector3>& OutPoints);

    /**
     * @brief Converts a vector containing radian values to a vector containing degree values.
     * @param radVector	Vector containing radian values
     * @return Vector containing degree values
     */
    static Vector3
    radiansToDegrees(const Vector3& radVector);

    /**
     * @brief Converts a vector containing degree values to a vector containing radian values.
     * @param degVector Vector containing degree values
     * @return Vector containing radian values
     */
    static Vector3
    degreesToRadians(const Vector3& DegVector);

    /**
     * @brief Given a current set of cluster centers, a set of points, iterate
     *        N times to move clusters to be central.
     * @param clusters Reference to array of Clusters.
     * @param points Set of points.
     * @param numIterations Number of iterations.
     * @param numConnectionsToBeValid Sometimes you will have long strings that
     *        come off the mass of points which happen to have been chosen as
     *        Cluster starting points.  You want to be able to disregard those.
     */
    static void
    generateClusterCenters(Vector<Vector3>& Clusters,
                           const Vector<Vector3>& Points,
                           int32 NumIterations,
                           int32 NumConnectionsToBeValid);

   public:
    /**
     * @brief Vector's X component.
     */
    float x;

    /**
     * @brief Vector's Y component.
     */
    float y;

    /**
     * @brief Vector's Z component.
     */
    float z;

   public:
    /**
     * @brief A zero vector (0,0,0)
     */
    static GE_UTILITIES_EXPORT const Vector3 ZERO;

    /**
     * @brief A unit vector (1,1,1)
     */
    static GE_UTILITIES_EXPORT const Vector3 UNIT;

    static GE_UTILITIES_EXPORT const Vector3 UNIT_X;
    static GE_UTILITIES_EXPORT const Vector3 UNIT_Y;
    static GE_UTILITIES_EXPORT const Vector3 UNIT_Z;

    /**
     * @brief World up vector (0,0,1)
     */
    static GE_UTILITIES_EXPORT const Vector3 UP;

    /**
     * @brief World down vector (0,0,-1)
     */
    static GE_UTILITIES_EXPORT const Vector3 DOWN;

    /**
     * @brief Forward vector (1,0,0)
     */
    static GE_UTILITIES_EXPORT const Vector3 FORWARD;
    
    /**
     * @brief Forward vector (-1,0,0)
     */
    static GE_UTILITIES_EXPORT const Vector3 BACKWARD;

    /**
    * @brief Right vector (0,1,0)
    */
    static GE_UTILITIES_EXPORT const Vector3 RIGHT;

    /**
    * @brief Right vector (0,-1,0)
    */
    static GE_UTILITIES_EXPORT const Vector3 LEFT;

    GE_REFLECTED_CLASS()
  };

  /***************************************************************************/
  /**
   * Vector3 inline functions
   */
  /***************************************************************************/

  /**
   * @brief Multiplies a vector by a scaling factor.
   * @param scale Scaling factor.
   * @param v Vector to scale.
   * @return Result of multiplication.
   */
  FORCEINLINE Vector3
  operator*(float scale, const Vector3& v) {
    return v.operator*(scale);
  }

  /**
   * @brief Utility to calculate distance from a point to a bounding box
   * @param mins 3D Point defining the lower values of the axis of the bound box
   * @param maxs 3D Point defining the lower values of the axis of the bound box
   * @param point 3D position of interest
   * @return the distance from the point to the bounding box.
   */
  FORCEINLINE float
  computeSquaredDistanceFromBoxToPoint(const Vector3& mins,
                                       const Vector3& maxs,
                                       const Vector3& point) {
    //Accumulates the distance as we iterate axis
    float DistSquared = 0.f;

    //Check each axis for min/max and add the distance accordingly
    //NOTE: Loop manually unrolled for > 2x speed up
    if (point.x < mins.x) {
      DistSquared += Math::square(point.x - mins.x);
    }
    else if (point.x > maxs.x) {
      DistSquared += Math::square(point.x - maxs.x);
    }

    if (point.y < mins.y) {
      DistSquared += Math::square(point.y - mins.y);
    }
    else if (point.y > maxs.y) {
      DistSquared += Math::square(point.y - maxs.y);
    }

    if (point.z < mins.z) {
      DistSquared += Math::square(point.z - mins.z);
    }
    else if (point.z > maxs.z) {
      DistSquared += Math::square(point.z - maxs.z);
    }

    return DistSquared;
  }

  inline Vector3
  Vector3::rotateAngleAxis(const float angleDeg, const Vector3& axis) const {
    float S, C;
    Math::sin_cos(&S, &C, angleDeg * Math::DEG2RAD);

    const float XX = axis.x * axis.x;
    const float YY = axis.y * axis.y;
    const float ZZ = axis.z * axis.z;

    const float XY = axis.x * axis.y;
    const float YZ = axis.y * axis.z;
    const float ZX = axis.z * axis.x;

    const float XS = axis.x * S;
    const float YS = axis.y * S;
    const float ZS = axis.z * S;

    const float OMC = 1.f - C;

    return Vector3(
      (OMC * XX + C ) * x + (OMC * XY - ZS) * y + (OMC * ZX + YS) * z,
      (OMC * XY + ZS) * x + (OMC * YY + C ) * y + (OMC * YZ - XS) * z,
      (OMC * ZX - YS) * x + (OMC * YZ + XS) * y + (OMC * ZZ + C ) * z
    );
  }

  inline bool
  Vector3::pointsAreSame(const Vector3& p, const Vector3& q) {
    float Temp;
    Temp = p.x - q.x;
    if ((-Math::THRESH_POINTS_ARE_SAME < Temp) && (Math::THRESH_POINTS_ARE_SAME > Temp)) {
      Temp = p.y - q.y;
      if ((-Math::THRESH_POINTS_ARE_SAME < Temp) && (Math::THRESH_POINTS_ARE_SAME > Temp)) {
        Temp = p.z - q.z;
        if ((-Math::THRESH_POINTS_ARE_SAME < Temp) && (Math::THRESH_POINTS_ARE_SAME > Temp)) {
          return true;
        }
      }
    }
    return false;
  }

  inline bool
  Vector3::pointsAreNear(const Vector3& point1, const Vector3& point2, float dist) {
    float Temp;
    Temp = (point1.x - point2.x);
    if (Math::abs(Temp) >= dist) {
      return false;
    }

    Temp = (point1.y - point2.y);
    if (Math::abs(Temp) >= dist) {
      return false;
    }
    
    Temp = (point1.z - point2.z);
    if (Math::abs(Temp) >= dist) {
      return false;
    }

    return true;
  }

  inline float
  Vector3::pointPlaneDist (
    const Vector3 &Point,
    const Vector3 &PlaneBase,
    const Vector3 &PlaneNormal) {
    return (Point - PlaneBase) | PlaneNormal;
  }

  inline Vector3
  Vector3::pointPlaneProject(const Vector3& point,
                             const Vector3& planeBase,
                             const Vector3& planeNormal) {
    //Find the distance of X from the plane
    //Add the distance back along the normal from the point
    return point - Vector3::pointPlaneDist(point, planeBase, planeNormal) * planeNormal;
  }

  inline Vector3
  Vector3::vectorPlaneProject(const Vector3& v, const Vector3& planeNormal) {
    return v - v.projectOnToNormal(planeNormal);
  }

  inline bool
  Vector3::parallel(const Vector3& normal1,
                    const Vector3& normal2,
                    float parallelCosineThreshold) {
    const float NormalDot = normal1 | normal2;
    return Math::abs(NormalDot) >= parallelCosineThreshold;
  }

  inline bool
  Vector3::coincident(const Vector3& normal1,
                      const Vector3& normal2,
                      float parallelCosineThreshold) {
    const float NormalDot = normal1 | normal2;
    return NormalDot >= parallelCosineThreshold;
  }

  inline bool
  Vector3::orthogonal(const Vector3& normal1,
                      const Vector3& normal2,
                      float orthogonalCosineThreshold) {
    const float NormalDot = normal1 | normal2;
    return Math::abs(NormalDot) <= orthogonalCosineThreshold;
  }

  inline bool
  Vector3::coplanar(const Vector3& base1,
                    const Vector3& normal1,
                    const Vector3& base2,
                    const Vector3& normal2,
                    float parallelCosineThreshold) {
    if (!Vector3::parallel(normal1, normal2, parallelCosineThreshold)) {
      return false;
    }
    else if (Vector3::pointPlaneDist(base2, base1, normal1) > Math::THRESH_POINT_ON_PLANE) {
      return false;
    }

    return true;
  }

  inline float
  Vector3::triple(const Vector3& inX, const Vector3& inY, const Vector3& inZ) {
    return ((inX.x * (inY.y * inZ.z - inY.z * inZ.y)) + 
            (inX.y * (inY.z * inZ.x - inY.x * inZ.z)) + 
            (inX.z * (inY.x * inZ.y - inY.y * inZ.x)));
  }

  inline Vector3
  Vector3::radiansToDegrees(const Vector3& radVector) {
    return radVector * Math::RAD2DEG;
  }

  inline Vector3 Vector3::degreesToRadians(const Vector3& degVector) {
    return degVector * Math::DEG2RAD;
  }

  FORCEINLINE Vector3::Vector3(float InF) : x(InF), y(InF), z(InF) {}

  FORCEINLINE Vector3::Vector3(float InX, float InY, float InZ) : x(InX), y(InY), z(InZ) {}

  FORCEINLINE Vector3::Vector3(FORCE_INIT::E) : x(0.0f), y(0.0f), z(0.0f) {}

  FORCEINLINE Vector3
  Vector3::operator^(const Vector3& v) const {
    return Vector3(y * v.z - z * v.y,
                   z * v.x - x * v.z,
                   x * v.y - y * v.x);
  }

  FORCEINLINE Vector3
  Vector3::crossProduct(const Vector3& a, const Vector3& b) {
    return a ^ b;
  }

  FORCEINLINE float
  Vector3::operator|(const Vector3& v) const {
    return dot(*this, v);
  }

  FORCEINLINE float
  Vector3::dot(const Vector3& a, const Vector3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }

  FORCEINLINE Vector3
  Vector3::operator+(const Vector3& v) const {
    return Vector3(x + v.x, y + v.y, z + v.z);
  }

  FORCEINLINE Vector3
  Vector3::operator-(const Vector3& v) const {
    return Vector3(x - v.x, y - v.y, z - v.z);
  }

  FORCEINLINE Vector3
  Vector3::operator-(float bias) const {
    return Vector3(x - bias, y - bias, z - bias);
  }

  FORCEINLINE Vector3
  Vector3::operator+(float bias) const {
    return Vector3(x + bias, y + bias, z + bias);
  }

  FORCEINLINE Vector3
  Vector3::operator*(float scale) const {
    return Vector3(x * scale, y * scale, z * scale);
  }

  FORCEINLINE Vector3
  Vector3::operator/(float scale) const {
    const float RScale = 1.f / scale;
    return Vector3(x * RScale, y * RScale, z * RScale);
  }

  FORCEINLINE Vector3
  Vector3::operator*(const Vector3& v) const {
    return Vector3(x * v.x, y * v.y, z * v.z);
  }

  FORCEINLINE Vector3
  Vector3::operator/(const Vector3& v) const {
    return Vector3(x / v.x, y / v.y, z / v.z);
  }

  FORCEINLINE bool
  Vector3::operator==(const Vector3& v) const {
    return x == v.x && y == v.y && z == v.z;
  }

  FORCEINLINE bool
  Vector3::operator!=(const Vector3& v) const {
    return x != v.x || y != v.y || z != v.z;
  }

  FORCEINLINE bool
  Vector3::equals(const Vector3& v, float tolerance) const {
    return  Math::abs(x - v.x) < tolerance &&
            Math::abs(y - v.y) < tolerance &&
            Math::abs(z - v.z) < tolerance;
  }

  FORCEINLINE bool
  Vector3::allComponentsEqual(float tolerance) const {
    return  Math::abs(x - y) < tolerance &&
            Math::abs(x - z) < tolerance &&
            Math::abs(y - z) < tolerance;
  }

  FORCEINLINE Vector3
  Vector3::operator-() const {
    return Vector3(-x, -y, -z);
  }

  FORCEINLINE Vector3&
  Vector3::operator+=(const Vector3& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  FORCEINLINE Vector3&
  Vector3::operator-=(const Vector3& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  FORCEINLINE Vector3&
  Vector3::operator*=(float scale) {
    x *= scale;
    y *= scale;
    z *= scale;
    return *this;
  }

  FORCEINLINE Vector3&
  Vector3::operator/=(float v) {
    const float RV = 1.f / v;
    x *= RV; y *= RV; z *= RV;
    return *this;
  }

  FORCEINLINE Vector3&
  Vector3::operator*=(const Vector3& v) {
    x *= v.x; y *= v.y; z *= v.z;
    return *this;
  }

  FORCEINLINE Vector3&
  Vector3::operator/=(const Vector3& v) {
    x /= v.x; y /= v.y; z /= v.z;
    return *this;
  }

  FORCEINLINE float&
  Vector3::operator[](uint32 index) {
    GE_ASSERT(index < 3);
    return (&x)[index];
  }

  FORCEINLINE float
  Vector3::operator[](uint32 index)const {
    GE_ASSERT(index < 3);
    return (&x)[index];
  }

  FORCEINLINE void
  Vector3::set(float inX, float inY, float inZ) {
    x = inX;
    y = inY;
    z = inZ;
  }

  FORCEINLINE float
  Vector3::getMax() const {
    return Math::max3(x, y, z);
  }

  FORCEINLINE float
  Vector3::getAbsMax() const {
    return Math::max3(Math::abs(x), Math::abs(y), Math::abs(z));
  }

  FORCEINLINE float
  Vector3::getMin() const {
    return Math::min3(x, y, z);
  }

  FORCEINLINE float
  Vector3::getAbsMin() const {
    return Math::min3(Math::abs(x), Math::abs(y), Math::abs(z));
  }

  FORCEINLINE Vector3
  Vector3::componentMin(const Vector3& other) const {
    return Vector3(Math::min(x, other.x), Math::min(y, other.y), Math::min(z, other.z));
  }

  FORCEINLINE Vector3
  Vector3::componentMax(const Vector3& other) const {
    return Vector3(Math::max(x, other.x), Math::max(y, other.y), Math::max(z, other.z));
  }

  FORCEINLINE Vector3
  Vector3::getAbs() const {
    return Vector3(Math::abs(x), Math::abs(y), Math::abs(z));
  }

  FORCEINLINE float
  Vector3::size() const {
    return Math::sqrt(x*x + y*y + z*z);
  }

  FORCEINLINE float
  Vector3::sizeSquared() const {
    return x*x + y*y + z*z;
  }

  FORCEINLINE float
  Vector3::size2D() const {
    return Math::sqrt(x*x + y*y);
  }

  FORCEINLINE float
  Vector3::sizeSquared2D() const {
    return x*x + y*y;
  }

  FORCEINLINE bool
  Vector3::isNearlyZero(float tolerance) const {
    return  Math::abs(x) < tolerance &&
            Math::abs(y) < tolerance &&
            Math::abs(z) < tolerance;
  }

  FORCEINLINE bool
  Vector3::isZero() const {
    return  x == 0.f &&
            y == 0.f &&
            z == 0.f;
  }

  FORCEINLINE bool
  Vector3::normalize(float tolerance) {
    const float SquareSum = x*x + y*y + z*z;
    if (SquareSum > tolerance) {
      const float Scale = Math::invSqrt(SquareSum);
      x *= Scale;
      y *= Scale;
      z *= Scale;
      return true;
    }
    return false;
  }

  FORCEINLINE bool
  Vector3::isNormalized() const {
    return (Math::abs(1.f - sizeSquared()) < Math::THRESH_VECTOR_NORMALIZED);
  }

  FORCEINLINE void
  Vector3::toDirectionAndLength(Vector3 &outDir, float &outLength) const {
    outLength = size();
    if (Math::SMALL_NUMBER < outLength) {
      float OneOverLength = 1.0f / outLength;
      outDir = Vector3(x*OneOverLength, y*OneOverLength, z*OneOverLength);
    }
    else {
      outDir = Vector3::ZERO;
    }
  }

  FORCEINLINE Vector3
  Vector3::getSignVector() const {
    return Vector3(Math::floatSelect(x, 1.f, -1.f),
                   Math::floatSelect(y, 1.f, -1.f),
                   Math::floatSelect(z, 1.f, -1.f));
  }

  FORCEINLINE Vector3
  Vector3::projection() const {
    const float RZ = 1.f / z;
    return Vector3(x*RZ, y*RZ, 1);
  }

  FORCEINLINE Vector3
  Vector3::getUnsafeNormal() const {
    const float Scale = Math::invSqrt(x*x + y*y + z*z);
    return Vector3(x*Scale, y*Scale, z*Scale);
  }

  FORCEINLINE Vector3
  Vector3::gridSnap(const float& gridSz) const {
    return Vector3(Math::gridSnap(x, gridSz),
                   Math::gridSnap(y, gridSz),
                   Math::gridSnap(z, gridSz));
  }

  FORCEINLINE Vector3
  Vector3::boundToCube(float radius) const {
    return Vector3(Math::clamp(x, -radius, radius),
                   Math::clamp(y, -radius, radius),
                   Math::clamp(z, -radius, radius));
  }

  FORCEINLINE Vector3
  Vector3::getClampedToSize(float Min, float Max) const {
    float VecSize = size();
    const Vector3 VecDir = (Math::SMALL_NUMBER < VecSize) ? (*this / VecSize) : Vector3::ZERO;
    VecSize = Math::clamp(VecSize, Min, Max);
    return VecSize * VecDir;
  }

  FORCEINLINE Vector3
  Vector3::getClampedToSize2D(float Min, float Max) const {
    float VecSize2D = size2D();
    const Vector3 VecDir = (Math::SMALL_NUMBER < VecSize2D) ? 
      (*this / VecSize2D) : Vector3::ZERO;
    VecSize2D = Math::clamp(VecSize2D, Min, Max);
    return Vector3(VecSize2D * VecDir.x, VecSize2D * VecDir.y, z);
  }

  FORCEINLINE Vector3
  Vector3::getClampedToMaxSize(float maxSize) const {
    if (Math::KINDA_SMALL_NUMBER > maxSize) {
      return Vector3::ZERO;
    }

    const float VSq = sizeSquared();
    if (VSq > Math::square(maxSize)) {
      const float Scale = maxSize * Math::invSqrt(VSq);
      return Vector3(x*Scale, y*Scale, z*Scale);
    }
    else
    {
      return *this;
    }
  }

  FORCEINLINE Vector3
  Vector3::getClampedToMaxSize2D(float maxSize) const {
    if (Math::KINDA_SMALL_NUMBER > maxSize) {
      return Vector3(0.f, 0.f, z);
    }

    const float VSq2D = sizeSquared2D();
    if (VSq2D > Math::square(maxSize)) {
      const float Scale = maxSize * Math::invSqrt(VSq2D);
      return Vector3(x*Scale, y*Scale, z);
    }
    else {
      return *this;
    }
  }

  FORCEINLINE void
  Vector3::addBounded(const Vector3& v, float radius) {
    *this = (*this + v).boundToCube(radius);
  }

  FORCEINLINE float&
  Vector3::component(uint32 index) {
    return (&x)[index];
  }

  FORCEINLINE float
  Vector3::component(uint32 index) const {
    return (&x)[index];
  }

  FORCEINLINE Vector3
  Vector3::reciprocal() const {
    Vector3 RecVector;
    if (x != 0.f) {
      RecVector.x = 1.f / x;
    }
    else {
      RecVector.x = Math::BIG_NUMBER;
    }
    if (y != 0.f) {
      RecVector.y = 1.f / y;
    }
    else {
      RecVector.y = Math::BIG_NUMBER;
    }
    if (z != 0.f) {
      RecVector.z = 1.f / z;
    }
    else {
      RecVector.z = Math::BIG_NUMBER;
    }

    return RecVector;
  }

  FORCEINLINE bool
  Vector3::isUniform(float tolerance) const {
    return (Math::abs(x - y) < tolerance) && (Math::abs(y - z) < tolerance);
  }

  FORCEINLINE Vector3
  Vector3::mirrorByVector(const Vector3& mirrorNormal) const {
    return *this - mirrorNormal * (2.f * (*this | mirrorNormal));
  }

  FORCEINLINE Vector3
  Vector3::getSafeNormal(float tolerance) const {
    const float SquareSum = x*x + y*y + z*z;

    //Not sure if it's safe to add tolerance in there. Might introduce too many errors
    if (1.f == SquareSum) {
      return *this;
    }
    else if (SquareSum < tolerance) {
      return Vector3::ZERO;
    }
    const float Scale = Math::invSqrt(SquareSum);
    return Vector3(x*Scale, y*Scale, z*Scale);
  }

  FORCEINLINE Vector3
  Vector3::getSafeNormal2D(float tolerance) const {
    const float SquareSum = x*x + y*y;

    //Not sure if it's safe to add tolerance in there. Might introduce too many errors
    if (1.f == SquareSum) {
      if (0.f == z) {
        return *this;
      }
      else {
        return Vector3(x, y, 0.f);
      }
    }
    else if (SquareSum < tolerance) {
      return Vector3::ZERO;
    }

    const float Scale = Math::invSqrt(SquareSum);
    return Vector3(x*Scale, y*Scale, 0.f);
  }

  FORCEINLINE float
  Vector3::cosineAngle2D(Vector3 b) const {
    Vector3 a(*this);
    a.z = 0.0f;
    b.z = 0.0f;
    a.normalize();
    b.normalize();
    return a | b;
  }

  FORCEINLINE Vector3
  Vector3::projectOnTo(const Vector3& a) const {
    return (a * ((*this | a) / (a | a)));
  }

  FORCEINLINE Vector3 Vector3::projectOnToNormal(const Vector3& normal) const {
    return (normal * (*this | normal));
  }

  FORCEINLINE bool
  Vector3::containsNaN() const {
    return (Math::isNaN(x) || !Math::isFinite(x) ||
            Math::isNaN(y) || !Math::isFinite(y) ||
            Math::isNaN(z) || !Math::isFinite(z));
  }

  FORCEINLINE bool
  Vector3::isUnit(float lengthSquaredTolerance) const {
    return Math::abs(1.0f - sizeSquared()) < lengthSquaredTolerance;
  }

  FORCEINLINE float
  Vector3::headingAngle() const {
    //Project Dir into Z plane.
    Vector3 PlaneDir = *this;
    PlaneDir.z = 0.f;
    PlaneDir = PlaneDir.getSafeNormal();

    Radian angle = Math::acos(PlaneDir.x);

    if (0.0f > PlaneDir.y) {
      angle *= -1.0f;
    }

    return angle.valueRadians();
  }

  FORCEINLINE float
  Vector3::distance(const Vector3& v1, const Vector3& v2) {
    return Math::sqrt(Math::square(v2.x - v1.x) + 
                      Math::square(v2.y - v1.y) + 
                      Math::square(v2.z - v1.z));
  }

  FORCEINLINE float
  Vector3::distSquared(const Vector3& v1, const Vector3& v2) {
    return  Math::square(v2.x - v1.x) +
            Math::square(v2.y - v1.y) +
            Math::square(v2.z - v1.z);
  }

  FORCEINLINE float
  Vector3::boxPushOut(const Vector3& normal, const Vector3& size) {
    return  Math::abs(normal.x*size.x) +
            Math::abs(normal.y*size.y) +
            Math::abs(normal.z*size.z);
  }

  FORCEINLINE Vector3
  clampVector(const Vector3& v, const Vector3& Min, const Vector3& Max) {
    return Vector3(Math::clamp(v.x, Min.x, Max.x),
                   Math::clamp(v.y, Min.y, Max.y),
                   Math::clamp(v.z, Min.z, Max.z));
  }
}
