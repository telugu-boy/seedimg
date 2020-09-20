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

#include <array>

#include <seedimg-extras.hpp>
#include <seedimg-utils.hpp>
#include <seedimg.hpp>

namespace seedimg::extras {
histogram_result histogram(simg &input) {
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

void scanline(simg& input, simg& output, simg_int rows, float intensity) {
    if(rows == 0) return;

    // if to alter group of scanlines or not.
    bool alter = false;

    for(simg_int y = 0; y < input->height(); ++y) {
        // once hit the boundary invert its state.
        if(y % rows == 0)
            alter = !alter;

        if(alter) {
            for(simg_int x = 0; x < input->width(); ++x) {
                auto& p = input->pixel(x, y);

                output->pixel(x, y) = seedimg::pixel{
                    {{seedimg::utils::clamp<std::uint8_t>(p.r + p.r*intensity, 0, 255),
                      seedimg::utils::clamp<std::uint8_t>(p.g + p.g*intensity, 0, 255),
                      seedimg::utils::clamp<std::uint8_t>(p.b + p.b*intensity, 0, 255)}},
                    p.a};
            }
        }
    }
}
} // namespace seedimg::extras
