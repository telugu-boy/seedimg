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

const double PI = 4 * std::atan(1);

static constexpr std::array<float, 9> const SEPIA_MAT = {
    .393f, .769f, .189f, .349f, .686f, .168f, .272f, .534f, .131f,
};

namespace seedimg::filters::ocl {
void apply_mat(simg &inp_img, simg &res_img, const std::array<float, 16> &mat,
               const std::array<float, 4> &vec) {
  auto context = ocl_singleton::instance().context;
  auto device = ocl_singleton::instance().device;
  auto sources = ocl_singleton::instance().sources;
  auto program = ocl_singleton::instance().program;

  cl_float16 matvec;
  cl_float4 vecvec;
  for (std::size_t i = 0; i < 16; i++)
    matvec.s[i] = mat[i];
  for (std::size_t i = 0; i < 4; i++)
    vecvec.s[i] = vec[i];

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
  apply_mat_ocl.setArg(1, vecvec);
  apply_mat_ocl.setArg(2, inp_img_buf);
  apply_mat_ocl.setArg(3, res_img_buf);
  apply_mat_ocl.setArg(4, inp_img->width() * inp_img->height());
  queue.enqueueNDRangeKernel(
      apply_mat_ocl, cl::NullRange,
      cl::NDRange(round_up(inp_img->width() * inp_img->height(), 128)),
      cl::NDRange(128));
  queue.finish();

  read_img_1d(queue, res_img, res_img_buf);
}

void apply_mat_i(simg &inp_img, const std::array<float, 16> &mat,
                 const std::array<float, 4> &vec) {
  apply_mat(inp_img, inp_img, mat, vec);
}

// stupid autoformatter keeps ruining my perfect alignment
void apply_mat(simg &inp_img, simg &res_img, const std::array<float, 9> &mat) {
  apply_mat(inp_img, res_img,
            {
                mat[0],
                mat[1],
                mat[2],
                0.0f,
                mat[3],
                mat[4],
                mat[5],
                0.0f,
                mat[6],
                mat[7],
                mat[8],
                0.0f,
                0.0f,
                0.0f,
                0.0f,
                1.0f,
            });
}
void apply_mat_i(simg &inp_img, const std::array<float, 9> &mat) {
  apply_mat(inp_img, inp_img, mat);
};

void sepia(simg &inp_img, simg &res_img) {
  apply_mat(inp_img, res_img, SEPIA_MAT);
}
void sepia_i(simg &inp_img) { apply_mat_i(inp_img, SEPIA_MAT); }

void rotate_hue(simg &inp_img, simg &res_img, int angle) {
  std::array<float, 9> hue_kernel;
  const float sinr = static_cast<float>(std::sin(angle * PI / 180));
  const float cosr = static_cast<float>(std::cos(angle * PI / 180));
  hue_kernel[0] = 0.213f + cosr * 0.787f - sinr * 0.213f;
  hue_kernel[1] = 0.715f - cosr * 0.715f - sinr * 0.715f;
  hue_kernel[2] = 0.072f - cosr * 0.072f + sinr * 0.928f;
  hue_kernel[3] = 0.213f - cosr * 0.213f + sinr * 0.143f;
  hue_kernel[4] = 0.715f + cosr * 0.285f + sinr * 0.140f;
  hue_kernel[5] = 0.072f - cosr * 0.072f - sinr * 0.283f;
  hue_kernel[6] = 0.213f - cosr * 0.213f - sinr * 0.787f;
  hue_kernel[7] = 0.715f - cosr * 0.715f + sinr * 0.715f;
  hue_kernel[8] = 0.072f + cosr * 0.928f + sinr * 0.072f;
  apply_mat(inp_img, res_img, hue_kernel);
}

void rotate_hue_i(simg &inp_img, int angle) {
  rotate_hue(inp_img, inp_img, angle);
}

} // namespace seedimg::filters::ocl
