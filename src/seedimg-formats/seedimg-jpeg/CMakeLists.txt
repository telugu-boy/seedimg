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
add_library(seedimg-jpeg ${LIB_TYPE} seedimg-jpeg.cpp)

ExternalProject_Add(libjpeg-turbo-2.0.5
    URL https://sourceforge.net/projects/libjpeg-turbo/files/2.0.5/libjpeg-turbo-2.0.5.tar.gz/download
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libjpeg-turbo-src/
    BINARY_DIR ${CMAKE_BINARY_DIR}/extern/libjpeg-turbo
    INSTALL_COMMAND ""
    PREFIX ${CMAKE_BINARY_DIR}/extern/libjpeg-turbo
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release"
)

ExternalProject_Get_Property(libjpeg-turbo-2.0.5 install_dir)

add_library(libjpeg-turbo STATIC IMPORTED)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set_property(TARGET libjpeg-turbo PROPERTY IMPORTED_LOCATION ${install_dir}/turbojpeg-static.lib)
    target_link_libraries(seedimg-jpeg PRIVATE libjpeg-turbo seedimg)
else()
    set_property(TARGET libjpeg-turbo PROPERTY IMPORTED_LOCATION ${install_dir}/libturbojpeg.a)
    target_link_libraries(seedimg-jpeg PRIVATE libjpeg-turbo seedimg stdc++fs)
endif()

target_include_directories(seedimg-jpeg PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libjpeg-turbo-2.0.5/ seedimg)
