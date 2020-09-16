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

# It is MANDATORY to build in-source, until we can run a `cmake .` in the downloaded folder
# regardless whether or not cmake is in PATH. This is because jconfig is generated at config time.

find_package(JPEG)
if(NOT JPEG_FOUND)
    ExternalProject_Add(libjpeg-turbo-2.0.5
        URL https://sourceforge.net/projects/libjpeg-turbo/files/2.0.5/libjpeg-turbo-2.0.5.tar.gz/download
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../extern-src/libjpeg-turbo-src
        PREFIX ${CMAKE_BINARY_DIR}/extern/libjpeg-turbo
        BINARY_DIR ${CMAKE_BINARY_DIR}/extern/libjpeg-turbo
        INSTALL_COMMAND ""
        CMAKE_ARGS ${CL_ARGS} -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DBUILD_SHARED=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    )

    ExternalProject_Get_Property(libjpeg-turbo-2.0.5 install_dir)
    ExternalProject_Get_Property(libjpeg-turbo-2.0.5 SOURCE_DIR)

    add_library(libjpeg-turbo STATIC IMPORTED)

    set(JPEG_LIBRARY libjpeg-turbo)
    set(JPEG_INCLUDE_DIR ${SOURCE_DIR})

    set(JPEG_LIBRARIES libjpeg-turbo)
    set(JPEG_INCLUDE_DIRS ${SOURCE_DIR})
    set(JPEG_CONF_INCLUDE_DIR ${install_dir})

    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set_property(TARGET libjpeg-turbo PROPERTY IMPORTED_LOCATION ${install_dir}/turbojpeg-static.lib)
    else()
        set_property(TARGET libjpeg-turbo PROPERTY IMPORTED_LOCATION ${install_dir}/libturbojpeg.a)
    endif()


    add_dependencies(libjpeg-turbo libjpeg-turbo-2.0.5)
endif()



add_library(seedimg-jpeg ${LIB_TYPE} seedimg-jpeg.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(seedimg-jpeg PRIVATE ${JPEG_LIBRARIES} seedimg)
else()
    target_link_libraries(seedimg-jpeg PRIVATE ${JPEG_LIBRARIES} seedimg stdc++fs)
endif()

target_include_directories(seedimg-jpeg PRIVATE ${JPEG_INCLUDE_DIRS} ${JPEG_CONF_INCLUDE_DIR})
