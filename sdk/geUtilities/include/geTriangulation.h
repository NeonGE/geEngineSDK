#/*****************************************************************************/
/**
 * @file    geTriangulation.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/15
 * @brief   Contains helper methods that triangulate point data.
 *
 * Contains helper methods that triangulate point data.
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

namespace geEngineSDK {
  /**
   * @brief Contains information about a single tetrahedron.
   */
  struct Tetrahedron
  {
    /**
     * @brief Indices of vertices that form the tetrahedron pointing to an
     *        external point array.
     */
    int32 vertices[4];
    
    /**
     * @brief Indices pointing to neighbor tetrahedrons. Each neighbor index maps to the
     *        @p vertices array, so neighbor/vertex pair at the same location will be the only
     *        neighbor not containing that vertex (i.e. neighbor opposite to the vertex). If a
     *        tetrahedron is on the volume edge, it has only three neighbors and its last
     *        neighbor will be set to -1.
     */
    int32 neighbors[4];
  };

  /**
   * @brief Contains information about a single face of a tetrahedron.
   */
  struct TetrahedronFace
  {
    int32 vertices[3];
    int32 tetrahedron;
  };

  /**
   * @brief Contains information about a volume made out of tetrahedrons.
   */
  struct TetrahedronVolume
  {
    Vector<Tetrahedron> tetrahedra;
    Vector<TetrahedronFace> outerFaces;
  };

  /**
   * @brief Contains helper methods that triangulate point data.
   */
  class GE_UTILITIES_EXPORT Triangulation
  {
   public:
    /**
     * @brief Converts a set of input points into a set of tetrahedrons generated using
     *        Delaunay tetrahedralization algorithm. Minimum of 4 points must be provided in
     *        order for the process to work.
     */
    static TetrahedronVolume
    tetrahedralize(const Vector<Vector3>& points);
  };
}
