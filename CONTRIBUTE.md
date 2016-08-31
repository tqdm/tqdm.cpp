# Contributing

To report bugs, propose features, or raise miscellaneous issues, create an
issue in the [issue tracker](https://github.com/tqdm/tqdm.cpp/issues).

To contribute code, [fork](https://help.github.com/articles/fork-a-repo/)
the [repository](https://github.com/tqdm/tqdm.cpp) and open a
[pull request](https://github.com/tqdm/tqdm.cpp/pulls).

There also exists an ad-hoc
[chat room](http://chat.stackexchange.com/rooms/43704/tqdm-cpp)
for discussion on small issues.


# TODO

Ranked by priority

- `format_meter`
- `close`
- support all `Params` (starting with file descriptor output)
- fix potential range update bug
- anon namespaces
- thread safety
- coverage reporting
- release 0.0.1-py4.8.4 (includes based-on-tqdm.py version)


# Building

To build and test on any system and IDE, use `CMake`.
For example, on a UNIX system with `Make`:

``` sh
tqdm.cpp$ mkdir -p build    # create build directory
tqdm.cpp$ cd build
tqdm.cpp/build$ cmake ..    # generate system config files
tqdm.cpp/build$ make        # build and test
```

Builds have been tested on these configurations:

- Windows 10 x64, MSVC 2013
- Ubuntu LTS x64
- Travis Ubuntu Trusty
    - g++-5, g++-4.9
    - clang++-3.6, clang++-3.7, clang++-3.8
- Debian Jessie ARMv8
