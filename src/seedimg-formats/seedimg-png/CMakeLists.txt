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

set(LIBPNG_CMAKE_ARGS ${CL_ARGS} -DCMAKE_BUILD_TYPE=Release -DPNG_SHARED=FALSE -DBUILD_TESTING=OFF -DPNG_TESTS=OFF)

find_package(ZLIB)
if(NOT ZLIB_FOUND)
ExternalProject_Add(zlib-1.2.11
    URL https://zlib.net/zlib-1.2.11.tar.xz
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../extern-src/zlib-src
    PREFIX ${CMAKE_BINARY_DIR}/extern/zlib
    BINARY_DIR ${CMAKE_BINARY_DIR}/extern/zlib
    INSTALL_COMMAND ""
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF"
)

ExternalProject_Get_Property(zlib-1.2.11 install_dir)
ExternalProject_Get_Property(zlib-1.2.11 SOURCE_DIR)

add_library(zlib STATIC IMPORTED)

set(ZLIB_LIBRARIES zlib)
set(ZLIB_INCLUDE_DIRS ${SOURCE_DIR})

set_property(TARGET zlib PROPERTY IMPORTED_LOCATION ${install_dir}/${prefix}z${suffix})

add_dependencies(zlib zlib-1.2.11)

list(APPEND LIBPNG_CMAKE_ARGS -DPNG_BUILD_ZLIB=ON)

endif()


find_package(PNG)
if(NOT PNG_FOUND)
    ExternalProject_Add(libpng-1.6.37
        URL https://sourceforge.net/projects/libpng/files/libpng16/1.6.37/libpng-1.6.37.tar.xz/download
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../extern-src/libpng-src
        PREFIX ${CMAKE_BINARY_DIR}/extern/libpng
        BINARY_DIR ${CMAKE_BINARY_DIR}/extern/libpng
        INSTALL_COMMAND ""
        CMAKE_ARGS LIBPNG_CMAKE_ARGS
    )

    ExternalProject_Get_Property(libpng-1.6.37 install_dir)
    ExternalProject_Get_Property(libpng-1.6.37 SOURCE_DIR)

    add_library(libpng STATIC IMPORTED)

    set(PNG_LIBRARIES libpng)
    set(PNG_INCLUDE_DIRS ${SOURCE_DIR})
    set(PNG_CONF_INCLUDE_DIR ${install_dir})

    set_property(TARGET libpng PROPERTY IMPORTED_LOCATION ${install_dir}/${prefix}png${suffix})

    add_dependencies(libpng libpng-1.6.37)
endif()




add_library(seedimg-png ${LIB_TYPE} seedimg-png.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(seedimg-png PRIVATE ${PNG_LIBRARIES} ${ZLIB_LIBRARIES} seedimg)
else()
    target_link_libraries(seedimg-png PRIVATE ${PNG_LIBRARIES} ${ZLIB_LIBRARIES} seedimg stdc++fs)
endif()

target_include_directories(seedimg-png PRIVATE ${PNG_INCLUDE_DIRS} ${PNG_CONF_INCLUDE_DIR})
