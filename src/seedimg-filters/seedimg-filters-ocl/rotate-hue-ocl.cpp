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

using namespace std;

const double PI = 4 * std::atan(1);

void get_hue_kernel(int angle, float hue_kernel[9]) {
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
}

namespace seedimg::filters::ocl {
void rotate_hue(simg &inp_img, simg &res_img, int angle) {
  auto context = ocl_singleton::instance().context;
  auto device = ocl_singleton::instance().device;
  auto sources = ocl_singleton::instance().sources;
  auto program = ocl_singleton::instance().program;

  // create buffers on device (allocate space on GPU)
  float hue_kernel[9];
  get_hue_kernel(angle, hue_kernel);

  // create buffers on device (allocate space on GPU)
  cl::Buffer hue_kernel_buf{context, CL_MEM_READ_ONLY, sizeof(float) * 9};
  cl::Buffer inp_img_buf{context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                         sizeof(seedimg::pixel) * inp_img->width() *
                             inp_img->height(),
                         inp_img->data()};
  cl::Buffer res_img_buf{context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                         sizeof(seedimg::pixel) * res_img->width() *
                             res_img->height(),
                         res_img->data()};

  // create a queue (a queue of commands that the GPU will execute)
  cl::CommandQueue queue(context, device);

  // push write commands to queue
  queue.enqueueWriteBuffer(hue_kernel_buf, CL_TRUE, 0, sizeof(float) * 9,
                           hue_kernel);

  cl_uchar4 *inp = static_cast<cl_uchar4 *>(queue.enqueueMapBuffer(
      inp_img_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
      sizeof(seedimg::pixel) * inp_img->width() * inp_img->height()));
  std::memcpy(inp, inp_img->data(),
              sizeof(seedimg::pixel) * inp_img->width() * inp_img->height());
  queue.enqueueUnmapMemObject(inp_img_buf, inp);

  cl::Kernel rotate_hue_ocl(program, "rotate_hue");

  rotate_hue_ocl.setArg(0, hue_kernel_buf);
  rotate_hue_ocl.setArg(1, inp_img_buf);
  rotate_hue_ocl.setArg(2, res_img_buf);
  rotate_hue_ocl.setArg(3, inp_img->width() * inp_img->height());
  queue.enqueueNDRangeKernel(
      rotate_hue_ocl, cl::NullRange,
      cl::NDRange(round_up(inp_img->width() * inp_img->height(), 128)),
      cl::NDRange(128));
  queue.finish();

  cl_uchar4 *res = static_cast<cl_uchar4 *>(queue.enqueueMapBuffer(
      res_img_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
      sizeof(seedimg::pixel) * res_img->width() * res_img->height()));
  std::memcpy(res_img->data(), res,
              sizeof(seedimg::pixel) * inp_img->width() * inp_img->height());
  queue.enqueueUnmapMemObject(res_img_buf, res);
}

void rotate_hue_i(simg &inp_img, int angle) {
  rotate_hue(inp_img, inp_img, angle);
}
} // namespace seedimg::filters::ocl
