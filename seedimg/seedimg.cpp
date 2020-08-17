/**********************************************************************
    seedimg - module based image manipulation library written in modern
                C++ Copyright(C) 2020 telugu-boy

    This program is free software : you can redistribute it and /
    or modify it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation,
    either version 3 of the License,
    or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/
// seedimg.cpp : Defines the functions for the static library.
//

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>

#include "seedimg.hpp"

inline bool is_on_rect(seedimg::point xy1, seedimg::point xy2,
                       seedimg::point point) {
  return xy1.first <= point.first && point.first <= xy2.first &&
         xy1.second <= point.second && point.second <= xy2.second;
}

namespace seedimg {
bool img::crop(seedimg::point p1, seedimg::point p2) {
  if (p1 == seedimg::point{0, 0} &&
      p2 == seedimg::point{this->width_, this->height_}) {
    return true;
  }
  if (!(is_on_rect({0, 0}, {this->width_, this->height_}, p1) &&
        is_on_rect({0, 0}, {this->width_, this->height_}, p2)))
    return false;
  auto ordered_crop_x = std::minmax(p1.first, p2.first);
  auto ordered_crop_y = std::minmax(p1.second, p2.second);
  simg_int init_height = this->height_;
  this->width_ = ordered_crop_x.second - ordered_crop_x.first;
  this->height_ = ordered_crop_y.second - ordered_crop_y.first;

  for (simg_int y = 0; y < this->height_; ++y) {
    std::memmove(this->row(y),
                 this->row(ordered_crop_y.first + y) + ordered_crop_x.first,
                 static_cast<std::size_t>(this->width_) *
                     sizeof(seedimg::pixel));

    auto tmp = reinterpret_cast<seedimg::pixel *>(
        std::realloc(this->data_[y], static_cast<std::size_t>(this->width_) *
                                         sizeof(seedimg::pixel)));
    if (tmp)
      this->data_[y] = tmp;
    else
      return false;
  }
  // must free vertically resized memory
  for (simg_int y = this->height_; y < init_height; ++y)
    std::free(this->data_[y]);
  auto tmp = reinterpret_cast<seedimg::pixel **>(
      std::realloc(this->data_, static_cast<std::size_t>(this->height_) *
                                    sizeof(seedimg::pixel *)));
  if (tmp)
    this->data_ = tmp;
  else
    return false;
  return true;
}
}
