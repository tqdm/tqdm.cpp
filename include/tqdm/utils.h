#pragma once

#ifndef CUR_OS

#if !defined(IS_WIN) && (defined(_WIN32) || defined(_WIN64))
#define IS_WIN
#elif !defined IS_NIX &&                                                     \
    (defined(unix) || defined(__unix) || defined(__unix__) ||                \
     defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__) ||     \
     defined(__MACH__))
#define IS_NIX
#endif

#endif  // CUR_OS

// #include <cassert>    // assert
// #include <cstdint>    // int64_t
#include <cstddef>   // ptrdiff_t, size_t
#include <iterator>  // iterator
// #include <limits>    // numeric_limits
#include <type_traits>  // is_pointer, ...

#ifndef constexpr
#define constexpr static const
#endif

#ifndef explicit
#define explicit
#endif

#ifndef noexcept
#define noexcept
#endif

namespace tqdm {

template <typename _Iterator>
/**
Wrapper for pointers and std containter iterators.
@author Casper da Costa-Luis
*/
class MyIteratorWrapper
    : public std::iterator<
          std::forward_iterator_tag,
          typename std::iterator_traits<_Iterator>::value_type> {
  mutable _Iterator p;  // TODO: remove this mutable

public:
  // already done by std::iterator
  typedef typename std::iterator_traits<_Iterator>::value_type value_type;

  explicit MyIteratorWrapper(_Iterator x) : p(x) {}
  // default construct gives end
  MyIteratorWrapper() : p(nullptr) {}
  explicit MyIteratorWrapper(const MyIteratorWrapper &mit) : p(mit.p) {}

  // override this in Tqdm class
  virtual void _incr() { ++p; }
  // override this in Tqdm class
  virtual void _incr() const { ++p; }

  MyIteratorWrapper &operator++() {
    // assert(this->bool() && "Out-of-bounds iterator increment");
    _incr();
    return *this;
  }
  const MyIteratorWrapper &operator++() const {
    _incr();
    return *this;
  }
  MyIteratorWrapper operator++(int) const {
    MyIteratorWrapper tmp(*this);
    _incr();
    return tmp;
  }
  template <class Other>
  // two-way comparison: v.begin() == v.cbegin() and vice versa
  bool operator==(const MyIteratorWrapper<Other> &rhs) const {
    return p == rhs.p;
  }
  template <class Other>
  bool operator!=(const MyIteratorWrapper<Other> &rhs) const {
    return p != rhs.p;
  }
  template <class Other>
  size_t operator-(const MyIteratorWrapper<Other> &rhs) {
    return p - rhs.p;
  }
  // template <typename = typename std::enable_if<
  //               !std::is_const<value_type>::value>::type>
  value_type &operator*() {
    // assert(this->bool() && "Invalid iterator dereference!");
    return *p;
  }
  const value_type &operator*() const {
    // assert(this->bool() && "Invalid iterator dereference!");
    return *p;
  }
  // template <typename = typename std::enable_if<
  //               !std::is_const<value_type>::value>::type>
  value_type &operator->() {
    // assert(this->bool() && "Invalid iterator dereference!");
    return *p;
  }
  const value_type &operator->() const {
    // assert(this->bool() && "Invalid iterator dereference!");
    return *p;
  }
  // @return the underlying iterator
  _Iterator &get() { return p; }
  const _Iterator &get() const { return p; }
  // TODO: const _Iterator &get() const { return p; }, etc ...
  //
  void swap(MyIteratorWrapper &other) noexcept { std::swap(p, other.p); }

  // One way conversion: iterator -> const_iterator
  // template <typename =
  //               typename
  //               std::enable_if<!std::is_const<_Iterator>::value>::type>
  // operator MyIteratorWrapper<typename std::add_const<_Iterator>::type>()
  // const {
  //   return MyIteratorWrapper<const _Iterator>(p);
  // }
  template <typename = typename std::is_pointer<_Iterator>>
  explicit operator bool() const {
    return p != nullptr;
  }
};

template <typename _Iterator,
          typename _MyIteratorWrapper = MyIteratorWrapper<_Iterator>>
_MyIteratorWrapper myIteratorWrapper(_Iterator x) {
  return _MyIteratorWrapper(x);
}

template <typename IntType = int>
class RangeIterator
    : public std::iterator<std::forward_iterator_tag, IntType> {
private:
  mutable IntType current;
  IntType total;
  IntType step;

public:
  RangeIterator(IntType total) : current(0), total(total), step(1) {}
  RangeIterator(IntType start, IntType total)
      : current(start), total(total), step(1) {}
  RangeIterator(IntType start, IntType total, IntType step)
      : current(start), total(total), step(step) {}
  IntType &operator*() { return current; }
  const IntType &operator*() const { return current; }
  RangeIterator &operator++() {
    current += step;
    return *this;
  }
  const RangeIterator &operator++() const {
    current += step;
    return *this;
  }
  RangeIterator operator++(int) const {
    RangeIterator tmp(*this);
    operator++();
    return tmp;
  }
  explicit operator bool() const { return current < total; }
  size_t size_remaining() const { return (total - current) / step; }

  /** here be dragons */

  // only use as (it != end), not as (end != it)
  bool operator!=(const RangeIterator &) const { return current < total; }
  bool operator==(const RangeIterator &) const { return current >= total; }
  IntType operator-(const RangeIterator &it) const {
    // it's used in `end - begin`, but `end` is only a sentinel
    // so let's use `begin `to be consistent
    return it.size_remaining();
  }
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
