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

#ifndef SIMG_OCL_PXAMT
#define SIMG_OCL_PXAMT 1
#endif

#ifndef SIMG_OCL_LOCAL_WG_SIZE
#define SIMG_OCL_LOCAL_WG_SIZE 64
#endif

#define SIMG_OCL_BUF_PADDING                                                   \
  sizeof(seedimg::pixel) * SIMG_OCL_LOCAL_WG_SIZE *SIMG_OCL_PXAMT

#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 100
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 100
#endif

#ifndef NDEBUG
#include <iostream>
#endif

#include <CL/cl2.hpp>

#include <seedimg-filters/seedimg-filters-core.hpp>
#include <seedimg-utils.hpp>

#include <chrono>
#include <functional>
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

namespace seedimg::filters {
namespace ocl {
// this is the default callback function is recommended to be executed when no
// customization is needed. Definition of exec_ocl_callback is at end of file.
inline void write_img_1d(cl::CommandQueue &queue, simg &inp_img,
                         cl::Buffer &inp_img_buf, bool blocking = false) {
  cl_uchar4 *inp = static_cast<cl_uchar4 *>(queue.enqueueMapBuffer(
      inp_img_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
      seedimg::utils::round_up(sizeof(seedimg::pixel) * inp_img->width() *
                                   inp_img->height(),
                               SIMG_OCL_BUF_PADDING)));
  std::memcpy(inp, inp_img->data(),
              sizeof(seedimg::pixel) * inp_img->width() * inp_img->height());
  queue.enqueueUnmapMemObject(inp_img_buf, inp);
  if (blocking)
    queue.finish();
}

inline void read_img_1d(cl::CommandQueue &queue, simg &res_img,
                        cl::Buffer &res_img_buf, bool blocking = false) {
  cl_uchar4 *res = static_cast<cl_uchar4 *>(queue.enqueueMapBuffer(
      res_img_buf, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
      seedimg::utils::round_up(sizeof(seedimg::pixel) * res_img->width() *
                                   res_img->height(),
                               SIMG_OCL_BUF_PADDING)));
  std::memcpy(res_img->data(), res,
              sizeof(seedimg::pixel) * res_img->width() * res_img->height());
  queue.enqueueUnmapMemObject(res_img_buf, res);
  if (blocking)
    queue.finish();
}

inline void init_ocl_singleton(std::size_t plat, std::size_t dev) {
  simgdetails::ocl_singleton::instance(plat, dev);
}
inline cl::Context &get_context() {
  return simgdetails::ocl_singleton::instance().context;
}
inline cl::CommandQueue &get_queue() {
  return simgdetails::ocl_singleton::instance().queue;
}
inline cl::Kernel &get_kernel(const std::string &kernel_name) {
  return simgdetails::ocl_singleton::instance().kernels.at(kernel_name);
}

// To avoid code duplication, this takes a callback that enqueues an execution
// and allows customization while reducing code duplication.
// All arguments must be passed in the order of which the callback AND kernel
// accepts them or else undefined behavior
void default_exec_callback(cl::CommandQueue &queue, cl::Kernel &kern,
                           std::size_t amt, bool blocking, ...) {

  std::cout << "start" << std::endl;
  for (int i = 0; i < 60; i++) {
    std::cout << i << std::endl;
    queue.enqueueNDRangeKernel(kern, cl::NullRange, cl::NDRange(amt),
                               cl::NDRange(SIMG_OCL_LOCAL_WG_SIZE));
    queue.finish();
  }
  std::cout << "end" << std::endl;
  if (blocking)
    queue.finish();

  /*
    queue.enqueueNDRangeKernel(kern, cl::NullRange, cl::NDRange(amt),
                               cl::NDRange(64));
    if (blocking)
      queue.finish();*/
} // namespace ocl

template <typename Func, typename... Args>
void exec_ocl_callback_1d(simg &inp_img, simg &res_img, cl::Buffer *inp_buf,
                          cl::Buffer *res_buf, const std::string &kernel_name,
                          Func &&callback, Args &&... kernel_args) {
  namespace ch = std::chrono;
  const auto &context = get_context();
  auto &queue = get_queue();

  // create buffers on device (allocate space on GPU)
  cl::Buffer *inp_img_buf = inp_buf;
  if (inp_buf == nullptr) {
    inp_img_buf = new cl::Buffer{
        context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
        seedimg::utils::round_up(sizeof(seedimg::pixel) * inp_img->width() *
                                     inp_img->height(),
                                 SIMG_OCL_BUF_PADDING)};
  }

  cl::Buffer *res_img_buf = inp_img_buf;
  if (res_buf != nullptr) {
    res_img_buf = res_buf;
  }

  auto start = ch::steady_clock::now();
  write_img_1d(queue, inp_img, *inp_img_buf);
  queue.finish();
  auto end = ch::steady_clock::now();

  std::cout << "write:"
            << ch::duration_cast<ch::milliseconds>(end - start).count()
            << std::endl;

  cl::Kernel &kern = get_kernel(kernel_name);

  cl_uint i = 0;
  // this sets all the arguments passed to the kernel with a fold expression,
  (kern.setArg(i++, std::forward<Args>(kernel_args)), ...);
  // sets the last two parameters as input and res. so, ALL kernels' last
  // arguments must be pointers to input image data and output.
  kern.setArg(i++, *inp_img_buf);
  kern.setArg(i, *res_img_buf);

  start = ch::steady_clock::now();
  // and then calls the callback with whatever parameters were passed.
  std::invoke(callback, queue, kern,
              seedimg::utils::round_up(
                  inp_img->width() * inp_img->height(),
                  static_cast<std::size_t>(SIMG_OCL_LOCAL_WG_SIZE)),
              false, std::forward<Args>(kernel_args)...);
  queue.finish();

  end = ch::steady_clock::now();
  std::cout << "exec:"
            << ch::duration_cast<ch::milliseconds>(end - start).count()
            << std::endl;

  start = ch::steady_clock::now();
  read_img_1d(queue, res_img, *res_img_buf);
  queue.finish();
  end = ch::steady_clock::now();
  std::cout << "read:"
            << ch::duration_cast<ch::milliseconds>(end - start).count()
            << std::endl;

  if (inp_buf == nullptr)
    delete inp_img_buf;
}

// same with filters-core. sepia and rotate_hue internally call this function.
inline void apply_mat(simg &inp_img, simg &res_img, const fsmat &mat,
                      cl::Buffer *inp_buf = nullptr,
                      cl::Buffer *res_buf = nullptr) {
  cl_float16 matvec;
  for (std::size_t i = 0; i < 16; i++)
    matvec.s[i] = mat[i];

  exec_ocl_callback_1d(inp_img, res_img, inp_buf, res_buf, "apply_mat",
                       default_exec_callback, matvec);
}

inline void apply_mat_i(simg &inp_img, const fsmat &mat,
                        cl::Buffer *inp_buf = nullptr,
                        cl::Buffer *res_buf = nullptr) {
  apply_mat(inp_img, inp_img, mat, inp_buf, res_buf);
}

// stupid autoformatter keeps ruining my perfect alignment
inline void apply_mat(simg &inp_img, simg &res_img, const smat &mat,
                      cl::Buffer *inp_buf = nullptr,
                      cl::Buffer *res_buf = nullptr) {
  apply_mat(inp_img, res_img, to_fsmat(mat), inp_buf, res_buf);
}
inline void apply_mat_i(simg &inp_img, const smat &mat,
                        cl::Buffer *inp_buf = nullptr) {
  apply_mat(inp_img, inp_img, mat, inp_buf, inp_buf);
}

inline void grayscale(simg &inp_img, simg &res_img,
                      cl::Buffer *inp_buf = nullptr,
                      cl::Buffer *res_buf = nullptr) {
  apply_mat(inp_img, res_img, GRAYSCALE_LUM_MAT, inp_buf, res_buf);
}
inline void grayscale_i(simg &inp_img, cl::Buffer *inp_buf = nullptr) {
  grayscale(inp_img, inp_img, inp_buf, inp_buf);
}

inline void sepia(simg &inp_img, simg &res_img, cl::Buffer *inp_buf = nullptr,
                  cl::Buffer *res_buf = nullptr) {
  apply_mat(inp_img, res_img, SEPIA_MAT, inp_buf, res_buf);
}
inline void sepia_i(simg &inp_img, cl::Buffer *inp_buf = nullptr) {
  sepia(inp_img, inp_img, inp_buf, inp_buf);
}

inline void rotate_hue(simg &inp_img, simg &res_img, float angle,
                       cl::Buffer *inp_buf = nullptr,
                       cl::Buffer *res_buf = nullptr) {
  apply_mat(inp_img, res_img, generate_hue_mat(angle), inp_buf, res_buf);
}

inline void rotate_hue_i(simg &inp_img, float angle,
                         cl::Buffer *inp_buf = nullptr) {
  rotate_hue(inp_img, inp_img, angle, inp_buf, inp_buf);
}

namespace cconv {
inline void rgb(simg &inp_img, simg &res_img, cl::Buffer *inp_buf = nullptr,
                cl::Buffer *res_buf = nullptr) {
  if (inp_img->colourspace() == seedimg::colourspaces::rgb)
    return;
  else if (inp_img->colourspace() != seedimg::colourspaces::hsv)
    throw std::invalid_argument("Colourspace is not HSV");

  exec_ocl_callback_1d(inp_img, res_img, inp_buf, res_buf, "hsv2rgb",
                       default_exec_callback);

  static_cast<seedimg::uimg *>(res_img.get())
      ->set_colourspace(seedimg::colourspaces::rgb);
}

inline void rgb_i(simg &inp_img, cl::Buffer *inp_buf = nullptr) {
  rgb(inp_img, inp_img, inp_buf, inp_buf);
}

inline void hsv(simg &inp_img, simg &res_img, cl::Buffer *inp_buf = nullptr,
                cl::Buffer *res_buf = nullptr) {
  if (inp_img->colourspace() == seedimg::colourspaces::hsv)
    return;
  else if (inp_img->colourspace() != seedimg::colourspaces::rgb)
    throw std::invalid_argument("Colourspace is not RGB");

  exec_ocl_callback_1d(inp_img, res_img, inp_buf, res_buf, "rgb2hsv",
                       default_exec_callback);

  static_cast<seedimg::uimg *>(res_img.get())
      ->set_colourspace(seedimg::colourspaces::hsv);
}

inline void hsv_i(simg &inp_img, cl::Buffer *inp_buf = nullptr) {
  hsv(inp_img, inp_img, inp_buf, inp_buf);
}
} // namespace cconv
} // namespace ocl
} // namespace seedimg::filters

#endif
