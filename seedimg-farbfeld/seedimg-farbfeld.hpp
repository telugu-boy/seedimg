/*********************************************************************** 
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
************************************************************************/ 
#ifndef SEEDIMG_FARBFELD_HPP
#define SEEDIMG_FARBFELD_HPP

#include <seedimg/seedimg.hpp>

namespace seedimg::modules {
namespace farbfeld {
bool check(const std::string &filename);
bool to(const std::string &filename,
        const std::unique_ptr<seedimg::img> &inp_img);
std::unique_ptr<seedimg::img> from(const std::string &filename);
} // namespace farbfeld
} // namespace seedimg::modules

#endif
