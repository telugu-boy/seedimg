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
#include <filesystem>
#include <iostream>
#include <seedimg-autodetect/seedimg-autodetect.hpp>
#include <seedimg-filters-core/seedimg-filters-core.hpp>
#include <seedimg/seedimg.hpp>

int main() {
  std::cout << "Current path is " << std::filesystem::current_path()
            << std::endl;
  {
    auto a = seedimg_autodetect_from("cat.png");
    auto res_img = seedimg::make(a->width(), a->height());
    if (a != nullptr) {
      // seedimg::filters::crop_i(a, {122, 166}, {244, 332});
      // seedimg::filters::grayscale_i(a, true);
      // seedimg::filters::invert_i(a);
      // seedimg::filters::blur(a, 10);
      // seedimg::filters::h_blur(a, 10);
      // seedimg::filters::v_blur(a, 10);
      // seedimg::filters::convolution(a, {{0, -1, 0}, {-1, 5, -1}, {0, -1,
      // 0}});
      // seedimg::filters::rotate_hue_i(a, 180);
      // seedimg::filters::h_mirror_i(a);
      bool b = seedimg_autodetect_to("biol.jpg", a);
      // bool b = seedimg::modules::jpeg::to("biol.jpg", a, 1);
    } else {
      std::cerr << "failed" << std::endl;
    }
    std::cerr << "done" << std::endl;
  }
}
