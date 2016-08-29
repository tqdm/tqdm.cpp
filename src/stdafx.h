#include <atomic>       // atomic
#include <cassert>      // assert
#include <cerrno>       // EAGAIN
#include <cinttypes>    // PRIu64
#include <cstddef>      // ptrdiff_t, size_t
#include <cstdint>      // uint64_t
#include <cstdio>       // printf
#include <cstring>      // strlen
#include <iterator>     // iterator
#include <limits>       // numeric_limits
#include <stdexcept>    // throw
#include <string>       // string
#include <type_traits>  // is_pointer, ...
#include <utility>      // swap
#include <chrono>       // time_point, steady_clock
#include <functional>   // function

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
#include <WinSock2.h>  // select
#include <io.h>        // write
#else                  // _WIN32
#include <poll.h>      // poll
#include <unistd.h>    // STDERR_FILENO
#endif                 // _WIN32
