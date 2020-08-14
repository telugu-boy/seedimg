/**********************************************************************
seedimg -
        module based image manipulation library written in modern
            C++ Copyright(C) 2020 telugu
        -
        boy

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
  std::cout << "current path is " << std::filesystem::current_path()
            << std::endl;

  auto a = seedimg_autodetect_from("cat.png");
  if (a != nullptr) {
    // a->crop({100, 100}, {250, 250});
    // seedimg::filters::grayscale(a, true);
    // seedimg::filters::invert(a);

    // applies a approixmate gaussian blurring matrix retrieved from:
    // https://en.wikipedia.org/wiki/Kernel_(image_processing)#Details
    seedimg::filters::convolution(a, {{  0.0625, 0.125, 0.0625 },
                                      {  0.125,  0.25,  0.125  },
                                      {  0.0625, 0.125, 0.0625 }});
    seedimg_autodetect_to("boileur_gblur.png", a);
  } else
    std::cout << "failed" << std::endl;
  std::cout << "done" << std::endl;
}
