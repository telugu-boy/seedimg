/***********************************************************************
    seedimg - module based image manipulation library written in modern C++
    Copyright (C) 2020 tripulse

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

#include <cmath>
#include <seedimg-filters/seedimg-filters-core.hpp>

static inline std::uint8_t diff(std::uint8_t a, std::uint8_t b) {
    return std::max(a, b) - std::min(a, b);
}

namespace seedimg::filters {
void difference  (simg& input, simg& output, simg& other, bool alpha) {
    for(simg_int y = 0; y < input->height(); ++y) {
        if(alpha) {
            for(simg_int x = 0; x < input->width(); ++x) {
                auto p  = input->pixel(x, y);
                auto po = other->pixel(x, y);

                output->pixel(x, y) = {{diff(p.r, po.r)},
                                       {diff(p.g, po.g)},
                                       {diff(p.b, po.b)},
                                        diff(p.b, po.b)};
            }
        } else {
            for(simg_int x = 0; x < input->width(); ++x) {
                auto p  = input->pixel(x, y);
                auto po = other->pixel(x, y);

                output->pixel(x, y) = {{diff(p.r, po.r)},
                                       {diff(p.g, po.g)},
                                       {diff(p.b, po.b)},
                                        0xFF};
            }
        }
    }
}
void difference_i(simg& img,   simg& other, bool alpha) {
    difference(img, img, other, alpha);
}
}
