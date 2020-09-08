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
find_package(WEBP QUIET)
if(NOT WEBP_FOUND)
    ExternalProject_Add(libwebp-1.1.0
        URL https://github.com/webmproject/libwebp/archive/v1.1.0.zip
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../extern-src/libwebp-src
        BINARY_DIR ${CMAKE_BINARY_DIR}/extern/libwebp
        INSTALL_COMMAND ""
        CMAKE_ARGS  ${CL_ARGS} -DCMAKE_BUILD_TYPE=Release
        -DWEBP_BUILD_GIF2WEBP=OFF
        -DWEBP_BUILD_IMG2WEBP=OFF
        -DWEBP_BUILD_VWEBP=OFF
        -DWEBP_BUILD_WEBPINFO=OFF
        -DWEBP_BUILD_WEBPMUX=OFF
        -DWEBP_BUILD_EXTRAS=OFF
        -DWEBP_BUILD_WEBP_JS=OFF
        PREFIX ${CMAKE_BINARY_DIR}/extern/libwebp
    )

    ExternalProject_Get_Property(libwebp-1.1.0 install_dir)
    ExternalProject_Get_Property(libwebp-1.1.0 SOURCE_DIR)

    add_library(libwebp STATIC IMPORTED)
    add_library(libwebpdemux STATIC IMPORTED)
    add_library(libwebpdecoder STATIC IMPORTED)

    set(WEBP_LIBRARIES libwebp libwebpdemux libwebpdecoder pthread)
    set(WEBP_INCLUDE_DIRS ${SOURCE_DIR}/src)

    set_property(TARGET libwebp PROPERTY IMPORTED_LOCATION ${install_dir}/${prefix}webp${suffix})
    set_property(TARGET libwebpdemux PROPERTY IMPORTED_LOCATION ${install_dir}/${prefix}webpdemux${suffix})
    set_property(TARGET libwebpdecoder PROPERTY IMPORTED_LOCATION ${install_dir}/${prefix}webpdecoder${suffix})


    add_dependencies(libwebp libwebp-1.1.0 pthread)
    add_dependencies(libwebpdemux libwebp-1.1.0 pthread)
    add_dependencies(libwebpdecoder libwebp-1.1.0 pthread)
endif()



add_library(seedimg-webp ${LIB_TYPE} seedimg-webp.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(seedimg-webp PRIVATE ${WEBP_LIBRARIES} pthread seedimg)
else()
    target_link_libraries(seedimg-webp PRIVATE ${WEBP_LIBRARIES} pthread seedimg stdc++fs)
endif()

target_include_directories(seedimg-webp PRIVATE ${WEBP_INCLUDE_DIRS} seedimg)
