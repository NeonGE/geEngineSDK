/*****************************************************************************/
/**
 * @file    geUnicode.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/14
 * @brief   Utilities to convert between UTF-8 encoding and other encodings.
 *
 * Provides methods to converting between UTF-8 character encoding and other
 * popular encodings.
 *
 * @bug     No known bugs.
 * 
 * @update  2025/01/23 Samuel Prince - Fixed issues and compilation for POSIX.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geUnicode.h"

#include <iterator>

namespace geEngineSDK {
  using std::random_access_iterator;
  using std::output_iterator;

  /**
   * @brief Converts an UTF-8 encoded character (possibly multibyte)
   *        into an UTF-32 character.
   */
  template<typename T>
  T
  UTF8To32(T begin, T end, char32_t& output, char32_t invalidChar = 0) {
#if USING(GE_CPP20_OR_LATER)
    static_assert(random_access_iterator<T>,
                  "UTF8To32 requires a random access iterator.");
#endif

    if (begin >= end) {
      return begin;
    }

    auto avail = cast::st<size_t>(end - begin);
    uint8 b0 = cast::st<uint8>(*begin);

    //1-byte (ASCII)
    if (b0 <= 0x7F) {
      output = static_cast<char32_t>(b0);
      return begin + 1;
    }

    //First byte cannot be a continuation byte
    if (b0 < 0xC2) { // 0x80..0xBF continuation, 0xC0..0xC1 overlong starters
      output = invalidChar;
      return begin + 1;
    }

    //Determine length and minimal checks
    uint32 numBytes = 0;
    char32_t cp = 0;

    if (b0 <= 0xDF) {
      numBytes = 2; cp = (b0 & 0x1F);
    }
    else if (b0 <= 0xEF) {
      numBytes = 3; cp = (b0 & 0x0F);
    }
    else if (b0 <= 0xF4) {
      numBytes = 4; cp = (b0 & 0x07); //0xF5..0xFF invalid
    }
    else {
      output = invalidChar;
      return begin + 1;
    }

    if (avail < numBytes) {
      output = invalidChar;
      return end;
    }

    // Validate and accumulate continuation bytes
    for (uint32 i = 1; i < numBytes; ++i) {
      uint8 bx = cast::st<uint8>(begin[i]);
      if ((bx & 0xC0) != 0x80) {
        output = invalidChar;
        return begin + i; // stop at first invalid byte
      }
      cp = (cp << 6) | (bx & 0x3F);
    }

    // Reject overlongs + invalid ranges with UTF-8 specific constraints
    if (numBytes == 2) {
      // already ensured b0 >= 0xC2 so no overlong
    }
    else if (numBytes == 3) {
      //overlong
      uint8 b1 = cast::st<uint8>(begin[1]);
      if (b0 == 0xE0 && b1 < 0xA0) {
        output = invalidChar;
        return begin + 1;
      }
      if (b0 == 0xED && b1 >= 0xA0) {
        //surrogate
        output = invalidChar;
        return begin + 1;
      }
    }
    else {
      //numBytes == 4
      uint8 b1 = cast::st<uint8>(begin[1]);
      if (b0 == 0xF0 && b1 < 0x90) {
        //overlong
        output = invalidChar; return begin + 1;
      }
      if (b0 == 0xF4 && b1 > 0x8F) {
        // > 0x10FFFF
        output = invalidChar; return begin + 1;
      }
    }

    //Final range checks
    if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) {
      output = invalidChar;
      return begin + 1;
    }

    output = cp;
    return begin + numBytes;
  }

  /**
   * @brief Converts an UTF-32 encoded character into an (possibly multibyte) UTF-8 character.
   */
  template<typename T>
  T
  UTF32To8(char32_t input, T output, uint32 maxElems, char invalidChar = 0) {
#if USING(GE_CPP20_OR_LATER)
    static_assert(output_iterator<T, char>, 
                  "UTF32To8 requires an output iterator of char.");
#endif

    if (maxElems == 0) {
      return output;
    }

    //UTF-32 válido: <= 0x10FFFF y NO surrogate range
    if (input > 0x0010FFFF || (input >= 0xD800 && input <= 0xDFFF)) {
      *output++ = invalidChar;
      return output;
    }

    uint32 numBytes = 0;
    if (input < 0x80) {
      numBytes = 1;
    }
    else if (input < 0x800) {
      numBytes = 2;
    }
    else if (input < 0x10000) {
      numBytes = 3;
    }
    else {
      numBytes = 4;
    }

    if (numBytes > maxElems) {
      *output++ = invalidChar;
      return output;
    }

    constexpr uint8 headers[5] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0 };

    char bytes[4];

    //We need a temporary variable since input is const
    char32_t v = input;

    switch (numBytes)
    {
      case 4:
        bytes[3] = cast::st<char>((v & 0x3F) | 0x80);
        v >>= 6;
        GE_FALLTHROUGH;

      case 3:
        bytes[2] = cast::st<char>((v & 0x3F) | 0x80);
        v >>= 6;
        GE_FALLTHROUGH;

      case 2:
        bytes[1] = cast::st<char>((v & 0x3F) | 0x80);
        v >>= 6;
        GE_FALLTHROUGH;

      case 1:
        bytes[0] = cast::st<char>(v | headers[numBytes]);
        break;

      default:
        *output++ = invalidChar;
        return output;
    }

    return std::copy(bytes, bytes + numBytes, output);
  }

  /**
   * @brief Converts an UTF-16 encoded character into an UTF-32 character.
   */
  template<typename T>
  T
  UTF16To32(T begin, T end, char32_t& output, char32_t invalidChar = 0) {
    //Nothing to parse
    if (begin >= end) {
      return begin;
    }

    char16_t firstElem = static_cast<char16_t>(*begin);
    ++begin;

    //Check if it's a surrogate pair
    if ((0xD800 <= firstElem) && (0xDBFF >= firstElem)) {
      //Invalid character
      if (begin >= end) {
        output = invalidChar;
        return end;
      }

      char32_t secondElem = static_cast<char32_t>(*begin);
      ++begin;

      if ((0xDC00 <= secondElem) && (0xDFFF >= secondElem)) {
        output = static_cast<char32_t>(((firstElem - 0xD800) << 10) +
                                        (secondElem - 0xDC00) + 0x0010000);
      }
      else {// Invalid character
        output = invalidChar;
      }
    }
    else {
      output = static_cast<char32_t>(firstElem);
      return begin;
    }

    return begin;
  }

  /**
   * @brief Converts an UTF-32 encoded character into an UTF-16 character.
   */
  template<typename T>
  T
  UTF32To16(char32_t input, T output, uint32 maxElems, char16_t invalidChar = 0) {
    //No place to write the character
    if (0 == maxElems) {
      return output;
    }

    //Invalid character
    if (0x0010FFFF < input) {
      *output = invalidChar;
      ++output;
      return output;
    }

    //Can be encoded as a single element
    if (0xFFFF >= input) {
      //Check if in valid range
      if ((0xD800 <= input) && (0xDFFF >= input)) {
        *output = invalidChar;
        ++output;
        return output;
      }

      *output = static_cast<char16_t>(input);
      ++output;
    }
    else {  //Must be encoded as two elements
      //Two elements won't fit
      if (2 > maxElems) {
        *output = invalidChar;
        ++output;
        return output;
      }

      input -= 0x0010000;

      *output = static_cast<char16_t>((input >> 10) + 0xD800);
      ++output;

      *output = static_cast<char16_t>((input & 0x3FFUL) + 0xDC00);
      ++output;
    }

    return output;
  }

  template<typename T>
  T
  wideToUTF32(T begin, T end, char32_t& output, char32_t invalidChar = 0) {
    //Assuming UTF-32 (i.e. Unix)
    SIZE_T sizeofWChar = sizeof(wchar_t);
    if (4 == sizeofWChar) {
      output = (char32_t)*begin;
      ++begin;

      return begin;
    }
    
    //Assuming UTF-16 (i.e. Windows)
    return UTF16To32(begin, end, output, invalidChar);
  }

  char32_t
  ANSIToUTF32(char input, const std::locale& locale = std::locale("")) {
    const std::ctype<wchar_t>& facet = std::use_facet<std::ctype<wchar_t>>(locale);

    /**
     * Note: Not exactly valid on Windows, since the input character could
     * require a surrogate pair. Consider improving this if it ever becomes an issue.
     */
    wchar_t wideChar = facet.widen(input);

    char32_t output;
    wideToUTF32(&wideChar, &wideChar + 1, output);

    return output;
  }

  template<typename T>
  T
  UTF32ToWide(char32_t input, T output, uint32 maxElems, wchar_t invalidChar = 0) {
    //Assuming UTF-32 (i.e. Unix)
    SIZE_T sizeofWChar = sizeof(wchar_t);
    if (4 == sizeofWChar) {
      *output = (wchar_t)input;
      ++output;
      return output;
    }
    
    //Assuming UTF-16 (i.e. Windows)
    return UTF32To16(input, output, maxElems, invalidChar);
  }

  char
  UTF32ToANSI(char32_t input,
              char invalidChar = 0,
              const std::locale& locale = std::locale("")) {
    const std::ctype<wchar_t>& facet = std::use_facet<std::ctype<wchar_t>>(locale);

    //Note: Same as above, not exactly correct as narrow() doesn't accept a surrogate pair
    return facet.narrow((wchar_t)input, invalidChar);
  }

  String
  UTF8::fromANSI(const String& input, const std::locale& locale) {
    String output;
    output.reserve(input.size());

    auto backInserter = std::back_inserter(output);

    auto iter = input.begin();
    while (iter != input.end()) {
      char32_t u32char = ANSIToUTF32(*iter, locale);
      UTF32To8(u32char, backInserter, 4);
      ++iter;
    }

    return output;
  }

  String
  UTF8::toANSI(const String& input, const std::locale& locale, char invalidChar) {
    String output;

    auto iter = input.begin();
    while (iter != input.end()) {
      char32_t u32char = 0;
      iter = UTF8To32(iter, input.end(), u32char, invalidChar);
      output.push_back(UTF32ToANSI(u32char, invalidChar, locale));
    }

    return output;
  }

  String
  UTF8::fromWide(const WString& input) {
    String output;
    output.reserve(input.size());

    auto backInserter = std::back_inserter(output);

    auto iter = input.begin();
    while (iter != input.end()) {
      char32_t u32char = 0;
      iter = wideToUTF32(iter, input.end(), u32char);
      UTF32To8(u32char, backInserter, 4);
    }

    return output;
  }

  WString
  UTF8::toWide(const String& input) {
    WString output;
    auto backInserter = std::back_inserter(output);

    auto iter = input.begin();
    while (iter != input.end()) {
      char32_t u32char = 0;
      iter = UTF8To32(iter, input.end(), u32char);
      UTF32ToWide(u32char, backInserter, 2);
    }

    return output;
  }

  String
  UTF8::fromUTF16(const U16String& input) {
    String output;
    output.reserve(input.size());

    auto backInserter = std::back_inserter(output);

    auto iter = input.begin();
    while (iter != input.end()) {
      char32_t u32char = 0;
      iter = UTF16To32(iter, input.end(), u32char);
      UTF32To8(u32char, backInserter, 4);
    }

    return output;
  }

  U16String
  UTF8::toUTF16(const String& input) {
    U16String output;
    auto backInserter = std::back_inserter(output);

    auto iter = input.begin();
    while (iter != input.end()) {
      char32_t u32char = 0;
      iter = UTF8To32(iter, input.end(), u32char);
      UTF32To16(u32char, backInserter, 2);
    }

    return output;
  }

  String
  UTF8::fromUTF32(const U32String& input) {
    String output;
    output.reserve(input.size());

    auto backInserter = std::back_inserter(output);

    auto iter = input.begin();
    while (iter != input.end()) {
      UTF32To8(*iter, backInserter, 4);
      ++iter;
    }

    return output;
  }

  U32String
  UTF8::toUTF32(const String& input) {
    U32String output;

    auto iter = input.begin();
    while (iter != input.end()) {
      char32_t u32char;
      iter = UTF8To32(iter, input.end(), u32char);
      output.push_back(u32char);
    }

    return output;
  }

  SIZE_T
  UTF8::count(const String& input) {
    SIZE_T length = 0;
    for (ANSICHAR c : input) {
      const uint8 b = static_cast<uint8>(c);
      length += (b & 0xC0) != 0x80;
    }
    return length;
  }

  SIZE_T
  UTF8::charToByteIndex(const String& input, SIZE_T charIdx) {
    SIZE_T curChar = 0;
    SIZE_T curByte = 0;

    for (ANSICHAR c : input) {
      const uint8 b = static_cast<uint8>(c);
      if ((b & 0xC0) != 0x80) {
        if (curChar == charIdx) {
          return curByte;
        }
        ++curChar;
      }
      ++curByte;
    }

    return input.size();
  }

  SIZE_T
  UTF8::charByteCount(const String& input, SIZE_T charIdx) {
    const SIZE_T byteIdx = charToByteIndex(input, charIdx);

    SIZE_T countBytes = 1;
    for (auto i = byteIdx + 1; i < input.size(); ++i) {
      const uint8 b = cast::st<uint8>(input[i]);
      if ((b & 0xC0) != 0x80) {
        break;
      }
      ++countBytes;
    }
    return countBytes;
  }

  String
  UTF8::toLower(const String& input) {
    return PlatformUtility::convertCaseUTF8(input, false);
  }

  String
  UTF8::toUpper(const String& input) {
    return PlatformUtility::convertCaseUTF8(input, true);
  }
}
