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

#include <seedimg-filters/seedimg-filters-core.hpp>

namespace seedimg::filters {
void apply_mat(simg &inp_img, simg &res_img, simg_int start, simg_int end,
               const std::array<float, 9> &mat,
               const std::array<float, 3> &vec) {
  for (; start < end; ++start) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start);
      res_img->pixel(x, start).y = static_cast<std::uint8_t>(
          vec[0] + mat[0] * pix.r + mat[1] * pix.g + mat[2] * pix.b);
      res_img->pixel(x, start).cb = static_cast<std::uint8_t>(
          vec[1] + mat[3] * pix.r + mat[4] * pix.g + mat[5] * pix.b);
      res_img->pixel(x, start).cr = static_cast<std::uint8_t>(
          vec[2] + mat[6] * pix.r + mat[7] * pix.g + mat[8] * pix.b);
    }
  }
}
} // namespace seedimg::filters
