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

#include <CL/cl.hpp>

#include "ocl-singleton.hpp"
#include <seedimg-filters/seedimg-filters-ocl.hpp>

namespace seedimg::filters {
namespace ocl {
void grayscale(simg &inp_img, simg &res_img, bool luminosity) {
  auto context = ocl_singleton::instance().context;
  auto device = ocl_singleton::instance().device;
  auto sources = ocl_singleton::instance().sources;
  auto program = ocl_singleton::instance().program;

  // create buffers on device (allocate space on GPU)
  cl::Buffer inp_img_buf{context, CL_MEM_READ_WRITE,
                         sizeof(seedimg::pixel) * inp_img->width() *
                             inp_img->height()};
  cl::Buffer res_img_buf{context, CL_MEM_READ_WRITE,
                         sizeof(seedimg::pixel) * res_img->width() *
                             res_img->height()};

  // create a queue (a queue of commands that the GPU will execute)
  cl::CommandQueue queue(context, device);
  cl_uchar4 *inp = static_cast<cl_uchar4 *>(queue.enqueueMapBuffer(
      inp_img_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
      sizeof(seedimg::pixel) * inp_img->width() * inp_img->height()));
  std::memcpy(inp, inp_img->data(),
              sizeof(seedimg::pixel) * inp_img->width() * inp_img->height());
  queue.enqueueUnmapMemObject(inp_img_buf, inp);
  /*
  queue.enqueueWriteBuffer(inp_img_buf, CL_TRUE, 0,
                           sizeof(seedimg::pixel) * inp_img->width() *
                               inp_img->height(),
                           inp_img->data());*/

  cl::Kernel grayscale;
  if (luminosity)
    grayscale = {program, "grayscale_lum"};
  else
    grayscale = {program, "grayscale_avg"};

  grayscale.setArg(0, inp_img_buf);
  grayscale.setArg(1, res_img_buf);
  grayscale.setArg(2, inp_img->width() * inp_img->height());
  queue.enqueueNDRangeKernel(
      grayscale, cl::NullRange,
      cl::NDRange(round_up(inp_img->width() * inp_img->height(), 128)),
      cl::NDRange(128));
  queue.finish();

  cl_uchar4 *res = static_cast<cl_uchar4 *>(queue.enqueueMapBuffer(
      res_img_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
      sizeof(seedimg::pixel) * res_img->width() * res_img->height()));
  std::memcpy(res_img->data(), res,
              sizeof(seedimg::pixel) * inp_img->width() * inp_img->height());
  queue.enqueueUnmapMemObject(res_img_buf, res);
  /*
  queue.enqueueReadBuffer(res_img_buf, CL_TRUE, 0,
                          sizeof(seedimg::pixel) * res_img->width() *
                              res_img->height(),
                          res_img->data());*/
}

void grayscale_i(simg &inp_img, bool luminosity) {
  grayscale(inp_img, inp_img, luminosity);
}
} // namespace ocl
} // namespace seedimg::filters
