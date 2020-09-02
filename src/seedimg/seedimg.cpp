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
#include <seedimg.hpp>

namespace seedimg {

simg_int round_up(simg_int inp, simg_int mul) noexcept {
  if (mul == 0)
    return inp;

  simg_int remainder = inp % mul;
  if (remainder == 0)
    return inp;

  return inp + mul - remainder;
}

bool is_on_rect(seedimg::point xy1, seedimg::point xy2,
                seedimg::point point) noexcept {
  return xy1.first <= point.first && point.first <= xy2.first &&
         xy1.second <= point.second && point.second <= xy2.second;
}

std::pair<simg_int, simg_int> get_rect_dimensions(seedimg::point p1,
                                                  seedimg::point p2) noexcept {
  auto ordered_x = std::minmax(p1.first, p2.first);
  auto ordered_y = std::minmax(p1.second, p2.second);
  // width, height
  return {ordered_x.second - ordered_x.first,
          ordered_y.second - ordered_y.first};
}

// create shared ptrs from certain suitable params
std::shared_ptr<seedimg::img> make(simg_int width, simg_int height) {
  return std::make_shared<seedimg::img>(width, height);
}

std::shared_ptr<seedimg::img>
make(const std::shared_ptr<seedimg::img> &inp_img) {
  return std::make_shared<seedimg::img>(*inp_img);
}

} // namespace seedimg
