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

find_package(JPEG)
if(NOT JPEG_FOUND)
    FetchContent_Declare(
        libjpeg-turbo
        GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
        GIT_TAG 2.0.5
    )

    FetchContent_GetProperties(libjpeg-turbo)
    if(NOT libjpeg-turbo_POPULATED)
      FetchContent_Populate(libjpeg-turbo)
    endif()

    cmake_policy(SET CMP0054 OLD)

    set(ENABLE_STATIC ON)
    set(ENABLE_SHARED OFF)
    add_subdirectory(${libjpeg-turbo_SOURCE_DIR} ${libjpeg-turbo_BINARY_DIR})
    set_property(TARGET turbojpeg-static PROPERTY POSITION_INDEPENDENT_CODE ON)
    set(JPEG_LIBRARIES turbojpeg-static)
    set(JPEG_INCLUDE_DIRS ${libjpeg-turbo_SOURCE_DIR} ${libjpeg-turbo_BINARY_DIR})
endif()


add_library(seedimg-jpeg ${LIB_TYPE} seedimg-jpeg.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(seedimg-jpeg PRIVATE ${JPEG_LIBRARIES} seedimg)
else()
    target_link_libraries(seedimg-jpeg PRIVATE ${JPEG_LIBRARIES} seedimg stdc++fs)
endif()

target_include_directories(seedimg-jpeg PRIVATE ${JPEG_INCLUDE_DIRS})
