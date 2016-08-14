cmake_minimum_required(VERSION 2.8.9)

project(tqdm)

# so we can set up visual studio "folders"
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

cmake_policy(SET CMP0020 NEW)

if(POLICY CMP0043)
cmake_policy(SET CMP0043 OLD)
endif(POLICY CMP0043)

# include(PrecompiledHeader.cmake)

# include(FlagUtilities.cmake)
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

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

# We're not consitent about whether global should be /hwk or not...
# Perhaps RTIP itself needs global hwk in path (to build cpps) but not client
# applications.
set(TQDM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(TQDM_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(TQDM_BIN_FILE "${TQDM_SRC_DIR}/main.cpp")
file(GLOB TQDM_TEST_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h")
set(TQDM_LIB_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/tqdm.cpp"
)
file(GLOB TQDM_INTERNAL_HEADERS "${TQDM_SRC_DIR}/*.h")
file(GLOB TQDM_PUBLIC_HEADERS "${TQDM_INCLUDE_DIR}/tqdm/*.h")
# set(TQDM_BIN_OBJECTS $(patsubst %,obj/%.o,${BIN_SOURCES}))
# set(LIB_OBJECTS $(patsubst %,obj/%.o,${LIB_SOURCES}))
# set(TEST_OBJECTS $(patsubst %,obj/%.o,${TEST_SOURCES}))

# Make extra debug info available on optimized builds.
add_definitions(-Zo)

# Function for creating RTIP apps.
# To create a new app just call this with some options from some subdirectory.
# Look at AnalysisApplication/CMakeLists.txt for examples.

set(options STATIC_LIBRARY)
set(BUILD_tqdm OFF CACHE BOOL "Build tqdm")
set(APPLICATION_NAME tqdm)
include_directories(${TQDM_INCLUDE_DIR} ${TQDM_SRC_DIR})
link_directories(${TQDM_SRC_DIR})
add_library(libtqdm STATIC ${TQDM_LIB_FILES})
add_executable(${APPLICATION_NAME} ${TQDM_BIN_FILE})  # ${TQDM_LIB_FILES}
add_executable(test_tqdm ${TQDM_TEST_FILES})
# add_precompiled_header(${APPLICATION_NAME} "stdafx.h" FORCEINCLUDE)
target_link_libraries(${APPLICATION_NAME})
add_custom_command(
TARGET ${APPLICATION_NAME}
POST_BUILD
COMMAND if 1==$<CONFIG:RelWithDebInfo> echo exec tests here || EXIT /B 0
COMMENT " Running POST BUILD action: Tests "
)
