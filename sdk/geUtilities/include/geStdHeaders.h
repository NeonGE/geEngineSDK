/*****************************************************************************/
/**
 * @file    geStdHeaders.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/09
 * @brief   Header for the STD libraries
 *
 * This file includes the most common and required STL objects
 * It takes account of the operating system of the build to modify
 * the required object
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * If we are on a Borland compiler (C++ Builder)
 */
/*****************************************************************************/
#ifdef __BORLANDC__
# define __STD_ALGORITHM
#endif

/*****************************************************************************/
/**
 * C type objects
 */
/*****************************************************************************/
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cmath>

/*****************************************************************************/
/**
 * For memory management
 */
/*****************************************************************************/
#include <memory>

/*****************************************************************************/
/**
 * STL containers
 */
/*****************************************************************************/
#include <vector>
#include <stack>
#include <map>
#include <string>
#include <set>
#include <list>
#include <forward_list>
#include <deque>
#include <queue>
#include <bitset>
#include <array>

#if USING(GE_CPP17_OR_LATER)
#   include <variant>
#   include <optional>
#endif

#include <unordered_map>
#include <unordered_set>

/*****************************************************************************/
/**
 * STL algorithms & functions
 */
/*****************************************************************************/
#include <algorithm>
#include <functional>
#include <limits>
#include <iterator>

/*****************************************************************************/
/**
 * C++ Stream stuff
 */
/*****************************************************************************/
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

/*****************************************************************************/
/**
 * C Types and Stats
 */
/*****************************************************************************/
extern "C" {
# include <sys/types.h>
# include <sys/stat.h>
}

/*****************************************************************************/
/**
 * Windows specifics
 */
/*****************************************************************************/
#if USING(GE_PLATFORM_WINDOWS)
  //Undefine min & max
# undef min
# undef max

# if !defined(NOMINMAX) && defined(_MSC_VER)
#   define NOMINMAX     //Required to stop windows.h messing up std::min
# endif
# if defined( __MINGW32__ )
#   include <unistd.h>
# endif
#endif

/*****************************************************************************/
/**
 * Linux specifics
 */
/*****************************************************************************/
#if USING(GE_PLATFORM_LINUX)
  extern "C" {
#   include <unistd.h>
#   include <dlfcn.h>
  }
#endif

/*****************************************************************************/
/**
 * Apple specifics
 */
/*****************************************************************************/
#if USING(GE_PLATFORM_OSX)
  extern "C" {
#   include <unistd.h>
#   include <sys/param.h>
#   include <CoreFoundation/CoreFoundation.h>
  }
#endif

/*****************************************************************************/
/**
 * PS4 specifics
 */
/*****************************************************************************/
#if USING(GE_PLATFORM_PS4) || USING(GE_PLATFORM_PS5)
  extern "C" {
#   include <unistd.h>
#   include <sys/param.h>
  }
#endif

#include "geEnumClassHash.h"

namespace geEngineSDK {
  /***************************************************************************/
	/**
   * Standard containers, for easier access in my own namespace
   */
  /***************************************************************************/

  /**
   * @brief Hasher that handles custom enums automatically.
   */
  template<typename Key>
  using HashType = typename std::conditional<std::is_enum<Key>::value,
                                             EnumClassHash,
                                             std::hash<Key>>::type;

  /**
   * @brief Double ended queue. Allows for fast insertion and removal at both
   *        its beginning and end.
   */
  template<typename T, typename A = StdAlloc<T>>
  using Deque = std::deque<T, A>;

  /**
  * @brief Dynamically sized array that stores element contiguously.
   */
  template<typename T, typename A = StdAlloc<T>>
  using Vector = std::vector<T, A>;

  /**
   * @brief Container that supports constant time insertion and removal, but
   *        without fast random access to elements. Internally implemented as a
   *        doubly linked list. Use ForwardList if you do not need reverse
   *        iteration.
   */
  template<typename T, typename A = StdAlloc<T>>
  using List = std::list<T, A>;

  /**
   * @brief Container that supports constant time insertion and removal, but
   *        without fast random access to elements. Internally implemented as a
   *        singly linked list that doesn't support reverse iteration.
   */
  template<typename T, typename A = StdAlloc<T>>
  using ForwardList = std::forward_list<T, A>;

  /**
   * @brief First-in, last-out data structure.
   */
  template<typename T, typename A = StdAlloc<T>>
  using Stack = std::stack<T, std::deque<T, A>>;

  /**
   * @brief First-in, first-out data structure.
   */
  template<typename T, typename A = StdAlloc<T>>
  using Queue = std::queue<T, std::deque<T, A>>;

  /**
   * @brief An associative container containing an ordered set of elements.
   */
  template<typename T, typename P = std::less<T>, typename A = StdAlloc<T>>
  using Set = std::set<T, P, A>;

  /**
   * @brief An associative container containing an ordered set of key-value
   *        pairs.
   */
  template<typename K, 
           typename V, 
           typename P = std::less<K>, 
           typename A = StdAlloc<std::pair<const K, V>>>
  using Map = std::map<K, V, P, A>;

  /**
   * @brief An associative container containing an ordered set of elements
   *        where multiple elements can have the same key.
   */
  template<typename T, typename P = std::less<T>, typename A = StdAlloc<T>>
  using MultiSet = std::multiset<T, P, A>;

  /**
   * @brief An associative container containing an ordered set of key-value
   *        pairs where multiple elements can have the same key.
   */
  template<typename K, 
           typename V, 
           typename P = std::less<K>, 
           typename A = StdAlloc<std::pair<const K, V>>>
  using MultiMap = std::multimap<K, V, P, A>;

  /**
   * @brief An associative container containing an unordered set of elements.
   *        Usually faster than Set for larger data sets.
   */
  template<typename T, 
           typename H = HashType<T>,
           typename C = std::equal_to<T>, 
           typename A = StdAlloc<T>>
  using UnorderedSet = std::unordered_set<T, H, C, A>;

  /**
   * @brief An associative container containing an unordered set of key-value
   *        pairs. Usually faster than Map for larger data sets.
   */
  template<typename K, 
           typename V, 
           typename H = HashType<K>,
           typename C = std::equal_to<K>, 
           typename A = StdAlloc<std::pair<const K, V>>>
  using UnorderedMap = std::unordered_map<K, V, H, C, A>;

  /**
   * @brief An associative container containing an ordered set of key-value
   *        pairs where multiple elements can have the same key. Usually faster
   *        than MultiMap for larger data sets.
   */
  template<typename K, 
           typename V, 
           typename H = HashType<K>,
           typename C = std::equal_to<K>, 
           typename A = StdAlloc<std::pair<const K, V>>>
  using UnorderedMultimap = std::unordered_multimap<K, V, H, C, A>;
  
  /**
   * @brief An associative container containing an ordered set of elements
   *        where multiple elements can have the same key. Usually faster than
   *        MultiSet for larger data sets.
   */
  template<typename T,
           typename Container = Vector<T>,
           typename Pr = std::less<typename Container::value_type>>
  using PriorityQueue = std::priority_queue<T, Container, Pr>;

  /**
   * @brief Fixed size array.
   */
  template<typename T, size_t N>
  using Array = std::array<T, N>;

  /**
   * @brief Bitset that stores bits in a fixed size array.
   */
  template<size_t N>
  using Bitset = std::bitset<N>;

#if USING(GE_CPP17_OR_LATER)
  /**
   * @brief Optional object that may or may not contain a value.
   */
  template<typename T>
  using Optional = std::optional<T>;

  inline constexpr std::nullopt_t NullOpt{ std::nullopt_t::_Tag{} };

  /**
   * @brief Variant object that may hold any of the specified
   */
  template<class... T>
  using Variant = std::variant<T...>;
#endif

  /***************************************************************************/
  /**
   * Smart pointers
   */
  /***************************************************************************/
  
  /**
   * @brief Smart pointer that retains shared ownership of a project through
   *        a pointer. The object is destroyed automatically when the last
   *        shared pointer to the object is destroyed.
   */
	template <typename T>
	using SPtr = std::shared_ptr<T>;

  /**
   * @brief Holds a reference to an object whose lifetime is managed by a SPtr,
   *        but doesn't increment the reference count.
   */
  template <typename T>
  using WeakSPtr = std::weak_ptr<T>;

  /**
   * @brief Smart pointer that retains shared ownership of a project through
   *        a pointer. Reference to the object must be unique. The object is
   *        destroyed automatically when the pointer to the object is destroyed.
   */
  template<typename T,
           typename Alloc = GenAlloc,
           typename Delete = Deleter<T, Alloc>>
  using UPtr = std::unique_ptr<T, Delete>;

  /**
   * @brief Create a new shared pointer using a custom allocator category.
   */
  template<class Type, class AllocCategory = GenAlloc, class... Args>
	SPtr<Type>
  ge_shared_ptr_new(Args&&... args) {
		return std::allocate_shared<Type>(StdAlloc<Type, AllocCategory>(),
                                      std::forward<Args>(args)...);
	}

	/**
   * @brief	Create a new shared pointer from a previously constructed object.
   *        Pointer specific data will be allocated using the provided allocator category.
   */
  template<typename Type,
           typename MainAlloc = GenAlloc,
           typename PtrDataAlloc = GenAlloc,
           typename Delete = Deleter<Type, MainAlloc>>
  SPtr<Type>
  ge_shared_ptr(Type* data, Delete del = Delete()) {
    return SPtr<Type>(data, std::move(del), StdAlloc<Type, PtrDataAlloc>());
  }

  /**
   * @brief Create a new unique pointer from a previously constructed object.
   *        Pointer specific data will be allocated using the provided allocator category.
   */
  template<typename Type,
           typename Alloc = GenAlloc,
           typename Delete = Deleter<Type, Alloc>>
  UPtr<Type, Alloc, Delete>
  ge_unique_ptr(Type* data, Delete del = Delete()) {
    return std::unique_ptr<Type, Delete>(data, std::move(del));
  }

  /**
   * @brief	Create a new unique pointer using a custom allocator category.
   */
  template<typename Type,
           typename Alloc = GenAlloc,
           typename Delete = Deleter<Type, Alloc>,
           typename... Args>
  UPtr<Type, Alloc, Delete>
  ge_unique_ptr_new(Args &&... args) {
    Type* rawPtr = ge_new<Type, Alloc>(std::forward<Args>(args)...);
    return ge_unique_ptr<Type, Alloc, Delete>(rawPtr);
  }

  template<typename T>
  struct NativePtr
  {
    explicit CONSTEXPR NativePtr(T* p = nullptr) : m_ptr(p) {}
    
    CONSTEXPR T& operator*() const {
      return *m_ptr;
    }

    CONSTEXPR T* operator->() const {
      return m_ptr;
    }

    CONSTEXPR T* get() const {
      return m_ptr;
    }

   private:
    T* m_ptr = nullptr;
  };

  /**
   * @brief "Smart" pointer that is not smart. Does nothing but hold a pointer
   *        value. No memory management is performed at all.
   *        This class exists to make storing pointers in containers easier to
   *        manage, such as with non-member comparison operators.
   */
  template <typename T>
  using NPtr = NativePtr<T>;

  template<typename L_T, typename R_T>
  CONSTEXPR bool
  operator<(const NPtr<L_T>& lhs, const NPtr<R_T>& rhs) {
    return lhs.get() < rhs.get();
  }

  template<typename L_T, typename R_T>
  CONSTEXPR bool
  operator>(const NPtr<L_T>& lhs, const NPtr<R_T>& rhs) {
    return lhs.get() > rhs.get();
  }

  template<typename L_T, typename R_T>
  CONSTEXPR bool
  operator<=(const NPtr<L_T>& lhs, const NPtr<R_T>& rhs) {
    return lhs.get() <= rhs.get();
  }

  template<typename L_T, typename R_T>
  CONSTEXPR bool
  operator>=(const NPtr<L_T>& lhs, const NPtr<R_T>& rhs) {
    return lhs.get() >= rhs.get();
  }

  template<typename L_T, typename R_T>
  CONSTEXPR bool
  operator==(const NPtr<L_T>& lhs, const NPtr<R_T>& rhs) {
    return lhs.get() == rhs.get();
  }

  template<typename L_T, typename R_T>
  CONSTEXPR bool
  operator!=(const NPtr<L_T>& lhs, const NPtr<R_T>& rhs) {
    return lhs.get() != rhs.get();
  }
}
