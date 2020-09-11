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

#include <cmath>
#include <seedimg-filters/seedimg-filters-cconv.hpp>
#include <seedimg-utils.hpp>

void hsv2rgb_worker(simg &inp_img, simg &res_img, simg_int start,
                    simg_int end) {
  for (; start < end; start++) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start);
      float C = static_cast<float>(pix.v) / 100.0f +
                static_cast<float>(pix.s) / 100.0f;
      float X =
          C *
          (1 -
           std::fabs(std::fmod(static_cast<float>(pix.h) / 3000.0f, 2.0f) - 1));

      float m = static_cast<float>(pix.v) - C;

      float componentsp[4] =
    }
  }
}
}

namespace seedimg::filters {
namespace cconv {
void rgb(simg &inp_img, simg &res_img) {
  if (inp_img->colourspace() == seedimg::colourspaces::rgb) {
    return;
  } else if (inp_img->colourspace() == seedimg::colourspaces::hsv) {
  }
}
void rgb_i(simg &inp_img) {
  if (inp_img->colourspace() == seedimg::colourspaces::rgb) {
    return;
  }
}
} // namespace cconv
} // namespace seedimg::filters
