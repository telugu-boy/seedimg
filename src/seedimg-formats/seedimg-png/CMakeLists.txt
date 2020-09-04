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

ExternalProject_Add(libpng-1.6.37
    URL https://sourceforge.net/projects/libpng/files/libpng16/1.6.37/libpng-1.6.37.tar.xz/download
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libpng-src
    BINARY_DIR ${CMAKE_BINARY_DIR}/extern/libpng
    INSTALL_COMMAND ""
    PREFIX ${CMAKE_BINARY_DIR}/extern/libpng
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release"
)

ExternalProject_Get_Property(libpng-1.6.37 install_dir)

add_library(libpng STATIC IMPORTED)

set(PNG_LIBRARIES libpng)
set(PNG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libpng-src/)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set_property(TARGET libpng PROPERTY IMPORTED_LOCATION ${install_dir}/turbojpeg-static.lib)
else()
    set_property(TARGET libpng PROPERTY IMPORTED_LOCATION ${install_dir}/libpng.a)
endif()

add_dependencies(libpng libpng-1.6.37)





ExternalProject_Add(zlib-1.2.11
    URL https://zlib.net/zlib-1.2.11.tar.xz
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/zlib-src
    BINARY_DIR ${CMAKE_BINARY_DIR}/extern/zlib
    INSTALL_COMMAND ""
    PREFIX ${CMAKE_BINARY_DIR}/extern/zlib
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release"
)

ExternalProject_Get_Property(zlib-1.2.11 install_dir)

add_library(zlib STATIC IMPORTED)

set(ZLIB_LIBRARIES zlib)
set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/zlib-src/)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set_property(TARGET zlib PROPERTY IMPORTED_LOCATION ${install_dir}/turbojpeg-static.lib)
else()
    set_property(TARGET zlib PROPERTY IMPORTED_LOCATION ${install_dir}/libz.a)
endif()

add_dependencies(zlib zlib-1.2.11)





add_library(seedimg-png ${LIB_TYPE} seedimg-png.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(seedimg-png PRIVATE ${PNG_LIBRARIES} ${ZLIB_LIBRARIES} seedimg)
else()
    target_link_libraries(seedimg-png PRIVATE ${PNG_LIBRARIES} ${ZLIB_LIBRARIES} seedimg stdc++fs)
endif()

target_include_directories(seedimg-png PRIVATE ${PNG_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} seedimg)
