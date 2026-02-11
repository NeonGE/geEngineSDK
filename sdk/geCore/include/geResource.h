/*****************************************************************************/
/**
 * @file    geResource.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/06/26
 * @brief   Resources Interface.
 *
 * Resources Interface. A resource is a piece of data that can be loaded
 * from a file and used by the engine.
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
#include "gePath.h"

namespace geEngineSDK {

  class GE_CORE_EXPORT Resource
  {
   public:
    virtual ~Resource() = default;

    virtual void
    moveFrom(Resource& other){
      GE_UNREFERENCED_PARAMETER(other);
    }

    virtual bool
    load(const Path& filePath) = 0;

    virtual void
    unload() = 0;

    virtual bool
    isLoaded() const = 0;

    virtual const String&
    getName() const = 0;

    virtual SIZE_T
    getMemoryUsage() const = 0;

    void
    setPath(const Path& path) {
      m_path = path;
    }

    void
    setCookedPath(const Path& path) {
      m_cookedPath = path;
    }

    const Path&
    getPath() const {
      return m_path;
    }

    const Path&
    getCookedPath() const {
      return m_cookedPath;
    }

   private:
    Path m_path = Path::BLANK;
    Path m_cookedPath = Path::BLANK;
  };
} // namespace geEngineSDK
