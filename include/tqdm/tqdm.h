#pragma once

/**
Customisable progressbar decorator for iterators.
Includes a default range iterator printing to stderr.

Usage:
  # include "tqdm/tqdm.h"
  for(int i : tqdm::range(4))
  // same as:
  //   for (int i : tqdm::tqdm({0, 1, 2, 3}))
  // or:
  //   std::vector<int> v{0, 1, 2, 3};
  //   for (int &i : tqdm::tqdm(v.begin(), v.end())
    ...
*/

#include "tqdm/utils.h"
#include <limits>       // numeric_limits
#include <cassert>      // assert
#include <cstddef>      // ptrdiff_t, size_t
#include <iterator>     // iterator
#include <type_traits>  // remove_cv
#include <utility>      // swap

#ifndef SIZE_T_MAX
static const size_t SIZE_T_MAX = std::numeric_limits<size_t>::max();
#endif

namespace tqdm {

const char* author[] = {"github.com/casperdcl"};

template <typename _Iterator>
class tqdm {
 private:
  _Iterator i;  // current
  _Iterator e;  // end
  size_t s;     // step
  size_t total;

 public:
  tqdm(_Iterator begin, _Iterator end, size_t step = 1)
      : i(begin), e(end), s(step), total((end - start) / step) {}

  tqdm(_Iterator begin, size_t total, size_t step = 1)
      : i(begin), e(nullptr), s(step), total(total) {}

  template <typename _Container>
  tqdm(_Container& v, size_t step = 1)
      : i(v.begin()),
        e(v.end()),
        s(step),
        total((v.end() - v.begin()) / step) {}

  template <class Type, class UnqualifiedType = std::remove_cv_t<Type> >
  class ForwardIterator
      : public std::iterator<std::forward_iterator_tag, UnqualifiedType,
                             std::ptrdiff_t, Type*, Type&> {
    tqdm<Type>* itr;

    explicit ForwardIterator(tqdm<Type>* nd) : itr(nd) {}

   public:
    ForwardIterator()  // Default construct gives end.
        : itr(nullptr) {}

    void swap(ForwardIterator& other) noexcept { std::swap(itr, other.iter); }

    ForwardIterator& operator++()  // Pre-increment
    {
      assert(itr != nullptr && "Out-of-bounds iterator increment");
      ++itr;
      return *this;
    }

    ForwardIterator operator++(int)  // Post-increment
    {
      assert(itr != nullptr && "Out-of-bounds iterator increment!");
      ForwardIterator tmp(*this);
      ++itr;
      return tmp;
    }

    // two-way comparison: v.begin() == v.cbegin() and vice versa
    template <class OtherType>
    bool operator==(const ForwardIterator<OtherType>& rhs) const {
      return itr == rhs.itr;
    }

    template <class OtherType>
    bool operator!=(const ForwardIterator<OtherType>& rhs) const {
      return itr != rhs.itr;
    }

    UnqualifiedType& operator*() const {
      assert(itr != nullptr && "Invalid iterator dereference!");
      return *(itr->i);
    }

    UnqualifiedType& operator->() const {
      assert(itr != nullptr && "Invalid iterator dereference!");
      return *(itr->i);
    }

    // One way conversion: iterator -> const_iterator
    operator ForwardIterator<const Type>() const {
      return ForwardIterator<const Type>(itr);
    }
  };

  typedef ForwardIterator<_Iterator> iterator;
  typedef ForwardIterator<const _Iterator> const_iterator;
};

}  // tqdm
