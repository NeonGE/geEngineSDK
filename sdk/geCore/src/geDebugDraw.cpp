/*****************************************************************************/
/**
 * @file    geDebugDraw.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
 /*****************************************************************************/
#include "geDebugDraw.h"
#include "geRenderAPI.h"

#include <geBox.h>
#include <geSphere.h>

namespace geEngineSDK {
  DebugDraw::DebugDraw() {
    auto& renderAPI = RenderAPI::instance();

    if (!renderAPI.isStarted()) {
      GE_EXCEPT(InternalErrorException,
                "RenderAPI module must be started before DebugDraw.");
    }

    m_frameData.resize(DEBUG_CACHE_OBJECTS_NUM);

    /*
    m_debugPass = ge_shared_ptr_new<Pass>("Data/Shaders/DebugShader.hlsl", "DebugVS", nullptr,
      "Data/Shaders/DebugShader.hlsl", "DebugPS", nullptr);
    m_debugPass->setBlendState(geBlend::DEFAULT);
    m_debugPass->setDepthStencilState(geDepth::DEFAULT);
    */

    //Create a vertex declaration for the debug vertices
    using VET = VERTEX_ELEMENT_TYPE::E;
    using VES = VERTEX_ELEMENT_SEMANTIC::E;
    auto debugVertexDecl = renderAPI.createVertexDeclaration(
    {
      VertexElement(0, 0, VET::FLOAT3, VES::POSITION),
      VertexElement(0, 12, VET::COLOR_ABGR, VES::COLOR)
    });

    SIZE_T bufferSize = DEBUG_CACHE_OBJECTS_NUM * sizeof(DEBUG_VERTEX);
    m_frameVB = renderAPI.createVertexBuffer(debugVertexDecl,
                                             bufferSize,
                                             nullptr,
                                             RESOURCE_USAGE::DYNAMIC);
  }

  void
  DebugDraw::update() {
    GE_ASSERT(m_numFrameObjsInUse < DEBUG_CACHE_OBJECTS_NUM);

    if (0 == m_numFrameObjsInUse) {
      return;
    }

    //Update the buffer with the data of the lines
    auto& renderAPI = RenderAPI::instance();
    
    MappedSubresource mappedRes;
    ge_zero_out(mappedRes);

    mappedRes = renderAPI.map(m_frameVB);
    memcpy(mappedRes.pData, m_frameData.data(), m_numFrameObjsInUse * sizeof(DEBUG_VERTEX));
    renderAPI.unmap(m_frameVB);
  }

  void
  DebugDraw::draw() {
    if (0 == m_numFrameObjsInUse) {
      return;
    }

    auto& renderAPI = RenderAPI::instance();

    renderAPI.setVertexBuffer(m_frameVB);
    renderAPI.setTopology(PRIMITIVE_TOPOLOGY::LINELIST);
    
    //m_debugPass->setPass();

    renderAPI.draw(m_numFrameObjsInUse);
  }

  void
  DebugDraw::reset() {
    //Resets the counter for the frame objects
    m_numFrameObjsInUse = 0;
  }

  void
    DebugDraw::addLine(const Vector3& posA,
                       const Vector3& posB,
                       Color color,
                       float duration) {
    GE_ASSERT(m_numFrameObjsInUse + 2 < DEBUG_CACHE_OBJECTS_NUM);
    if (0.0f == duration) { //This is a frame only line
      m_frameData[m_numFrameObjsInUse++] = { posA, color };
      m_frameData[m_numFrameObjsInUse++] = { posB, color };
    }
  }

  void
  DebugDraw::addBox(const AABox& box,
                    Color color,
                    float duration) {
    GE_UNREFERENCED_PARAMETER(duration);

    Vector3 Vertices[8] =
    {
      Vector3(box.m_min),
      Vector3(box.m_min.x, box.m_min.y, box.m_max.z),
      Vector3(box.m_min.x, box.m_max.y, box.m_min.z),
      Vector3(box.m_max.x, box.m_min.y, box.m_min.z),
      Vector3(box.m_max.x, box.m_max.y, box.m_min.z),
      Vector3(box.m_max.x, box.m_min.y, box.m_max.z),
      Vector3(box.m_min.x, box.m_max.y, box.m_max.z),
      Vector3(box.m_max)
    };

    //Bottom frame of the box
    addLine(Vertices[0], Vertices[2], color);
    addLine(Vertices[0], Vertices[3], color);
    addLine(Vertices[2], Vertices[4], color);
    addLine(Vertices[3], Vertices[4], color);

    //Top frame of the box
    addLine(Vertices[7], Vertices[5], color);
    addLine(Vertices[7], Vertices[6], color);
    addLine(Vertices[5], Vertices[1], color);
    addLine(Vertices[6], Vertices[1], color);

    //Side frames of the box
    addLine(Vertices[0], Vertices[1], color);
    addLine(Vertices[2], Vertices[6], color);
    addLine(Vertices[3], Vertices[5], color);
    addLine(Vertices[7], Vertices[4], color);
  }

  void
  DebugDraw::addSphere(const Sphere& sphere,
                       uint32 numSegments,
                       Color color,
                       float duration) {
    GE_UNREFERENCED_PARAMETER(duration);

    numSegments = Math::clamp(numSegments, 3u, 32u);

    const Vector3& center = sphere.m_center;
    const float& radius = sphere.m_radius;

    const float step = Math::TWO_PI / cast::st<float>(numSegments);

    for (uint32 i = 0; i < numSegments; ++i) {
      const float angle0 = step * cast::st<float>(i);
      const float angle1 = step * cast::st<float>(i + 1);

      const float cos0 = Math::cos(angle0);
      const float sin0 = Math::sin(angle0);
      const float cos1 = Math::cos(angle1);
      const float sin1 = Math::sin(angle1);

      //Circle on XY plane
      {
        Vector3 p0(center.x + cos0 * radius,
                   center.y + sin0 * radius,
                   center.z);

        Vector3 p1(center.x + cos1 * radius,
                   center.y + sin1 * radius,
                   center.z);

        addLine(p0, p1, color, duration);
      }

      //Circle on XZ plane
      {
        Vector3 p0(center.x + cos0 * radius,
                   center.y,
                   center.z + sin0 * radius);

        Vector3 p1(center.x + cos1 * radius,
                   center.y,
                   center.z + sin1 * radius);

        addLine(p0, p1, color, duration);
      }

      // Circle on YZ plane
      {
        Vector3 p0(center.x,
                   center.y + cos0 * radius,
                   center.z + sin0 * radius);

        Vector3 p1(center.x,
                   center.y + cos1 * radius,
                   center.z + sin1 * radius);

        addLine(p0, p1, color, duration);
      }
    }
  }

  void
  DebugDraw::addGrid(const Vector3& center,
                     const Vector3& up,
                     const Vector3& right,
                     uint32 numRows,
                     uint32 numCols,
                     float cellSize,
                     Color color,
                     float duration) {
    GE_UNREFERENCED_PARAMETER(duration);

    //Calculate the half size of the grid
    float halfSize = cellSize * 0.5f;

    //Calculate the start position of the grid
    Vector3 startPos = center -
      (up * halfSize * cast::st<float>(numRows)) -
      (right * halfSize * cast::st<float>(numCols));

    //Add the horizontal lines
    Vector3 start = startPos;
    for (uint32 i = 0; i <= numCols; ++i) {
      Vector3 end = start + (up * cellSize * static_cast<float>(numCols));
      addLine(start, end, color);
      start += (right * cellSize);
    }

    //Add the vertical lines
    start = startPos;
    for (uint32 i = 0; i <= numRows; ++i) {
      Vector3 end = start + (right * cellSize * static_cast<float>(numRows));
      addLine(start, end, color);
      start += (up * cellSize);
    }
  }

} // namespace geEngineSDK
