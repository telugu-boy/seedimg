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

ExternalProject_Add(libtiff-4.1.0
    URL https://download.osgeo.org/libtiff/tiff-4.1.0.zip
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libtiff-src
    PREFIX ${CMAKE_BINARY_DIR}/extern/libtiff
    BINARY_DIR ${CMAKE_BINARY_DIR}/extern/libtiff
    INSTALL_COMMAND ""
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release"
)

ExternalProject_Get_Property(libtiff-4.1.0 install_dir)

add_library(libtiff SHARED IMPORTED)

set(TIFF_LIBRARIES libtiff)
set(TIFF_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libtiff-src/libtiff)
set(TIFF_CONF_INCLUDE_DIR ${install_dir}/libtiff)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set_property(TARGET libtiff PROPERTY IMPORTED_LOCATION ${install_dir}/libtiff/libtiff.so)
else()
    set_property(TARGET libtiff PROPERTY IMPORTED_LOCATION ${install_dir}/libtiff/libtiff.so)
endif()

add_dependencies(libtiff libtiff-4.1.0)



#execute_process(COMMAND ${CMAKE_COMMAND} .)

add_library(seedimg-tiff ${LIB_TYPE} seedimg-tiff.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(seedimg-tiff PRIVATE ${TIFF_LIBRARIES} seedimg)
else()
    target_link_libraries(seedimg-tiff PRIVATE ${TIFF_LIBRARIES} seedimg stdc++fs)
endif()

target_include_directories(seedimg-tiff PRIVATE ${TIFF_INCLUDE_DIR} ${TIFF_CONF_INCLUDE_DIR})
