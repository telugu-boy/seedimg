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

#ifndef SEEDIMG_OCL_SINGLETON_H
#define SEEDIMG_OCL_SINGLETON_H

#include <CL/cl.hpp>
#include <iostream>

class ocl_singleton {
public:
  std::vector<cl::Platform> all_platforms;
  cl::Platform platform;
  std::vector<cl::Device> all_devices;
  cl::Context context;
  cl::Device device;
  cl::Program::Sources sources;
  cl::Program program;

  static ocl_singleton &instance(std::size_t plat = 0, std::size_t dev = 0) {
    static ocl_singleton singleton(plat, dev);
    return singleton;
  }
  ocl_singleton(ocl_singleton const &) = delete;
  void operator=(ocl_singleton const &) = delete;

private:
  ocl_singleton(std::size_t plat, std::size_t dev) {
    // get all platforms (drivers), e.g. NVIDIA
    cl::Platform::get(&all_platforms);
    if (all_platforms.size() == 0) {
      throw std::runtime_error("No platforms found");
    }
    platform = all_platforms[plat];
    std::cout << "Using platform: " << platform.getInfo<CL_PLATFORM_NAME>()
              << "\n";

    platform.getDevices(CL_DEVICE_TYPE_GPU, &all_devices);
    if (all_devices.size() == 0) {
      throw std::runtime_error("No GPUs found");
    }
    device = all_devices[dev];
    context = {device};

    const std::string kernels[] = {
#include "cl_kernels/rotate_hue_kernel.clh"
    };

    for (auto &kernel : kernels)
      sources.push_back({kernel.c_str(), kernel.length()});

    program = {context, sources};
    if (program.build({device}) != CL_SUCCESS) {
      std::cout << "Error building: "
                << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)
                << std::endl;
      exit(1);
    }
  }
};

#endif
