cmake_minimum_required(VERSION 3.0.0)

# edit this to wherever your vcpkg is
set(VCPKG_DIR ../vcpkg CACHE PATH "path to vcpkg root")
set(VCPKG_TRIPLET x64-linux CACHE PATH "installed img lib triplet")

set(CMAKE_TOOLCHAIN_FILE ${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake)

project(seedimg_full VERSION 1.0 DESCRIPTION "seedimg module based image manipulation library")

option(BUILD_TESTING "" OFF)
include(CTest)
include(CheckCXXCompilerFlag)

include_directories(${VCPKG_DIR}/installed/${VCPKG_TRIPLET}/include ${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(BUILD_SHARED_LIBS TRUE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

CHECK_CXX_COMPILER_FLAG(-Wextra COMPILER_SUPPORTS_WEXTRA)
if(COMPILER_SUPPORTS_WEXTRA)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
else()
    set(CMAKE_CXX_FLAGS "-Wall")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# core
add_subdirectory(seedimg)

# image modules
add_subdirectory(seedimg-jpeg)
add_subdirectory(seedimg-png)
add_subdirectory(seedimg-webp)
add_subdirectory(seedimg-farbfeld)

# filter modules
add_subdirectory(seedimg-filters-core)

# automatic detection of image types:
add_subdirectory(seedimg-autodetect)

# dependent project
add_subdirectory(example)

# tests
if (${BUILD_TESTING})
    add_subdirectory(tests)
endif()
