/**********************************************************************
seedimg - module based image manipulation library written in modern
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
#include <iostream>
#include <iomanip>

#include <seedimg-subimage.hpp>
#include <seedimg-autodetect.hpp>

void print_pixel(seedimg::pixel p) {
    std::cout << '#'
              << std::hex
              << std::setw(2)

              << int(p.r)
              << int(p.g)
              << int(p.b)
              << int(p.a)
              << std::endl;
}

int main() {
    auto im      = seedimg::load("cat.jpg");
    auto im_view = im->sub(0, 0, 4, 4)
                      .sub(0, 0, 2, 2)
                      .sub(0, 0, 1, 1);

    print_pixel(im_view.pixel(1, 1));
    print_pixel(im->pixel(1, 1));
}
