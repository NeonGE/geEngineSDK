/*****************************************************************************/
/**
 * @file    geGameConfig.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/29
 * @brief   Config Module for the engine.
 *
 * Config Module for the engine.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geGameConfig.h"
#include <geMountManager.h>

namespace geEngineSDK {
  bool
  GameConfig::load(const Path& filePath) {
    //Load the file and parse it in lines, remove the empty lines and comments
    auto pFile = MountManager::instance().open(filePath);
    if (!pFile) {
      return false;
    }
    auto fileContent = pFile->getAsString();

    auto splitLines = StringUtil::split(fileContent, "\n");
    String currentSection = "GLOBAL";
    for (auto& line : splitLines) {
      StringUtil::trim(line);
      if (line.empty() || line.starts_with("#")) {
        continue;
      }

      if (line.starts_with("[") && line.ends_with("]")) {
        currentSection = line.substr(1, line.size() - 2);
        StringUtil::trim(currentSection);
        StringUtil::toUpperCase(currentSection);
        continue;
      }

      auto keyValue = StringUtil::split(line, "=");
      if (keyValue.size() == 2) {
        StringUtil::trim(keyValue[0]);
        StringUtil::trim(keyValue[1]);
        StringUtil::toUpperCase(keyValue[0]);
        m_configData[currentSection][keyValue[0]] = keyValue[1];
      }
    }
    return true;
  }

}
