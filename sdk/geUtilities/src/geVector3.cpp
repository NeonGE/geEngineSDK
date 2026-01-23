/*****************************************************************************/
/**
 * @file    geVector3.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geVector3.h"
#include "geRotator.h"
#include "geQuaternion.h"

namespace geEngineSDK {
  const Vector3 Vector3::ZERO     = Vector3(0.f, 0.f, 0.f);
  const Vector3 Vector3::UNIT     = Vector3(1.f, 1.f, 1.f);
  const Vector3 Vector3::UP       = Vector3(0.f, 1.f, 0.f);
  const Vector3 Vector3::DOWN     = Vector3(0.f,-1.f, 0.f);
  const Vector3 Vector3::FORWARD  = Vector3(0.f, 0.f, 1.f);
  const Vector3 Vector3::BACKWARD = Vector3(0.f, 0.f,-1.f);
  const Vector3 Vector3::RIGHT    = Vector3(1.f, 0.f, 0.f);
  const Vector3 Vector3::LEFT     = Vector3(-1.f, 0.f, 0.f);

  Rotator
  Vector3::toOrientationRotator() const {
    Rotator R;

    //Find yaw.
    R.yaw = Math::atan2(y, x).valueDegrees();

    // Find pitch.
    R.pitch = Math::atan2(z, Math::sqrt(x*x + y*y)).valueDegrees();

    //Find roll.
    R.roll = 0;

# if USING(GE_DEBUG_MODE)
    if (R.containsNaN()) {
      GE_LOG(kWarning, Generic, "Vector::rotation(): Rotator result contains NaN!");
      R = Rotator::ZERO;
    }
# endif
    return R;
  }

  Quaternion
  Vector3::toOrientationQuat() const {
    //Essentially an optimized Vector->Rotator->Quat made possible by knowing
    //Roll == 0, and avoiding radians->degrees->radians.
    //This is done to avoid adding any roll (which our API states as a constraint).
    const float YawRad = Math::atan2(y, x).valueRadians();
    const float PitchRad = Math::atan2(z, Math::sqrt(x*x + y*y)).valueRadians();

    const float DIVIDE_BY_2 = 0.5f;
    float SP, SY;
    float CP, CY;

    Math::sin_cos(&SP, &CP, PitchRad * DIVIDE_BY_2);
    Math::sin_cos(&SY, &CY, YawRad * DIVIDE_BY_2);

    Quaternion RotationQuat;
    RotationQuat.x = SP*SY;
    RotationQuat.y = -SP*CY;
    RotationQuat.z = CP*SY;
    RotationQuat.w = CP*CY;
    return RotationQuat;
  }

  Rotator Vector3::rotation() const {
    return toOrientationRotator();
  }

  void
  Vector3::findBestAxisVectors(Vector3& Axis1, Vector3& Axis2) const {
    const float NX = Math::abs(x);
    const float NY = Math::abs(y);
    const float NZ = Math::abs(z);

    //Find best basis vectors.
    if (NZ > NX && NZ > NY) {
      Axis1 = Vector3(1.f, 0.f, 0.f);
    }
    else {
      Axis1 = Vector3(0.f, 0.f, 1.f);
    }

    Axis1 = (Axis1 - *this * (Axis1 | *this)).getSafeNormal();
    Axis2 = Axis1 ^ *this;
  }

  Vector3
  Math::closestPointOnLine(const Vector3& LineStart,
                           const Vector3& LineEnd,
                           const Vector3& Point) {
    /**
     * Solve to find alpha along line that is closest point
     * Weisstein, Eric W. "Point-Line Distance--3-Dimensional." From MathWorld
     * A Switchram Web Resource.
     * http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
     */
    const float A = (LineStart - Point) | (LineEnd - LineStart);
    const float B = (LineEnd - LineStart).sizeSquared();
    
    //This should be robust to B == 0 (resulting in NaN) because clamp should return 1.
    const float T = Math::clamp(-A / B, 0.f, 1.f);

    //Generate closest point
    Vector3 ClosestPoint = LineStart + (T * (LineEnd - LineStart));

    return ClosestPoint;
  }

  void
  Vector3::createOrthonormalBasis(Vector3& XAxis, Vector3& YAxis, Vector3& ZAxis)
  {
    //Project the X and Y axes onto the plane perpendicular to the Z axis.
    XAxis -= (XAxis | ZAxis) / (ZAxis | ZAxis) * ZAxis;
    YAxis -= (YAxis | ZAxis) / (ZAxis | ZAxis) * ZAxis;

    //If the X axis was parallel to the Z axis, choose a vector which is
    //orthogonal to the Y and Z axes.
    if (XAxis.sizeSquared() < Math::DELTA*Math::DELTA) {
      XAxis = YAxis ^ ZAxis;
    }

    //If the Y axis was parallel to the Z axis, choose a vector which is
    //orthogonal to the X and Z axes.
    if (YAxis.sizeSquared() < Math::DELTA*Math::DELTA) {
      YAxis = XAxis ^ ZAxis;
    }

    //Normalize the basis vectors.
    XAxis.normalize();
    YAxis.normalize();
    ZAxis.normalize();
  }

  void Vector3::unwindEuler() {
    x = Math::unwindDegrees(x);
    y = Math::unwindDegrees(y);
    z = Math::unwindDegrees(z);
  }


  float
  Vector3::evaluateBezier(const Vector3* ControlPoints,
                          int32 NumPoints,
                          Vector<Vector3>& OutPoints) {
    GE_ASSERT(ControlPoints);
    GE_ASSERT(NumPoints > 1);

    //var q is the change in t between successive evaluations.
    const float q = 1.f / (NumPoints - 1); //q is dependent on the number of GAPS = POINTS-1

    //recreate the names used in the derivation
    const Vector3& P0 = ControlPoints[0];
    const Vector3& P1 = ControlPoints[1];
    const Vector3& P2 = ControlPoints[2];
    const Vector3& P3 = ControlPoints[3];

    //coefficients of the cubic polynomial that we're FDing -
    const Vector3 a = P0;
    const Vector3 b = 3 * (P1 - P0);
    const Vector3 c = 3 * (P2 - 2 * P1 + P0);
    const Vector3 d = P3 - 3 * P2 + 3 * P1 - P0;

    //initial values of the poly and the 3 diffs -
    Vector3 S = a;                        //the poly value
    Vector3 U = b * q + c*q*q + d*q*q*q;	//1st order diff (quadratic)
    Vector3 V = 2 * c*q*q + 6 * d*q*q*q;  //2nd order diff (linear)
    Vector3 W = 6 * d*q*q*q;              // 3rd order diff (constant)

    //Path length.
    float Length = 0.f;

    Vector3 OldPos = P0;
    OutPoints.push_back(P0);  //first point on the curve is always P0.

    for (int32 i = 1; i < NumPoints; ++i) {
      //calculate the next value and update the deltas
      S += U;     //update poly value
      U += V;     //update 1st order diff value
      V += W;     //update 2st order diff value
                  //3rd order diff is constant => no update needed.

      //Update Length.
      Length += Vector3::distance(S, OldPos);
      OldPos = S;

      OutPoints.push_back(S);
    }

    //Return path length as experienced in sequence (linear interpolation between points).
    return Length;
  }

  struct ClusterMovedHereToMakeCompile
  {
    ClusterMovedHereToMakeCompile() {
      memset(this, 0, sizeof(ClusterMovedHereToMakeCompile));
    }

    Vector3 clusterPosAccum;
    int32 clusterSize;
  };

  void Vector3::generateClusterCenters(Vector<Vector3>& Clusters,
                                       const Vector<Vector3>& Points,
                                       int32 NumIterations,
                                       int32 NumConnectionsToBeValid) {
    //Check we have > 0 points and clusters
    if (Points.empty() || Clusters.empty()) {
      return;
    }

    //Temp storage for each cluster that mirrors the order of the passed in Clusters array
    Vector<ClusterMovedHereToMakeCompile> ClusterData;
    ClusterData.resize(Clusters.size());

    //Then iterate
    for (int32 ItCount = 0; ItCount < NumIterations; ++ItCount) {
      //Classify each point - find closest cluster center
      for (const Vector3& Pos : Points) {
        //Iterate over all clusters to find closes one
        int32 NearestClusterIndex = -1;
        float NearestClusterDistSqr = Math::BIG_NUMBER;
        for (SIZE_T j = 0; j < Clusters.size(); ++j) {
          const float DistSqr = (Pos - Clusters[j]).sizeSquared();
          if (DistSqr < NearestClusterDistSqr) {
            NearestClusterDistSqr = DistSqr;
            NearestClusterIndex = static_cast<int32>(j);
          }
        }
        
        //Update its info with this point
        if (-1 != NearestClusterIndex ) {
          ClusterData[NearestClusterIndex].clusterPosAccum += Pos;
          ClusterData[NearestClusterIndex].clusterSize++;
        }
      }

      //All points classified - update cluster center as average of membership
      for (SIZE_T i = 0; i < Clusters.size(); ++i) {
        if (0 < ClusterData[i].clusterSize) {
          Clusters[i] = ClusterData[i].clusterPosAccum / 
                         static_cast<float>(ClusterData[i].clusterSize);
        }
      }
    }

    //So now after we have possible cluster centers we want to remove the ones
    //that are outliers and not part of the main cluster
    for (int32 i = static_cast<int32>(ClusterData.size()-1); i >= 0; --i) {
      if (ClusterData[i].clusterSize < NumConnectionsToBeValid) {
        Vector<Vector3>::iterator nth = Clusters.begin() + i;
        Clusters.erase(nth);
      }
    }
  }
}
