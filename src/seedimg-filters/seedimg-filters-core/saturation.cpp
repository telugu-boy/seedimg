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
#include <seedimg-utils.hpp>

inline std::uint8_t clamp(int a) {
  if (a > 100)
    return 100;
  else if (a < 0)
    return 0;
  return static_cast<std::uint8_t>(a);
}

void saturation_worker(simg &inp_img, simg &res_img, simg_int start,
                       simg_int end, float mul) {

  for (; start < end; ++start) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      res_img->pixel(x, start).s =
          clamp(static_cast<int>(inp_img->pixel(x, start).s * mul));
    }
  }
}

namespace seedimg::filters {
void saturation(simg &inp_img, simg &res_img, float mul) {
  if (inp_img->colourspace() != seedimg::colourspaces::hsv)
    throw std::invalid_argument("Colourspace is not HSV");
  if (mul < 0)
    return;
  seedimg::utils::hrz_thread(saturation_worker, inp_img, res_img, mul);
}
void saturation_i(simg &inp_img, float mul) {
  saturation(inp_img, inp_img, mul);
}
} // namespace seedimg::filters
