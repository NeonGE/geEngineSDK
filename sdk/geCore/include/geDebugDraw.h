/*****************************************************************************/
/**
 * @file    geDebugDraw.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/03/12
 * @brief   Class for drawing debug primitives.
 *
 * This class allows for drawing simple debug primitives such as lines, boxes,
 * spheres, grids and meshes. These primitives can be used for visual debugging
 * purposes, such as visualizing bounding volumes, collision shapes, or other
 * non-renderable data in the scene.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"
#include "geGraphicsInterfaces.h"
#include <geModule.h>
#include <geVector3.h>
#include <geVector4.h>
#include <geColor.h>

namespace geEngineSDK {

  //Define the number of vertices that will be cached on the 
  constexpr uint32 DEBUG_CACHE_OBJECTS_NUM = 4096;

  struct DEBUG_VERTEX
  {
    DEBUG_VERTEX() = default;
    DEBUG_VERTEX(Vector4 _position, Color _color)
      : position(_position),
        color(_color)
    {}

    Vector3 position;
    Color color;
  };

  class GE_CORE_EXPORT DebugDraw : public Module<DebugDraw>
  {
   public:
    DebugDraw();
    ~DebugDraw() = default;

    void
    update();

    void
    draw();

    void
    reset();

    void
    addLine(const Vector3& posA,
            const Vector3& posB,
            Color color = Color::Red,
            float duration = 0.0f);

    void
    addBox(const AABox& box,
           Color color = Color::Red,
           float duration = 0.0f);

    void
    addSphere(const Sphere& sphere,
              uint32 numSegments = 16,
              Color color = Color::Red,
              float duration = 0.0f);

    void
    addGrid(const Vector3& center,
            const Vector3& up,
            const Vector3& right,
            uint32 numRows,
            uint32 numCols,
            float cellSize,
            Color color = Color::Red,
            float duration = 0.0f);

    void
    addMesh(const Vector<Vector3>& vertices,
            const Vector<uint32>& indices,
            Color color = Color::Red,
            float duration = 0.0f);

   private:
    SPtr<VertexBuffer> m_frameVB;

    uint32 m_numFrameObjsInUse = 0;
    Vector<DEBUG_VERTEX> m_frameData;

    uint32 m_numPersistentObjsInUse = 0;
    Vector<DEBUG_VERTEX> m_persistentData;
  };
}