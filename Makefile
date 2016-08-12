# Standard variables
CC = gcc
CXX = g++
CFLAGS = -g -O2 ${WARNINGS}
CXXFLAGS = -g -O2 ${WARNINGS}
LDFLAGS =
LDLIBS =
AR = ar
ARFLAGS = rcs

# Project variables
WARNINGS = -Werror -Wall -Wextra -Wunused -Wformat=2

override CFLAGS += -std=c99 -fPIC
override CXXFLAGS += -std=c++11 -fPIC
override CPPFLAGS += -I./include/

.DEFAULT_GOAL = all

BIN_SOURCES := src/main.cpp
LIB_SOURCES := $(filter-out ${BIN_SOURCES},$(wildcard src/*.cpp))
TEST_SOURCES := $(wildcard test/*.cpp)
INTERNAL_HEADERS := $(wildcard src/*.h)
PUBLIC_HEADERS := $(wildcard include/tqdm/*.h)
BIN_OBJECTS := $(patsubst %,obj/%.o,${BIN_SOURCES})
LIB_OBJECTS := $(patsubst %,obj/%.o,${LIB_SOURCES})
TEST_OBJECTS := $(patsubst %,obj/%.o,${TEST_SOURCES})

BIN := bin/tqdm
LIB := lib/libtqdm.a
PCH := $(INTERNAL_HEADERS:%=%.gch)
# second foreach is equivalent to scoped variable binding
TESTS := $(foreach test_obj,${TEST_OBJECTS},$(foreach test_bin,$(basename $(patsubst obj/%.o,bin/%,${test_obj})),${test_bin} $(eval ${test_bin}: ${test_obj} ${LIB})))
TEST_RUNS := $(patsubst bin/%,run/%,${TESTS})

MKDIR_FIRST = @mkdir -p ${@D}
RM_FIRST = @rm -f $@

.PHONY: ${TEST_RUNS}
all: ${PCH} ${BIN} ${LIB}
test: ${TEST_RUNS}
tests: ${TESTS}

clean:
	rm -rf ${PCH} bin/ lib/ obj/

${BIN}: ${BIN_OBJECTS} ${LIB}
${LIB}: ${LIB_OBJECTS}

# Temporary - should set up -MD and include .d files for finer control.
${BIN_OBJECTS} ${LIB_OBJECTS} ${TEST_OBJECTS}: ${INTERNAL_HEADERS} ${PUBLIC_HEADERS}

.SECONDARY:
.DELETE_ON_ERROR:
.SUFFIXES:

%.gch: %
	${CXX} ${CXXFLAGS} ${CPPFLAGS} -c -o $@ $<

obj/%.c.o: %.c
	${MKDIR_FIRST}
	${CC} ${CFLAGS} ${CPPFLAGS} -c -o $@ $<

obj/%.cpp.o: %.cpp
	clang-format -style=file -i $^
	${MKDIR_FIRST}
	${CXX} ${CXXFLAGS} ${CPPFLAGS} -c -o $@ $<

# Must always use ${CXX} to link to resolve tqdm's dependencies,
# even if the user's application is only in C and using our C interface.
# This may change if we ever support shared libraries.
bin/%:
	${MKDIR_FIRST}
	${CXX} ${LDFLAGS} $^ ${LDLIBS} -o $@

${TEST_RUNS}: run/%: bin/%
	./$<

# No support for shared libraries (yet?) - we don't have a stable ABI,
# only a (hopefully) stable API.
lib/%.a:
	${MKDIR_FIRST}
	${RM_FIRST}
	${AR} ${ARFLAGS} $@ $^
