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

namespace cl {
class Buffer;
}
#include <seedimg-filters/seedimg-filters-core.hpp>
#include <seedimg.hpp>

namespace seedimg::filters {
namespace ocl {
void init_ocl_singleton(std::size_t plat, std::size_t dev);

// same with filters-core. sepia and rotate_hue internally call this function.
void apply_mat(simg &inp_img, simg &res_img, const fsmat &mat,
               cl::Buffer *inp_buf = nullptr, cl::Buffer *res_buf = nullptr);
void apply_mat_i(simg &inp_img, const fsmat &mat,
                 cl::Buffer *inp_buf = nullptr);
void apply_mat(simg &inp_img, simg &res_img, const smat &mat,
               cl::Buffer *inp_buf = nullptr, cl::Buffer *res_buf = nullptr);
void apply_mat_i(simg &inp_img, const smat &mat, cl::Buffer *inp_buf = nullptr);

void rotate_hue(simg &inp_img, simg &res_img, float angle,
                cl::Buffer *inp_buf = nullptr, cl::Buffer *res_buf = nullptr);
void rotate_hue_i(simg &inp_img, float angle, cl::Buffer *inp_buf = nullptr);

void sepia(simg &inp_img, simg &res_img, cl::Buffer *inp_buf = nullptr,
           cl::Buffer *res_buf = nullptr);
void sepia_i(simg &inp_img, cl::Buffer *inp_buf = nullptr);

void grayscale(simg &inp_img, simg &res_img, cl::Buffer *inp_buf = nullptr,
               cl::Buffer *res_buf = nullptr);
void grayscale_i(simg &inp_img, cl::Buffer *inp_buf = nullptr);

namespace cconv {
void rgb(simg &inp_img, simg &res_img, cl::Buffer *inp_buf = nullptr,
         cl::Buffer *res_buf = nullptr);
void rgb_i(simg &inp_img, cl::Buffer *inp_buf = nullptr);

void hsv(simg &inp_img, simg &res_img, cl::Buffer *inp_buf = nullptr,
         cl::Buffer *res_buf = nullptr);
void hsv_i(simg &inp_img, cl::Buffer *inp_buf = nullptr);
} // namespace cconv

} // namespace ocl
} // namespace seedimg::filters

#endif
