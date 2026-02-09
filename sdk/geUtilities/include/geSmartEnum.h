/*****************************************************************************/
/**
 * @file    geSmartEnum.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2021/10/06
 * @brief   Header to handle enumerators in a way that can be called by name
 *
 * Header to handle enumerators in a way that can be called by name.
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
#include "gePlatformDefines.h"
#include "gePlatformTypes.h"
#include "geStdHeaders.h"
#include "geString.h"
#include "geUtil.h"

namespace geEngineSDK {
  inline UnorderedMap<int32, String>
  makeEnumNameMap(String enumValuesString) {
    int32 currentEnumValue = 0;
    UnorderedMap<int32, String> nameMap;

    auto elements = StringUtil::split(enumValuesString, ",");

    for (const auto& element : elements) {
      auto enumComponents = StringUtil::split(element, "=");
      for (auto& enumComponent : enumComponents) {
        StringUtil::trim(enumComponent);
      }

      GE_ASSERT(!enumComponents.empty() && enumComponents.size() <= 2);

      if (enumComponents.size() == 2) {
        currentEnumValue = parseInt(enumComponents[1]);
      }

      nameMap[currentEnumValue] = enumComponents[0];
      ++currentEnumValue;
    }

    return nameMap;
  }

  template<typename Type>
  Vector<Type> makeEnumList(String enumValuesString) {
    int32 currentEnumValue = 0;
    Vector<Type> enumList;

    auto elements = StringUtil::split(enumValuesString, ",");

    for (const auto& element : elements) {
      auto enumComponents = StringUtil::split(element, "=");
      for (auto& enumComponent : enumComponents) {
        StringUtil::trim(enumComponent);
      }

      GE_ASSERT(!enumComponents.empty() && enumComponents.size() <= 2);

      if (enumComponents.size() == 2) {
        currentEnumValue = parseInt(enumComponents[1]);
      }

      enumList.push_back(static_cast<Type>(currentEnumValue));
      ++currentEnumValue;
    }

    return enumList;
  }

  inline UnorderedMap<String, int32>
  makeEnumValuesMap(String enumValuesString) {
    int32 currentEnumValue = 0;
    UnorderedMap<String, int32> nameMap;

    auto elements = StringUtil::split(enumValuesString, ",");

    for (const auto& element : elements) {
      auto enumComponents = StringUtil::split(element, "=");
      for (auto& enumComponent : enumComponents) {
        StringUtil::trim(enumComponent);
      }

      GE_ASSERT(!enumComponents.empty() && enumComponents.size() <= 2);

      if (enumComponents.size() == 2) {
        currentEnumValue = parseInt(enumComponents[1]);
      }

      nameMap[enumComponents[0]] = currentEnumValue;
      ++currentEnumValue;
    }

    return nameMap;
  }

#define GE_SMART_ENUM(Type, ...)                                              \
  enum Type { __VA_ARGS__};                                                   \
  static UnorderedMap<int32, String>                                          \
    Type##_ENUM_NAMES = makeEnumNameMap(#__VA_ARGS__);                        \
  static Vector<Type>                                                         \
    Type##_LIST = makeEnumList<Type>(#__VA_ARGS__);                           \
  static UnorderedMap<String, int32>                                          \
    Type##_ENUM_VALUES = makeEnumValuesMap(#__VA_ARGS__);                     \
                                                                              \
  inline const String&                                                        \
  Type##_toString(Type value) {                                               \
    return Type##_ENUM_NAMES.at(static_cast<int32>(value));                   \
  }                                                                           \
                                                                              \
  inline const Type                                                           \
  Type##_toEnum(const String& name) {                                         \
    return static_cast<Type>(Type##_ENUM_VALUES.at(name));                    \
  }

#define GE_SMART_ENUM_CLASS(Type, ...)                                        \
  enum class Type { __VA_ARGS__};                                             \
  static UnorderedMap<int32, String>                                          \
    Type##_ENUM_NAMES = makeEnumNameMap(#__VA_ARGS__);                        \
  static Vector<Type>                                                         \
    Type##_LIST = makeEnumList<Type>(#__VA_ARGS__);                           \
  static UnorderedMap<String, int32>                                          \
    Type##_ENUM_VALUES = makeEnumValuesMap(#__VA_ARGS__);                     \
                                                                              \
  inline const String&                                                        \
  Type##_toString(Type value) {                                               \
    return Type##_ENUM_NAMES.at(static_cast<int32>(value));                   \
  }                                                                           \
                                                                              \
  inline const Type                                                           \
  Type##_toEnum(const String& name) {                                         \
    return static_cast<Type>(Type##_ENUM_VALUES.at(name));                    \
  }
}
