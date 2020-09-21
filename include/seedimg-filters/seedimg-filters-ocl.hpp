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

#include <CL/cl.hpp>

#include <seedimg-filters/seedimg-filters-core.hpp>

#include <functional>

namespace seedimg::filters {
namespace ocl {

// this is the default callback function is recommended to be executed when no
// customization is needed. Definition of exec_ocl_callback is at end of file.
void default_exec_callback(cl::CommandQueue &queue, cl::Kernel &kern,
                      std::size_t amt, ...);

void write_img_1d(cl::CommandQueue &queue, simg &inp_img,
                  cl::Buffer &inp_img_buf);
void read_img_1d(cl::CommandQueue &queue, simg &res_img,
                 cl::Buffer &res_img_buf);

void init_ocl_singleton(std::size_t plat, std::size_t dev);
cl::Context &get_context();
cl::CommandQueue &get_queue();
cl::Kernel &get_kernel(const std::string &kernel_name);

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

// To avoid code duplication, this takes a callback that enqueues an execution
// and allows customization while reducing code duplication.
// All arguments must be passed in the order of which the callback AND kernel
// accepts them or else undefined behavior
template <typename Func, typename... Args>
void exec_ocl_callback(simg &inp_img, simg &res_img, cl::Buffer *inp_buf,
                       cl::Buffer *res_buf, const std::string &kernel_name,
                       Func &&callback, Args &&... kernel_args) {
  const auto &context = get_context();
  auto &queue = get_queue();

  // create buffers on device (allocate space on GPU)
  cl::Buffer *inp_img_buf = inp_buf;
  if (inp_buf == nullptr) {
    inp_img_buf = new cl::Buffer{
        context,
        CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
        sizeof(seedimg::pixel) * inp_img->width() * inp_img->height(),
    };
  }

  cl::Buffer *res_img_buf = inp_img_buf;
  if (res_buf != nullptr) {
    res_img_buf = res_buf;
  }

  write_img_1d(queue, inp_img, *inp_img_buf);

  cl::Kernel &kern = get_kernel(kernel_name);

  cl_uint i = 0;
  // this sets all the arguments passed to the kernel with a fold expression,
  (kern.setArg(i++, kernel_args), ...);
  // sets the last two parameters as input and res. so, ALL kernels' last
  // arguments must be pointers to input image data and output.
  kern.setArg(i++, *inp_img_buf);
  kern.setArg(i++, *res_img_buf);

  // and then calls the callback with whatever parameters were passed.
  std::invoke(callback, queue, kern, std::forward<Args>(kernel_args)...);

  read_img_1d(queue, res_img, *res_img_buf);
  if (inp_buf == nullptr)
    delete inp_img_buf;
}
} // namespace ocl
} // namespace seedimg::filters

#endif
