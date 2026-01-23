/*****************************************************************************/
/**
 * @file    geMath.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/04
 * @brief   Utility class providing common scalar math operations.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geMath.h"
#include "geColor.h"
#include "geVector2.h"
#include "geVector3.h"

#if USING(GE_PLATFORM_WINDOWS)
# include <intrin.h>
#endif

namespace geEngineSDK {
  /*!< PI constant */
  const float Math::PI = static_cast<float>(4.0f * std::atan(1.0f));

  /*!< Inverse of PI */
  const float Math::INV_PI = static_cast<float>(1.0f / PI);
  
  /*!< Half PI  */
  const float Math::HALF_PI = static_cast<float>(0.5f * PI);
  
  /*!< 2 Times PI */
  const float Math::TWO_PI = static_cast<float>(2.0f * PI);	

  /*!< Euler number or Napier constant (1+1/N)^N */
  const float Math::EULERS_NUMBER = (2.71828182845904523536f);

  const float Math::SMALL_NUMBER        = (1.e-6f);
  const float Math::KINDA_SMALL_NUMBER  = (1.e-4f);
  const float Math::BIG_NUMBER          = (3.4e+38f);

  const float Math::DEG2RAD = static_cast<float>(PI / 180.0f);
  const float Math::RAD2DEG = static_cast<float>(180.0f / PI);
  const float Math::LOG2 = static_cast<float>(std::log(2.0f));

  const float  Math::DELTA = (0.00001f);

  const float Math::FLOAT_NORMAL_THRESH = (0.0001f);

  const float Math::THRESH_POINT_ON_PLANE   = (0.10f);
  const float Math::THRESH_POINT_ON_SIDE    = (0.20f);
  const float Math::THRESH_POINTS_ARE_SAME  = (0.00002f);
  const float Math::THRESH_POINTS_ARE_NEAR  = (0.015f);
  const float Math::THRESH_NORMALS_ARE_SAME = (0.00002f);
  const float Math::THRESH_UVS_ARE_SAME     = (0.0009765625f);

  const float Math::THRESH_VECTORS_ARE_NEAR = (0.0004f);

  const float Math::THRESH_SPLIT_POLY_WITH_PLANE  = (0.25f);
  const float Math::THRESH_SPLIT_POLY_PRECISELY   = (0.01f);
  const float Math::THRESH_ZERO_NORM_SQUARED      = (0.0001f);
  const float Math::THRESH_NORMALS_ARE_PARALLEL   = (0.999845f);
  const float Math::THRESH_NORMALS_ARE_ORTHOGONAL = (0.017455f);

  const float Math::THRESH_VECTOR_NORMALIZED = (0.01f);
  const float Math::THRESH_QUAT_NORMALIZED   = (0.01f);

  const float Math::ZERO_ANIMWEIGHT_THRESH = (0.00001f);

  Radian
  Math::acos(float Value) {
    if (-1.0f < Value) {
      if (1.0f > Value) {
        return Radian(std::acos(Value));
      }
      else {
        return Radian(0.0f);
      }
    }
    return Radian(PI);
  }

  Radian
  Math::asin(float Value) {
    if (-1.0f < Value) {
      if (1.0f > Value) {
        return Radian(std::asin(Value));
      }
      else {
        return Radian(HALF_PI);
      }
    }
    return Radian(-HALF_PI);
  }

#if USING(GE_PLATFORM_WINDOWS)
  uint32
  Math::floorLog2(uint32 Value) {
    //Use BSR to return the log2 of the integer
    unsigned long Log2;
    if (_BitScanReverse(&Log2, Value) != 0) {
      return static_cast<uint32>(Log2);
    }

    return 0;
  }
#else
  uint32
  Math::floorLog2(uint32 Value) {
    uint32 pos = 0;
    if (Value >= 1 << 16) { Value >>= 16; pos += 16; }
    if (Value >= 1 << 8) { Value >>= 8; pos += 8; }
    if (Value >= 1 << 4) { Value >>= 4; pos += 4; }
    if (Value >= 1 << 2) { Value >>= 2; pos += 2; }
    if (Value >= 1 << 1) { pos += 1; }
    return (Value == 0) ? 0 : pos;
  }
#endif

#if USING(GE_PLATFORM_WINDOWS)
  uint32
  Math::countLeadingZeros(uint32 Value) {
    //Use BSR to return the log2 of the integer
    unsigned long Log2;
    if (_BitScanReverse(&Log2, Value) != 0) {
      return static_cast<uint32>(31 - Log2);
    }
    return 32;
  }
#else
  uint32
  Math::countLeadingZeros(uint32 Value) {
    if (0 == Value) {
      return 32;
    }
    return __builtin_clz(Value);
  }
#endif

#if USING(GE_PLATFORM_WINDOWS)
  uint32
  Math::countTrailingZeros(uint32 Value) {
    if (0 == Value ) {
      return 32;
    }

    unsigned long BitIndex; //0-based, where the LSB is 0 and MSB is 31
    _BitScanForward(&BitIndex, Value);  //Scans from LSB to MSB
    return BitIndex;
  }
#else
  uint32
  Math::countTrailingZeros(uint32 Value) {
    if (0 == Value) {
      return 32;
    }
    return __builtin_ctz(Value);
  }
#endif

  float
  Math::fastSin0(float Value) {
    float angleSqr = square(Value);
    float result = 7.61e-03f;
    result *= angleSqr;
    result -= 1.6605e-01f;
    result *= angleSqr;
    result += 1.0f;
    result *= Value;
    return result;
  }

  float
  Math::fastSin1(float Value) {
    float angleSqr = square(Value);
    float result = -2.39e-08f;
    result *= angleSqr;
    result += 2.7526e-06f;
    result *= angleSqr;
    result -= 1.98409e-04f;
    result *= angleSqr;
    result += 8.3333315e-03f;
    result *= angleSqr;
    result -= 1.666666664e-01f;
    result *= angleSqr;
    result += 1.0f;
    result *= Value;
    return result;
  }

  float
  Math::fastCos0(float Value) {
    float angleSqr = square(Value);
    float result = 3.705e-02f;
    result *= angleSqr;
    result -= 4.967e-01f;
    result *= angleSqr;
    result += 1.0f;
    return result;
  }

  float
  Math::fastCos1(float Value) {
    float angleSqr = square(Value);
    float result = -2.605e-07f;
    result *= angleSqr;
    result += 2.47609e-05f;
    result *= angleSqr;
    result -= 1.3888397e-03f;
    result *= angleSqr;
    result += 4.16666418e-02f;
    result *= angleSqr;
    result -= 4.999999963e-01f;
    result *= angleSqr;
    result += 1.0f;
    return result;
  }

  float
  Math::fastTan0(float Value) {
    float angleSqr = square(Value);
    float result = 2.033e-01f;
    result *= angleSqr;
    result += 3.1755e-01f;
    result *= angleSqr;
    result += 1.0f;
    result *= Value;
    return result;
  }

  float
  Math::fastTan1(float Value) {
    float angleSqr = square(Value);
    float result = 9.5168091e-03f;
    result *= angleSqr;
    result += 2.900525e-03f;
    result *= angleSqr;
    result += 2.45650893e-02f;
    result *= angleSqr;
    result += 5.33740603e-02f;
    result *= angleSqr;
    result += 1.333923995e-01f;
    result *= angleSqr;
    result += 3.333314036e-01f;
    result *= angleSqr;
    result += 1.0f;
    result *= Value;
    return result;
  }

  float Math::fastASin0(float Value) {
    float root = sqrt(abs(1.0f - Value));
    float result = -0.0187293f;
    result *= Value;
    result += 0.0742610f;
    result *= Value;
    result -= 0.2121144f;
    result *= Value;
    result += 1.5707288f;
    result = HALF_PI - root*result;
    return result;
  }

  float
  Math::fastASin1(float Value) {
    float root = sqrt(abs(1.0f - Value));
    float result = -0.0012624911f;
    result *= Value;
    result += 0.0066700901f;
    result *= Value;
    result -= 0.0170881256f;
    result *= Value;
    result += 0.0308918810f;
    result *= Value;
    result -= 0.0501743046f;
    result *= Value;
    result += 0.0889789874f;
    result *= Value;
    result -= 0.2145988016f;
    result *= Value;
    result += 1.5707963050f;
    result = HALF_PI - root*result;
    return result;
  }

  float
  Math::fastACos0(float Value) {
    float root = sqrt(abs(1.0f - Value));
    float result = -0.0187293f;
    result *= Value;
    result += 0.0742610f;
    result *= Value;
    result -= 0.2121144f;
    result *= Value;
    result += 1.5707288f;
    result *= root;
    return result;
  }

  float
  Math::fastACos1(float Value) {
    float root = sqrt(abs(1.0f - Value));
    float result = -0.0012624911f;
    result *= Value;
    result += 0.0066700901f;
    result *= Value;
    result -= 0.0170881256f;
    result *= Value;
    result += 0.0308918810f;
    result *= Value;
    result -= 0.0501743046f;
    result *= Value;
    result += 0.0889789874f;
    result *= Value;
    result -= 0.2145988016f;
    result *= Value;
    result += 1.5707963050f;
    result *= root;
    return result;
  }

  float
  Math::fastATan0(float Value) {
    float valueSqr = square(Value);
    float result = 0.0208351f;
    result *= valueSqr;
    result -= 0.085133f;
    result *= valueSqr;
    result += 0.180141f;
    result *= valueSqr;
    result -= 0.3302995f;
    result *= valueSqr;
    result += 0.999866f;
    result *= Value;
    return result;
  }

  float
  Math::fastATan1(float Value) {
    float valueSqr = square(Value);
    float result = 0.0028662257f;
    result *= valueSqr;
    result -= 0.0161657367f;
    result *= valueSqr;
    result += 0.0429096138f;
    result *= valueSqr;
    result -= 0.0752896400f;
    result *= valueSqr;
    result += 0.1065626393f;
    result *= valueSqr;
    result -= 0.1420889944f;
    result *= valueSqr;
    result += 0.1999355085f;
    result *= valueSqr;
    result -= 0.3333314528f;
    result *= valueSqr;
    result += 1.0f;
    result *= Value;
    return result;
  }

  /***************************************************************************/
  /**
   * Math mixed functions
   */
  /***************************************************************************/

  bool
  Math::lineLineIntersection(const Vector2& aa,
                             const Vector2& ab,
                             const Vector2& ba,
                             const Vector2& bb) {
    Vector2 r = ab - aa;
    Vector2 s = bb - ba;
    Vector2 diff = ba - aa;
    float rCs = 1.0f / (r ^ s);

    float t = diff^s * rCs;
    float u = diff^r * rCs;

    return (0.f <= t && 1.f >= t) && (0.f <= u && 1.f >= u);
  }

  Vector2
  Math::getLineLineIntersect(const Vector2& aa,
                             const Vector2& ab,
                             const Vector2& ba,
                             const Vector2& bb) {
    float aaCab = aa ^ ab;
    float baCbb = ba ^ bb;

    float pX = aaCab * (ba.x - bb.x) - baCbb * (aa.x - ab.x);
    float pY = aaCab * (ba.y - bb.y) - baCbb * (aa.y - ab.y);
    float invDenom = 1.0f / ((aa.x - ab.x) * (ba.y - bb.y) - (aa.y - ab.y) * (ba.x - bb.x));

    return Vector2(pX * invDenom, pY * invDenom);
  }

  Vector3
  Math::calculateTriTangent(const Vector3& position1,
                            const Vector3& position2,
                            const Vector3& position3,
                            float u1,
                            float v1,
                            float u2,
                            float v2,
                            float u3,
                            float v3) {
    Vector3 side0 = position1 - position2;
    Vector3 side1 = position3 - position1;

    //Calculate face normal
    Vector3 normal = side1 ^ side0;
    normal.normalize();

    //Now we use a formula to calculate the tangent.
    float deltaV0 = v1 - v2;
    float deltaV1 = v3 - v1;
    Vector3 tangent = deltaV1 * side0 - deltaV0 * side1;
    tangent.normalize();

    //Calculate binormal
    float deltaU0 = u1 - u2;
    float deltaU1 = u3 - u1;
    Vector3 binormal = deltaU1 * side0 - deltaU0 * side1;
    binormal.normalize();

    //Now, we take the cross product of the tangents to get a vector which should point in the same direction as our normal calculated above.
    //If it points in the opposite direction (the dot product between the normals is less than zero), then we need to reverse the s and t tangents.
    //This is because the triangle has been mirrored when going from tangent space to object space.
    Vector3 tangentCross = tangent ^ binormal;

    //Reverse tangents if necessary.
    if (0.f > (tangentCross | normal)) {
      tangent = -tangent;
      binormal = -binormal;
    }

    return tangent;
  }

  /***************************************************************************/
  /**
   * Mixed math objects implementations
   */
  /***************************************************************************/

  /*
  inline Vector3 Math::LinePlaneIntersection(const Vector3& Point1, const Vector3& Point2, const Vector3& PlaneOrigin, const Vector3& PlaneNormal)
  {
  return Point1 + (Point2 - Point1) *	(((PlaneOrigin - Point1) | PlaneNormal) / ((Point2 - Point1) | PlaneNormal));
  }

  inline Vector3 Math::LinePlaneIntersection(const Vector3& Point1, const Vector3& Point2, const Plane& plane)
  {
  return Point1 + (Point2 - Point1) *	((plane.W - (Point1 | plane)) / ((Point2 - Point1) | plane));
  }

  bool Math::PlaneAABBIntersection(const Plane& InP, const AABox& AABB)
  {
  //Find diagonal most closely aligned with normal of plane
  Vector3 Vmin, Vmax;

  //Bypass the slow Vector3[] operator. Not RESTRICT because it won't update Vmin, Vmax
  float* VminPtr = (float*)&Vmin;
  float* VmaxPtr = (float*)&Vmax;

  //Use restrict to get better instruction scheduling and to bypass the slow Vector3[] operator
  const float* RESTRICT AABBMinPtr = (const float*)&AABB.Min;
  const float* RESTRICT AABBMaxPtr = (const float*)&AABB.Max;
  const float* RESTRICT PlanePtr = (const float*)&InP;

  for( int32 Idx = 0; Idx < 3; ++Idx )
  {
  if (PlanePtr[Idx] >= 0.f)
  {
  VminPtr[Idx] = AABBMinPtr[Idx];
  VmaxPtr[Idx] = AABBMaxPtr[Idx];
  }
  else
  {
  VminPtr[Idx] = AABBMaxPtr[Idx];
  VmaxPtr[Idx] = AABBMinPtr[Idx];
  }
  }

  //If either diagonal is right on the plane, or one is on either side we have an intersection
  float dMax = InP.PlaneDot(Vmax);
  float dMin = InP.PlaneDot(Vmin);

  //If Max is below plane, or Min is above we know there is no intersection. Otherwise there must be one
  return (dMax >= 0.f && dMin <= 0.f);
  }

  bool Math::PointBoxIntersection(const Vector3& Point, const AABox& Box)
  {
  return (Point.X >= Box.Min.X && Point.X <= Box.Max.X && Point.Y >= Box.Min.Y && Point.Y <= Box.Max.Y && Point.Z >= Box.Min.Z && Point.Z <= Box.Max.Z);
  }

  inline bool Math::LineBoxIntersection(const AABox& Box, const Vector3& Start, const Vector3& End, const Vector3& Direction)
  {
  return LineBoxIntersection(Box, Start, End, Direction, Direction.Reciprocal());
  }

  inline bool Math::LineBoxIntersection(const AABox& Box, const Vector3& Start, const Vector3& End, const Vector3& Direction, const Vector3& OneOverDirection)
  {
  Vector3 Time;
  bool bStartIsOutside = false;

  if(Start.X < Box.Min.X)
  {
  bStartIsOutside = true;
  if(End.X >= Box.Min.X)
  {
  Time.X = (Box.Min.X - Start.X) * OneOverDirection.X;
  }
  else
  {
  return false;
  }
  }
  else if(Start.X > Box.Max.X)
  {
  bStartIsOutside = true;
  if(End.X <= Box.Max.X)
  {
  Time.X = (Box.Max.X - Start.X) * OneOverDirection.X;
  }
  else
  {
  return false;
  }
  }
  else
  {
  Time.X = 0.0f;
  }

  if(Start.Y < Box.Min.Y)
  {
  bStartIsOutside = true;
  if(End.Y >= Box.Min.Y)
  {
  Time.Y = (Box.Min.Y - Start.Y) * OneOverDirection.Y;
  }
  else
  {
  return false;
  }
  }
  else if(Start.Y > Box.Max.Y)
  {
  bStartIsOutside = true;
  if(End.Y <= Box.Max.Y)
  {
  Time.Y = (Box.Max.Y - Start.Y) * OneOverDirection.Y;
  }
  else
  {
  return false;
  }
  }
  else
  {
  Time.Y = 0.0f;
  }

  if(Start.Z < Box.Min.Z)
  {
  bStartIsOutside = true;
  if(End.Z >= Box.Min.Z)
  {
  Time.Z = (Box.Min.Z - Start.Z) * OneOverDirection.Z;
  }
  else
  {
  return false;
  }
  }
  else if(Start.Z > Box.Max.Z)
  {
  bStartIsOutside = true;
  if(End.Z <= Box.Max.Z)
  {
  Time.Z = (Box.Max.Z - Start.Z) * OneOverDirection.Z;
  }
  else
  {
  return false;
  }
  }
  else
  {
  Time.Z = 0.0f;
  }

  if(bStartIsOutside)
  {
  const float	MaxTime = Max3(Time.X, Time.Y, Time.Z);

  if(MaxTime >= 0.0f && MaxTime <= 1.0f)
  {
  const Vector3 Hit = Start + Direction * MaxTime;
  const float BOX_SIDE_THRESHOLD = 0.1f;

  if(	Hit.X > Box.Min.X - BOX_SIDE_THRESHOLD && Hit.X < Box.Max.X + BOX_SIDE_THRESHOLD &&
  Hit.Y > Box.Min.Y - BOX_SIDE_THRESHOLD && Hit.Y < Box.Max.Y + BOX_SIDE_THRESHOLD &&
  Hit.Z > Box.Min.Z - BOX_SIDE_THRESHOLD && Hit.Z < Box.Max.Z + BOX_SIDE_THRESHOLD)
  {
  return true;
  }
  }

  return false;
  }
  else
  {
  return true;
  }
  }

  inline bool Math::LineSphereIntersection(const Vector3& Start, const Vector3& Dir, float Length, const Vector3& Origin, float Radius)
  {
  const Vector3 EO = Start - Origin;
  const float	  v = (Dir | (Origin - Start));
  const float	  disc = Radius * Radius - ((EO | EO) - v * v);

  if( disc >= 0.0f )
  {
  const float	Time = (v - Sqrt(disc)) / Length;

  if( Time >= 0.0f && Time <= 1.0f )
  {
  return true;
  }
  else
  {
  return false;
  }
  }

  return false;
  }

  inline bool Math::IntersectPlanes2(Vector3& I, Vector3& D, const Plane& P1, const Plane& P2)
  {
  //Compute line direction, perpendicular to both plane normals.
  D = P1 ^ P2;
  const float DD = D.SizeSquared();

  if( DD < Square(0.001f) )
  {
  //Parallel or nearly parallel planes.
  D = I = Vector3::ZERO;

  return false;
  }
  else
  {
  //Compute intersection.
  I = (P1.W*(P2^D) + P2.W*(D^P1)) / DD;
  D.Normalize();

  return true;
  }
  }

  inline bool Math::IntersectPlanes3(Vector3& I, const Plane& P1, const Plane& P2, const Plane& P3)
  {
  //Compute determinant, the triple product P1|(P2^P3)==(P1^P2)|P3.
  const float Det = (P1 ^ P2) | P3;

  if(Square(Det) < Square(0.001f))
  {
  //Degenerate.
  I = Vector3::ZERO;
  return false;
  }
  else
  {
  //Compute the intersection point, guaranteed valid if determinant is nonzero.
  I = (P1.W*(P2^P3) + P2.W*(P3^P1) + P3.W*(P1^P2)) / Det;
  }
  return true;
  }

  FORCEINLINE float Math::GetRangePct(Vector2 const& Range, float Value)
  {
  return (Range.X != Range.Y) ? (Value - Range.X) / (Range.Y - Range.X) : Range.X;
  }

  FORCEINLINE float Math::GetRangeValue(Vector2 const& Range, float Pct)
  {
  return Lerp<float>(Range.X, Range.Y, Pct);
  }
  */
  /************************************************************************************************************************/
  /**
  * @brief	Performs a sphere vs box intersection test using Arvo's algorithm:
  *
  *	for each i in (x, y, z)
  *		if (SphereCenter(i) < BoxMin(i)) d2 += (SphereCenter(i) - BoxMin(i)) ^ 2
  *		else if (SphereCenter(i) > BoxMax(i)) d2 += (SphereCenter(i) - BoxMax(i)) ^ 2
  *
  * @param	Sphere the center of the sphere being tested against the AABB
  * @param	RadiusSquared the size of the sphere being tested
  * @param	AABB the box being tested against
  * @return	Whether the sphere/box intersect or not.
  */
  /************************************************************************************************************************/
  /*
  FORCEINLINE bool Math::SphereAABBIntersection(const Vector3& SphereCenter, const float RadiusSquared, const AABox& AABB)
  {
  //Accumulates the distance as we iterate axis
  float DistSquared = 0.f;

  //Check each axis for min/max and add the distance accordingly
  //NOTE: Loop manually unrolled for > 2x speed up
  if( SphereCenter.X < AABB.Min.X )
  {
  DistSquared += Math::Square(SphereCenter.X - AABB.Min.X);
  }
  else if( SphereCenter.X > AABB.Max.X )
  {
  DistSquared += Math::Square(SphereCenter.X - AABB.Max.X);
  }

  if( SphereCenter.Y < AABB.Min.Y )
  {
  DistSquared += Math::Square(SphereCenter.Y - AABB.Min.Y);
  }
  else if( SphereCenter.Y > AABB.Max.Y )
  {
  DistSquared += Math::Square(SphereCenter.Y - AABB.Max.Y);
  }

  if( SphereCenter.Z < AABB.Min.Z )
  {
  DistSquared += Math::Square(SphereCenter.Z - AABB.Min.Z);
  }
  else if( SphereCenter.Z > AABB.Max.Z )
  {
  DistSquared += Math::Square(SphereCenter.Z - AABB.Max.Z);
  }

  //If the distance is less than or equal to the radius, they intersect
  return DistSquared <= RadiusSquared;
  }
  */
  /************************************************************************************************************************/
  /**
  * @brief	Converts a sphere into a point plus radius squared for the test above
  */
  /************************************************************************************************************************/
  /*
  FORCEINLINE bool Math::SphereAABBIntersection(const Sphere& InSphere, const AABox& AABB)
  {
  float RadiusSquared = Math::Square(InSphere.W);

  //If the distance is less than or equal to the radius, they intersect
  return SphereAABBIntersection(InSphere.Center, RadiusSquared, AABB);
  }
  */
}
