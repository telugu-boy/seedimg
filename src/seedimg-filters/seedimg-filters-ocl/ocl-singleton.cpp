/**********************************************************************
    seedimg - module based image manipulation library written in modern
                C++ Copyright(C) 2020 telugu-boy

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

#include "ocl-singleton.hpp"

#define SEEDIMG_DEBUG_
#ifdef SEEDIMG_DEBUG_
#include <iostream>
#endif

void write_img_1d(cl::CommandQueue &queue, simg &inp_img,
                  cl::Buffer &inp_img_buf) {
  cl_uchar4 *inp = static_cast<cl_uchar4 *>(queue.enqueueMapBuffer(
      inp_img_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
      sizeof(seedimg::pixel) * inp_img->width() * inp_img->height()));
  std::memcpy(inp, inp_img->data(),
              sizeof(seedimg::pixel) * inp_img->width() * inp_img->height());
  queue.enqueueUnmapMemObject(inp_img_buf, inp);
  queue.finish();
}

void read_img_1d(cl::CommandQueue &queue, simg &res_img,
                 cl::Buffer &res_img_buf) {
  cl_uchar4 *res = static_cast<cl_uchar4 *>(queue.enqueueMapBuffer(
      res_img_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
      sizeof(seedimg::pixel) * res_img->width() * res_img->height()));
  std::memcpy(res_img->data(), res,
              sizeof(seedimg::pixel) * res_img->width() * res_img->height());
  queue.enqueueUnmapMemObject(res_img_buf, res);
  queue.finish();
}

ocl_singleton &ocl_singleton::instance(std::size_t plat, std::size_t dev) {
  static ocl_singleton singleton(plat, dev);
  return singleton;
}

ocl_singleton::ocl_singleton(std::size_t plat, std::size_t dev) {
  // get all platforms (drivers), e.g. NVIDIA
  cl::Platform::get(&all_platforms);
  if (all_platforms.size() == 0) {
    throw std::runtime_error("No platforms found");
  }
  platform = all_platforms[plat];
#ifdef SEEDIMG_DEBUG_
  std::cout << "Using platform: " << platform.getInfo<CL_PLATFORM_NAME>()
            << std::endl;
#endif

  platform.getDevices(CL_DEVICE_TYPE_GPU, &all_devices);
  if (all_devices.size() == 0) {
    throw std::runtime_error("No GPUs found");
  }
  device = all_devices[dev];
  context = {device};

  static constexpr const char *const kernels[] = {
#include "cl_kernels/filters/apply_mat_kernel.clh"
      ,
#include "cl_kernels/filters/grayscale_lum_kernel.clh"
      ,
#include "cl_kernels/filters/grayscale_avg_kernel.clh"
      ,
#include "cl_kernels/cconv/rgb2hsv_kernel.clh"
      ,
#include "cl_kernels/cconv/hsv2rgb_kernel.clh"
      ,
  };

  // didn't use emplace back because it's slower as they're not
  // large. std::pair<const char*, ::size_t> is the definition of
  // cl::Program::Sources.
  for (auto &kernel : kernels)
    sources.push_back({kernel, std::strlen(kernel)});

  program = {context, sources};
  if (program.build({device}) != CL_SUCCESS) {
    throw std::runtime_error(
        "Error building: " +
        program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device));
  }
}
