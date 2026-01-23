/*****************************************************************************/
/**
 * @file   geException.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2015/02/22
 * @brief  Exceptions Base Classes
 *
 * This file contains the basic declaration of Exception objects
 * and classes for the use on the library.
 *
 * @bug	   warning C4275: non dll-interface class 'std::exception'
 * used as base for dll-interface class 'geEngineSDK::Exception'
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"

namespace geEngineSDK {
  /**
   * @brief Base class for all geEngine exceptions.
   */
#if USING(GE_COMPILER_MSVC)
# pragma warning( push )
# pragma warning( disable : 4275 )
#endif

  class Exception : public std::exception
  {
   public:
    Exception(const char* type, const String& description, const String& source)
      : m_typeName(type), 
        m_description(description), 
        m_source(source) {}

    Exception(const char* type,
              const String& description,
              const String& source,
              const char* file,
              long line)
      : m_line(line),
        m_typeName(type),
        m_description(description),
        m_source(source),
        m_file(file),
        m_fullDesc("") {}

    Exception(const Exception& rhs)
      : m_line(rhs.m_line),
        m_typeName(rhs.m_typeName),
        m_description(rhs.m_description),
        m_source(rhs.m_source),
        m_file(rhs.m_file),
        m_fullDesc(rhs.m_fullDesc) {}

    ~Exception() _NOEXCEPT = default;

    Exception&
    operator=(const Exception& rhs) {
      m_description = rhs.m_description;
      m_source = rhs.m_source;
      m_file = rhs.m_file;
      m_line = rhs.m_line;
      m_typeName = rhs.m_typeName;
      m_fullDesc = rhs.m_fullDesc;
      return *this;
    }

    /**
     * @brief Returns a string with the full description of the exception.
     * @note  The description contains the error number, the description
     *        supplied by the thrower, what routine threw the exception, and
     *        will also supply extra platform-specific information where applicable.
     */
    virtual const String&
    getFullDescription() const {
      if (m_fullDesc.empty()) {
        StringStream desc;
        desc << "GEENGINE EXCEPTION(" << m_typeName << "): " 
             << m_description << " in " << m_source;

        if (m_line > 0) {
          desc << " at " << m_file << " (line " << m_line << ")";
        }

        m_fullDesc = desc.str();
      }

      return m_fullDesc;
    }

    /**
     * @brief Gets the source function that threw the exception.
     */
    virtual const String&
    getSource() const {
      return m_source;
    }

    /**
     * @brief Gets the source file name in which the exception was thrown.
     */
    virtual const String&
    getFile() const {
      return m_file;
    }

    /**
     * @brief Gets line number on which the exception was thrown.
     */
    virtual long
    getLine() const {
      return m_line;
    }

    /**
     * @brief Gets a short description about the exception.
     */
    virtual const String&
    getDescription(void) const {
      return m_description;
    }

    /**
     * @brief Overriden std::exception::what. Returns the same value as "getFullDescription".
     */
    const char*
    what() const _NOEXCEPT {
      return getFullDescription().c_str();
    }

   protected:
    long m_line = 0;
    String m_typeName;
    String m_description;
    String m_source;
    String m_file;
    mutable String m_fullDesc;
  };

  /**
   * @brief Exception for signaling not implemented parts of the code.
   */
  class NotImplementedException : public Exception
  {
  public:
    NotImplementedException(const String& inDescription,
                            const String& inSource,
                            const char* inFile,
                            long inLine)
      : Exception("NotImplementedException", inDescription, inSource, inFile, inLine) {}
  };

  /**
   * @brief Exception for signaling file system errors when file could not be found.
   */
  class FileNotFoundException : public Exception
  {
  public:
    FileNotFoundException(const String& inDescription,
                          const String& inSource,
                          const char* inFile,
                          long inLine)
      : Exception("FileNotFoundException", inDescription, inSource, inFile, inLine) {}
  };

  /**
   * @brief Exception for signaling general IO errors.
   * @note  An example being failed to open a file or a network connection.
   */
  class IOException : public Exception
  {
  public:
    IOException(const String& inDescription,
                const String& inSource,
                const char* inFile,
                long inLine)
      : Exception("IOException", inDescription, inSource, inFile, inLine) {}
  };

  /**
   * @brief Exception for signaling not currently executing code in not in a valid state.
   */
  class InvalidStateException : public Exception
  {
  public:
    InvalidStateException(const String& inDescription,
                          const String& inSource,
                          const char* inFile,
                          long inLine)
      : Exception("InvalidStateException", inDescription, inSource, inFile, inLine) {}
  };

  /**
   * @brief Exception for signaling not some parameters you have provided are not valid.
   */
  class InvalidParametersException : public Exception
  {
  public:
    InvalidParametersException(const String& inDescription,
                               const String& inSource,
                               const char* inFile,
                               long inLine)
      : Exception("InvalidParametersException", inDescription, inSource, inFile, inLine) {}
  };

  /**
   * @brief	Exception for signaling an internal error, normally something that shouldn't
   *        have happened or wasn't anticipated by the programmers of that system.
   */
  class InternalErrorException : public Exception
  {
  public:
    InternalErrorException(const String& inDescription,
                           const String& inSource,
                           const char* inFile,
                           long inLine)
      : Exception("InternalErrorException", inDescription, inSource, inFile, inLine) {}
  };

  /**
   * @brief Exception for signaling an error in a rendering API.
   */
  class RenderingAPIException : public Exception
  {
  public:
    RenderingAPIException(const String& inDescription,
                          const String& inSource,
                          const char* inFile,
                          long inLine)
      : Exception("RenderingAPIException", inDescription, inSource, inFile, inLine) {}
  };

  /**
   * @brief Exception for signaling an error in an unit test.
   */
  class UnitTestException : public Exception
  {
  public:
    UnitTestException(const String& inDescription,
                      const String& inSource,
                      const char* inFile,
                      long inLine)
      : Exception("UnitTestException", inDescription, inSource, inFile, inLine) {}
  };

  /**
   * @brief Macro for throwing exceptions that will automatically fill out
   *        function name, file name and line number of the exception.
   */
   //TODO: Add Crash handler functionality and the platform terminate data
#ifndef GE_EXCEPT
# define GE_EXCEPT(type, desc)                                                \
{                                                                             \
  static_assert((std::is_base_of<geEngineSDK::Exception, type>::value),       \
                "Invalid exception type (" #type ") for GE_EXCEPT macro."     \
                "It needs to derive from geEngineSDK::Exception.");           \
  geEngineSDK::g_crashHandler().reportCrash(#type,                            \
                               desc,                                          \
                               __PRETTY_FUNCTION__,                           \
                               __FILE__,                                      \
                               __LINE__);                                     \
  geEngineSDK::PlatformUtility::terminate(true);                              \
}
#endif

#if USING(GE_COMPILER_MSVC)
# pragma warning( pop )
#endif
}
