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
void rgb(simg &inp_img, simg &res_img) {
  if (inp_img->colourspace() == seedimg::colourspaces::rgb)
    return;
  else if (inp_img->colourspace() != seedimg::colourspaces::hsv)
    throw std::invalid_argument("Colourspace is not HSV");

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

  write_img_1d(queue, inp_img, inp_img_buf);

  cl::Kernel hsv2rgb;
  hsv2rgb = {program, "hsv2rgb"};

  hsv2rgb.setArg(0, inp_img_buf);
  hsv2rgb.setArg(1, res_img_buf);
  hsv2rgb.setArg(2, inp_img->width() * inp_img->height());
  queue.enqueueNDRangeKernel(
      hsv2rgb, cl::NullRange,
      cl::NDRange(round_up(inp_img->width() * inp_img->height(), 128)),
      cl::NDRange(128));
  queue.finish();

  read_img_1d(queue, res_img, res_img_buf);
  static_cast<seedimg::uimg *>(res_img.get())
      ->set_colourspace(seedimg::colourspaces::rgb);
}

void rgb_i(simg &inp_img) { rgb(inp_img, inp_img); }
} // namespace cconv
} // namespace seedimg::filters::ocl
