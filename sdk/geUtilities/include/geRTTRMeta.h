/*****************************************************************************/
/**
 * @file    geRTTRMeta.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/02/04
 * @brief   MetaData elements for RTTR reflection system.
 *
 * MetaData elements for RTTR reflection system. These are used to annotate
 * reflected types with additional information.
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

#if USING(GE_REFLECTION)
#include <rttr/registration>

namespace geEngineSDK {
  using MetaData = rttr::detail::metadata;

  enum class MetaData_Type : uint32 {
    SCRIPTABLE = 1,   //Scriptable element (exposed to scripting languages).
    STATIC_MEMBER,    //The element is a static member.
    OPERATOR,         //The element is an operator overload.
    CATEGORY,         //Category name for grouping in editors.
    DISPLAY_NAME,     //Display name for editors.
    TOOLTIP,          //Tooltip description for editors.
    DEFAULT_VALUE,    //Default value for properties.
    MIN_VALUE,        //Minimum value for properties.
    MAX_VALUE,        //Maximum value for properties.
    READ_ONLY         //The property is read-only.
  };

  template<typename T>
  inline MetaData
  metaData(MetaData_Type type, T v) {
    return rttr::metadata(type, v);
  }

  inline MetaData
  metaScriptable(bool v = true) {
    return metaData(MetaData_Type::SCRIPTABLE, v);
  }

  inline MetaData
  metaCategory(const char* cat) {
    return metaData(MetaData_Type::CATEGORY, String(cat));
  }

  inline MetaData
  metaTooltip(const char* tip) {
    return metaData(MetaData_Type::TOOLTIP, String(tip));
  }

  inline MetaData
  metaReadOnly(bool v = true) {
    return metaData(MetaData_Type::READ_ONLY, v);
  }

}

#endif //GE_REFLECTION
