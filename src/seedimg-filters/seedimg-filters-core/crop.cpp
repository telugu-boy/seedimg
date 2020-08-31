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

#include <algorithm>
#include <cstring>
#include <seedimg-filters/seedimg-filters-core.hpp>
#include <seedimg.hpp>

namespace seedimg::filters {
bool crop(simg &inp_img, simg &res_img, seedimg::point p1, seedimg::point p2) {
  if (p1 == seedimg::point{0, 0} &&
      p2 == seedimg::point{inp_img->width(), inp_img->height()}) {
    return true;
  }
  if (!(seedimg::is_on_rect({0, 0}, {inp_img->width(), inp_img->height()},
                            p1) &&
        seedimg::is_on_rect({0, 0}, {inp_img->width(), inp_img->height()}, p2)))
    return false;
  auto ordered_crop_x = std::minmax(p1.first, p2.first);
  auto ordered_crop_y = std::minmax(p1.second, p2.second);

  for (simg_int y = 0; y < res_img->height(); ++y) {
    std::memcpy(res_img->row(y),
                inp_img->row(y + ordered_crop_y.first) + ordered_crop_x.first,
                res_img->width() * sizeof(seedimg::pixel));
  }
  return true;
}
} // namespace seedimg::filters
