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

#include <seedimg-autodetect.hpp>
#include <seedimg-filters/seedimg-filters-core.hpp>
#include <seedimg-filters/seedimg-filters-ocl.hpp>

int main() {
  using namespace seedimg::filters;
  std::cout << "Current path is " << std::filesystem::current_path()
            << std::endl;
  {
    // ocl::init_ocl_singleton(1, 0);
    auto a = seedimg::load("cat.png");
    // auto res_img = seedimg::make(a->width(), a->height());
    if (a != nullptr) {
      // crop_i(a, {122, 166}, {244, 332});
      // grayscale_i(a, true);
      // invert_i(a);
      // blur_i(a, 10);
      // h_blur_i(a, 10);
      // v_blur_i(a, 100, 1);
      // convolution(a, {{-1, 1, -1}, {-2, 4, -2}, {-1, 1, -1}});
      // rotate_hue_i(a, 180);
      v_mirror_i(a);
      // ocl::rotate_hue_i(a, -90);
      seedimg::save("biol.jpg", a);
      // bool b = seedimg::modules::jpeg::to("biol.jpg", a, 1);
    } else {
      std::cerr << "failed" << std::endl;
    }
    std::cerr << "done" << std::endl;
  }
}
