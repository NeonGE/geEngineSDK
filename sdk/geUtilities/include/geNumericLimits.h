/*****************************************************************************/
/**
 * @file    geNumericLimits.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/20
 * @brief   Constants with the numeric limits for each numeric data type.
 *
 * Constants with the numeric limits for each numeric data type.
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
#include "gePlatformTypes.h"

#include <limits>

namespace geEngineSDK {
  using std::numeric_limits;

  struct GE_UTILITIES_EXPORT NumLimit
  {
    static CONSTEXPR uint8  MIN_UINT8  = numeric_limits<uint8 >::min();
    static CONSTEXPR uint16 MIN_UINT16 = numeric_limits<uint16>::min();
    static CONSTEXPR uint32 MIN_UINT32 = numeric_limits<uint32>::min();
    static CONSTEXPR uint64 MIN_UINT64 = numeric_limits<uint64>::min();

    static CONSTEXPR int8   MIN_INT8  = numeric_limits<int8  >::min();
    static CONSTEXPR int16  MIN_INT16 = numeric_limits<int16 >::min();
    static CONSTEXPR int32  MIN_INT32 = numeric_limits<int32 >::min();
    static CONSTEXPR int64  MIN_INT64 = numeric_limits<int64 >::min();

    static CONSTEXPR uint8  MAX_UINT8  = numeric_limits<uint8 >::max();
    static CONSTEXPR uint16 MAX_UINT16 = numeric_limits<uint16>::max();
    static CONSTEXPR uint32 MAX_UINT32 = numeric_limits<uint32>::max();
    static CONSTEXPR uint64 MAX_UINT64 = numeric_limits<uint64>::max();

    static CONSTEXPR int8   MAX_INT8  = numeric_limits<int8  >::max();
    static CONSTEXPR int16  MAX_INT16 = numeric_limits<int16 >::max();
    static CONSTEXPR int32  MAX_INT32 = numeric_limits<int32 >::max();
    static CONSTEXPR int64  MAX_INT64 = numeric_limits<int64 >::max();

    static CONSTEXPR SIZE_T MIN_SIZET = numeric_limits<SIZE_T >::min();
    static CONSTEXPR SIZE_T MAX_SIZET = numeric_limits<SIZE_T >::max();

    static CONSTEXPR float  MIN_FLOAT = numeric_limits<float >::min();
    static CONSTEXPR float  MAX_FLOAT = numeric_limits<float >::max();

    static CONSTEXPR float  POS_INFINITY = numeric_limits<float>::infinity();
    static CONSTEXPR float  NEG_INFINITY = -numeric_limits<float>::infinity();

    static CONSTEXPR float FLOAT_EPSILON = numeric_limits<float >::epsilon();
    static CONSTEXPR double DOUBLE_EPSILON = numeric_limits<double>::epsilon();
  };
}
