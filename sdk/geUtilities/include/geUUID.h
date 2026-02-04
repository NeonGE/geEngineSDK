/*****************************************************************************/
/**
 * @file    geUUID.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/29
 * @brief   Class that represents a universally unique identifier.
 *
 * Class that represents a universally unique identifier.
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
#include "geString.h"
#include "geUtil.h"

namespace geEngineSDK {
  class Path;

  /**
   * @brief Represents a universally unique identifier.
   */
  class GE_UTILITIES_EXPORT UUID
  {
   public:
    /**
     * @brief Initializes an empty UUID.
     */
    CONSTEXPR UUID() = default;

    /**
     * @brief Initializes an UUID using geEngine's UUID representation.
     */
    CONSTEXPR UUID(uint32 data1, uint32 data2, uint32 data3, uint32 data4)
      : m_data{ data1, data2, data3, data4 }
    {}

    /**
     * @brief Initializes an UUID using its string representation.
     */
    explicit UUID(const String& uuid);

    explicit UUID(const Path& filePath);

    CONSTEXPR bool
    operator==(const UUID& rhs) const {
      return m_data[0] == rhs.m_data[0] &&
             m_data[1] == rhs.m_data[1] &&
             m_data[2] == rhs.m_data[2] &&
             m_data[3] == rhs.m_data[3];
    }

    CONSTEXPR bool
    operator!=(const UUID& rhs) const {
      return !(*this == rhs);
    }

    CONSTEXPR bool
    operator<(const UUID& rhs) const {
      for (uint32 i = 0; i < 4; ++i) {
        if (m_data[i] < rhs.m_data[i]) {
          return true;
        }
        else if (m_data[i] > rhs.m_data[i]) {
          return false;
        }
        //Move to next element if equal
      }

      //They're equal
      return false;
    }

    /**
     * @brief Checks has the UUID been initialized to a valid value.
     */
    CONSTEXPR bool
    empty() const {
      return m_data[0] == 0 &&
             m_data[1] == 0 &&
             m_data[2] == 0 &&
             m_data[3] == 0;
    }

    /**
     * @brief Converts the UUID into its string representation.
     */
    String
    toString() const;

    static UUID EMPTY;
   
   private:
    friend struct std::hash<UUID>;
    uint32 m_data[4] = { 0,0,0,0 };
  };

    /**
    * @brief Utility class for generating universally unique identifiers.
    *
    * @note	Thread safe.
    */
  class GE_UTILITIES_EXPORT UUIDGenerator
  {
   public:
    /**
     * @brief Generate a new random universally unique identifier.
     */
    static UUID
    generateRandom();
  };
}

namespace std {
  /**
   * @brief Hash value generator for UUID.
   */
  template<>
  struct hash<geEngineSDK::UUID>
  {
    size_t
    operator()(const geEngineSDK::UUID& value) const {
      size_t hashVal = 0;
      geEngineSDK::ge_hash_combine(hashVal, value.m_data[0]);
      geEngineSDK::ge_hash_combine(hashVal, value.m_data[1]);
      geEngineSDK::ge_hash_combine(hashVal, value.m_data[2]);
      geEngineSDK::ge_hash_combine(hashVal, value.m_data[3]);
      return hashVal;
    }
  };
}
