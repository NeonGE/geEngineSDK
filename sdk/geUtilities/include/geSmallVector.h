/*****************************************************************************/
/**
 * @file    geSmallVector.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/12/02
 * @brief   Dynamically sized container
 *
 * Dynamically sized container that statically allocates enough room for @p N
 * elements of type @p Type. If the element count exceeds the statically
 * allocated buffer size the vector falls back to general purpose dynamic
 * allocator.
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

#include "gePrerequisitesUtilities.h"

namespace geEngineSDK {
  using std::copy;
  using std::move;
  using std::equal;
  using std::distance;
  using std::exchange;
  using std::reverse_iterator;
  using std::initializer_list;
  using std::uninitialized_copy;
  using std::uninitialized_fill_n;
  using std::make_move_iterator;
  using std::lexicographical_compare;
  using std::aligned_storage_t;
  
  template<class Type, uint32 N>
  class SmallVector final
  {
   public:
    typedef Type ValueType;
    typedef Type* Iterator;
    typedef const Type* ConstIterator;
    typedef reverse_iterator<Type*> ReverseIterator;
    typedef reverse_iterator<const Type*> ConstReverseIterator;

    SmallVector() = default;

    SmallVector(const SmallVector<ValueType, N>& other) {
      if (!other.empty()) {
        *this = other;
      }
    }

    SmallVector(SmallVector<ValueType, N>&& other) {
      if (!other.empty()) {
        *this = move(other);
      }
    }

    explicit SmallVector(uint32 size, const Type& value = Type()) {
      append(size, value);
    }

    SmallVector(std::initializer_list<Type> list) {
      append(list);
    }

    ~SmallVector() {
      for (const auto& entry : *this) {
        entry.~Type();
      }

      if (!isSmall()) {
        ge_free(m_elements);
      }
    }

    SmallVector<ValueType, N>&
    operator=(const SmallVector<ValueType, N>& other) {
      if (&other == this) {
        return *this;
      }

      uint32 mySize = size();
      const uint32 otherSize = other.size();

      // Use assignment copy if we have more elements than the other array, and destroy any excess elements
      if (mySize > otherSize) {
        Iterator newEnd;
        if (otherSize > 0) {
          newEnd = copy(other.begin(), other.end(), begin());
        }
        else {
          newEnd = begin();
        }

        for (; newEnd != end(); ++newEnd) {
          (*newEnd).~Type();
        }

      }
      // Otherwise we need to partially copy (up to our size), and do uninitialized copy for rest. And an optional
      // grow if our capacity isn't enough (in which case we do uninitialized copy for all).
      else {
        if (otherSize > m_capacity) {
          clear();
          mySize = 0;

          grow(otherSize);
        }
        else if (0 < mySize) {
          copy(other.begin(), other.begin() + mySize, begin());
        }

        uninitialized_copy(other.begin() + mySize, other.end(), begin() + mySize);
      }

      m_size = otherSize;
      return *this;
    }

    SmallVector<ValueType, N>&
    operator=(SmallVector<ValueType, N>&& other) {
      if (&other == this) {
        return *this;
      }

      // If the other buffer isn't small, we can just steal its buffer
      if (!other.isSmall()) {
        for (const auto& entry : *this) {
          entry.~Type();
        }

        if (!isSmall()) {
          ge_free(m_elements);
        }

        m_elements = other.m_elements;
        other.m_elements = reinterpret_cast<Type*>(other.m_storage);
        m_size = exchange(other.m_size, 0);
        m_capacity = exchange(other.m_capacity, N);
      }
      
      //Otherwise we do essentially the same thing as in non-move assignment,
      //except for also clearing the other vector
      else {
        uint32 mySize = size();
        const uint32 otherSize = other.size();

        //Use assignment copy if we have more elements than the other array,
        //and destroy any excess elements
        if (mySize > otherSize) {
          Iterator newEnd;
          if (otherSize > 0) {
            newEnd = move(other.begin(), other.end(), begin());
          }
          else {
            newEnd = begin();
          }

          for (; newEnd != end(); ++newEnd) {
            (*newEnd).~Type();
          }
        }
        else {
          if (otherSize > m_capacity) {
            clear();
            mySize = 0;

            grow(otherSize);
          }
          else if (mySize > 0) {
            move(other.begin(), other.begin() + mySize, begin());
          }

          uninitialized_copy(make_move_iterator(other.begin() + mySize),
                             make_move_iterator(other.end()),
                             begin() + mySize);
        }

        m_size = otherSize;
        other.clear();
      }

      return *this;
    }

    SmallVector<ValueType, N>&
    operator=(initializer_list<Type> list) {
      uint32 mySize = size();
      const uint32 otherSize = static_cast<uint32>(list.size());

      //Use assignment copy if we have more elements than the list, and destroy
      //any excess elements
      if (mySize > otherSize) {
        Iterator newEnd;
        if (otherSize > 0) {
          newEnd = copy(list.begin(), list.end(), begin());
        }
        else {
          newEnd = begin();
        }

        for (; newEnd != end(); ++newEnd) {
          (*newEnd).~Type();
        }

      }
      //Otherwise we need to partially copy (up to our size), and do
      //uninitialized copy for rest. And an optional grow if our capacity isn't
      //enough (in which case we do uninitialized copy for all).
      else {
        if (otherSize > m_capacity) {
          clear();
          mySize = 0;

          grow(otherSize);
        }
        else if (0 < mySize) {
          copy(list.begin(), list.begin() + mySize, begin());
        }

        uninitialized_copy(list.begin() + mySize, list.end(), begin() + mySize);
      }

      m_size = otherSize;
      return *this;;
    }

    bool
    operator==(const SmallVector<ValueType, N>& other) {
      if (this->size() != other.size()) {
        return false;
      }
      
      return equal(this->begin(), this->end(), other.begin());
    }

    bool
    operator!=(const SmallVector<ValueType, N>& other) {
      return !(other == *this);
    }

    bool
    operator<(const SmallVector<ValueType, N>& other) const {
      return lexicographical_compare(begin(), end(), other.begin(), other.end());
    }

    bool
    operator>(const SmallVector<ValueType, N>& other) const {
      return other < *this;
    }

    bool
    operator<=(const SmallVector<ValueType, N>& other) const {
      return !(other < *this);
    }

    bool
    operator>=(const SmallVector<ValueType, N>& other) const {
      return !(*this < other);
    }

    Type&
    operator[](uint32 index) {
      GE_ASSERT(index < m_size && "Array index out-of-range.");
      return m_elements[index];
    }

    const Type&
    operator[](uint32 index) const {
      GE_ASSERT(index < m_size && "Array index out-of-range.");
      return m_elements[index];
    }

    bool
    empty() const {
      return 0 == m_size;
    }

    Iterator
    begin() {
      return m_elements;
    }

    Iterator
    end() {
      return m_elements + m_size;
    }

    ConstIterator
    begin() const {
      return m_elements;
    }

    ConstIterator
    end() const {
      return m_elements + m_size;
    }

    ConstIterator
    cbegin() const {
      return m_elements;
    }

    ConstIterator cend() const {
      return m_elements + m_size;
    }

    ReverseIterator
    rbegin() {
      return ReverseIterator(end());
    }
    ReverseIterator
    rend() {
      return ReverseIterator(begin());
    }

    ConstReverseIterator
    rbegin() const {
      return ConstReverseIterator(end());
    }
    ConstReverseIterator
    rend() const {
      return ConstReverseIterator(begin());
    }

    ConstReverseIterator
    crbegin() const {
      return ConstReverseIterator(end());
    }

    ConstReverseIterator
    crend() const {
      return ConstReverseIterator(begin());
    }

    uint32
    size() const {
      return m_size;
    }

    uint32
    capacity() const {
      return m_capacity;
    }

    Type*
    data() {
      return m_elements;
    }

    const Type*
    data() const {
      return m_elements;
    }

    Type&
    front() {
      GE_ASSERT(!empty());
      return m_elements[0];
    }

    Type&
    back() {
      GE_ASSERT(!empty());
      return m_elements[m_size - 1];
    }

    const Type&
    front() const {
      GE_ASSERT(!empty());
      return m_elements[0];
    }

    const Type&
    back() const {
      GE_ASSERT(!empty());
      return m_elements[m_size - 1];
    }

    void
    add(const Type& element) {
      if (m_size == m_capacity) {
        grow(m_capacity << 1);
      }

      new (&m_elements[m_size++]) Type(element);
    }

    void
    add(Type&& element) {
      if (m_size == m_capacity) {
        grow(m_capacity << 1);
      }

      new (&m_elements[m_size++]) Type(move(element));
    }

    void
    append(ConstIterator start, ConstIterator end) {
      const uint32 count = static_cast<uint32>(distance(start, end));

      if ((size() + count) > capacity()) {
        this->grow(size() + count);
      }

      uninitialized_copy(start, end, this->end());
      m_size += count;
    }

    void
    append(uint32 count, const Type& element) {
      if ((size() + count) > capacity()) {
        this->grow(size() + count);
      }

      uninitialized_fill_n(end(), count, element);
      m_size += count;
    }

    void
    append(initializer_list<Type> list) {
      append(list.begin(), list.end());
    }

    void
    pop() {
      GE_ASSERT(0 < m_size && "Popping an empty array.");
      --m_size;
      m_elements[m_size].~Type();
    }

    Iterator
    erase(ConstIterator iter) {
      GE_ASSERT(begin() <= iter  && "Iterator to erase is out of bounds.");
      GE_ASSERT(iter < end() && "Erasing at past-the-end iterator.");

      Iterator toErase = const_cast<Iterator>(iter);
      move(toErase + 1, end(), toErase);
      pop();

      return toErase;
    }

    void
    remove(uint32 index) {
      erase(begin() + index);
    }

    bool
    contains(const Type& element) {
      for (uint32 i = 0; i < m_size; ++i) {
        if (element == m_elements[i]) {
          return true;
        }
      }

      return false;
    }

    void
    removeValue(const Type& element) {
      for (uint32 i = 0; i < m_size; ++i) {
        if (m_elements[i] == element) {
          remove(i);
          break;
        }
      }
    }

    void
    clear() {
      for (uint32 i = 0; i < m_size; ++i) {
        m_elements[i].~Type();
      }

      m_size = 0;
    }

    void
    reserve(uint32 _capacity) {
      if (_capacity > m_capacity) {
        grow(_capacity);
      }
    }

    void
    resize(uint32 size, const Type& value = Type()) {
      if (size > m_capacity) {
        grow(size);
      }

      if (size > m_size) {
        for (uint32 i = m_size; i < size; ++i) {
          new (&m_elements[i]) Type(value);
        }
      }
      else {
        for (uint32 i = size; i < m_size; ++i) {
          m_elements[i].~Type();
        }
      }

      m_size = size;
    }

   private:
    /**
     * @brief Returns true if the vector is still using its static memory and
     *        hasn't made any dynamic allocations.
     */
    bool
    isSmall() const {
      return m_elements == reinterpret_cast<Type*>(m_storage);
    }

    void
    grow(uint32 _capacity) {
      GE_ASSERT(_capacity > N);

      //Allocate memory with the new capacity (caller guarantees never to call
      //this with capacity <= N, so we don't need to worry about the static
      //buffer)
      Type* buffer = ge_allocN<Type>(_capacity);

      //Move any existing elements
      uninitialized_copy(make_move_iterator(begin()),
                         make_move_iterator(end()),
                         buffer);

      //Destroy existing elements in old memory
      for (const auto& entry : *this) {
        entry.~Type();
      }

      //If the current buffer is dynamically allocated, free it
      if (!isSmall()) {
        ge_free(m_elements);
      }

      m_elements = buffer;
      m_capacity = _capacity;
    }

    aligned_storage_t<sizeof(Type), alignof(Type)> m_storage[N];
    Type* m_elements = reinterpret_cast<Type*>(m_storage);

    uint32 m_size = 0;
    uint32 m_capacity = N;
  };
}
