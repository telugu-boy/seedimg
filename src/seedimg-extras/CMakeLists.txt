#[[
seedimg - module based image manipulation library written in modern C++
Copyright (C) 2020 tripulse, telugu-boy

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

add_library(seedimg-extras ${LIB_TYPE} seedimg-extras.cpp)

target_link_libraries(seedimg-extras PRIVATE seedimg)
target_include_directories(seedimg-extras PRIVATE seedimg)
 
