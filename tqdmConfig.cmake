# tqdm cmake module
#
# This module sets the target:
#
#   tqdm.cpp
#
# In addition, it sets the following variables:
#
#   tqdm_FOUND - true if tqdm found
#   tqdm_VERSION - tqdm's version
#   tqdm_INCLUDE_DIRS - the directory containing tqdm headers
#

include(CMakeFindDependencyMacro)

# Define target "tqdm"

if(NOT TARGET tqdm.cpp)
    include("${CMAKE_CURRENT_LIST_DIR}/tqdmTargets.cmake")
    get_target_property(tqdm_INCLUDE_DIRS tqdm.cpp INTERFACE_INCLUDE_DIRECTORIES)
endif()
