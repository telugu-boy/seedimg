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

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}")

find_package(WebP)
if(NOT WebP_FOUND)
    FetchContent_Declare(
        libwebp
        URL https://github.com/webmproject/libwebp/archive/v1.1.0.zip
    )

    FetchContent_GetProperties(libwebp)
    if(NOT libwebp_POPULATED)
      FetchContent_Populate(libwebp)
    endif()

    set(BUILD_SHARED_LIBS OFF)

    set(WEBP_BUILD_GIF2WEBP OFF)
    set(WEBP_BUILD_IMG2WEBP OFF)
    set(WEBP_BUILD_VWEBP OFF)
    set(WEBP_BUILD_WEBPINFO OFF)
    set(WEBP_BUILD_WEBPMUX OFF)
    set(WEBP_BUILD_EXTRAS OFF)
    set(WEBP_BUILD_WEBP_JS OFF)

    #cmake_policy(SET CMP0072 OLD)
    #cmake_policy(SET CMP0048 OLD)
    add_subdirectory(${libwebp_SOURCE_DIR} ${libwebp_BINARY_DIR})
    set_property(TARGET webp PROPERTY POSITION_INDEPENDENT_CODE ON)
    set(WebP_LIBRARIES webp)
    set(WebP_INCLUDE_DIRS ${libwebp_SOURCE_DIR}/src ${libwebp_BINARY_DIR})
endif()



add_library(seedimg-webp ${LIB_TYPE} seedimg-webp.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(seedimg-webp PRIVATE ${WebP_LIBRARIES})
else()
    target_link_libraries(seedimg-webp PRIVATE ${WebP_LIBRARIES})
endif()

target_include_directories(seedimg-webp PRIVATE ${WebP_INCLUDE_DIRS} seedimg)
