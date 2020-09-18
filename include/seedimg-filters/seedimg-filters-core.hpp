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
#ifndef SEEDIMG_FILTERS_CORE_H
#define SEEDIMG_FILTERS_CORE_H

#define SEEDIMG_CROP_I_FRIEND

#include "seedimg-filters-cconv.hpp"
#include <cmath>
#include <seedimg.hpp>

const double PI = 4 * std::atan(1);
// Seedimg MATrix
typedef std::array<float, 9> smat;
// Full size Seedimg MATrix
typedef std::array<float, 16> fsmat;

namespace seedimg::filters {

static constexpr smat const SEPIA_MAT = {.393f, .349f, .272f, .769f, .686f,
                                         .534f, .189f, .168f, .131f};

inline fsmat compose_mats(const std::vector<fsmat> &mats) {
  fsmat res = mats[0];
  for (std::size_t i = 1; i < mats.size(); i++) {
    for (std::size_t j = 0; j < 4; j++) {
      for (std::size_t k = 0; k < 4; k++) {
        res[j * 4 + k] = res[j * 4] * mats[i][0 + j] +
                         res[j * 4 + 1] * mats[i][4 + j] +
                         res[j * 4 + 2] * mats[i][8 + j] + mats[i][12 + j];
      }
    }
  }
  return res;
}

inline smat compose_mats(const std::vector<smat> &mats) {
  smat res = mats[0];
  for (std::size_t i = 1; i < mats.size(); i++) {
    for (std::size_t j = 0; j < 3; j++) {
      for (std::size_t k = 0; k < 3; k++) {
        res[j * 3 + k] = res[j * 3] * mats[i][0 + j] +
                         res[j * 3 + 1] * mats[i][3 + j] +
                         res[j * 3 + 2] * mats[i][6 + j];
      }
    }
  }
  return res;
}

inline fsmat to_fsmat(const smat &mat) {
  return {
      mat[0], mat[1], mat[2], 0.0f, mat[3], mat[4], mat[5], 0.0f,
      mat[6], mat[7], mat[8], 0.0f, 0.0f,   0.0f,   0.0f,   1.0f,
  };
}

inline smat generate_hue_mat(int angle) {
  const float sinr = static_cast<float>(std::sin(angle * PI / 180));
  const float cosr = static_cast<float>(std::cos(angle * PI / 180));
  return {0.213f + cosr * 0.787f - sinr * 0.213f,
          0.213f - cosr * 0.213f + sinr * 0.143f,
          0.213f - cosr * 0.213f - sinr * 0.787f,
          0.715f - cosr * 0.715f - sinr * 0.715f,
          0.715f + cosr * 0.285f + sinr * 0.140f,
          0.715f - cosr * 0.715f + sinr * 0.715f,
          0.072f - cosr * 0.072f + sinr * 0.928f,
          0.072f - cosr * 0.072f - sinr * 0.283f,
          0.072f + cosr * 0.928f + sinr * 0.072f};
}
// filters that exclusively use this functionality will go in apply-mat.cpp to
// save unnecessary files.
// Current list: sepia, rotate_hue
void apply_mat(simg &inp_img, simg &res_img, const fsmat &mat);
void apply_mat_i(simg &inp_img, const fsmat &mat);
void apply_mat(simg &inp_img, simg &res_img, const smat &mat);
void apply_mat_i(simg &inp_img, const smat &mat);

void grayscale(simg &inp_img, simg &res_img, bool luminosity = true);
void grayscale_i(simg &inp_img, bool luminosity = true);

void invert(simg &inp_img, simg &res_img);
void invert_a(simg &inp_img, simg &res_img, bool invert_alpha_only = false);
void invert_i(simg &inp_img);
void invert_a_i(simg &inp_img, bool invert_alpha_only = false);

void rotate_cw(simg &inp_img, simg &res_img);
void rotate_180(simg &inp_img, simg &res_img);
void rotate_ccw(simg &inp_img, simg &res_img);
void rotate_cw_i(simg &inp_img);
void rotate_180_i(simg &inp_img);
void rotate_ccw_i(simg &inp_img);

void v_mirror(simg &inp_img, simg &res_img);
void h_mirror(simg &inp_img, simg &res_img);
void v_mirror_i(simg &inp_img);
void h_mirror_i(simg &inp_img);

bool crop(simg &inp_img, simg &res_img, seedimg::point p1, seedimg::point p2);
bool crop_i(simg &inp_img, seedimg::point p1, seedimg::point p2);

void blur_i(simg &inp_img, unsigned int blur_level, std::uint8_t it = 3);
void h_blur_i(simg &inp_img, unsigned int blur_level, std::uint8_t it = 3);
void v_blur_i(simg &inp_img, unsigned int blur_level, std::uint8_t it = 3);

/** Apply a square kernel convolution to an image.
 * NOTE: if weren't a square kernel, the image stays intact.
 * NOTE: alpha is passed-as it is, it's not convoluted.
 */
void convolution(simg &input, std::vector<std::vector<float>> kernel);

void brightness(simg &input, simg &output, std::uint8_t intensity);
void brightness_i(simg &image, std::uint8_t intensity);
void brightness_a(simg &input, simg &output, std::uint8_t intensity);
void brightness_a_i(simg &image, std::uint8_t intensity);

void blend(std::pair<simg &, const std::uint8_t> input,
           std::pair<simg &, const std::uint8_t> other, simg &output);
void blend_i(std::pair<simg &, const std::uint8_t> input,
             std::pair<simg &, const std::uint8_t> other);

// apply_mat filters
void sepia(simg &inp_img, simg &res_img);
void sepia_i(simg &inp_img);

void rotate_hue(simg &inp_img, simg &res_img, int angle);
void rotate_hue_i(simg &inp_img, int angle);

// HSV colourspace filters
void saturation(simg &inp_img, simg &res_img, float mul);
void saturation_i(simg &inp_img, float mul);

namespace cconv {};
namespace ocl {}
} // namespace seedimg::filters

#endif
