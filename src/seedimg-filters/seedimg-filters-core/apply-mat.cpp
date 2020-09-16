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

#include <seedimg-filters/seedimg-filters-core.hpp>
#include <seedimg-utils.hpp>

static constexpr std::array<float, 9> const SEPIA_MAT = {
    .393f, .769f, .189f, .349f, .686f, .168f, .272f, .534f, .131f};

void apply_mat_worker(simg &inp_img, simg &res_img, simg_int start,
                      simg_int end, const std::array<float, 9> &mat,
                      const std::array<float, 3> &vec) {
  for (; start < end; ++start) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start);
      res_img->pixel(x, start).r = seedimg::utils::clamp<std::uint8_t>(
          vec[0] + mat[0] * pix.r + mat[1] * pix.g + mat[2] * pix.b, 0,
          seedimg::img::MAX_PIXEL_VALUE);
      res_img->pixel(x, start).g = seedimg::utils::clamp<std::uint8_t>(
          vec[1] + mat[3] * pix.r + mat[4] * pix.g + mat[5] * pix.b, 0,
          seedimg::img::MAX_PIXEL_VALUE);
      res_img->pixel(x, start).b = seedimg::utils::clamp<std::uint8_t>(
          vec[2] + mat[6] * pix.r + mat[7] * pix.g + mat[8] * pix.b, 0,
          seedimg::img::MAX_PIXEL_VALUE);
    }
  }
}

namespace seedimg::filters {
void apply_mat(simg &inp_img, simg &res_img, const std::array<float, 9> &mat,
               const std::array<float, 3> &vec) {
  seedimg::utils::hrz_thread(apply_mat_worker, inp_img, res_img, mat, vec);
}

void apply_mat_i(simg &inp_img, const std::array<float, 9> &mat,
                 const std::array<float, 3> &vec) {
  apply_mat(inp_img, inp_img, mat, vec);
}

// sepia
void sepia(simg &inp_img, simg &res_img) {
  apply_mat(inp_img, res_img, SEPIA_MAT);
}
void sepia_i(simg &inp_img) { apply_mat_i(inp_img, SEPIA_MAT); }

} // namespace seedimg::filters
