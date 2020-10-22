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

add_executable(${APP_EXE} main.cpp)

file(COPY jat.ff DESTINATION "${CMAKE_BINARY_DIR}/${APP_EXE}/")

target_link_libraries(${APP_EXE} seedimg
    seedimg-farbfeld
    seedimg-irdump
    seedimg-filters-core
    stdc++fs)
#target_include_directories(${APP_EXE} PRIVATE "${CMAKE_SOURCE_DIR}/seedimg")
