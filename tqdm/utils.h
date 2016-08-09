#pragma once

#ifndef CUR_OS

#if !defined(IS_WIN) && (defined(_WIN32) || defined(_WIN64))
#define IS_WIN
#elif !defined IS_NIX && (\
  defined(unix) || defined(__unix) || defined(__unix__) || \
  defined(__linux__) || defined(__FreeBSD__) || \
  defined(__APPLE__) || defined(__MACH__))
#define IS_NIX
#endif

#endif  // CUR_OS

#include <set>
#include <limits>

namespace tqdm{

// namespace utils{

template <typename IntType>
/** TODO */
_range(IntType a, IntType b=std::numeric_limits<IntType>::max(), IntType step=IntType(1));

// }  // utils

}  // tqdm

// colorama win
// weakset
// _is_utf(encoding)
// _is_ascii(s)
// _supports_unicode(file)
// _environ_cols()
// _sh(const char *cmd[], ...)
const char *_term_move_up()
{
  return
#if defined(IS_WIN) && !defined(colorama)
  ""
#else
  "\x1b[A";
#endif
}
