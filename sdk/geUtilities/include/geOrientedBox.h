/*****************************************************************************/
/**
 * @file    geOrientedBox.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/29
 * @brief   Class for arbitrarily oriented boxes (not necessarily axis-aligned)
 *
 * Class for arbitrarily oriented boxes (not necessarily axis-aligned).
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
#include "geInterval.h"

namespace geEngineSDK {
  class OrientedBox final
  {
   public:
    /**
     * @brief Default constructor.
     * Constructs a unit-sized, origin-centered box with axes aligned to the
     * coordinate system.
     */
    OrientedBox() = default;

   public:
    /**
     * @brief Fills in the Verts array with the eight vertices of the box.
     * @param Verts The array to fill in with the vertices.
     */
    FORCEINLINE void
    calcVertices(Vector3* Verts) const;

    /**
     * @brief Finds the projection interval of the box when projected onto Axis.
     * @param Axis The unit vector defining the axis to project the box onto.
     */
    FORCEINLINE FloatInterval
    project(const Vector3& Axis) const;

   public:
    /**
     * @brief Holds the center of the box.
     */
    Vector3 m_center{0.0f};

    /**
     * @brief Holds the x-axis vector of the box. Must be a unit vector.
     */
    Vector3 m_axisX{ 1.0f, 0.0f, 0.0f };

    /**
     * @brief Holds the y-axis vector of the box. Must be a unit vector.
     */
    Vector3 m_axisY{ 0.0f, 1.0f, 0.0f };

    /**
     * @brief Holds the z-axis vector of the box. Must be a unit vector.
     */
    Vector3 m_axisZ{ 0.0f, 0.0f, 1.0f };

    /**
     * @brief Holds the extent of the box along its x-axis.
     */
    float m_extentX = 1.0f;

    /**
     * @brief Holds the extent of the box along its y-axis.
     */
    float m_extentY = 1.0f;

    /**
     * @brief Holds the extent of the box along its z-axis.
     */
    float m_extentZ = 1.0f;
  };

  /***************************************************************************/
  /**
   * OrientedBox inline functions
   */
  /***************************************************************************/

  FORCEINLINE void
  OrientedBox::calcVertices(Vector3* Verts) const {
    static const float Signs[] = { -1.0f, 1.0f };

    for (int32 i = 0; i < 2; ++i) {
      for (int32 j = 0; j < 2; ++j) {
        for (int32 k = 0; k < 2; ++k) {
          *Verts++ = m_center + Signs[i] * m_axisX * m_extentX +
                                Signs[j] * m_axisY * m_extentY +
                                Signs[k] * m_axisZ * m_extentZ;
        }
      }
    }
  }

  FORCEINLINE FloatInterval
  OrientedBox::project(const Vector3& Axis) const {
    static const float Signs[] = { -1.0f, 1.0f };

    //Calculate the projections of the box center and the extent-scaled axes.
    float ProjectedCenter = Axis | m_center;
    float ProjectedAxisX = Axis | (m_extentX * m_axisX);
    float ProjectedAxisY = Axis | (m_extentY * m_axisY);
    float ProjectedAxisZ = Axis | (m_extentZ * m_axisZ);

    FloatInterval ProjectionInterval;

    for (int32 i = 0; i < 2; ++i) {
      for (int32 j = 0; j < 2; ++j) {
        for (int32 k = 0; k < 2; ++k) {
          //Project the box vertex onto the axis.
          float ProjectedVertex = ProjectedCenter + Signs[i] * ProjectedAxisX +
                                                    Signs[j] * ProjectedAxisY +
                                                    Signs[k] * ProjectedAxisZ;

          //If necessary, expand the projection interval to include the box
          //vertex projection.
          ProjectionInterval.include(ProjectedVertex);
        }
      }
    }

    return ProjectionInterval;
  }

  GE_ALLOW_MEMCPY_SERIALIZATION(OrientedBox);
}
