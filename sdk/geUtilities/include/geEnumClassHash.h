/*****************************************************************************/
/**
* @file    geEnumClassHash.h
* @author  Samuel Prince (samuel.prince.quezada@gmail.com)
* @date    2017/05/15
* @brief   Hash for enum types, to be used instead of hash<T> when T is an enum
*
* Until C++14, std::hash<T> is not defined if T is a enum (see
* http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#2148).  But
* even with C++14, as of october 2016, std::hash for enums is not widely
* implemented by compilers, so here when T is a enum, we use EnumClassHash
* instead of std::hash. (For instance, in geEngineSDK::hash_combine(), or
* geEngineSDK::UnorderedMap.)
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

namespace geEngineSDK {
  struct EnumClassHash
  {
    template<typename T>
    CONSTEXPR std::size_t
    operator()(T t) const {
      return static_cast<std::size_t>(t);
    }
  };
}
