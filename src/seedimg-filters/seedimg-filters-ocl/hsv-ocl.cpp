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
#include <seedimg-utils.hpp>

namespace seedimg::filters::ocl {
namespace cconv {
void hsv(simg &inp_img, simg &res_img, cl::Buffer *inp_buf,
         cl::Buffer *res_buf) {
  if (inp_img->colourspace() == seedimg::colourspaces::hsv)
    return;
  else if (inp_img->colourspace() != seedimg::colourspaces::rgb)
    throw std::invalid_argument("Colourspace is not RGB");

  exec_ocl_callback(inp_img, res_img, inp_buf, res_buf, "rgb2hsv",
                    default_exec_callback,
                    inp_img->width() * inp_img->height());

  static_cast<seedimg::uimg *>(res_img.get())
      ->set_colourspace(seedimg::colourspaces::hsv);
}

void hsv_i(simg &inp_img, cl::Buffer *inp_buf) {
  hsv(inp_img, inp_img, inp_buf, inp_buf);
}
} // namespace cconv
} // namespace seedimg::filters::ocl
