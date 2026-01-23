/*****************************************************************************/
/**
 * @file    geDynArray.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/12/02
 * @brief   Dynamically sized array, similar to std::vector
 *
 * Dynamically sized array, similar to std::vector
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
  using std::exchange;
  using std::equal;
  using std::max;
  using std::distance;
  using std::forward;
  using std::rotate;
  using std::reverse_iterator;
  using std::initializer_list;
  using std::uninitialized_copy;
  using std::uninitialized_fill_n;
  using std::lexicographical_compare;
  using std::enable_if;
  using std::is_integral;
  using std::make_move_iterator;

  template<class Type>
  class DynArray final
  {
   public:
    using ValueType = Type;
    using Iterator = Type*;
    using ConstIterator = const Type*;
    using ReverseIterator = reverse_iterator<Type*>;
    using ConstReverseIterator = reverse_iterator<const Type*>;
    using DifferenceType = ptrdiff_t;

    DynArray() = default;

    explicit DynArray(uint32 size, const ValueType& value = ValueType()) {
      append(size, value);
    }

    DynArray(Iterator first, Iterator last) {
      append(first, last);
    }

    DynArray(initializer_list<ValueType> list) {
      append(list);
    }

    DynArray(const DynArray<ValueType>& other) {
      if (!other.empty()) {
        *this = other;
      }
    }

    DynArray(DynArray<ValueType>&& other) {
      if (!other.empty()) {
        *this = move(other);
      }
    }

    ~DynArray() {
      for (const auto& entry : *this) {
        entry.~Type();
      }
      ge_free(m_elements);
    }

    DynArray<ValueType>&
    operator=(const DynArray<ValueType>& other) {
      if (&other == this) {
        return *this;
      }

      uint32 mySize = size();
      const uint32 otherSize = other.size();

      //Use assignment copy if we have more elements than the other array, and
      //destroy any excess elements
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
      //Otherwise we need to partially copy (up to our size), and do
      //uninitialized copy for rest. And an optional grow if our capacity isn't
      //enough (in which case we do uninitialized copy for all).
      else {
        if (otherSize > m_capacity) {
          clear();
          mySize = 0;

          realloc(otherSize);
        }
        else if (mySize > 0) {
          copy(other.begin(), other.begin() + mySize, begin());
        }

        uninitialized_copy(other.begin() + mySize,
                           other.end(),
                           begin() + mySize);
      }

      m_size = otherSize;
      return *this;
    }

    DynArray<ValueType>&
    operator=(DynArray<ValueType>&& other) {
      if (this == &other) {
        return *this;
      }

      //Just steal the buffer
      for (const auto& entry : *this) {
        entry.~Type();
      }

      ge_free(m_elements);

      m_elements = exchange(other.m_elements, nullptr);
      m_size = exchange(other.m_size, 0);
      m_capacity = exchange(other.m_capacity, 0);

      return *this;
    }

    DynArray<ValueType>&
    operator=(initializer_list<ValueType> list) {
      uint32 mySize = size();
      const auto otherSize = static_cast<uint32>(list.size());

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

          realloc(otherSize);
        }
        else if (0 < mySize) {
          copy(list.begin(), list.begin() + mySize, begin());
        }

        uninitialized_copy(list.begin() + mySize, list.end(), begin() + mySize);
      }

      m_size = otherSize;
      return *this;
    }

    bool
    operator==(const DynArray<ValueType>& other) const {
      if (size() != other.size()) {
        return false;
      }

      return equal(this->begin(), this->end(), other.begin());
    }

    bool
    operator!=(const DynArray<ValueType>& other) const {
      return !(*this == other);
    }

    bool
    operator<(const DynArray<ValueType>& other) const {
      return std::lexicographical_compare(begin(),
                                          end(),
                                          other.begin(),
                                          other.end());
    }

    bool
    operator>(const DynArray<ValueType>& other) const {
      return other < *this;
    }

    bool
    operator<=(const DynArray<ValueType>& other) const {
      return !(other < *this);
    }

    bool
    operator>=(const DynArray<ValueType>& other) const {
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

    ConstIterator
    cend() const {
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
      return ReverseIterator(end());
    }

    ConstReverseIterator
    rend() const {
      return ReverseIterator(begin());
    }

    ConstReverseIterator
    crbegin() const {
      return ReverseIterator(end());
    }

    ConstReverseIterator
    crend() const {
      return ReverseIterator(begin());
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
      return *m_elements[0];
    }

    Type&
    back() {
      GE_ASSERT(!empty());
      return *m_elements[m_size - 1];
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
      if (size() == capacity()) {
        realloc(max(1U, capacity() * 2));
      }

      new (&m_elements[m_size++]) Type(element);
    }

    void
    add(Type&& element) {
      if (size() == capacity()) {
        realloc(std::max(1U, capacity() * 2));
      }

      new (&m_elements[m_size++]) Type(move(element));
    }

    void
    pop() {
      GE_ASSERT(0 < m_size && "Popping an empty array.");
      --m_size;
      m_elements[m_size].~Type();
    }

    void
    remove(uint32 index) {
      erase(begin() + index);
    }

    bool
    contains(const Type& element) {
      for (uint32 i = 0; i < m_size; ++i) {
        if (m_elements[i] == element) {
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
    resize(uint32 size, const Type& value = Type()) {
      if (size > capacity()) {
        realloc(size);
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

    void
    reserve(uint32 size) {
      if (size > capacity()) {
        realloc(size);
      }
    }

    void
    shrink() {
      realloc(m_size);
    }

    void
    append(ConstIterator start, ConstIterator end) {
      const auto count = static_cast<uint32>(distance(start, end));

      if ((size() + count) > capacity()) {
        realloc(size() + count);
      }

      uninitialized_copy(start, end, this->end());
      m_size += count;
    }

    void
    append(uint32 count, const Type& element) {
      if ((size() + count) > capacity()) {
        realloc(size() + count);
      }

      uninitialized_fill_n(end(), count, element);
      m_size += count;
    }

    void
    append(initializer_list<Type> list) {
      append(list.begin(), list.end());
    }

    bool
    swapAndErase(Iterator iter) {
      GE_ASSERT(!empty());

      auto iterLast = end() - 1;

      bool swapped = false;
      if (iter != iterLast) {
        swap(*iter, *iterLast);
        swapped = true;
      }

      pop();
      return swapped;
    }

    void
    swap(DynArray<ValueType>& other) {
      const uint32 tmpSize = size();
      const uint32 tmpCapacity = capacity();
      Type* tmp = data();

      m_size = other.size();
      m_capacity = other.capacity();
      m_elements = other.data();

      other.m_size = tmpSize;
      other.m_capacity = tmpCapacity;
      other.m_elements = tmp;
    }

    template<typename ...Args>
    void
    emplaceBack(Args&& ...args) {
      if (size() == capacity()) {
        realloc(max(1U, capacity() * 2));
      }

      new (&m_elements[m_size++]) Type(forward<Args>(args)...);
    }

    template <typename ...Args>
    Iterator
    emplace(ConstIterator it, Args&&... args) {
      auto iterc = const_cast<Iterator>(it);
      DifferenceType offset = iterc - begin();

      if (size() == capacity()) {
        realloc(max(1U, capacity() * 2));
      }

      new (&m_elements[m_size++]) Type(forward<Args>(args) ...);
      rotate(begin() + offset, end() - 1, end());

      return begin() + offset;
    }

    Iterator
    insert(ConstIterator it, const ValueType& element) {
      auto iterc = const_cast<Iterator>(it);
      DifferenceType offset = iterc - begin();

      if (size() == capacity()) {
        realloc(max(1U, capacity() * 2));
      }

      new (&m_elements[m_size++]) Type(element);
      rotate(begin() + offset, end() - 1, end());

      return begin() + offset;
    }

    Iterator
    insert(ConstIterator it, ValueType&& element) {
      auto iterc = const_cast<Iterator>(it);
      DifferenceType offset = iterc - begin();

      if (size() == capacity()) {
        realloc(max(1U, capacity() * 2));
      }

      new (&m_elements[m_size++]) Type(move(element));
      rotate(begin() + offset, end() - 1, end());

      return begin() + offset;
    }

    Iterator
    insert(ConstIterator it, uint32 n, const ValueType& element) {
      auto iterc = const_cast<Iterator>(it);
      DifferenceType offset = iterc - begin();
      Iterator iter = &m_elements[offset];

      if (!n) {
        return iter;
      }

      if (size() + n > capacity()) {
        realloc((size() + n) * 2);
      }

      uint32 c = n;
      while (c--) {
        new (&m_elements[m_size++]) Type(element);
      }

      rotate(begin() + offset, end() - n, end());

      return begin() + offset;
    }

    template<typename InputIt>
    typename enable_if<!is_integral<InputIt>::value, void>::type
    insert(ConstIterator it, InputIt first, InputIt last) {
      auto iterc = const_cast<Iterator>(it);
      DifferenceType offset = iterc - begin();
      auto n = static_cast<uint32>(last - first);

      if (size() + n > capacity()) {
        realloc((size() + n) * 2);
      }

      while (first != last) {
        new (&m_elements[m_size++]) Type(*first++);
      }

      rotate(begin() + offset, end() - n, end());
    }

    Iterator
    insert(ConstIterator it, initializer_list<ValueType> list) {
      auto iterc = const_cast<Iterator>(it);
      DifferenceType offset = iterc - begin();
      Iterator iter = &m_elements[offset];
      auto n = static_cast<uint32>(list.size());

      if (!n) {
        return iter;
      }

      if (size() + n > capacity()) {
        realloc((size() + n) * 2);
      }

      for (auto& entry : list) {
        new (&m_elements[m_size++]) Type(entry);
      }

      rotate(begin() + offset, end() - n, end());

      return iter;
    }

    Iterator
    erase(ConstIterator first, ConstIterator last) {
      GE_ASSERT(begin() <= first && "Iterator to insert is out of bounds.");
      GE_ASSERT(last < end() && "Inserting at past-the-end iterator.");

      auto iter = const_cast<Iterator>(first);

      if (first == last) {
        return iter;
      }

      auto iterLast = const_cast<Iterator>(last);
      move(iterLast, end(), iter);

      for (Iterator it = iter; it < iterLast; ++it) {
        pop();
      }

      return iter;
    }

    Iterator erase(ConstIterator it) {
      GE_ASSERT(it >= begin() && "Iterator to erase is out of bounds.");
      GE_ASSERT(it < end() && "Erasing at past-the-end iterator.");

      auto toErase = const_cast<Iterator>(it);
      move(toErase + 1, end(), toErase);
      pop();

      return toErase;
    }

   private:
    void
    realloc(uint32 capacity) {
      Type* buffer = ge_allocN<Type>(capacity);

      if (m_elements) {
        uninitialized_copy(make_move_iterator(begin()),
                             make_move_iterator(end()),
                                                buffer);

        for (const auto& entry : *this) {
          entry.~Type();
        }

        ge_free(m_elements);
      }

      m_elements = buffer;
      m_capacity = capacity;
    }

    Type* m_elements = nullptr;
    uint32 m_size = 0;
    uint32 m_capacity = 0;
  };
}
