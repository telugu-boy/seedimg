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

#ifndef SEEDIMG_UTILS_HPP
#define SEEDIMG_UTILS_HPP

#include <seedimg.hpp>

namespace seedimg::utils {
typedef struct {
  std::array<std::size_t, 256> r, g, b, a;
} histogram_result;

/**
 * @brief Calculate how many r, g, b, a components total in an image
 * @param input Input image to do the analysis on.
 * @return a structure of 4 channels as 256-length arrays.
 */
histogram_result histogram(simg &input);
} // namespace seedimg::utils
#endif
