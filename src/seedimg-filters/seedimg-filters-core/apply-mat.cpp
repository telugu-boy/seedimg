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
#include <seedimg-filters/seedimg-filters-core.hpp>
#include <seedimg-utils.hpp>

const double PI = 4 * std::atan(1);

static constexpr std::array<float, 9> const SEPIA_MAT = {
    .393f, .769f, .189f, .349f, .686f, .168f, .272f, .534f, .131f};

void apply_mat_worker(simg &inp_img, simg &res_img, simg_int start,
                      simg_int end, const std::array<float, 16> &mat,
                      const std::array<float, 4> &vec) {
  for (; start < end; ++start) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start);
      res_img->pixel(x, start).r = seedimg::utils::clamp<std::uint8_t>(
          vec[0] + mat[0] * pix.r + mat[1] * pix.g + mat[2] * pix.b +
              mat[3] * pix.a,
          0, seedimg::img::MAX_PIXEL_VALUE);
      res_img->pixel(x, start).g = seedimg::utils::clamp<std::uint8_t>(
          vec[1] + mat[4] * pix.r + mat[5] * pix.g + mat[6] * pix.b +
              mat[7] * pix.a,
          0, seedimg::img::MAX_PIXEL_VALUE);
      res_img->pixel(x, start).b = seedimg::utils::clamp<std::uint8_t>(
          vec[2] + mat[8] * pix.r + mat[9] * pix.g + mat[10] * pix.b +
              mat[11] * pix.a,
          0, seedimg::img::MAX_PIXEL_VALUE);
      res_img->pixel(x, start).a = seedimg::utils::clamp<std::uint8_t>(
          vec[3] + mat[12] * pix.r + mat[13] * pix.g + mat[14] * pix.b +
              mat[15] * pix.a,
          0, seedimg::img::MAX_PIXEL_VALUE);
    }
  }
}

namespace seedimg::filters {
void apply_mat(simg &inp_img, simg &res_img, const std::array<float, 16> &mat,
               const std::array<float, 4> &vec) {
  seedimg::utils::hrz_thread(apply_mat_worker, inp_img, res_img, mat, vec);
}

void apply_mat_i(simg &inp_img, const std::array<float, 16> &mat,
                 const std::array<float, 4> &vec) {
  apply_mat(inp_img, inp_img, mat, vec);
}
void apply_mat(simg &inp_img, simg &res_img, const std::array<float, 9> &mat) {
  apply_mat(inp_img, res_img,
            {
                mat[0],
                mat[1],
                mat[2],
                0.0f,
                mat[3],
                mat[4],
                mat[5],
                0.0f,
                mat[6],
                mat[7],
                mat[8],
                0.0f,
                0.0f,
                0.0f,
                0.0f,
                1.0f,
            });
}
void apply_mat_i(simg &inp_img, const std::array<float, 9> &mat) {
  apply_mat(inp_img, inp_img, mat);
};

// sepia
void sepia(simg &inp_img, simg &res_img) {
  apply_mat(inp_img, res_img, SEPIA_MAT);
}
void sepia_i(simg &inp_img) { apply_mat_i(inp_img, SEPIA_MAT); }

void rotate_hue(simg &inp_img, simg &res_img, int angle) {
  std::array<float, 9> hue_kernel;
  const float sinr = static_cast<float>(std::sin(angle * PI / 180));
  const float cosr = static_cast<float>(std::cos(angle * PI / 180));
  hue_kernel[0] = 0.213f + cosr * 0.787f - sinr * 0.213f;
  hue_kernel[1] = 0.715f - cosr * 0.715f - sinr * 0.715f;
  hue_kernel[2] = 0.072f - cosr * 0.072f + sinr * 0.928f;
  hue_kernel[3] = 0.213f - cosr * 0.213f + sinr * 0.143f;
  hue_kernel[4] = 0.715f + cosr * 0.285f + sinr * 0.140f;
  hue_kernel[5] = 0.072f - cosr * 0.072f - sinr * 0.283f;
  hue_kernel[6] = 0.213f - cosr * 0.213f - sinr * 0.787f;
  hue_kernel[7] = 0.715f - cosr * 0.715f + sinr * 0.715f;
  hue_kernel[8] = 0.072f + cosr * 0.928f + sinr * 0.072f;
  apply_mat(inp_img, res_img, hue_kernel);
}

void rotate_hue_i(simg &inp_img, int angle) {
  rotate_hue(inp_img, inp_img, angle);
}
} // namespace seedimg::filters
