/***********************************************************************
    seedimg - module based image manipulation library written in modern C++
    Copyright (C) 2020 telugu-boy, tripulse

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
#ifndef SEEDIMG_FILTERS_OCL_H
#define SEEDIMG_FILTERS_OCL_H

#include <seedimg-filters/seedimg-filters-core.hpp>
#include <seedimg.hpp>

namespace seedimg::filters {
namespace ocl {
void init_ocl_singleton(std::size_t plat, std::size_t dev);

void rotate_hue(simg &inp_img, simg &res_img, int angle);
void rotate_hue_i(simg &inp_img, int angle);

void grayscale(simg &inp_img, simg &res_img, int angle);
void grayscale_i(simg &inp_img, int angle);

} // namespace ocl
} // namespace seedimg::filters

#endif
