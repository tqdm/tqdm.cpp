#pragma once

#ifndef CUR_OS

#if !defined(IS_WIN) && (defined(_WIN32) || defined(_WIN64))
#define IS_WIN
#elif !defined IS_NIX &&                                                 \
    (defined(unix) || defined(__unix) || defined(__unix__) ||            \
     defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__) || \
     defined(__MACH__))
#define IS_NIX
#endif

#endif  // CUR_OS

#include <cassert>
#include <iterator>
#include <limits>
#include <type_traits>

namespace std {

// #ifndef always_true
// template<typename T>
// struct always_true{ enum {value}; };
// #endif  // always_true
}

namespace tqdm {

template <typename T, typename U>
struct is_same : std::false_type {};

template <typename T>
struct is_same<T, T> : std::true_type {};

// namespace utils{

template <typename IT, typename Enabled = void>
// stl-compliant iterable object which has its own value_type typedef
struct IT_value_type {
  typedef typename IT::value_type value_type;
};

template <typename IT>
struct IT_value_type<
    IT, typename std::enable_if<std::is_pointer<IT>::value>::type> {
  typedef typename std::remove_pointer<IT>::type value_type;
};

template <typename IT>
struct IT_value_type<IT,
                     typename std::enable_if<std::is_array<IT>::value>::type> {
  typedef typename std::remove_extent<IT>::type value_type;
};

// template <typename IT>
// struct IT_value_type <IT, typename
// std::enable_if<std::_Is_iterator<IT>::value>::type>
// {
// 	typedef typename IT::value_type value_type;
// };

template <typename _Iterator>
/**
Wrapper for pointers and std containter iterators.
@author Casper da Costa-Luis
*/
class MyIteratorWrapper
    : public std::iterator<std::forward_iterator_tag,
                           typename IT_value_type<_Iterator>::value_type> {
  _Iterator p;

 public:
  // already done by std::iterator
  typedef typename IT_value_type<_Iterator>::value_type value_type;

  explicit MyIteratorWrapper(_Iterator x) : p(x) {}
  // default construct gives end
  MyIteratorWrapper() : p(nullptr) {}
  explicit MyIteratorWrapper(const MyIteratorWrapper &mit) : p(mit.p) {}
  // override this in Tqdm class
  MyIteratorWrapper &operator++() {
    // assert(this->bool() && "Out-of-bounds iterator increment");
    ++p;
    return *this;
  }
  MyIteratorWrapper operator++(int) {
    // assert(this->bool() && "Out-of-bounds iterator increment!");
    MyIteratorWrapper tmp(*this);
    operator++();
    return tmp;
  }
  template <class Other>
  // two-way comparison: v.begin() == v.cbegin() and vice versa
  bool operator==(const MyIteratorWrapper<Other> &rhs) {
    return p == rhs.p;
  }
  template <class Other>
  bool operator!=(const MyIteratorWrapper<Other> &rhs) {
    return p != rhs.p;
  }
  // template <class Other>
  // size_t operator-(const MyIteratorWrapper<Other> &rhs) {
  //   return p - rhs.p;
  // }
  virtual value_type &operator*() {
    // assert(this->bool() && "Invalid iterator dereference!");
    return *p;
  }
  virtual value_type &operator->() {
    // assert(this->bool() && "Invalid iterator dereference!");
    return *p;
  }
  // @return the underlying iterator
  virtual _Iterator &get() { return p; }
  virtual const _Iterator &get() const { return p; }
  // TODO: const _Iterator &get() const { return p; }, etc ...
  //
  virtual void swap(MyIteratorWrapper &other) noexcept {
    std::swap(p, other.p);
  }

  // One way conversion: iterator -> const_iterator
  /*operator MyIteratorWrapper<const value_type>() const {
    return MyIteratorWrapper<const value_type>(p);
  }*/
  template <typename = typename std::is_pointer<_Iterator> >
  operator bool() const {
    return p != nullptr;
  }
};

template <typename _Iterator,
          typename _MyIteratorWrapper = MyIteratorWrapper<_Iterator> >
_MyIteratorWrapper myIteratorWrapper(_Iterator x) {
  return _MyIteratorWrapper(x);
}

template <typename IntType>
/** TODO */
class _range {
  static const IntType IntTypeMax = std::numeric_limits<IntType>::max();
  _range(IntType a, IntType b = IntTypeMax, IntType step = IntType(1));
};

// colorama win
// weakset
// _is_utf(encoding)
// _is_ascii(s)
// _supports_unicode(file)
// _environ_cols()
// _sh(const char *cmd[], ...)
const char *_term_move_up() {
  return
#if defined(IS_WIN) && !defined(colorama)
      ""
#else
      "\x1b[A"
#endif
      ;
}

// }  // utils

}  // tqdm
