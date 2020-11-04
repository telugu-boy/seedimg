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

#ifndef SEEDIMG_OCL_SINGLETON
#define SEEDIMG_OCL_SINGLETON

#include <CL/cl2.hpp>
#include <string>
#include <unordered_map>

namespace simgdetails {
class ocl_singleton {
public:
  std::vector<cl::Platform> all_platforms;
  cl::Platform platform;
  std::vector<cl::Device> all_devices;
  cl::Context context;
  cl::Device device;
  cl::Program::Sources sources;
  cl::Program program;
  cl::CommandQueue queue;

  std::unordered_map<std::string, cl::Kernel> kernels;

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
#ifndef NDEBUG
    std::cout << "Using platform: " << platform.getInfo<CL_PLATFORM_NAME>()
              << std::endl;
#endif

    platform.getDevices(CL_DEVICE_TYPE_GPU, &all_devices);
    if (all_devices.size() == 0) {
      throw std::runtime_error("No GPUs found");
    }
    device = all_devices[dev];
    context = {device};

    queue = {context, device};

    static constexpr const char *const kernels_src[] = {
#include "cl_kernels/apply_mat_kernel.clh"
        ,
#include "cl_kernels/rgb2hsv_kernel.clh"
        ,
#include "cl_kernels/hsv2rgb_kernel.clh"
        ,
    };

    static constexpr const char *const kernels_names[]{"apply_mat", "rgb2hsv",
                                                       "hsv2rgb"};

    // didn't use emplace back because it's slower as they're not
    // large. std::pair<const char*, ::size_t> is the definition of
    // cl::Program::Sources.
    for (auto &kernel : kernels_src)
      sources.push_back({kernel, std::strlen(kernel)});

    program = {context, sources};
    if (program.build({device}) != CL_SUCCESS) {
      throw std::runtime_error(
          "Error building: " +
          program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device));
    }

    for (auto &name : kernels_names) {
      kernels[name] = {program, name};
    }
  }
};
} // namespace simgdetails
#endif
