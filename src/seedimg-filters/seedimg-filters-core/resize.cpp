/***********************************************************************
    seedimg - module based image manipulation library written in modern C++
    Copyright (C) 2020 tripulse

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

#include <seedimg-filters/seedimg-filters-core.hpp>

namespace seedimg::filters {
void resize(simg& input, simg& output) {
    if(input->width()  == output->width() &&
       input->height() == output->height()) {
        output = seedimg::make(input);
    }

    // find out the nearest pixel coordinate in input image,
    // from the output's coordinates. this is a very fast
    // and easy algorithm but yields poor quality.
    for(simg_int y = 0; y < output->height(); ++y) {
        for(simg_int x = 0; x < output->width(); ++x) {
            output->pixel(static_cast<simg_int>(x),
                          static_cast<simg_int>(y)) = input->pixel(
                static_cast<simg_int>(static_cast<float>(x)/output->width()  * input->width()),
                static_cast<simg_int>(static_cast<float>(y)/output->height() * input->height()));
        }
    }
}
}
