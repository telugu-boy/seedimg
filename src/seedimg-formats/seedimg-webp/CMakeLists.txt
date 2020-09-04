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
add_library(seedimg-webp ${LIB_TYPE} seedimg-webp.cpp)

ExternalProject_Add(libwebp-1.1.0
    URL https://github.com/webmproject/libwebp/archive/v1.1.0.zip
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libwebp-src/
    BINARY_DIR ${CMAKE_BINARY_DIR}/extern/libwebp
    INSTALL_COMMAND ""
    PREFIX ${CMAKE_BINARY_DIR}/extern/libwebp
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release
    -DWEBP_BUILD_GIF2WEBP=OFF
    -DWEBP_BUILD_IMG2WEBP=OFF
    -DWEBP_BUILD_VWEBP=OFF
    -DWEBP_BUILD_WEBPINFO=OFF
    -DWEBP_BUILD_WEBPMUX=OFF
    -DWEBP_BUILD_EXTRAS=OFF
    -DWEBP_BUILD_WEBP_JS=OFF"
)

ExternalProject_Get_Property(libwebp-1.1.0 install_dir)

add_library(libwebp STATIC IMPORTED)
add_library(libwebpdemux STATIC IMPORTED)
add_library(libwebpdecoder STATIC IMPORTED)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set_property(TARGET libwebp PROPERTY IMPORTED_LOCATION ${install_dir}/libwebp.a)
    set_property(TARGET libwebpdemux PROPERTY IMPORTED_LOCATION ${install_dir}/libwebpdemux.a)
    set_property(TARGET libwebpdecoder PROPERTY IMPORTED_LOCATION ${install_dir}/libwebpdecoder.a)
    target_link_libraries(seedimg-webp PRIVATE libwebp libwebpdemux libwebpdecoder seedimg)
else()
    set_property(TARGET libwebp PROPERTY IMPORTED_LOCATION ${install_dir}/libwebp.a)
    set_property(TARGET libwebpdemux PROPERTY IMPORTED_LOCATION ${install_dir}/libwebpdemux.a)
    set_property(TARGET libwebpdecoder PROPERTY IMPORTED_LOCATION ${install_dir}/libwebpdecoder.a)
    target_link_libraries(seedimg-webp PRIVATE libwebp libwebpdemux libwebpdecoder seedimg stdc++fs)
endif()

target_include_directories(seedimg-webp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libwebp-1.1.0/src/ seedimg)
