/*****************************************************************************/
/**
 * @file    geUtil.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/06
 * @brief   Utility functions
 *
 * Some utility functions used across the whole engine but not specific to any
 * object type
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

namespace geEngineSDK {
  using std::size_t;
  using std::conditional;
  using std::is_enum;
  using std::hash;
  using std::memset;
  using std::memcpy;
  using std::swap;

  /**
   * @brief Generates a new hash for the provided type using the default hasher
   *        and combines it with a previous hash.
   * @note  This one came out of boost::hash_combine
  */
  template<class T>
  void
  ge_hash_combine(size_t& seed, const T& v) {
    using HashType = typename conditional<is_enum<T>::value, EnumClassHash, hash<T>>::type;
    
    HashType hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  /**
   * @brief Generates a hash for the provided type. Type must have a std::hash
   *        specialization.
   */
  template<class T>
  size_t
  ge_hash(const T& v) {
    using HashType = typename conditional<is_enum<T>::value, EnumClassHash, hash<T>>::type;

    HashType hasher;
    return hasher(v);
  }

  /**
   * @brief Generates an MD5 hash string for the provided source string.
   */
  String GE_UTILITIES_EXPORT
  md5(const WString& source);

  /**
   * @brief Generates an MD5 hash string for the provided source string.
   */
  String GE_UTILITIES_EXPORT
  md5(const String& source);

  /**
   * @brief Sets contents of a struct to zero.
   */
  template<class T>
  void
  ge_zero_out(T& s) {
    memset(&s, 0, sizeof(T));
  }

  /**
   * @brief Sets contents of a static array to zero.
   */
  template<class T, SIZE_T N>
  void
  ge_zero_out(T(&arr)[N]) {
    memset(arr, 0, sizeof(T) * N);
  }

  /**
   * @brief Sets contents of a block of memory to zero.
   */
  template<class T>
  void
  ge_zero_out(T* arr, SIZE_T count) {
    GE_ASSERT(nullptr != arr);
    memset(arr, 0, sizeof(T) * count);
  }

  /**
   * @brief Copies the contents of one array to another.
   *        Automatically accounts for array element size.
   */
  template<class T, size_t N>
  void
  ge_copy(T(&dst)[N], T(&src)[N], SIZE_T count) {
    memcpy(dst, src, sizeof(T) * count);
  }

  /**
   * @brief Copies the contents of one array to another.
   *        Automatically accounts for array element size.
   */
  template<class T>
  void
  ge_copy(T* dst, T* src, SIZE_T count) {
    memcpy(dst, src, sizeof(T) * count);
  }

  /**
   * @brief Returns the size of the provided static array.
   */
  template <class T, SIZE_T N>
  CONSTEXPR SIZE_T
  ge_size(const T(&)[N]) {
    return N;
  }

  /**
   * @brief Erases the provided element from the container, but first swaps the
   *        element so it's located at the end of the container, making the
   *        erase operation cheaper at the cost of an extra copy.
   *        Return true if a swap occurred, or false if the element was already
   *        at the end of the container.
   */
  template<class T, class A = StdAlloc<T>>
  bool
  ge_swap_and_erase(Vector<T, A>& container,
                    const typename Vector<T, A>::iterator iter) {
    GE_ASSERT(!container.empty());

    auto iterLast = container.end() - 1;

    bool swapped = false;
    if (iter != iterLast) {
      swap(*iter, *iterLast);
      swapped = true;
    }

    container.pop_back();
    return swapped;
  }
}
