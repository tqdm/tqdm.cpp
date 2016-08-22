#include <cassert>      // assert
#include <cinttypes>    // PRIu64
#include <cstddef>      // ptrdiff_t, size_t
#include <cstdint>      // uint64_t
#include <cstdio>       // printf
#include <iterator>     // iterator
#include <limits>       // numeric_limits
#include <stdexcept>    // throw
#include <string>       // string
#include <type_traits>  // is_pointer, ...
#include <utility>      // swap
#include <atomic>       // atomic
#include <cstring>      // strlen
#include <cerrno>       // EAGAIN

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif
// #ifndef ssize_t
//   typedef std::make_unsigned<size_t>::type ssize_t;
// #endif
#include <io.h>        // write
#include <WinSock2.h>  // select
#else
#include <unistd.h>  // STDERR_FILENO
#include <poll.h>    // poll
#endif               // _WIN32
