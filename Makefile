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
WARNINGS = -Wall -Wextra -Wunused -Wformat=2 -Wno-missing-field-initializers

# No end-user servicable parts below.

override CFLAGS += -std=c99 -fPIC
override CXXFLAGS += -std=c++11 -fPIC

override CPPFLAGS += -I ./include/

.DEFAULT_GOAL = all

BIN_SOURCES := src/main.cpp
LIB_SOURCES := $(filter-out ${BIN_SOURCES},$(wildcard src/*.cpp))
TEST_SOURCES := $(wildcard test/*.cpp test/*.c)
INTERNAL_HEADERS := $(wildcard src/*.hpp)
PUBLIC_HEADERS := $(wildcard include/tqdm/*.hpp include/tqdm/*.h include/tqdm/*.tcc)

BIN_OBJECTS := $(patsubst %,obj/%.o,${BIN_SOURCES})
LIB_OBJECTS := $(patsubst %,obj/%.o,${LIB_SOURCES})
TEST_OBJECTS := $(patsubst %,obj/%.o,${TEST_SOURCES})

BIN := bin/tqdm
LIB := lib/libtqdm.a
# The second foreach is just a poor man's scoped variable binding.
TESTS := $(foreach test_obj,${TEST_OBJECTS},$(foreach test_bin,$(basename $(patsubst obj/%.o,bin/%,${test_obj})),${test_bin} $(eval ${test_bin}: ${test_obj} ${LIB})))

TEST_RUNS := $(patsubst bin/%,run/%,${TESTS})
.PHONY: ${TEST_RUNS}

MKDIR_FIRST = @mkdir -p ${@D}
RM_FIRST = @rm -f $@

all: ${BIN} ${LIB}

test: ${TEST_RUNS}
tests: ${TESTS}

clean:
	rm -rf bin/ lib/ obj/

${BIN}: ${BIN_OBJECTS} ${LIB}
${LIB}: ${LIB_OBJECTS}

# Temporary - should set up -MD and include .d files for finer control.
${BIN_OBJECTS} ${LIB_OBJECTS} ${TEST_OBJECTS}: ${INTERNAL_HEADERS} ${PUBLIC_HEADERS}

.SECONDARY:
.DELETE_ON_ERROR:
.SUFFIXES:

obj/%.c.o: %.c
	${MKDIR_FIRST}
	${CC} ${CFLAGS} ${CPPFLAGS} -c -o $@ $<

obj/%.cpp.o: %.cpp
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
