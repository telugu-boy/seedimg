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

#include <algorithm>
#include <cstring>
#include <exception>
#include <seedimg-filters/seedimg-filters-core.hpp>
#include <seedimg-utils.hpp>
#include <seedimg.hpp>
#include <stdexcept>

namespace seedimg::filters {
bool crop(simg& inp_img, simg& res_img, seedimg::point p1, seedimg::point p2) {
    if (inp_img == res_img) crop_i(inp_img, p1, p2);
    if (p1 == seedimg::point{0, 0} && p2 == seedimg::point{inp_img->width(), inp_img->height()}) { return true; }
    if (!(seedimg::is_on_rect({0, 0}, {inp_img->width(), inp_img->height()}, p1)
          && seedimg::is_on_rect({0, 0}, {inp_img->width(), inp_img->height()}, p2)))
        return false;
    auto least_crop_x = std::min(p1.x, p2.x);
    auto least_crop_y = std::min(p1.y, p2.y);

    for (simg_int y = 0; y < res_img->height(); ++y) {
        std::memcpy(res_img->row(y), inp_img->row(y + least_crop_y) + least_crop_x,
                    res_img->width() * sizeof(seedimg::pixel));
    }
    return true;
}

bool crop_i(simg& inp_img, seedimg::point p1, seedimg::point p2) {
    seedimg::uimg* unmanaged = static_cast<seedimg::uimg*>(inp_img.get());
    if (p1 == seedimg::point{0, 0} && p2 == seedimg::point{unmanaged->width(), unmanaged->height()}) { return true; }
    if (!(seedimg::is_on_rect({0, 0}, {unmanaged->width(), unmanaged->height()}, p1)
          && seedimg::is_on_rect({0, 0}, {unmanaged->width(), unmanaged->height()}, p2)))
        return false;

    auto least_crop_x = std::min(p1.x, p2.x);
    auto least_crop_y = std::min(p1.y, p2.y);

    auto dims = get_rect_dimensions(p1, p2);

    // width is dims.x, height is dims.y.
    for (simg_int y = 0; y < dims.y; ++y) {
        std::memmove(unmanaged->data() + y * dims.x, unmanaged->row(y + least_crop_y) + least_crop_x,
                     dims.x * sizeof(seedimg::pixel));
    }

    unmanaged->set_width(dims.x);
    unmanaged->set_height(dims.y);

    auto* tmp = static_cast<seedimg::pixel*>(
        std::realloc(unmanaged->data(), unmanaged->width() * unmanaged->height() * sizeof(seedimg::pixel)));
    if (tmp == nullptr) throw std::bad_alloc{};
    unmanaged->set_data(tmp);
    return true;
}
} // namespace seedimg::filters
