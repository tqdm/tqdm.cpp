#pragma once
#include "tqdm/utils.h"
#include <limits>    // numeric_limits
#include <stddef.h>  // size_t

#ifndef SIZE_T_MAX
static const size_t SIZE_T_MAX = std::numeric_limits<size_t>::max();
#endif

namespace tqdm {

template <typename _Iterator>
class tqdm {
 private:
  _Iterator i;  // current
  _Iterator e;  // end
  size_t total;

 public:
  tqdm(_Iterator _Start, _Iterator _End, size_t total = SIZE_T_MAX)
      : i(_Start), e(_End), total(total == SIZE_T_MAX ? e - i : total) {}
};

}  // tqdm
