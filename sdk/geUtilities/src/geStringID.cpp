/*****************************************************************************/
/**
 * @file    geStringID.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/07
 * @brief   A string identifier for very fast comparisons
 *
 * A string identifier that provides very fast comparisons to other string ids.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
* Includes
*/
/*****************************************************************************/
#include "geStringID.h"

namespace geEngineSDK {
  const StringID StringID::NONE;

  volatile StringID::InitStatics StringID::m_initStatics = StringID::InitStatics();
  StringID::InternalData* StringID::m_stringHashTable[HASH_TABLE_SIZE];
  StringID::InternalData* StringID::m_chunks[MAX_CHUNK_COUNT];

  uint32 StringID::m_nextId = 0;
  uint32 StringID::m_numChunks = 0;
  SpinLock StringID::m_sync;

  StringID::InitStatics::InitStatics() {
    ScopedSpinLock lock(m_sync);
    memset(m_stringHashTable, 0, sizeof(m_stringHashTable));
    memset(m_chunks, 0, sizeof(m_chunks));
    m_chunks[0] = reinterpret_cast<InternalData*>
                    (ge_alloc(sizeof(InternalData) * ELEMENTS_PER_CHUNK));
    memset(m_chunks[0], 0, sizeof(InternalData) * ELEMENTS_PER_CHUNK);
    ++m_numChunks;
  }

  template<class T>
  void
  StringID::construct(T const& name) {
    GE_ASSERT(StringIDUtil<T>::size(name) <= STRING_SIZE);

    uint32 hash = calcHash(name)
                  & (sizeof(m_stringHashTable) / sizeof(m_stringHashTable[0]) - 1);
    InternalData* existingEntry = m_stringHashTable[hash];

    while (nullptr != existingEntry) {
      if (StringIDUtil<T>::compare(name, existingEntry->m_chars)) {
        m_data = existingEntry;
        return;
      }

      existingEntry = existingEntry->m_next;
    }

    ScopedSpinLock lock(m_sync);

    //Search for the value again in case other thread just added it
    existingEntry = m_stringHashTable[hash];
    InternalData* lastEntry = nullptr;
    while (nullptr != existingEntry) {
      if (StringIDUtil<T>::compare(name, existingEntry->m_chars)) {
        m_data = existingEntry;
        return;
      }

      lastEntry = existingEntry;
      existingEntry = existingEntry->m_next;
    }

    m_data = allocEntry();
    StringIDUtil<T>::copy(name, m_data->m_chars);
    if (nullptr == lastEntry) {
      m_stringHashTable[hash] = m_data;
    }
    else {
      lastEntry->m_next = m_data;
    }
  }

  template<class T>
  uint32
  StringID::calcHash(T const& input) const {
    uint32 size = StringIDUtil<T>::size(input);

    uint32 hash = 0;
    for (uint32 i = 0; i < size; ++i) {
      hash = hash * 101 + input[i];
    }

    return hash;
  }

  StringID::InternalData*
  StringID::allocEntry() {
    uint32 chunkIdx = m_nextId / ELEMENTS_PER_CHUNK;

    GE_ASSERT(chunkIdx < MAX_CHUNK_COUNT);
    GE_ASSERT(chunkIdx <= m_numChunks); //Can only increment sequentially

    if (chunkIdx >= m_numChunks) {
      m_chunks[chunkIdx] = reinterpret_cast<InternalData*>
                             (ge_alloc(sizeof(InternalData) * ELEMENTS_PER_CHUNK));
      memset(m_chunks[chunkIdx], 0, sizeof(InternalData) * ELEMENTS_PER_CHUNK);
      ++m_numChunks;
    }

    InternalData* chunk = m_chunks[chunkIdx];
    uint32 chunkSpecificIndex = m_nextId % ELEMENTS_PER_CHUNK;

    InternalData* newEntry = &chunk[chunkSpecificIndex];
    newEntry->m_id = m_nextId++;
    newEntry->m_next = nullptr;

    return newEntry;
  }

  template<>
  class StringID::StringIDUtil<const ANSICHAR*>
  {
   public:
    static uint32
    size(const ANSICHAR* const& input) {
      return (uint32)strlen(input);
    }

    static void
    copy(const ANSICHAR* const& input, ANSICHAR* dest) {
      memcpy(dest, input, strlen(input) + 1);
    }

    static bool
    compare(const ANSICHAR* const& a,
            ANSICHAR* b) {
      return strcmp(a, b) == 0;
    }
  };

  template<>
  class StringID::StringIDUtil<String>
  {
   public:
    static uint32
    size(String const& input) {
        return(uint32)input.length();
    }

    static void
    copy(String const& input, ANSICHAR* dest) {
      auto len = static_cast<uint32>(input.length());
      input.copy(dest, len);
      dest[len] = '\0';
    }

    static bool
    compare(String const& a, ANSICHAR* b) {
      return a.compare(b) == 0;
    }
  };

  template
  GE_UTILITIES_EXPORT void
  StringID::construct(const ANSICHAR* const&);
  
  template
  GE_UTILITIES_EXPORT void
  StringID::construct(String const&);

  template
  GE_UTILITIES_EXPORT uint32 StringID::calcHash(const ANSICHAR* const&) const;
  
  template
  GE_UTILITIES_EXPORT uint32 StringID::calcHash(String const&) const;
}
