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

#include <seedimg-filters/seedimg-filters-ocl.hpp>

// static constexpr smat const SEPIA_MAT = {
//    .393f, .349f, .272f, .769f, .686f, .534f, .189f, .168f, .131f};

namespace seedimg::filters {
namespace ocl {
void apply_mat(simg& inp_img, simg& res_img, const fsmat& mat, cl::Buffer* inp_buf, cl::Buffer* res_buf) {

    cl_float16 matvec;
    for (std::size_t i = 0; i < 16; i++) matvec.s[i] = mat[i];

    exec_ocl_callback(inp_img, res_img, inp_buf, res_buf, "apply_mat", default_exec_callback,
                      inp_img->width() * inp_img->height(), matvec);
}

void apply_mat_i(simg& inp_img, const fsmat& mat, cl::Buffer* inp_buf, cl::Buffer* res_buf) {
    apply_mat(inp_img, inp_img, mat, inp_buf, res_buf);
}

// stupid autoformatter keeps ruining my perfect alignment
void apply_mat(simg& inp_img, simg& res_img, const smat& mat, cl::Buffer* inp_buf, cl::Buffer* res_buf) {
    apply_mat(inp_img, res_img, to_fsmat(mat), inp_buf, res_buf);
}
void apply_mat_i(simg& inp_img, const smat& mat, cl::Buffer* inp_buf) {
    apply_mat(inp_img, inp_img, mat, inp_buf, inp_buf);
}

void grayscale(simg& inp_img, simg& res_img, cl::Buffer* inp_buf, cl::Buffer* res_buf) {
    apply_mat(inp_img, res_img, GRAYSCALE_LUM_MAT, inp_buf, res_buf);
}
void grayscale_i(simg& inp_img, cl::Buffer* inp_buf) { grayscale(inp_img, inp_img, inp_buf, inp_buf); }

void sepia(simg& inp_img, simg& res_img, cl::Buffer* inp_buf, cl::Buffer* res_buf) {
    apply_mat(inp_img, res_img, SEPIA_MAT, inp_buf, res_buf);
}
void sepia_i(simg& inp_img, cl::Buffer* inp_buf) { sepia(inp_img, inp_img, inp_buf, inp_buf); }

void rotate_hue(simg& inp_img, simg& res_img, float angle, cl::Buffer* inp_buf, cl::Buffer* res_buf) {
    apply_mat(inp_img, res_img, generate_hue_mat(angle), inp_buf, res_buf);
}

void rotate_hue_i(simg& inp_img, float angle, cl::Buffer* inp_buf) {
    rotate_hue(inp_img, inp_img, angle, inp_buf, inp_buf);
}
} // namespace ocl
} // namespace seedimg::filters
