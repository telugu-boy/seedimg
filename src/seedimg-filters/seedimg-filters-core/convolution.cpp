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

#include <seedimg-filters/seedimg-filters-core.hpp>
#include <seedimg.hpp>

#include <seedimg-utils.hpp>

void seedimg::filters::convolution(simg &input,
                                   std::vector<std::vector<float>> kernel) {
  if(kernel.size() == 0 || kernel[0].size() == 0)
      return;

  simg_int kw = kernel[0].size();
  simg_int kh = kernel   .size();

  float neg_sum = 0.0f, pos_sum = 0.0f;

  for(const auto& r : kernel)
     for(auto e : r)
         if(std::signbit(e))
             neg_sum -= e;
         else
             pos_sum += e;

  // approximated the center coordinate of kernel, for a symmetric one
  // the origin is the current pixel.
  simg_int ko_x = kw / 2, ko_y = kh / 2;

  // normalised kernel.
  std::vector<std::vector<float>> norm_kernel(kh, std::vector<float>(kw, 0.0));

  // flip the kernel both vertically and horizontally +
  // normalise all the elements.
  for(simg_int y = 0; y < kh; ++y)
      for(simg_int x = 0; x < kw; ++x)
          norm_kernel[kh - y - 1][kw - x - 1] =
                  kernel[y][x] / (std::signbit(kernel[y][x]) ? neg_sum : pos_sum);

  for (simg_int y = 0; y < input->height(); ++y) {
    for (simg_int x = 0; x < input->width(); ++x) {     
      struct { float r=0, g=0, b=0, a=0; } o;

      for (std::size_t dy = 0; dy < kh; ++dy) {
        for (std::size_t dx = 0; dx < kw; ++dx) {
          // When the 2D sliding window is at the edge of the image plane,
          // the edges of the kernel fall outside the bound, to conceal that
          // a mixture of wrapping and mirroring is used.
          auto pix = input->pixel(
              static_cast<simg_int>(
                  static_cast<unsigned long long>(std::llabs(
                      static_cast<long long>(x) + static_cast<long long>(dx) -
                      static_cast<long long>(ko_x))) %
                  input->width()),
              static_cast<simg_int>(
                  static_cast<unsigned long long>(std::llabs(
                      static_cast<long long>(y) + static_cast<long long>(dy) -
                      static_cast<long long>(ko_y))) %
                  input->height()));

          o.r += static_cast<float>(pix.r) * norm_kernel[dy][dx];
          o.g += static_cast<float>(pix.g) * norm_kernel[dy][dx];
          o.b += static_cast<float>(pix.b) * norm_kernel[dy][dx];
          // o.a += static_cast<float>(pix.a) * norm_kernel[dy][dx];
        }
      }

      using namespace seedimg::utils;

      input->pixel(x, y) = {{static_cast<std::uint8_t>(clamp(o.r, 0, 255))},
                            {static_cast<std::uint8_t>(clamp(o.g, 0, 255))},
                            {static_cast<std::uint8_t>(clamp(o.b, 0, 255))},
                             static_cast<std::uint8_t>(clamp(o.a, 0, 255))};


    }
  }
}
