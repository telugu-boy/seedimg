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
#include <algorithm>
#include <cstring>
#include <seedimg.hpp>

inline bool is_on_rect(seedimg::point xy1, seedimg::point xy2,
                       seedimg::point point) {
  return xy1.first <= point.first && point.first <= xy2.first &&
         xy1.second <= point.second && point.second <= xy2.second;
}

std::pair<simg_int, simg_int> get_rect_dimensions(seedimg::point p1,
                                                  seedimg::point p2) {
  auto ordered_x = std::minmax(p1.first, p2.first);
  auto ordered_y = std::minmax(p1.second, p2.second);
  // width, height
  return {ordered_x.second - ordered_x.first,
          ordered_y.second - ordered_y.first};
}

namespace seedimg::filters {
bool crop(simg &inp_img, simg &res_img, seedimg::point p1, seedimg::point p2) {
  if (p1 == seedimg::point{0, 0} &&
      p2 == seedimg::point{inp_img->width(), inp_img->height()}) {
    return true;
  }
  if (!(is_on_rect({0, 0}, {inp_img->width(), inp_img->height()}, p1) &&
        is_on_rect({0, 0}, {inp_img->width(), inp_img->height()}, p2)))
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

bool crop_i(simg &inp_img, seedimg::point p1, seedimg::point p2) {
  auto dims = get_rect_dimensions(p1, p2);
  auto res_img = seedimg::make(dims.first, dims.second);
  bool result = crop(inp_img, res_img, p1, p2);
  if (result)
    inp_img.reset(res_img.release());
  return result;
}
} // namespace seedimg::filters
