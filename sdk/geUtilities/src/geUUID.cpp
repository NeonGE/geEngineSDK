/*****************************************************************************/
/**
 * @file    geUUID.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/29
 * @brief   Class that represents a universally unique identifier.
 *
 * Class that represents a universally unique identifier.
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
#include "geUUID.h"
#include "gePlatformUtility.h"
#include "gePath.h"
#include "geFileSystem.h"

using std::hex;
using std::setw;
using std::setfill;

namespace {
  CONSTEXPR const geEngineSDK::ANSICHAR HEX_TO_LITERAL[16] = {
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    'a',
    'b',
    'c',
    'd',
    'e',
    'f'
  };

  CONSTEXPR const geEngineSDK::uint8 LITERAL_TO_HEX[256] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    //0 through 9 translate to 0 though 9
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    //A through F translate to 10 though 15
    0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    //a through f translate to 10 though 15
    0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
  };
}

namespace geEngineSDK {
  UUID UUID::EMPTY;

  UUID::UUID(const String& uuid) {
    memset(m_data, 0, sizeof(m_data));

    if (uuid.size() < 36) {
      return;
    }

    uint32 idx = 0;

    //First group: 8 digits
    for (int32 i = 7; i >= 0; --i) {
      char charVal = uuid[idx++];
      uint8 hexVal = LITERAL_TO_HEX[static_cast<int>(charVal)];
      m_data[0] |= hexVal << (i * 4);
    }

    ++idx;

    //Second group: 4 digits
    for (int32 i = 7; i >= 4; --i) {
      char charVal = uuid[idx++];
      uint8 hexVal = LITERAL_TO_HEX[static_cast<int>(charVal)];
      m_data[1] |= hexVal << (i * 4);
    }

    ++idx;

    //Third group: 4 digits
    for (int32 i = 3; i >= 0; --i) {
      char charVal = uuid[idx++];
      uint8 hexVal = LITERAL_TO_HEX[static_cast<int>(charVal)];
      m_data[1] |= hexVal << (i * 4);
    }

    ++idx;

    //Fourth group: 4 digits
    for (int32 i = 7; i >= 4; --i) {
      char charVal = uuid[idx++];
      uint8 hexVal = LITERAL_TO_HEX[static_cast<int>(charVal)];
      m_data[2] |= hexVal << (i * 4);
    }

    ++idx;

    //Fifth group: 12 digits
    for (int32 i = 3; i >= 0; --i) {
      char charVal = uuid[idx++];
      uint8 hexVal = LITERAL_TO_HEX[static_cast<int>(charVal)];
      m_data[2] |= hexVal << (i * 4);
    }

    for (int32 i = 7; i >= 0; --i) {
      char charVal = uuid[idx++];
      uint8 hexVal = LITERAL_TO_HEX[static_cast<int>(charVal)];
      m_data[3] |= hexVal << (i * 4);
    }
  }

  UUID::UUID(const Path& filePath) {
    StringStream saltStream;
    saltStream << hex << FileSystem::getLastModifiedTime(filePath);

    //Hash the file path
    hash<String> hashFn;
    String filePathStr = filePath.toString();
#if USING(GE_ARCHITECTURE_x86_64) || USING(GE_ARCHITECTURE_ARM_64)
    uint64 hashValue1 = hashFn(filePathStr);
    uint64 hashValue2 = hashFn(filePathStr + saltStream.str());
#else
    uint64 hashValue1 = static_cast<uint64>(hashFn(filePathStr));
    uint64 hashValue2 = static_cast<uint64>(hashFn(filePathStr + saltStream.str()));
#endif

    //Convert hash value to hex string (UUID-like format)
    StringStream ss;
    ss << hex << setw(8) << setfill('0') << (hashValue1 >> 32);
    ss << "-" << setw(4) << setfill('0') << ((hashValue1 >> 16) & 0xFFFF);
    ss << "-" << setw(4) << setfill('0') << (hashValue1 & 0xFFFF);
    ss << "-" << setw(4) << setfill('0') << ((hashValue2 >> 48) & 0xFFFF);
    ss << "-" << setw(12) << setfill('0') << (hashValue2 & 0xFFFFFFFFFFFF);

    //Format the hex string as a UUID (optional)
    String uuid = ss.str();
    *this = UUID(uuid);
  }

  String
  UUID::toString() const {
    ANSICHAR output[36];
    uint32 idx = 0;

    auto emit32 = [&](uint32 v, int32 from, int32 to)
    {
      for (int32 i = from; i >= to; --i) {
        const uint32 hexVal = (v >> (i * 4)) & 0xFu;
        output[idx++] = HEX_TO_LITERAL[hexVal];
      }
    };

    emit32(static_cast<uint32>(m_data[0]), 7, 0);
    output[idx++] = '-';

    emit32(static_cast<uint32>(m_data[1]), 7, 4);
    output[idx++] = '-';

    emit32(static_cast<uint32>(m_data[1]), 3, 0);
    output[idx++] = '-';

    emit32(static_cast<uint32>(m_data[2]), 7, 4);
    output[idx++] = '-';

    emit32(static_cast<uint32>(m_data[2]), 3, 0);
    emit32(static_cast<uint32>(m_data[3]), 7, 0);

    GE_ASSERT(idx == 36);
    return String(output, 36);
  }

  UUID
  UUIDGenerator::generateRandom() {
    return PlatformUtility::generateUUID();
  }
}
