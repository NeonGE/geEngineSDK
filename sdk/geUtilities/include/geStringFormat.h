/*****************************************************************************/
/**
 * @file    geStringFormat.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/05
 * @brief   Helper class used for string formatting operations
 *
 * Helper class used for string formatting operations
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
#include "geNumericLimits.h"

namespace geEngineSDK {
  using std::forward;
  using std::basic_string;
  using std::string;
  using std::wstring;
  using std::to_string;
  using std::to_wstring;
  using std::is_same;

  /**
   * @class StringFormat
   * @brief Helper class used for string formatting operations
   */
  class StringFormat
  {
   private:
    /**
     * @brief Data structure used during string formatting. It holds information
     *        about parameter identifiers to replace with actual parameters.
     */
    struct FormatParamRange
    {
      FormatParamRange() = default;
      FormatParamRange(SIZE_T start, uint32 identifierSize, uint32 paramIdx) :
        m_start(start), m_identifierSize(identifierSize), m_paramIdx(paramIdx) {}

      SIZE_T m_start = 0;
      uint32 m_identifierSize = 0;
      uint32 m_paramIdx = 0;
    };

    /**
     * @brief Structure that holds value of a parameter during string formatting.
     */
    template<class T>
    struct ParamData
    {
      T* m_buffer = nullptr;
      SIZE_T m_size = 0;
    };

   public:
    /**
     * @brief	Formats the provided string by replacing the identifiers with the
     *        provided parameters. The identifiers are represented like "{0}, {1}"
     *        in the source string, where the number represents the position of the
     *        parameter that will be used for replacing the identifier.
     *
     * @note  You may use "\" to escape ID brackets.
     * @note  Maximum ID number is 19 (for a total of 20 unique IDs.
     *        e.g. {20} won't be recognized as an Identifier).
     * @note  Total number of parameters that can be referenced is 200.
     */
    template<class T, class... Args>
    static BasicString<T>
    format(const T* source, Args&&... args) {
      //Get the length of the string
      SIZE_T strLength = getLength(source);

      //Create an array to store the parameters and fill it with the parameters sent
      ParamData<T> parameters[MAX_PARAMS];
      memset(parameters, 0, sizeof(parameters));
      getParams(parameters, 0U, std::forward<Args>(args)...);

      //Brackets characters plus NULL terminator
      T bracketChars[MAX_IDENTIFIER_SIZE + 1];
      uint32 bracketWriteIdx = 0;

      FormatParamRange paramRanges[MAX_PARAM_REFERENCES];
      memset(paramRanges, 0, sizeof(paramRanges));
      SIZE_T paramRangeWriteIdx = 0;

      //Determine parameter positions
      int32 lastBracket = -1;
      bool escaped = false;
      SIZE_T charWriteIdx = 0;
      for (SIZE_T i = 0; i < strLength; ++i) {
        if ('\\' == source[i] && !escaped && MAX_PARAM_REFERENCES > paramRangeWriteIdx) {
          escaped = true;
          //TODO: Test -1 this with PS4 or change for MAX_UINT32
          paramRanges[paramRangeWriteIdx++] = FormatParamRange(charWriteIdx,
                                                               1,
                                                               NumLimit::MAX_UINT32);
          continue;
        }

        if (-1 == lastBracket) {
          //If current char is non-escaped opening bracket start parameter definition
          if ('{' == source[i] && !escaped) {
            lastBracket = static_cast<int32>(i);
          }
          else {
            ++charWriteIdx;
          }
        }
        else {
          if (isdigit(source[i]) && bracketWriteIdx < MAX_IDENTIFIER_SIZE) {
            bracketChars[bracketWriteIdx++] = source[i];
          }
          else {
            //If current char is non-escaped closing bracket end parameter definition
            uint32 numParamChars = bracketWriteIdx;
            bool processedBracket = false;
            if ('}' == source[i] && 0 < numParamChars && !escaped)
            {
              bracketChars[bracketWriteIdx] = '\0';
              uint32 paramIdx = strToInt(bracketChars);

              //Check if exceeded maximum parameter limit
              if (MAX_PARAMS > paramIdx && MAX_PARAM_REFERENCES > paramRangeWriteIdx) {
                paramRanges[paramRangeWriteIdx++] = FormatParamRange(charWriteIdx,
                                                                     numParamChars + 2,
                                                                     paramIdx);
                charWriteIdx += parameters[paramIdx].m_size;
                processedBracket = true;
              }
            }

            if (!processedBracket) {
              //Last bracket wasn't really a parameter
              for (auto j = static_cast<uint32>(lastBracket); j <= i; ++j) {
                ++charWriteIdx;
              }
            }

            lastBracket = -1;
            bracketWriteIdx = 0;
          }
        }

        escaped = false;
      }

      //Copy the clean string into output buffer
      SIZE_T finalStringSize = charWriteIdx;

      auto outputBuffer = reinterpret_cast<T*>(ge_alloc(finalStringSize * sizeof(T)));
      SIZE_T copySourceIdx = 0;
      SIZE_T copyDestIdx = 0;

      for (SIZE_T i = 0; i < paramRangeWriteIdx; ++i) {
        const FormatParamRange& rangeInfo = paramRanges[i];
        SIZE_T copySize = rangeInfo.m_start - copyDestIdx;

        memcpy(outputBuffer + copyDestIdx, source + copySourceIdx, copySize * sizeof(T));
        copySourceIdx += copySize + rangeInfo.m_identifierSize;
        copyDestIdx += copySize;

        if (NumLimit::MAX_UINT32 == rangeInfo.m_paramIdx) {
          continue;
        }

        SIZE_T paramSize = parameters[rangeInfo.m_paramIdx].m_size;
        memcpy(outputBuffer + copyDestIdx,
               parameters[rangeInfo.m_paramIdx].m_buffer,
               paramSize * sizeof(T));
        copyDestIdx += paramSize;
      }

      memcpy(outputBuffer + copyDestIdx, 
             source + copySourceIdx, 
             (finalStringSize - copyDestIdx) * sizeof(T));

      BasicString<T> outputStr(outputBuffer, finalStringSize);
      ge_free(outputBuffer);

      //Free the memory of all the parameters buffers
      for (auto& param : parameters) {
        if (nullptr != param.m_buffer) {
          ge_free(param.m_buffer);
        }
      }

      return outputStr;
    }

   private:
    /**
     * @brief Set of methods that can be specialized so we have a generalized
     *        way for retrieving length of strings of different types.
     */
    static SIZE_T
    getLength(const ANSICHAR* source) {
      return strlen(source);
    }

    /**
     * @brief Set of methods that can be specialized so we have a generalized
     *        way for retrieving length of strings of different types.
     */
    static SIZE_T
    getLength(const UNICHAR* source) {
      return wcslen(source);
    }

    /**
     * @brief Parses the string and returns an integer value extracted from string characters.
     */
    static uint32
    strToInt(const ANSICHAR* buffer) {
      return static_cast<uint32>(strtoul(buffer, nullptr, 10));
    }

    /**
     * @brief Parses the string and returns an integer value extracted from string characters.
     */
    static uint32
    strToInt(const UNICHAR* buffer) {
      return static_cast<uint32>(wcstoul(buffer, nullptr, 10));
    }

    /**
     * @brief Helper method for converting any data type to a narrow string.
     */
    template<class T>
    static String
    toString(const T& param) {
      return geEngineSDK::toString(param);
    }

    /**
     * @brief Helper method that "converts" a narrow string to a narrow string
     *        (simply a pass through).
     */
    static String
    toString(const String& param) {
      return param;
    }

    /**
     * @brief Helper method that converts a narrow character array to a narrow string.
     */
    template<class T>
    static String
    toString(T* /*param*/) {
      static_assert(!is_same<T, T>::value, "Invalid pointer type.");
      return "";
    }

    /**
     * @brief Helper method that converts a narrow character array to a narrow string.
     */
    static String
    toString(const char* param) {
      if (nullptr == param) {
        return String();
      }
      return String(param);
    }

    /**
     * @brief Helper method that converts a narrow character array to a narrow string.
     */
    static String
    toString(char* param) {
      if (nullptr == param) {
        return String();
      }
      return String(param);
    }

    /**
     * @brief Helper method for converting any data type to a wide string.
     */
    template<class T>
    static WString
    toWString(const T& param) {
      return geEngineSDK::toWString(param);
    }

    /**
     * @brief Helper method that "converts" a wide string to a wide string
     *        (simply a pass through).
     */
    static WString
    toWString(const WString& param) {
      return param;
    }

    /**
     * @brief Helper method that converts a wide character array to a wide string.
     */
    template<class T>
    static WString toWString(T* /*param*/) {
      static_assert(!is_same<T, T>::value, "Invalid pointer type.");
      return L"";
    }

    /**
     * @brief Helper method that converts a wide character array to a wide string.
     */
    static WString
    toWString(const wchar_t* param) {
      if (nullptr == param) {
        return WString();
      }
      return WString(param);
    }

    /**
     * @brief Helper method that converts a wide character array to a wide string.
     */
    static WString
    toWString(wchar_t* param) {
      if (nullptr == param) {
        return WString();
      }
      return WString(param);
    }

    /**
     * @brief Converts all the provided parameters into string representations
     *        and populates the provided @p parameters array.
     */
    template<class P, class... Args>
    static void
    getParams(ParamData<char>* parameters, uint32 idx, P&& param, Args&&... args) {
      if (MAX_PARAMS <= idx) {
        return;
      }

      BasicString<char> sourceParam = toString(param);
      parameters[idx].m_buffer = reinterpret_cast<char*>(ge_alloc(sourceParam.size()
                                                                    * sizeof(char)));
      parameters[idx].m_size = sourceParam.size();
      sourceParam.copy(parameters[idx].m_buffer, parameters[idx].m_size, 0);
      getParams(parameters, idx + 1, std::forward<Args>(args)...);
    }

    /**
     * @brief Converts all the provided parameters into string representations
     *        and populates the provided @p parameters array.
     */
    template<class P, class... Args>
    static void
    getParams(ParamData<UNICHAR>* parameters, uint32 idx, P&& param, Args&&... args) {
      if (MAX_PARAMS <= idx) {
        return;
      }

      BasicString<UNICHAR> sourceParam = toWString(param);
      parameters[idx].m_buffer = reinterpret_cast<UNICHAR*>(ge_alloc(sourceParam.size()
                                                                   * sizeof(UNICHAR)));
      parameters[idx].m_size = sourceParam.size();
      sourceParam.copy(parameters[idx].m_buffer, parameters[idx].m_size, 0);
      getParams(parameters, idx + 1, forward<Args>(args)...);
    }

    /**
     * @brief Helper method used for parameter size calculation.
     *        Used as a stopping point in template recursion.
     */
    static void
    getParams(ParamData<ANSICHAR>*, uint32) {
      // Do nothing
    }

    /**
     * @brief Helper method used for parameter size calculation.
     *        Used as a stopping point in template recursion.
     */
    static void
    getParams(ParamData<UNICHAR>*, uint32) {
      // Do nothing
    }

   private:
    static CONSTEXPR const uint32 MAX_PARAMS = 20;
    static CONSTEXPR const uint32 MAX_IDENTIFIER_SIZE = 2;
    static CONSTEXPR const uint32 MAX_PARAM_REFERENCES = 200;
  };
}
