/***********************************************************************
    seedimg - module based image manipulation library written in modern C++
    Copyright (C) 2020 tripulse, telugu-boy

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
#ifndef SEEDIMG_EXTRAS_HPP
#define SEEDIMG_EXTRAS_HPP

#include <array>
#include <seedimg.hpp>

namespace seedimg::extras {
struct histogram_result {
  std::array<std::size_t, 256> r, g, b, a;
};

/**
 * @brief Calculate how many r, g, b, a components total in an image
 * @param input Input image to do the analysis on.
 * @return a structure of 4 channels as 256-length arrays.
 */
static inline histogram_result histogram(simg &input) {
  histogram_result result;

  for (simg_int y = 0; y < input->height(); ++y) {
    for (simg_int x = 0; x < input->width(); ++x) {
      auto &pix = input->pixel(x, y);

      ++result.r[pix.r];
      ++result.g[pix.g];
      ++result.b[pix.b];
      ++result.a[pix.a];
    }
  }
  return result;
}
} // namespace seedimg::extras
#endif
