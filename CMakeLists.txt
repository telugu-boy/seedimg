#[[ 
seedimg - module based image manipulation library written in modern C++ 
Copyright (C) 2020 telugu-boy 
 
This program is free software: you can redistribute it and/or modify 
it under the terms of the GNU Lesser General Public License as published by 
the Free Software Foundation, either version 3 of the License, or 
(at your option) any later version. 
 
This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
GNU Lesser General Public License for more details. 
 
You should have received a copy of the GNU Lesser General Public License 
along with this program.  If not, see <http://www.gnu.org/licenses/>. 
]]
cmake_minimum_required(VERSION 3.2)

# edit this to wherever your vcpkg is
set(VCPKG_DIR ../vcpkg CACHE PATH "path to vcpkg root")
set(VCPKG_TRIPLET x64-linux CACHE PATH "installed img lib triplet")

set(CMAKE_TOOLCHAIN_FILE ${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake)

set(AUTODETECT_ENABLED ON CACHE BOOL "Enable automatic detection of images. Requires ALL format modules.")

if(NOT AUTODETECT_ENABLED)
    set(PNG_ENABLED ON CACHE BOOL "Enable PNG")
    set(JPEG_ENABLED ON CACHE BOOL "Enable JPEG")
    set(WEBP_ENABLED ON CACHE BOOL "Enable WebP")
    set(TIFF_ENABLED ON CACHE BOOL "Enable TIFF")
    set(FARBFELD_ENABLED ON CACHE BOOL "Enable Farbfeld")
    set(IRDUMP_ENABLED OFF CACHE BOOL "Enable IR dumping")
endif()

set(OPENCL_ENABLED OFF CACHE BOOL "Enable OpenCL filters")

project(seedimg_full VERSION 1.0 DESCRIPTION "seedimg module based image manipulation library")

option(BUILD_TESTING "" OFF)
include(CTest)
include(CheckCXXCompilerFlag)

include_directories(${VCPKG_DIR}/installed/${VCPKG_TRIPLET}/include "${CMAKE_SOURCE_DIR}/include")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(BUILD_SHARED_LIBS TRUE)
set(LIB_TYPE STATIC CACHE STRING "Build and link as STATIC or SHARED")

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


add_subdirectory(src)

# tests
if (${BUILD_TESTING})
    add_subdirectory(tests)
endif()

# dependent project
add_subdirectory(example)



