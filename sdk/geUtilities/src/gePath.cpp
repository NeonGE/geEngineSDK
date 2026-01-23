/*****************************************************************************/
/**
* @file   gePath.cpp
* @author Samuel Prince (samuel.prince.quezada@gmail.com)
* @date   2016/03/06
* @brief  Class for storing and manipulating file paths
*
* Class for storing and manipulating file paths. Paths may be
* parsed from and to raw strings according to various platform
* specific path types.
*
* @bug	   No known bugs.
*/
/*****************************************************************************/

/*****************************************************************************/
/**
* Includes
*/
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geException.h"
#include "geDebug.h"
#include "geUnicode.h"

namespace geEngineSDK {
  const Path Path::BLANK = Path();

  Path::Path(const ANSICHAR* pathStr, PATH_TYPE::E type) {
    assign(pathStr, type);
  }

  Path::Path(const String& pathStr, PATH_TYPE::E type) {
    assign(pathStr, type);
  }

  Path::Path(const Path& other) {
    assign(other);
  }

  Path& Path::operator=(const Path& path) {
    assign(path);
    return *this;
  }

  Path& Path::operator=(const String& pathStr) {
    assign(pathStr);
    return *this;
  }

  Path& Path::operator=(const ANSICHAR* pathStr) {
    assign(pathStr);
    return *this;
  }

  void
  Path::swap(Path& path) {
    std::swap(m_directories, path.m_directories);
    std::swap(m_filename, path.m_filename);
    std::swap(m_device, path.m_device);
    std::swap(m_node, path.m_node);
    std::swap(m_isAbsolute, path.m_isAbsolute);
  }

  void
  Path::assign(const Path& path) {
    m_directories = path.m_directories;
    m_filename = path.m_filename;
    m_device = path.m_device;
    m_node = path.m_node;
    m_isAbsolute = path.m_isAbsolute;
  }

  void
  Path::assign(const ANSICHAR* pathStr, PATH_TYPE::E type) {
    assign(pathStr, strlen(pathStr), type);
  }

  void
  Path::assign(const String& pathStr, PATH_TYPE::E type) {
    assign(pathStr.data(), pathStr.length(), type);
  }

  void
  Path::assign(const ANSICHAR* pathStr, SIZE_T numChars, PATH_TYPE::E type) {
    switch (type)
    {
      case PATH_TYPE::kWindows:
        parseWindows(pathStr, numChars);
        break;
      case PATH_TYPE::kUnix:
        parseUnix(pathStr, numChars);
        break;
      case PATH_TYPE::kDefault:
      default:
#if USING(GE_PLATFORM_WINDOWS)
        parseWindows(pathStr, numChars);
#elif USING(GE_PLATFORM_OSX) || USING(GE_PLATFORM_LINUX) || \
      USING(GE_PLATFORM_PS4) || USING(GE_PLATFORM_PS5)
        //TODO: Test parsing with PS4
        parseUnix(pathStr, numChars);
#else
        static_assert(false, "Unsupported platform for path.");
#endif
        break;
    }
  }

#if USING(GE_PLATFORM_WINDOWS)
  WString
  Path::toPlatformString() const {
    return UTF8::toWide(toString());
  }
#endif

  String
  Path::toString(PATH_TYPE::E type) const {
    switch (type)
    {
      case PATH_TYPE::kWindows:
        return buildWindows();
      case PATH_TYPE::kUnix:
        return buildUnix();
      case PATH_TYPE::kDefault:
      default:
#if USING(GE_PLATFORM_WINDOWS)
        return buildWindows();
#elif USING(GE_PLATFORM_OSX) || USING(GE_PLATFORM_LINUX) || USING(GE_PLATFORM_PS4) || USING(GE_PLATFORM_PS5)
        return buildUnix();
#else
        static_assert(false, "Unsupported platform for path.");
#endif
        break;
    }
  }

  Path
  Path::getParent() const {
    Path copy = *this;
    copy.makeParent();
    return copy;
  }

  Path
  Path::getAbsolute(const Path& base) const {
    Path copy = *this;
    copy.makeAbsolute(base);
    return copy;
  }

  Path
  Path::getRelative(const Path& base) const {
    Path copy = *this;
    copy.makeRelative(base);
    return copy;
  }

  Path
  Path::getDirectory() const {
    Path copy = *this;
    copy.m_filename.clear();
    return copy;
  }

  Path&
  Path::makeParent() {
    if (m_filename.empty()) {
      if (m_directories.empty()) {
        if (!m_isAbsolute) {
          m_directories.emplace_back("..");
        }
      }
      else {
        if (".." == m_directories.back()) {
          m_directories.emplace_back("..");
        }
        else {
          m_directories.pop_back();
        }
      }
    }
    else {
      m_filename.clear();
    }

    return *this;
  }

  Path&
  Path::makeAbsolute(const Path& base) {
    if (m_isAbsolute) {
      return *this;
    }

    Path absDir = base.getDirectory();
    if (base.isFile()) {
      absDir.pushDirectory(base.m_filename);
    }

    for (auto& dir : m_directories) {
      absDir.pushDirectory(dir);
    }

    absDir.setFilename(m_filename);
    *this = absDir;
    return *this;
  }

  Path&
  Path::makeRelative(const Path& base) {
    if (!base.includes(*this)) {
      return *this;
    }

    m_directories.erase(m_directories.begin(), 
                        m_directories.begin() + base.m_directories.size());

    /**
     * Sometimes a directory name can be interpreted as a file and we're okay with that.
     * Check for that special case.
     */
    if (base.isFile()) {
      if (!m_directories.empty()) {
        m_directories.erase(m_directories.begin());
      }
      else {
        m_filename = "";
      }
    }

    m_device = "";
    m_node = "";
    m_isAbsolute = false;
    return *this;
  }

  bool
  Path::includes(const Path& child) const {
    if (m_device != child.m_device) {
      return false;
    }

    if (m_node != child.m_node) {
      return false;
    }

    auto iterParent = m_directories.begin();
    auto iterChild = child.m_directories.begin();

    for (; iterParent != m_directories.end(); ++iterChild, ++iterParent) {
      if (iterChild == child.m_directories.end()) {
        return false;
      }

      if (!comparePathElem(*iterChild, *iterParent)) {
        return false;
      }
    }

    if (!m_filename.empty()) {
      if (iterChild == child.m_directories.end()) {
        if (child.m_filename.empty()) {
          return false;
        }

        if (!comparePathElem(child.m_filename, m_filename)) {
          return false;
        }
      }
      else {
        if (!comparePathElem(*iterChild, m_filename)) {
          return false;
        }
      }
    }

    return true;
  }

  bool
  Path::equals(const Path& other) const {
    if (m_isAbsolute != other.m_isAbsolute) {
      return false;
    }

    if (m_isAbsolute) {
      if (!comparePathElem(m_device, other.m_device)) {
        return false;
      }
    }

    SIZE_T myNumElements = m_directories.size();
    SIZE_T otherNumElements = other.m_directories.size();

    if (!m_filename.empty()) {
      ++myNumElements;
    }

    if (!other.m_filename.empty()) {
      ++otherNumElements;
    }

    if (myNumElements != otherNumElements) {
      return false;
    }
    
    if (0 < myNumElements) {
      auto iterMe = m_directories.begin();
      auto iterOther = other.m_directories.begin();

      for (SIZE_T i = 0; i < (myNumElements - 1); ++i, ++iterMe, ++iterOther) {
        if (!comparePathElem(*iterMe, *iterOther)) {
          return false;
        }
      }

      if (!m_filename.empty()) {
        if (!other.m_filename.empty()) {
          if (!comparePathElem(m_filename, other.m_filename)) {
            return false;
          }
        }
        else {
          if (!comparePathElem(m_filename, *iterOther)) {
            return false;
          }
        }
      }
      else {
        if (!other.m_filename.empty()) {
          if (!comparePathElem(*iterMe, other.m_filename)) {
            return false;
          }
        }
        else {
          if (!comparePathElem(*iterMe, *iterOther)) {
            return false;
          }
        }
      }
    }

    return true;
  }

  Path&
  Path::append(const Path& path) {
    if (!m_filename.empty()) {
      pushDirectory(m_filename);
    }

    for (auto& dir : path.m_directories) {
      pushDirectory(dir);
    }

    m_filename = path.m_filename;
    return *this;
  }

  void
  Path::setBasename(const String& basename) {
    m_filename = basename + getExtension();
  }

  void
  Path::setExtension(const String& extension) {
    StringStream stream;
    stream << getFilename(false);
    stream << extension;
    m_filename = stream.str();
  }

  String
  Path::getFilename(bool extension) const {
    if (extension) {
      return m_filename;
    }
    else {
      WString::size_type pos = m_filename.rfind(L'.');
      if (pos != WString::npos) {
        return m_filename.substr(0, pos);
      }
      else {
        return m_filename;
      }
    }
  }

  String
  Path::getExtension() const {
    WString::size_type pos = m_filename.rfind(L'.');
    if (pos != WString::npos)
      return m_filename.substr(pos);
    else
      return String();
  }

  const String&
  Path::getDirectory(SIZE_T idx) const {
    if (idx >= m_directories.size()) {
      GE_EXCEPT(InvalidParametersException,
                "Index out of range: " + 
                geEngineSDK::toString(static_cast<uint32>(idx)) +
                ". Valid range: [0, " + 
                geEngineSDK::toString(static_cast<uint32>(m_directories.size() - 1)) +
                "]");
    }
    return m_directories[idx];
  }

  const String&
  Path::getTail() const {
    if (isFile()) {
      return m_filename;
    }
    else if (!m_directories.empty()) {
      return m_directories.back();
    }
    else {
      return StringUtil::BLANK;
    }
  }

  void
  Path::clear() {
    m_directories.clear();
    m_device.clear();
    m_filename.clear();
    m_node.clear();
    m_isAbsolute = false;
  }

  void
  Path::throwInvalidPathException(const String& path) {
    GE_EXCEPT(InvalidParametersException, "Incorrectly formatted path provided: " + path);
  }

  String
  Path::buildWindows() const {
    StringStream result;
    if (!m_node.empty()) {
      result << "\\\\";
      result << m_node;
      result << "\\";
    }
    else if (!m_device.empty()) {
      result << m_device;
      result << ":\\";
    }
    else if (m_isAbsolute) {
      result << "\\";
    }

    for (auto& dir : m_directories) {
      result << dir;
      result << "\\";
    }

    result << m_filename;
    return result.str();
  }

  String
  Path::buildUnix() const {
    StringStream result;
    auto dirIter = m_directories.begin();

    if (!m_device.empty()) {
      result << "/";
      result << m_device;
      result << ":/";
    }
    else if (m_isAbsolute) {
      if (dirIter != m_directories.end() && "~" == *dirIter) {
        result << "~";
        dirIter++;
      }
      result << "/";
    }

    for (; dirIter != m_directories.end(); ++dirIter) {
      result << *dirIter;
      result << "/";
    }

    result << m_filename;
    return result.str();
  }

  Path
  Path::operator+(const Path& rhs) const {
    return Path::combine(*this, rhs);
  }

  Path&
  Path::operator+=(const Path& rhs) {
    return append(rhs);
  }

  bool
  Path::comparePathElem(const String& left, const String& right) {
    if (left.size() != right.size()) {
      return false;
    }

    //TODO: Case sensitive/insensitive file path actually depends on used file-system
    for (SIZE_T i = 0; i<left.size(); ++i) {
      if (tolower(left[i]) != tolower(right[i])) {
        return false;
      }
    }

    return true;
  }

  Path
  Path::combine(const Path& left, const Path& right) {
    Path output = left;
    return output.append(right);
  }

  void
  Path::stripInvalid(String& path) {
    String illegalChars = "\\/:?\"<>|";

    for (auto& entry : path) {
      if (illegalChars.find(entry) != String::npos) {
        entry = ' ';
      }
    }
  }

  void
  Path::pushDirectory(const String& dir) {
    if (!dir.empty() && "." != dir) {
      if (dir == "..") {
        if (!m_directories.empty() && m_directories.back() != "..") {
          m_directories.pop_back();
        }
        else {
          m_directories.push_back(dir);
        }
      }
      else {
        m_directories.push_back(dir);
      }
    }
  }
}
