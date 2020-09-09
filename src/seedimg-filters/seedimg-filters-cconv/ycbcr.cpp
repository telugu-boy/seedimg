/***********************************************************************
    seedimg - module based image manipulation library written in modern C++
    Copyright (C) 2020 telugu-boy, <contributor name>

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
#include <seedimg-filters/seedimg-filters-cconv.hpp>

namespace seedimg::filters {
namespace cconv {
void ycbcr(const simg &inp_img, const simg &res_img) {
  if (inp_img->colourspace() == seedimg::colourspaces::ycbcr) {
    return;
  }
}
void ycbcr_i(const simg &inp_img) {
  if (inp_img->colourspace() == seedimg::colourspaces::ycbcr) {
    return;
  }
}
} // namespace cconv
} // namespace seedimg::filters
