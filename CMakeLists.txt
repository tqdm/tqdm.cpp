cmake_minimum_required(VERSION 2.8.9)

project(tqdm)

# so we can set up visual studio "folders"
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

cmake_policy(SET CMP0020 NEW)

if(POLICY CMP0043)
cmake_policy(SET CMP0043 OLD)
endif(POLICY CMP0043)

if(MSVC)
set(STANDARD_COMPILE_OPTS /WX /D_SCL_SECURE_NO_WARNINGS)
set(REL_OPTS /GL /Zi)
set(REL_WITH_DEB_OPTS /Ob2) # Ob1 override a strange default on inlining that cmake uses for RelWithDeb builds
add_compile_options("$<$<CONFIG:DEBUG>:${STANDARD_COMPILE_OPTS}>")
add_compile_options("$<$<CONFIG:MINSIZEREL>:${STANDARD_COMPILE_OPTS}>")
add_compile_options("$<$<CONFIG:RELWITHDEBINFO>:${STANDARD_COMPILE_OPTS}>")
add_compile_options("$<$<CONFIG:RELWITHDEBINFO>:${REL_WITH_DEB_OPTS}>")
add_compile_options("$<$<CONFIG:RELEASE>:${STANDARD_COMPILE_OPTS}>")
add_compile_options("$<$<CONFIG:RELEASE>:${REL_OPTS}>")
SET(CMAKE_MFC_FLAG 0)

include(FlagUtilities.cmake)
# This is the only use of FlagUtilities.cmake - can we do it another way?
ADD_LINKER_FLAGS("/LTCG" Release FORCE)
ADD_LINKER_FLAGS_EXE("/DEBUG" Release FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif(MSVC)

if (NOT (("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel") OR MSVC))
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Werror -Wall -Wextra -Wunused -Wformat=2 -Wno-missing-field-initializers -std=c++11 -fPIC -O2")
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fwhole-program")
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# We're not consitent about whether global should be /hwk or not...
# Perhaps RTIP itself needs global hwk in path (to build cpps) but not client
# applications.
set(TQDM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(TQDM_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB TQDM_PCH "${TQDM_SRC_DIR}/*.h" "${TQDM_INCLUDE_DIR}/tqdm/*.h")
set(TQDM_BIN_FILES
  "${TQDM_SRC_DIR}/main.cpp"
  # ${TQDM_PCH}
)
file(GLOB TQDM_TEST_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h"
  # ${TQDM_PCH}
)
file(GLOB TQDM_LIB_FILES
  # "${TQDM_SRC_DIR}/utils.cpp"
  # "${TQDM_SRC_DIR}/tqdm.cpp"
  # ${TQDM_PCH}
  "${TQDM_SRC_DIR}/*.cpp"
)
# file(GLOB TQDM_PUBLIC_HEADERS "${TQDM_INCLUDE_DIR}/tqdm/*.h")

# set(TQDM_BIN_OBJECTS $(patsubst %,obj/%.o,${BIN_SOURCES}))
# set(LIB_OBJECTS $(patsubst %,obj/%.o,${LIB_SOURCES}))
# set(TEST_OBJECTS $(patsubst %,obj/%.o,${TEST_SOURCES}))

# Make extra debug info available on optimised builds
if(MSVC)
add_definitions(-Zo)
endif(MSVC)

include_directories(${TQDM_INCLUDE_DIR} ${TQDM_SRC_DIR})
link_directories(${TQDM_SRC_DIR})

# include(PrecompiledHeader.cmake)

# lib
set(options STATIC_LIBRARY)
add_library(tqdmlib STATIC ${TQDM_LIB_FILES})  # ${TQDM_PCH}
# add_precompiled_header(libtqdm "${TQDM_SRC_DIR}/stdafx.h" FORCEINCLUDE
#   SOURCE_CXX "${TQDM_SRC_DIR}/stdafx.cpp")

# bin
add_executable(tqdm ${TQDM_BIN_FILES})  # ${TQDM_PCH}
# add_precompiled_header(tqdm "${TQDM_SRC_DIR}/stdafx.h" FORCEINCLUDE
#   SOURCE_CXX "${TQDM_SRC_DIR}/stdafx.cpp")

# test
add_executable(test_tqdm ${TQDM_TEST_FILES})  # ${TQDM_PCH}
# add_precompiled_header(test_tqdm "${TQDM_SRC_DIR}/stdafx.h" FORCEINCLUDE
#   SOURCE_CXX "${TQDM_SRC_DIR}/stdafx.cpp")

if(CMAKE_COMPILER_IS_GNUCXX)
# add_executable(stdafx.h.gch "${TQDM_SRC_DIR}/stdafx.h")

# add_custom_command(
#   OUTPUT "${TQDM_SRC_DIR}/stdafx.h.gch"
#   COMMAND "${CXX}" "${CMAKE_CXX_FLAGS}" "-o" "${TQDM_SRC_DIR}/stdafx.h.gch" "${TQDM_SRC_DIR}/stdafx.h"
#   MAIN_DEPENDENCY "${TQDM_SRC_DIR}/stdafx.h"
#   COMMENT "Precompiling headers"
# )

add_custom_target(
  pch
  ${CMAKE_CXX_COMPILER} -I "${TQDM_INCLUDE_DIR}" -g -Werror -Wall -Wextra -Wunused -Wformat=2 -std=c++11 -fPIC -O2 -fwhole-program -c -o "src/stdafx.h.gch" "src/stdafx.h"
  DEPENDS ${TQDM_PCH}
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  VERBATIM
)

add_custom_target(
  cfmt
  clang-format "-i" "-style=file" ${TQDM_LIB_FILES} ${TQDM_TEST_FILES} ${TQDM_PCH}
  COMMENT "Linting ${TQDM_LIB_FILES} ${TQDM_TEST_FILES} ${TQDM_PCH}"
  DEPENDS ${TQDM_LIB_FILES} ${TQDM_TEST_FILES} ${TQDM_PCH}
  VERBATIM
)

endif()

## target_link_libraries(tqdm)

if(UNIX)

macro(STRIP_FMT TARG STRIP_FMT_TARG STRIP_FMT_DEPS)
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_dependencies(${TARG} pch)
  endif()
  if(CFMT)
    add_dependencies(${TARG} cfmt)
  endif(CFMT)
  add_custom_command(
    TARGET ${TARG}
    COMMAND objcopy --only-keep-debug ${STRIP_FMT_TARG} ${STRIP_FMT_TARG}.debug
    COMMAND strip -g ${STRIP_FMT_TARG}
    COMMAND objcopy --add-gnu-debuglink=${STRIP_FMT_TARG}.debug ${STRIP_FMT_TARG}
    VERBATIM
    COMMENT "Separating debug symbols"
  )
endmacro(STRIP_FMT)

STRIP_FMT(     tqdm "$<TARGET_FILE:tqdm>"      "${TQDM_BIN_FILES}")
STRIP_FMT(test_tqdm "$<TARGET_FILE:test_tqdm>" "${TQDM_TEST_FILES}")
STRIP_FMT(tqdmlib   "$<TARGET_FILE:tqdmlib>"   "${TQDM_LIB_FILES}")

endif(UNIX)

add_custom_command(
  TARGET test_tqdm
  POST_BUILD
  COMMAND test_tqdm
  VERBATIM
  USES_TERMINAL
  COMMENT "Testing"
)
