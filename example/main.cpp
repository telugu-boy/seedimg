/**********************************************************************
seedimg -
        module based image manipulation library written in modern
            C++ Copyright(C) 2020 telugu-boy, tripulse

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
#include <seedimg-irdump/irdump.h>

int main() {
  auto inimg = seedimg_autodetect_from("cat.png");
  if (inimg != nullptr) {
    auto ain_img = std::make_unique<seedimg::img>(*inimg);
    seedimg::filters::v_blur(ain_img, 20, 4);
    seedimg::filters::h_blur(ain_img, 10, 8);

    seedimg::filters::blend_i({inimg, 50}, {*ain_img, 50});
    seedimg_autodetect_to("boileur.png", inimg);
  }
}
