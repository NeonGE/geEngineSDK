/*****************************************************************************/
/**
 * @file    geRotator.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/06/28
 * @brief   Implements a container for rotation information.
 *
 * All rotation values are stored in degrees.
 *
 * @note    Rotator contract:
 *          - pitch: look up/down (+up), applied as -pitch around +X
 *          - yaw:   turn left/right (+right), around +Y
 *          - roll:  twist (+clockwise), applied as -roll around +Z
 *          - order: yaw * pitch * roll
 * 
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

#ifndef _INC_ROTATOR_H_
# define _INC_ROTATOR_H_
#endif

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geMath.h"
#include "geVector3.h"
#include "geDebug.h"

namespace geEngineSDK {
  /**
   * @brief Implements a container for rotation information.
   * All rotation values are stored in degrees.
   */
  class Rotator
  {
   public:
#if USING(GE_DEBUG_MODE)
    FORCEINLINE void
    diagnosticCheckNaN() const {
      if (containsNaN()) {
        GE_LOG(kWarning, Generic, "Rotator contains NaN.");
        *const_cast<Rotator*>(this) = ZERO;
      }
    }

    FORCEINLINE void
    diagnosticCheckNaN(const String& Message) const {
      if (containsNaN()) {
        GE_LOG(kWarning, Generic, "{0}: Rotator contains NaN", Message);
        *const_cast<Rotator*>(this) = ZERO;
      }
    }
#else
    FORCEINLINE void diagnosticCheckNaN() const {}
    FORCEINLINE void diagnosticCheckNaN(const String&) const {}
#endif

    /**
     * @brief Default constructor (no initialization).
     */
    FORCEINLINE Rotator() = default;

    /**
     * @brief Constructor
     * @param InF Value to set all components to.
     */
    explicit FORCEINLINE Rotator(float InF)
      : pitch(InF), yaw(InF), roll(InF) {
      diagnosticCheckNaN();
    }

    /**
     * @brief Constructor.
     * @param InPitch Pitch in degrees.
     * @param InYaw Yaw in degrees.
     * @param InRoll Roll in degrees.
     */
    FORCEINLINE Rotator(float InPitch, float InYaw, float InRoll)
      : pitch(InPitch), yaw(InYaw), roll(InRoll) {
      diagnosticCheckNaN();
    }

    /**
     * @brief Constructor.
     * @param FORCE_INIT Force Init Enum.
     */
    explicit FORCEINLINE Rotator(FORCE_INIT::E)
      : pitch(0), yaw(0), roll(0) {
    }

    /**
     * @brief Constructor.
     * @param Quat Quaternion used to specify rotation.
     */
    explicit GE_UTILITIES_EXPORT Rotator(const Quaternion& Quat);

   public:
    /*************************************************************************/
    /**
     * Binary arithmetic operators.
     */
    /*************************************************************************/

    /**
     * @brief Get the result of adding a rotator to this.
     * @param R The other rotator.
     * @return The result of adding a rotator to this.
     */
    GE_NODISCARD FORCEINLINE Rotator
    operator+(const Rotator& R) const {
      return Rotator(pitch + R.pitch, yaw + R.yaw, roll + R.roll);
    }

    /**
     * @brief Get the result of subtracting a rotator from this.
     * @param R The other rotator.
     * @return The result of subtracting a rotator from this.
     */
    GE_NODISCARD FORCEINLINE Rotator
    operator-(const Rotator& R) const {
      return Rotator(pitch - R.pitch, yaw - R.yaw, roll - R.roll);
    }

    /**
     * @brief Get the result of scaling this rotator.
     * @param Scale The scaling factor.
     * @return The result of scaling.
     */
    GE_NODISCARD FORCEINLINE Rotator
    operator*(float Scale) const {
      return Rotator(pitch * Scale, yaw * Scale, roll * Scale);
    }

    /**
     * @brief Multiply this rotator by a scaling factor.
     * @param Scale The scaling factor.
     * @return Copy of the rotator after scaling.
     */
    FORCEINLINE Rotator&
    operator*=(float Scale) {
      pitch = pitch * Scale; yaw = yaw * Scale; roll = roll * Scale;
      diagnosticCheckNaN();
      return *this;
    }

    /*************************************************************************/
    /**
     * Binary comparison operators.
     */
    /*************************************************************************/

    /**
     * @brief Checks whether two rotators are identical. This checks each
     *        component for exact equality.
     * @param R The other rotator.
     * @return true if two rotators are identical, otherwise false.
     * @see equals()
     */
    GE_NODISCARD FORCEINLINE bool
    operator==(const Rotator& R) const {
      return pitch == R.pitch && yaw == R.yaw && roll == R.roll;
    }

    /**
     * @brief Checks whether two rotators are different.
     * @param V The other rotator.
     * @return true if two rotators are different, otherwise false.
     */
    GE_NODISCARD FORCEINLINE bool
    operator!=(const Rotator& V) const {
      return !((*this) == V);
    }

    /*************************************************************************/
    /**
     * Assignment operators.
     */
    /*************************************************************************/

    /**
     * @brief Adds another rotator to this.
     * @param R The other rotator.
     * @return Copy of rotator after addition.
     */
    FORCEINLINE Rotator&
    operator+=(const Rotator& R) {
      pitch += R.pitch; yaw += R.yaw; roll += R.roll;
      diagnosticCheckNaN();
      return *this;
    }

    /**
     * @brief Subtracts another rotator from this.
     * @param R The other rotator.
     * @return Copy of rotator after subtraction.
     */
    FORCEINLINE Rotator&
    operator-=(const Rotator& R) {
      pitch -= R.pitch; yaw -= R.yaw; roll -= R.roll;
      diagnosticCheckNaN();
      return *this;
    }

   public:
    /*************************************************************************/
    /**
     * Functions.
     */
    /*************************************************************************/

    /**
     * @brief Checks whether rotator is nearly zero within specified tolerance,
     *        when treated as an orientation. This means that Rotator(0, 0, 360)
     *        is "ZERO", because it is the same final orientation as the zero rotator.
     * @param Tolerance Error Tolerance.
     * @return true if rotator is nearly zero, within specified tolerance, otherwise false.
     */
    GE_NODISCARD FORCEINLINE bool
    isNearlyZero(float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      return Math::abs(normalizeAxis(pitch)) <= Tolerance
          && Math::abs(normalizeAxis(yaw)) <= Tolerance
          && Math::abs(normalizeAxis(roll)) <= Tolerance;
    }

    /**
     * @brief Checks whether this has exactly zero rotation, when treated as an
     *        orientation. This means that FRotator(0, 0, 360) is "ZERO",
     *        because it is the same final orientation as the zero rotator.
     * @return true if this has exactly zero rotation, otherwise false.
     */
    GE_NODISCARD FORCEINLINE bool
    isZero() const {
      return (clampAxis(pitch) == 0.f)
          && (clampAxis(yaw) == 0.f)
          && (clampAxis(roll) == 0.f);
    }

    /**
     * @brief Checks whether two rotators are equal within specified tolerance,
     *        when treated as an orientation. This means that 
     *        Rotator(0, 0, 360).equals(Rotator(0,0,0)) is true, because they
     *        represent the same final orientation.
     * @param R The other rotator.
     * @param Tolerance Error Tolerance.
     * @return true if two rotators are equal, within specified tolerance,
     *         otherwise false.
     */
    GE_NODISCARD FORCEINLINE bool
    equals(const Rotator& R, float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      return (Math::abs(normalizeAxis(pitch - R.pitch)) <= Tolerance)
          && (Math::abs(normalizeAxis(yaw - R.yaw)) <= Tolerance)
          && (Math::abs(normalizeAxis(roll - R.roll)) <= Tolerance);
    }

    /**
     * @brief Adds to each component of the rotator.
     * @param DeltaPitch Change in pitch. (+/-)
     * @param DeltaYaw Change in yaw. (+/-)
     * @param DeltaRoll Change in roll. (+/-)
     * @return Copy of rotator after addition.
     */
    FORCEINLINE Rotator&
    add(float DeltaPitch, float DeltaYaw, float DeltaRoll) {
      yaw += DeltaYaw;
      pitch += DeltaPitch;
      roll += DeltaRoll;
      diagnosticCheckNaN();
      return *this;
    }

    /**
     * @brief Returns the inverse of the rotator.
     */
    GE_NODISCARD GE_UTILITIES_EXPORT Rotator
    getInverse() const;

    /**
     * @brief Get the rotation, snapped to specified degree segments.
     * @param RotGrid A Rotator specifying how to snap each component.
     * @return Snapped version of rotation.
     */
    GE_NODISCARD FORCEINLINE Rotator
    gridSnap(const Rotator& RotGrid) const {
      return Rotator(Math::gridSnap(pitch, RotGrid.pitch),
                     Math::gridSnap(yaw, RotGrid.yaw),
                     Math::gridSnap(roll, RotGrid.roll));
    }

    /**
     * @brief Convert a rotation into a unit vector facing in its direction.
     * @return Rotation as a unit direction vector.
     */
    GE_NODISCARD GE_UTILITIES_EXPORT Vector3
    toVector() const;

    /**
     * @brief Get Rotation as a quaternion.
     * @return Rotation as a quaternion.
     */
    GE_NODISCARD GE_UTILITIES_EXPORT Quaternion
    toQuaternion() const;

    /**
     * @brief Convert a Rotator into floating-point Euler angles (in degrees).
     *        Rotator now stored in degrees.
     * @return Rotation as a Euler angle vector.
     */
    GE_UTILITIES_EXPORT Vector3
    euler() const;

    /**
     * @brief Rotate a vector rotated by this rotator.
     * @param V The vector to rotate.
     * @return The rotated vector.
     */
    GE_UTILITIES_EXPORT Vector3
    rotateVector(const Vector3& V) const;

    /**
     * @brief Returns the vector rotated by the inverse of this rotator.
     * @param V The vector to rotate.
     * @return The rotated vector.
     */
    GE_UTILITIES_EXPORT Vector3
    unrotateVector(const Vector3& V) const;

    /**
     * @brief Gets the rotation values so they fall within the range [0,360]
     * @return Clamped version of rotator.
     */
    GE_NODISCARD FORCEINLINE Rotator
    clamp() const {
      return Rotator(clampAxis(pitch), clampAxis(yaw), clampAxis(roll));
    }

    /**
     * @brief Create a copy of this rotator and normalize, removes all winding
     *        and creates the "shortest route" rotation.
     * @return Normalized copy of this rotator
     */
    GE_NODISCARD FORCEINLINE Rotator
    getNormalized() const {
      Rotator Rot = *this;
      Rot.normalize();
      return Rot;
    }

    /**
     * @brief Create a copy of this rotator and denormalize, clamping each axis to 0 - 360.
     * @return Denormalized copy of this rotator
     */
    GE_NODISCARD FORCEINLINE Rotator
    getDenormalized() const {
      Rotator Rot = *this;
      Rot.pitch = clampAxis(Rot.pitch);
      Rot.yaw = clampAxis(Rot.yaw);
      Rot.roll = clampAxis(Rot.roll);
      return Rot;
    }

    /**
     * @brief Get a specific component of the vector, given a specific axis by enum
     */
    GE_NODISCARD FORCEINLINE float
    getComponentForAxis(AXIS::E Axis) const {
      switch (Axis)
      {
        case AXIS::kX:
          return pitch;
        case AXIS::kY:
          return yaw;
        case AXIS::kZ:
          return roll;
        case AXIS::kNone:
        default: {
          GE_LOG(kWarning, Generic, "Invalid Axis.");
        }
      }
      return 0.f;
    }

    /**
     * @brief Set a specified componet of the vector, given a specific axis by enum
     */
    GE_NODISCARD FORCEINLINE void
    setComponentForAxis(AXIS::E Axis, float Component){
      switch (Axis)
      {
        case AXIS::kX:
          pitch = Component;
          break;
        case AXIS::kY:
          yaw = Component;
          break;
        case AXIS::kZ:
          roll = Component;
          break;
        case AXIS::kNone:
        default: {
          GE_LOG(kWarning, Generic, "Invalid Axis.");
        }
      }
    }

    /**
     * @brief In-place normalize, removes all winding and creates the
     *        "shortest route" rotation.
     */
    FORCEINLINE void
    normalize() {
      pitch = normalizeAxis(pitch);
      yaw = normalizeAxis(yaw);
      roll = normalizeAxis(roll);
      diagnosticCheckNaN();
    }

    /**
     * @brief Decompose this Rotator into a Winding part (multiples of 360)
     *        and a Remainder part.
     * Remainder will always be in [-180, 180] range.
     * @param Winding[Out] the Winding part of this Rotator
     * @param Remainder[Out] the Remainder
     */
    GE_UTILITIES_EXPORT void
    getWindingAndRemainder(Rotator& Winding, Rotator& Remainder) const;

    /**
     * @brief Return the Manhattan distance in degrees between this Rotator
     *        and the passed in one.
     * @param Rotator[In] the Rotator we are comparing with.
     * @return Distance(Manhattan) between the two rotators.
     */
    GE_NODISCARD FORCEINLINE float
    getManhattanDistance(const Rotator& Rotator) const {
      return Math::abs<float>(pitch - Rotator.pitch) +
             Math::abs<float>(yaw - Rotator.yaw) +
             Math::abs<float>(roll - Rotator.roll);
    }

    /**
     * @brief Return a Rotator that has the same rotation but has different
     *        degree values for Yaw, Pitch, and Roll.
     *        This rotator should be within -180, 180 range,
     * @return A Rotator with the same rotation but different degrees.
     */
    GE_NODISCARD FORCEINLINE Rotator
    getEquivalentRotator() const {
      return Rotator(180.0f - pitch, yaw + 180.0f, roll + 180.0f);
    }

    /**
     * @brief Modify if necessary the passed in rotator to be the closest
     *        rotator to it based upon it's equivalent.
     *        This Rotator should be within (-180, 180], usually just
     *        constructed from a Matrix or a Quaternion.
     * @param MakeClosest[In/Out] the Rotator we want to make closest to us.
     *        Should be between (-180, 180]. This Rotator may change if we need
     *        to use different degree values to make it closer.
     */
    FORCEINLINE void
    setClosestToMe(Rotator& MakeClosest) const {
      Rotator OtherChoice = MakeClosest.getEquivalentRotator();
      float FirstDiff = getManhattanDistance(MakeClosest);
      float SecondDiff = getManhattanDistance(OtherChoice);
      if (SecondDiff < FirstDiff) {
        MakeClosest = OtherChoice;
      }
    }

    /**
     * @brief Utility to check if there are any non-finite values (NaN, Inf)
     *        in this Rotator.
     * @return true if there are any non-finite values in this Rotator,
     *         otherwise false.
     */
    GE_NODISCARD FORCEINLINE bool
    containsNaN() const {
      return (!Math::isFinite(pitch) ||
        !Math::isFinite(yaw) ||
        !Math::isFinite(roll));
    }

   public:
    /**
     * @brief Clamps an angle to the range of [0, 360).
     * @param Angle The angle to clamp.
     * @return The clamped angle.
     */
    GE_NODISCARD static float
    clampAxis(float Angle) {
      Angle = Math::fmod(Angle, 360.f);

      if (0.f > Angle) {
        //shift to [0,360) range
        Angle += 360.f;
      }

      return Angle;
    }

    /**
     * @brief Clamps an angle to the range of (-180, 180].
     * @param Angle The Angle to clamp.
     * @return The clamped angle.
     */
    GE_NODISCARD static float
    normalizeAxis(float Angle) {
      Angle = clampAxis(Angle);

      if (180.f < Angle) {
        //shift to (-180,180]
        Angle -= 360.f;
      }

      return Angle;
    }

    /**
     * @brief Compresses a floating point angle into a byte.
     * @param Angle The angle to compress.
     * @return The angle as a byte.
     */
    GE_NODISCARD static uint8
    compressAxisToByte(float Angle) {
      //map [0->360) to [0->256) and mask off any winding
      return static_cast<uint8>(Math::round(Angle * 256.f / 360.f) & 0xFF);
    }

    /**
     * @brief Decompress a byte into a floating point angle.
     * @param Angle The word angle.
     * @return The decompressed angle.
     */
    GE_NODISCARD static float
    decompressAxisFromByte(uint8 Angle) {
      //map [0->256) to [0->360)
      return (Angle * 360.f / 256.f);
    }

    /**
     * @brief Compress a floating point angle into a word.
     * @param Angle The angle to compress.
     * @return The decompressed angle.
     */
    GE_NODISCARD static uint16
    compressAxisToShort(float Angle) {
      //map [0->360) to [0->65536) and mask off any winding
      return static_cast<uint16>(Math::round(Angle * 65536.f / 360.f) & 0xFFFF);
    }

    /**
     * @brief Decompress a short into a floating point angle.
     * @param Angle The word angle.
     * @return The decompressed angle.
     */
    GE_NODISCARD static float
    decompressAxisFromShort(uint16 Angle) {
      //map [0->65536) to [0->360)
      return (Angle * 360.f / 65536.f);
    }

    /**
     * @brief Convert a vector of floating-point Euler angles (in degrees) into
     *        a Rotator. Rotator now stored in degrees
     * @param Euler Euler angle vector.
     * @return A rotator from a Euler angle.
     */
    GE_NODISCARD static GE_UTILITIES_EXPORT Rotator
    makeFromEuler(const Vector3& Euler);

   public:
    /**
     * @brief A rotator of zero degrees on each axis.
     */
    static GE_UTILITIES_EXPORT const Rotator ZERO;

   public:
    /**
     * @brief Rotation around the right axis (around X axis), Looking up and down
     *        (0=Straight Ahead, +Up, -Down)
     */
    float pitch;

    /**
     * @brief Rotation around the up axis (Y axis).
     * 0 = Forward (+Z)
     * Positive values rotate to the right (towards +X).
     */
    float yaw;

    /**
     * @brief Rotation around the forward axis (around Z axis), Tilting your head,
     *        0=Straight, +Clockwise, -CCW.
     */
    float roll;
  };

  /***************************************************************************/
  /**
   * Rotator inline functions
   */
  /***************************************************************************/

  /**
   * @brief Scale a rotator and return.
   * @param Scale scale to apply to R.
   * @param R rotator to be scaled.
   * @return Scaled rotator.
   */
  FORCEINLINE Rotator
  operator*(float Scale, const Rotator& R) {
    return R.operator*(Scale);
  }

  /***************************************************************************/
  /**
   * Math inline functions
   */
  /***************************************************************************/
  template<class U>
  Rotator
  Math::lerp(const Rotator& A, const Rotator& B, const U& Alpha) {
    return A + (B - A).getNormalized() * Alpha;
  }

  template<class U>
  Rotator
  Math::lerpRange(const Rotator& A, const Rotator& B, const U& Alpha)
  {
    //Similar to lerp, but does not take the shortest path.
    //Allows interpolation over more than 180 degrees.
    return (A * (1 - Alpha) + B * Alpha).getNormalized();
  }
}
