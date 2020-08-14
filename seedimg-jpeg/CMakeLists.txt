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
add_library(seedimg-jpeg SHARED seedimg-jpeg.cpp)

find_package(JPEG REQUIRED)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(seedimg-jpeg PRIVATE ${JPEG_LIBRARIES} seedimg)
else()
    target_link_libraries(seedimg-jpeg PRIVATE ${JPEG_LIBRARIES} seedimg stdc++fs)
endif()

target_include_directories(seedimg-jpeg PRIVATE ${JPEG_INCLUDE_DIR} seedimg)

