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

namespace seedimg::filters {
void rotate_cw(simg &inp_img, simg &res_img) {
  if (inp_img == res_img)
    rotate_cw_i(inp_img);
  for (simg_int y = 0; y < res_img->height(); ++y) {
    for (simg_int x = 0; x < res_img->width(); ++x) {
      res_img->pixel(x, y) = inp_img->pixel(res_img->height() - y - 1, x);
    }
  }
}
void rotate_180(simg &inp_img, simg &res_img) {
  for (simg_int y = 0; y < inp_img->height(); ++y) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      res_img->pixel(x, y) =
          inp_img->pixel(inp_img->width() - x - 1, inp_img->height() - y - 1);
    }
  }
}
void rotate_ccw(simg &inp_img, simg &res_img) {
  if (inp_img == res_img)
    rotate_ccw_i(inp_img);
  for (simg_int y = 0; y < res_img->height(); ++y) {
    for (simg_int x = 0; x < res_img->width(); ++x) {
      res_img->pixel(x, y) = inp_img->pixel(y, x);
    }
  }
}
void rotate_cw_i(simg &inp_img) {
  simg res_img = seedimg::make(inp_img->height(), inp_img->width());
  rotate_cw(inp_img, res_img);
  inp_img.reset(res_img.release());
}
void rotate_180_i(simg &inp_img) {
  // reverse each row
  std::reverse(inp_img->data(),
               inp_img->data() + inp_img->width() * inp_img->height());
}
void rotate_ccw_i(simg &inp_img) {
  simg res_img = seedimg::make(inp_img->height(), inp_img->width());
  rotate_ccw(inp_img, res_img);
  inp_img.reset(res_img.release());
}

void v_mirror(simg &inp_img, simg &res_img) {
  for (simg_int y = 0; y < inp_img->height(); ++y) {
    std::memcpy(res_img->row(res_img->height() - y - 1), inp_img->row(y),
                inp_img->width() * sizeof(seedimg::pixel));
  }
}
void h_mirror(simg &inp_img, simg &res_img) {
  for (simg_int y = 0; y < res_img->height(); ++y) {
    for (simg_int x = 0; x < res_img->width(); ++x) {
      res_img->pixel(inp_img->width() - x - 1, y) = inp_img->pixel(x, y);
    }
  }
}
void v_mirror_i(simg &inp_img) {
  simg res_img = seedimg::make(inp_img->width(), inp_img->height());
  v_mirror(inp_img, res_img);
  inp_img.reset(res_img.release());
}
void h_mirror_i(simg &inp_img) {
  for (simg_int y = 0; y < inp_img->height(); ++y) {
    std::reverse(inp_img->row(y), inp_img->row(y) + inp_img->width());
  }
}
} // namespace seedimg::filters
