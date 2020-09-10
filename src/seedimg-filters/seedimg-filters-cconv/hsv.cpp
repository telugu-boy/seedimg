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

#include <cmath>
#include <numeric>
#include <seedimg-filters/seedimg-filters-cconv.hpp>

inline bool feq(float a, float b) {
  return std::fabs(a - b) < std::numeric_limits<float>::epsilon();
}

namespace seedimg::filters {
namespace cconv {

void hsv(const simg &inp_img, const simg &res_img) {
  if (inp_img->colourspace() == seedimg::colourspaces::hsv) {
    return;
  } else if (inp_img->colourspace() == seedimg::colourspaces::rgb) {
    // the p suffix in this sense stands for prime. normally we use R' G' B' to
    // represent normalized colour.
    for (simg_int y = 0; y < inp_img->height(); y++) {
      for (simg_int x = 0; x < inp_img->width(); x++) {
        std::uint8_t r = inp_img->pixel(x, y).r;
        std::uint8_t g = inp_img->pixel(x, y).g;
        std::uint8_t b = inp_img->pixel(x, y).b;
        std::uint8_t rmax = std::max(r, std::max(g, b));

        float rp = static_cast<float>(inp_img->pixel(x, y).r) / 255.0f;
        float gp = static_cast<float>(inp_img->pixel(x, y).g) / 255.0f;
        float bp = static_cast<float>(inp_img->pixel(x, y).b) / 255.0f;
        float cmax = std::max(rp, std::max(gp, bp));
        float cmin = std::min(rp, std::min(gp, bp));
        float delta = cmax - cmin;

        std::uint8_t hue = static_cast<std::uint8_t>(delta);
        std::uint8_t sat;
        std::uint8_t val = static_cast<std::uint8_t>(cmax * 100.0f);

        if (r == rmax) {
          hue = static_cast<std::uint8_t>(60 * std::fmod((gp - bp) / delta, 3));
        } else if (g == rmax) {
          hue = static_cast<std::uint8_t>(60 * ((bp - rp) / delta + 1));
        } else if (b == rmax) {
          hue = static_cast<std::uint8_t>(60 * ((rp - gp) / delta + 2));
        }

        // saturation
        if (rmax != 0) {
          sat = static_cast<std::uint8_t>((delta / cmax) * 100.0f);
        } else {
          sat = 0;
        }
        res_img->pixel(x, y) = {{{hue, sat, val}}, inp_img->pixel(x, y).a};
      }
    }
  }
}
void hsv_i(const simg &inp_img) {
  if (inp_img->colourspace() == seedimg::colourspaces::hsv) {
    return;
  } else if (inp_img->colourspace() == seedimg::colourspaces::rgb) {
    hsv(inp_img, inp_img);
  }
}
} // namespace cconv
} // namespace seedimg::filters
