/*****************************************************************************/
/**
 * @file    geGameConfig.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/30
 * @brief   Config Module for the engine.
 *
 * Config Module for the engine.
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
#include "geModule.h"
#include "gePath.h"

namespace geEngineSDK {
  
  class GE_CORE_EXPORT GameConfig final : public Module<GameConfig>
  {
   public:
    GameConfig() = default;
    ~GameConfig() = default;

    bool
    load(const Path& filePath);

    template <typename T>
    T
    get(const String& section, const String& key, const T& defaultVal) {
      String uppSection = section;
      String uppKey = key;
      StringUtil::toUpperCase(uppSection);
      StringUtil::toUpperCase(uppKey);

      auto sectionIt = m_configData.find(uppSection);
      if (sectionIt != m_configData.end()) {
        auto keyIt = sectionIt->second.find(uppKey);
        if (keyIt != sectionIt->second.end()) {
          StringStream str(keyIt->second);
          T ret = defaultVal;

#if USING(GE_CPP17_OR_LATER)
          if CONSTEXPR (std::is_same_v<T, bool>) {
            str >> std::boolalpha >> ret;
          }
          else {
#endif
            str >> ret;
#if USING(GE_CPP17_OR_LATER)
          }
#endif
          return ret;
        }
      }

      return defaultVal;
    }

#if !USING(GE_CPP17_OR_LATER)
    //Specialization for bool type
    template <>
    bool
    get<bool>(const String& section, const String& key, const bool& defaultVal) {
      String uppSection = section;
      String uppKey = key;
      StringUtil::toUpperCase(uppSection);
      StringUtil::toUpperCase(uppKey);

      auto sectionIt = m_configData.find(uppSection);
      if (sectionIt != m_configData.end()) {
        auto keyIt = sectionIt->second.find(uppKey);
        if (keyIt != sectionIt->second.end()) {
          StringStream str(keyIt->second);
          bool ret = defaultVal;
          str >> std::boolalpha >> ret;
          return ret;
        }
      }

      return defaultVal;
    }
#endif
    
    template <typename T>
    void
    set(const String& section, const String& key, const T& value) {
      String uppSection = section;
      String uppKey = key;
      StringUtil::toUpperCase(uppSection);
      StringUtil::toUpperCase(uppKey);
#if USING(GE_CPP17_OR_LATER)
      if CONSTEXPR(std::is_same_v<T, bool>) {
        m_configData[uppSection][key] = toString(value, false);
      }
      else if CONSTEXPR(std::is_same_v<T, String>) {
        m_configData[uppSection][key] = value;
      }
      else {
#endif
        m_configData[uppSection][key] = toString(value);
#if USING(GE_CPP17_OR_LATER)
      }
#endif
    }

#if !USING(GE_CPP17_OR_LATER)
    //Specialization for bool type
    template <>
    void
    set<bool>(const String& section, const String& key, const bool& value) {
      String uppSection = section;
      String uppKey = key;
      StringUtil::toUpperCase(uppSection);
      StringUtil::toUpperCase(uppKey);
      m_configData[uppSection][key] = toString(value, false);
    }

    //Specialization for String type
    template <>
    void
    set<String>(const String& section, const String& key, const String& value) {
      String uppSection = section;
      String uppKey = key;
      StringUtil::toUpperCase(uppSection);
      StringUtil::toUpperCase(uppKey);
      m_configData[uppSection][key] = value;
    }
#endif

   private:
    /**
     * @brief Container for configuration data.
     *        First key is the section name
     *        second key is the configuration key
     *        and value is the configuration value.
     */
    UnorderedMap<String, UnorderedMap<String, String>> m_configData;
  };
}
