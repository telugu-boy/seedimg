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

#define CL_TARGET_OPENCL_VERSION 110

// Standard library to make some things easier.
#include <cmath>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <vector>

#include <CL/cl.hpp>

#include "seedimg-filters-ocl.hpp"
#include <seedimg/seedimg.hpp>

#define BUFFER_ENTRY_COUNT 256

using namespace std;

const double PI = 4 * std::atan(1);

simg_int round_up(simg_int inp, simg_int mul) {
  if (mul == 0)
    return inp;

  simg_int remainder = inp % mul;
  if (remainder == 0)
    return inp;

  return inp + mul - remainder;
}

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
  // get all platforms (drivers), e.g. NVIDIA
  std::vector<cl::Platform> all_platforms;
  cl::Platform::get(&all_platforms);

  if (all_platforms.size() == 0) {
    std::cout << " No platforms found. Check OpenCL installation!\n";
    exit(1);
  }
  cl::Platform default_platform = all_platforms[0];
  std::cout << "Using platform: "
            << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

  // get default device (CPUs, GPUs) of the default platform
  std::vector<cl::Device> all_devices;
  default_platform.getDevices(CL_DEVICE_TYPE_GPU, &all_devices);
  if (all_devices.size() == 0) {
    std::cerr << " No GPUs found. Check OpenCL installation!\n";
    exit(1);
  }
  cl::Device default_device = all_devices[0];
  cl::Context context({default_device});
  cl::Program::Sources sources;

  // calculates for each element; C = A + B
  std::string kernel_code =
#include "cl_kernels/rotate_hue_kernel.hpp"
      ;

  sources.push_back({kernel_code.c_str(), kernel_code.length()});

  cl::Program program(context, sources);
  if (program.build({default_device}) != CL_SUCCESS) {
    std::cout << "Error building: "
              << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device)
              << std::endl;
    exit(1);
  }

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
  cl::CommandQueue queue(context, default_device);

  // push write commands to queue
  queue.enqueueWriteBuffer(hue_kernel_buf, CL_TRUE, 0, sizeof(float) * 9,
                           hue_kernel);

  cl_int err;

  cl::Kernel rotate_hue(program, "rotate_hue");
  rotate_hue.setArg(0, hue_kernel_buf);
  rotate_hue.setArg(1, inp_img_buf);
  rotate_hue.setArg(2, res_img_buf);
  err = queue.enqueueNDRangeKernel(
      rotate_hue, cl::NullRange,
      cl::NDRange(round_up(inp_img->width() * inp_img->width(), 128)),
      cl::NDRange(128));
  err = queue.finish();

  // read result from GPU to here
  err = queue.enqueueReadBuffer(res_img_buf, CL_TRUE, 0,
                                sizeof(seedimg::pixel) * res_img->width() *
                                    res_img->height(),
                                res_img->data());
}
} // namespace seedimg::filters::ocl
