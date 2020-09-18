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

#include <cmath>

#include <CL/cl.hpp>

#include "ocl-singleton.hpp"
#include <seedimg-filters/seedimg-filters-ocl.hpp>

// static constexpr smat const SEPIA_MAT = {
//    .393f, .349f, .272f, .769f, .686f, .534f, .189f, .168f, .131f};

namespace seedimg::filters {
namespace ocl {
void apply_mat(simg &inp_img, simg &res_img, const fsmat &mat) {
  auto context = ocl_singleton::instance().context;
  auto device = ocl_singleton::instance().device;
  auto sources = ocl_singleton::instance().sources;
  auto program = ocl_singleton::instance().program;

  cl_float16 matvec;
  for (std::size_t i = 0; i < 16; i++)
    matvec.s[i] = mat[i];

  cl::Buffer inp_img_buf{context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                         sizeof(seedimg::pixel) * inp_img->width() *
                             inp_img->height(),
                         inp_img->data()};
  cl::Buffer res_img_buf{context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                         sizeof(seedimg::pixel) * res_img->width() *
                             res_img->height(),
                         res_img->data()};

  cl::CommandQueue queue(context, device);

  write_img_1d(queue, inp_img, inp_img_buf);

  cl::Kernel apply_mat_ocl(program, "apply_mat");

  apply_mat_ocl.setArg(0, matvec);
  apply_mat_ocl.setArg(1, inp_img_buf);
  apply_mat_ocl.setArg(2, res_img_buf);
  apply_mat_ocl.setArg(3, inp_img->width() * inp_img->height());
  queue.enqueueNDRangeKernel(
      apply_mat_ocl, cl::NullRange,
      cl::NDRange(round_up(inp_img->width() * inp_img->height(), 128)),
      cl::NDRange(128));
  queue.finish();

  read_img_1d(queue, res_img, res_img_buf);
}

void apply_mat_i(simg &inp_img, const fsmat &mat) {
  apply_mat(inp_img, inp_img, mat);
}

// stupid autoformatter keeps ruining my perfect alignment
void apply_mat(simg &inp_img, simg &res_img, const smat &mat) {
  apply_mat(inp_img, res_img, to_fsmat(mat));
}
void apply_mat_i(simg &inp_img, const smat &mat) {
  apply_mat(inp_img, inp_img, mat);
}

void sepia(simg &inp_img, simg &res_img) {
  apply_mat(inp_img, res_img, SEPIA_MAT);
}
void sepia_i(simg &inp_img) { sepia(inp_img, inp_img); }

void rotate_hue(simg &inp_img, simg &res_img, int angle) {
  apply_mat(inp_img, res_img, generate_hue_mat(angle));
}

void rotate_hue_i(simg &inp_img, int angle) {
  rotate_hue(inp_img, inp_img, angle);
}
} // namespace ocl
} // namespace seedimg::filters
