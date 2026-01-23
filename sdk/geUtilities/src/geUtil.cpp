/*****************************************************************************/
/**
 * @file    geUtil.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/06
 * @brief
 *
 * Long Description
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "externals/md5.h"

namespace geEngineSDK {
  String
  md5(const WString& source) {
    MD5 md5;
    SIZE_T dataSize = source.length() * sizeof(WString::value_type);
    md5.update(reinterpret_cast<uint8*>(const_cast<UNICHAR*>(source.data())),
               static_cast<uint32>(dataSize));
    md5.finalize();

    uint8 digest[16];
    md5.decdigest(digest, sizeof(digest));

    String buf;
    buf.resize(32);
    for (uint32 i = 0; i < 16; ++i) {
      sprintf(&(buf[0]) + i * 2, "%02x", static_cast<uint32>(digest[i]));
    }

    return buf;
  }

  String
  md5(const String& source) {
    MD5 md5;
    SIZE_T dataSize = source.length() * sizeof(String::value_type);
    md5.update(reinterpret_cast<uint8*>(const_cast<ANSICHAR*>(source.data())),
               static_cast<uint32>(dataSize));
    md5.finalize();

    uint8 digest[16];
    md5.decdigest(digest, sizeof(digest));

    String buf;
    buf.resize(32);
    for (uint32 i = 0; i < 16; ++i) {
      sprintf(&(buf[0]) + i * 2, "%02x", static_cast<uint32>(digest[i]));
    }

    return buf;
  }
}
