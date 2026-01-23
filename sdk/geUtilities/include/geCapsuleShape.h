/*****************************************************************************/
/**
 * @file    geCapsuleShape.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/29
 * @brief   Structure for capsules.
 *
 * A capsule consists of two sphere connected by a cylinder.
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

namespace geEngineSDK {
  /**
   * @brief Structure for capsules.
   * A capsule consists of two sphere connected by a cylinder.
   */
  struct CapsuleShape
  {
    /**
     * @brief The capsule's center point.
     */
    Vector3 center;

    /**
     * @brief The capsule's radius.
     */
    float radius;

    /**
     * @brief The capsule's orientation in space.
     */
    Vector3 orientation;

    /**
     * @brief The capsule's length.
     */
    float length;

   public:
    /**
     * @brief Default constructor.
     */
    CapsuleShape() = default;

    /**
     * @brief Create and initialize a new instance.
     * @param InCenter The capsule's center point.
     * @param InRadius The capsule's radius.
     * @param InOrientation The capsule's orientation in space.
     * @param InLength The capsule's length.
     */
    CapsuleShape(Vector3 InCenter, float InRadius, Vector3 InOrientation, float InLength)
      : center(InCenter),
        radius(InRadius),
        orientation(InOrientation),
        length(InLength)
    {}
  };
}
