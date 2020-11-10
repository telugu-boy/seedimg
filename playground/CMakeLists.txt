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

set(APP_EXE playground)

file(COPY jat.jpg jat.ff DESTINATION "${CMAKE_BINARY_DIR}/${APP_EXE}/")

add_executable(${APP_EXE} main.cpp)

target_include_directories(${APP_EXE} PRIVATE ${SIMG_ALL_IMGFMT_INCLUDE_DIRS})
target_link_libraries(${APP_EXE} PRIVATE ${SIMG_ALL_IMGFMT_LIBRARIES} stdc++fs)

if(OPENCL_TESTS_ENABLED)
    find_package(OpenCL REQUIRED)
    target_link_libraries(${APP_EXE} PRIVATE ${OpenCL_LIBRARY})
    target_compile_definitions(${APP_EXE} PUBLIC -DSEEDIMG_TESTS_OCL)
endif()
