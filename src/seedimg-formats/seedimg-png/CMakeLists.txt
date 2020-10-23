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

find_package(ZLIB)
if(NOT ZLIB_FOUND)
    FetchContent_Declare(
        zlib
        URL https://zlib.net/zlib-1.2.11.tar.xz
    )

    FetchContent_GetProperties(zlib)
    if(NOT zlib_POPULATED)
      FetchContent_Populate(zlib)
    endif()

    add_subdirectory(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
    set_property(TARGET zlibstatic PROPERTY POSITION_INDEPENDENT_CODE ON)
    set(ZLIB_LIBRARIES zlibstatic)
    set(ZLIB_INCLUDE_DIRS ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
    set(ZLIB_LIBRARY ${ZLIB_LIBRARIES} CACHE BOOL "" FORCE)
    set(ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_DIRS} CACHE BOOL "" FORCE)
    set(PNG_BUILD_ZLIB ON CACHE BOOL "" FORCE)
endif()


find_package(PNG)
if(NOT PNG_FOUND)
    set(M_LIBRARY m CACHE BOOL "" FORCE)
    FetchContent_Declare(
        libpng
        URL https://sourceforge.net/projects/libpng/files/libpng16/1.6.37/libpng-1.6.37.tar.xz/download
    )

    FetchContent_GetProperties(libpng)
    if(NOT libpng_POPULATED)
      FetchContent_Populate(libpng)
    endif()

    set(PNG_SHARED ON CACHE BOOL "" FORCE)
    set(PNG_TESTS ON CACHE BOOL "" FORCE)

    add_subdirectory(${libpng_SOURCE_DIR} ${libpng_BINARY_DIR})
    set_property(TARGET png_static
        PROPERTY POSITION_INDEPENDENT_CODE ON)
    #target_include_directories(png_static PUBLIC ${ZLIB_INCLUDE_DIRS})
    #target_link_libraries(png_static m)
    set(PNG_LIBRARIES png_static)
    set(PNG_INCLUDE_DIRS ${libpng_SOURCE_DIR} ${libpng_BINARY_DIR})
endif()




add_library(seedimg-png ${LIB_TYPE} seedimg-png.cpp)

target_link_libraries(seedimg-png PRIVATE ${ZLIB_LIBRARIES} ${PNG_LIBRARIES} seedimg)

target_include_directories(seedimg-png PRIVATE ${PNG_INCLUDE_DIRS})
