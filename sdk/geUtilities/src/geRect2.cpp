/*****************************************************************************/
/**
 * @file    geRect2.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2019/02/14
 * @brief   Represents a 2D rectangle using real values.
 *
 * Represents a 2D rectangle using real values. Rectangle is represented with
 * an origin in top left and width/height.
 *
 * @bug     No known bugs.
 */
 /*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
 /*****************************************************************************/
#include "geRect2.h"
#include "geMath.h"
#include "geMatrix4.h"

namespace geEngineSDK {
  const Rect2 Rect2::EMPTY;

  bool
  Rect2::contains(const Vector2& point) const {
    if (point.x >= x && point.x <= (x + width)) {
      return (point.y >= y && point.y <= (y + height));
    }

    return false;
  }

  bool
  Rect2::overlaps(const Rect2& other) const {
    float otherRight = other.x + other.width;
    float myRight = x + width;

    float otherBottom = other.y + other.height;
    float myBottom = y + height;

    return (x < otherRight && myRight > other.x &&
            y < otherBottom && myBottom > other.y);
  }

  void
  Rect2::encapsulate(const Rect2& other) {
    float myRight = x + width;
    float myBottom = y + height;
    float otherRight = other.x + other.width;
    float otherBottom = other.y + other.height;

    if (other.x < x) {
      x = other.x;
    }

    if (other.y < y) {
      y = other.y;
    }

    if (otherRight > myRight) {
      width = otherRight - x;
    }
    else {
      width = myRight - x;
    }

    if (otherBottom > myBottom) {
      height = otherBottom - y;
    }
    else {
      height = myBottom - y;
    }
  }

  void
  Rect2::clip(const Rect2& clipRect) {
    float newLeft = Math::max(x, clipRect.x);
    float newTop = Math::max(y, clipRect.y);

    float newRight = Math::min(x + width, clipRect.x + clipRect.width);
    float newBottom = Math::min(y + height, clipRect.y + clipRect.height);

    x = newLeft;
    y = newTop;
    width = newRight - newLeft;
    height = newBottom - newTop;
  }

  void
  Rect2::transform(const Matrix4& matrix) {
    Vector4 verts[4];
    verts[0] = Vector4(x, y, 0.0f, 1.0f);
    verts[1] = Vector4(x + width, y, 0.0f, 1.0f);
    verts[2] = Vector4(x, y + height, 0.0f, 1.0f);
    verts[3] = Vector4(x + width, y + height, 0.0f, 1.0f);

    for (auto& vert : verts) {
      vert = matrix.transformVector4(vert);
    }

    float minX = NumLimit::MAX_FLOAT;
    float maxX = NumLimit::MIN_FLOAT;
    float minY = NumLimit::MAX_FLOAT;
    float maxY = NumLimit::MIN_FLOAT;

    for (auto& vert : verts) {
      if (vert.x < minX) {
        minX = vert.x;
      }

      if (vert.y < minY) {
        minY = vert.y;
      }

      if (vert.x > maxX) {
        maxX = vert.x;
      }

      if (vert.y > maxY) {
        maxY = vert.y;
      }
    }

    x = minX;
    y = minY;
    width = maxX - x;
    height = maxY - y;
  }

  Vector2
  Rect2::getCenter() const {
    return Vector2(x + width * 0.5f, y + height * 0.5f);
  }

  Vector2
  Rect2::getHalfSize() const {
    return Vector2(width, height) * 0.5f;
  }
}
