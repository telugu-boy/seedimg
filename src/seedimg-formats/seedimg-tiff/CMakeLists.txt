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


find_package(TIFF)
if(NOT TIFF_FOUND)
    FetchContent_Declare(
        libtiff
        URL https://download.osgeo.org/libtiff/tiff-4.1.0.zip
    )

    FetchContent_GetProperties(libtiff)
    if(NOT libtiff_POPULATED)
      FetchContent_Populate(libtiff)
    endif()

    set(BUILD_SHARED_LIBS OFF)

    #set(zlib OFF)
    #set(pixarlog OFF)
    #set(jpeg OFF)
    #set(old-jpeg OFF)
    #set(jbig OFF)
    #set(lzma OFF)
    #set(zstd OFF)
    #set(webp OFF)
    #set(jpeg12 OFF)

    cmake_policy(SET CMP0072 OLD)
    cmake_policy(SET CMP0048 OLD)
    add_subdirectory(${libtiff_SOURCE_DIR} ${libtiff_BINARY_DIR})
    set_property(TARGET tiff PROPERTY POSITION_INDEPENDENT_CODE ON)
    set(TIFF_LIBRARIES tiff)
    set(TIFF_INCLUDE_DIRS ${libtiff_SOURCE_DIR}/libtiff ${libtiff_BINARY_DIR})
endif()



#execute_process(COMMAND ${CMAKE_COMMAND} .)

add_library(seedimg-tiff ${LIB_TYPE} seedimg-tiff.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(seedimg-tiff PRIVATE ${TIFF_LIBRARIES})
else()
    target_link_libraries(seedimg-tiff PRIVATE ${TIFF_LIBRARIES})
endif()

target_include_directories(seedimg-tiff PRIVATE ${TIFF_INCLUDE_DIRS})
