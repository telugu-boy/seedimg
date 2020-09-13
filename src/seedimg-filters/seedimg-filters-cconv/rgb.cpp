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

// https://en.wikipedia.org/wiki/YCbCr

static constexpr float const ycbcr_jpeg_mat[9] = {
    1.0f, 0.0f, 1.402f, 1.0f, -0.34414f, -0.71414f, 1.0f, 1.772f, 0.0f};

static constexpr float const ycbcr_bt601_mat[9] = {
    298.082f / 256.0f, 0.0f / 256.0f,      408.583f / 256.0f,
    298.082f / 256.0f, -100.291f / 256.0f, -208.120f / 256.0f,
    298.082f / 256.0f, 516.412f / 256.0f,  0.0f / 256.0f};

static constexpr float const ycbcr_bt601_vec[3] = {-222.921f, 135.576f,
                                                   -276.836f};

void ycbcr_jpeg2rgb_worker(simg &inp_img, simg &res_img, simg_int start,
                           simg_int end) {
  for (; start < end; ++start) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start);
      res_img->pixel(x, start).r = static_cast<std::uint8_t>(
          ycbcr_jpeg_mat[0] * pix.y +
          ycbcr_jpeg_mat[1] * (static_cast<int>(pix.cb - 128)) +
          ycbcr_jpeg_mat[2] * (static_cast<int>(pix.cr - 128)));
      res_img->pixel(x, start).g = static_cast<std::uint8_t>(
          ycbcr_jpeg_mat[3] * pix.y +
          ycbcr_jpeg_mat[4] * (static_cast<int>(pix.cb - 128)) +
          ycbcr_jpeg_mat[5] * (static_cast<int>(pix.cr - 128)));
      res_img->pixel(x, start).b = static_cast<std::uint8_t>(
          ycbcr_jpeg_mat[6] * pix.y +
          ycbcr_jpeg_mat[7] * (static_cast<int>(pix.cb - 128)) +
          ycbcr_jpeg_mat[8] * (static_cast<int>(pix.cr - 128)));
    }
  }
}

void ycbcr_bt6012rgb_worker(simg &inp_img, simg &res_img, simg_int start,
                            simg_int end) {
  for (; start < end; ++start) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start);
      res_img->pixel(x, start).r = static_cast<std::uint8_t>(
          ycbcr_bt601_vec[0] + ycbcr_bt601_mat[0] * pix.y +
          ycbcr_bt601_mat[1] * pix.cb + ycbcr_bt601_mat[2] * pix.cr);
      res_img->pixel(x, start).g = static_cast<std::uint8_t>(
          ycbcr_bt601_vec[1] + ycbcr_bt601_mat[3] * pix.y +
          ycbcr_bt601_mat[4] * pix.cb + ycbcr_bt601_mat[5] * pix.cr);
      res_img->pixel(x, start).b = static_cast<std::uint8_t>(
          ycbcr_bt601_vec[2] + ycbcr_bt601_mat[6] * pix.y +
          ycbcr_bt601_mat[7] * pix.cb + ycbcr_bt601_mat[8] * pix.cr);
    }
  }
}

namespace seedimg::filters {
namespace cconv {
void rgb(simg &inp_img, simg &res_img) {
  if (inp_img->colourspace() == seedimg::colourspaces::rgb) {
    return;
  } else if (inp_img->colourspace() == seedimg::colourspaces::hsv) {
    seedimg::utils::hrz_thread(hsv2rgb_worker, inp_img, res_img);
  } else if (inp_img->colourspace() == seedimg::colourspaces::ycbcr_jpeg) {
    seedimg::utils::hrz_thread(ycbcr_jpeg2rgb_worker, inp_img, res_img);
  } else if (inp_img->colourspace() == seedimg::colourspaces::ycbcr_bt601) {
    seedimg::utils::hrz_thread(ycbcr_bt6012rgb_worker, inp_img, res_img);
  }
  std::static_pointer_cast<seedimg::uimg>(res_img)->set_colourspace(
      seedimg::colourspaces::rgb);
}
void rgb_i(simg &inp_img) { rgb(inp_img, inp_img); }
} // namespace cconv
} // namespace seedimg::filters
