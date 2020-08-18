/***********************************************************************
seedimg - module based image manipulation library written in modern C++
Copyright (C) 2020 tripulse

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


#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>

#include <seedimg-filters-core/seedimg-filters-core.hpp>
#include <seedimg/seedimg.hpp>


void seedimg::filters::convolution(simg &input,
                                   std::vector<std::vector<float>> kernel) {
  seedimg::point kernel_dims{kernel.size() ? kernel[0].size() : 0,
                             kernel.size()};
  // NOTE: only square aka. NxN kernels are allowed as they're symmetric and
  //       thus very easy to implement, if not then just ignore.
  if (kernel_dims.first != kernel_dims.second || kernel_dims.first == 0)
    return;

  {
    // will be used as the divisor to normalize all the kernel elements.
    auto kernmatrix_normdiv = std::abs(std::accumulate(
      kernel.begin(), kernel.end(), 0.0, [](float s, auto r) -> float {
      return s + std::accumulate(r.begin(), r.end(), 0.0); }));

      for (simg_int r = 0; r < kernel_dims.second; ++r)
        std::transform(kernel[r].begin(), kernel[r].end(), kernel[r].begin(),
          [&](auto elem) -> float { return elem / kernmatrix_normdiv; });
  }

  // the centerpoint of the kernel, where the current pixel is located.
  // kernel { y-axis: [-a..+a], x-axis: [-b..b] }, (a) and (b) are half
  // of kernel's (width) and (height).
  seedimg::point kernel_origin{kernel_dims.first / 2, kernel_dims.second / 2};

  for (simg_int y = 0; y < input->height(); ++y) {
    for (simg_int x = 0; x < input->width(); ++x) {
      // output value to be used as an accumulator.
      // this will be resulting pixel in the kernel's origin.
      struct { float r=0, g=0, b=0, a=0; } outpix;

      for (std::size_t dy = 0; dy < kernel_dims.second; ++dy) {
        for (std::size_t dx = 0; dx < kernel_dims.first; ++dx) {
          // NOTE: for edge handling a mixture of mirroring and wrapping is
          // done. wrapping when it reaches the end, mirroring when it reaches
          // the start.
          auto pix = input->pixel(
              static_cast<simg_int>(
                  static_cast<unsigned long long>(std::llabs(
                      static_cast<long long>(x) + static_cast<long long>(dx) -
                      static_cast<long long>(kernel_origin.first))) %
                  input->width()),
              static_cast<simg_int>(
                  static_cast<unsigned long long>(std::llabs(
                      static_cast<long long>(y) + static_cast<long long>(dy) -
                      static_cast<long long>(kernel_origin.second))) %
                  input->height()));

          // TODO: alpha isn't altered, need to add an option for it.
          outpix.r += static_cast<float>(pix.r) * kernel[dy][dx];
          outpix.g += static_cast<float>(pix.g) * kernel[dy][dx];
          outpix.b += static_cast<float>(pix.b) * kernel[dy][dx];
          outpix.a += static_cast<float>(pix.a) * kernel[dy][dx];
        }
      }

      input->pixel(x, y) = { static_cast<std::uint8_t>(outpix.r),
                             static_cast<std::uint8_t>(outpix.g),
                             static_cast<std::uint8_t>(outpix.b),
                             static_cast<std::uint8_t>(outpix.a) };
    }
  }
}
