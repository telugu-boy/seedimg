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

void apply_mat_worker(simg &inp_img, simg &res_img, simg_int start,
                      simg_int end, const fsmat &mat) {
  for (; start < end; ++start) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start);
      res_img->pixel(x, start).r = seedimg::utils::clamp<std::uint8_t>(
          mat[0] * pix.r + mat[4] * pix.g + mat[8] * pix.b + mat[12], 0,
          seedimg::img::MAX_PIXEL_VALUE);
      res_img->pixel(x, start).g = seedimg::utils::clamp<std::uint8_t>(
          mat[1] * pix.r + mat[5] * pix.g + mat[9] * pix.b + mat[13], 0,
          seedimg::img::MAX_PIXEL_VALUE);
      res_img->pixel(x, start).b = seedimg::utils::clamp<std::uint8_t>(
          mat[2] * pix.r + mat[6] * pix.g + mat[10] * pix.b + mat[14], 0,
          seedimg::img::MAX_PIXEL_VALUE);
    }
  }
}

namespace seedimg::filters {
fsmat scalar_mat_mul(const fsmat &mat, float sc) {
  fsmat res;
  for (std::size_t i = 0; i < 16; i++) {
    res[i] = mat[i] * sc;
  }
  return res;
}

smat scalar_mat_mul(const smat &mat, float sc) {
  smat res;
  for (std::size_t i = 0; i < 9; i++) {
    res[i] = mat[i] * sc;
  }
  return res;
}

fsmat compose_mats(const std::vector<fsmat> &mats) {
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

smat compose_mats(const std::vector<smat> &mats) {
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

fsmat to_fsmat(const smat &mat) {
  return {
      mat[0], mat[1], mat[2], 0.0f, mat[3], mat[4], mat[5], 0.0f,
      mat[6], mat[7], mat[8], 0.0f, 0.0f,   0.0f,   0.0f,   1.0f,
  };
}

smat generate_hue_mat(int angle) {
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

void apply_mat(simg &inp_img, simg &res_img, const fsmat &mat) {
  seedimg::utils::hrz_thread(apply_mat_worker, inp_img, res_img, mat);
}

void apply_mat_i(simg &inp_img, const fsmat &mat) {
  apply_mat(inp_img, inp_img, mat);
}
void apply_mat(simg &inp_img, simg &res_img, const smat &mat) {
  apply_mat(inp_img, res_img, to_fsmat(mat));
}
void apply_mat_i(simg &inp_img, const smat &mat) {
  apply_mat(inp_img, inp_img, mat);
};

// sepia
void sepia(simg &inp_img, simg &res_img) {
  apply_mat(inp_img, res_img, SEPIA_MAT);
}
void sepia_i(simg &inp_img) { apply_mat_i(inp_img, SEPIA_MAT); }

void rotate_hue(simg &inp_img, simg &res_img, int angle) {
  apply_mat(inp_img, res_img, generate_hue_mat(angle));
}

void rotate_hue_i(simg &inp_img, int angle) {
  rotate_hue(inp_img, inp_img, angle);
}
} // namespace seedimg::filters
