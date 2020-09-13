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
#include <seedimg-filters/seedimg-filters-cconv.hpp>
#include <seedimg-utils.hpp>

inline bool feq(float a, float b) {
  return std::fabs(a - b) < std::numeric_limits<float>::epsilon();
}

void hsv2rgb_worker(simg &inp_img, simg &res_img, simg_int start,
                    simg_int end) {
  for (; start < end; start++) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix_src = inp_img->pixel(x, start);
      struct {
        float h;
        float s;
        float v;
      } pix{static_cast<float>(pix_src.h) * 2,
            static_cast<float>(pix_src.s) / 100,
            static_cast<float>(pix_src.v) / 100};
      float C = static_cast<float>(pix.v) * static_cast<float>(pix.s);
      float X =
          C * (1 - std::fabs(
                       std::fmod(static_cast<float>(pix.h) / 60.0f, 2.0f) - 1));

      float m = static_cast<float>(pix.v) - C;

      float componentsp[4] = {m, m, m};
      switch (static_cast<int>(pix.h / 60)) {
      case 0: {
        componentsp[0] += C;
        componentsp[1] += X;
        componentsp[2] += 0;
      } break;
      case 1: {
        componentsp[0] += X;
        componentsp[1] += C;
        componentsp[2] += 0;
      } break;
      case 2: {
        componentsp[0] += 0;
        componentsp[1] += C;
        componentsp[2] += X;
      } break;
      case 3: {
        componentsp[0] += 0;
        componentsp[1] += X;
        componentsp[2] += C;
      } break;
      case 4: {
        componentsp[0] += X;
        componentsp[1] += 0;
        componentsp[2] += C;
      } break;
      case 5: {
        componentsp[0] += C;
        componentsp[1] += 0;
        componentsp[2] += X;
      } break;
      }
      res_img->pixel(x, start) = {
          {{static_cast<std::uint8_t>(componentsp[0] * 255),
            static_cast<std::uint8_t>(componentsp[1] * 255),
            static_cast<std::uint8_t>(componentsp[2] * 255)}},
          pix_src.a};
    }
  }
}

void ycbcr2rgb_worker(simg &inp_img, simg &res_img, simg_int start,
                      simg_int end) {}

namespace seedimg::filters {
namespace cconv {
void rgb(simg &inp_img, simg &res_img) {
  if (inp_img->colourspace() == seedimg::colourspaces::rgb) {
    return;
  } else if (inp_img->colourspace() == seedimg::colourspaces::hsv) {
    seedimg::utils::hrz_thread(hsv2rgb_worker, inp_img, res_img);
  } else if (inp_img->colourspace() == seedimg::colourspaces::ycbcr_jpeg) {
  } else if (inp_img->colourspace() == seedimg::colourspaces::ycbcr_bt601) {
  }
}
void rgb_i(simg &inp_img) { rgb(inp_img, inp_img); }
} // namespace cconv
} // namespace seedimg::filters
