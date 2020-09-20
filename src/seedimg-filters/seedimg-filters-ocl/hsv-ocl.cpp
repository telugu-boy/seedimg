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

namespace seedimg::filters::ocl {
namespace cconv {
void hsv(simg &inp_img, simg &res_img, cl::Buffer *inp_buf,
         cl::Buffer *res_buf) {
  if (inp_img->colourspace() == seedimg::colourspaces::hsv)
    return;
  else if (inp_img->colourspace() != seedimg::colourspaces::rgb)
    throw std::invalid_argument("Colourspace is not RGB");

  const auto &context = ocl_singleton::instance().context;
  const auto &device = ocl_singleton::instance().device;
  const auto &program = ocl_singleton::instance().program;

  // create buffers on device (allocate space on GPU)
  cl::Buffer inp_img_buf;
  if (inp_buf == nullptr) {
    inp_img_buf = {context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                   sizeof(seedimg::pixel) * inp_img->width() *
                       inp_img->height(),
                   inp_img->data()};
  } else {
    inp_img_buf = *inp_buf;
  }
  cl::Buffer &res_img_buf = inp_img_buf;
  if (inp_img != res_img) {
    if (res_img == nullptr) {
      res_img_buf = {context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                     sizeof(seedimg::pixel) * res_img->width() *
                         res_img->height()};
    } else {
      res_img_buf = *res_buf;
    }
  }

  // create a queue (a queue of commands that the GPU will execute)
  cl::CommandQueue queue(context, device);

  write_img_1d(queue, inp_img, inp_img_buf);

  cl::Kernel rgb2hsv;
  rgb2hsv = {program, "rgb2hsv"};

  rgb2hsv.setArg(0, inp_img_buf);
  rgb2hsv.setArg(1, res_img_buf);
  rgb2hsv.setArg(2, inp_img->width() * inp_img->height());
  queue.enqueueNDRangeKernel(
      rgb2hsv, cl::NullRange,
      cl::NDRange(round_up(inp_img->width() * inp_img->height(), 128)),
      cl::NDRange(128));
  queue.finish();

  read_img_1d(queue, res_img, res_img_buf);
  static_cast<seedimg::uimg *>(res_img.get())
      ->set_colourspace(seedimg::colourspaces::hsv);
} // namespace cconv

void hsv_i(simg &inp_img, cl::Buffer *inp_buf) {
  hsv(inp_img, inp_img, inp_buf, inp_buf);
}
} // namespace cconv
} // namespace seedimg::filters::ocl
