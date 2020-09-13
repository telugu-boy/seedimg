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
#include <seedimg-filters/seedimg-filters-cconv.hpp>
#include <seedimg-utils.hpp>

static constexpr float const ycbcr_jpeg_mat[9] = {
    0.299000f, 0.587000f, 0.114000f,  -0.168736f, -0.331264f,
    0.500000f, 0.500000f, -0.418688f, -0.081312f};
static constexpr int const ycbcr_jpeg_vec[3] = {0, 128, 128};

static constexpr float const ycbcr_bt601_mat[9] = {
    65.738f / 256.0f,  129.057f / 256.0f, 25.064f / 256.0f,
    -37.945f / 256.0f, -74.494f / 256.0f, 112.439f / 256.0f,
    112.439f / 256.0f, -94.154f / 256.0f, -18.285f / 256.0f};
static constexpr int const ycbcr_bt601_vec[3] = {16, 128, 128};

void rgb2ycbcr_worker(simg &inp_img, simg &res_img, simg_int start,
                      simg_int end, const float mat[9], const int vec[3]) {
  for (; start < end; ++start) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start);
      res_img->pixel(x, start).y = static_cast<std::uint8_t>(
          vec[0] + mat[0] * pix.r + mat[1] * pix.g + mat[2] * pix.b);
      res_img->pixel(x, start).cb = static_cast<std::uint8_t>(
          vec[1] + mat[3] * pix.r + mat[4] * pix.g + mat[5] * pix.b);
      res_img->pixel(x, start).cr = static_cast<std::uint8_t>(
          vec[2] + mat[6] * pix.r + mat[7] * pix.g + mat[8] * pix.b);
    }
  }
}

namespace seedimg::filters {
namespace cconv {
void ycbcr(simg &inp_img, simg &res_img, seedimg::colourspaces type) {
  if (inp_img->colourspace() == seedimg::colourspaces::ycbcr_jpeg ||
      inp_img->colourspace() == seedimg::colourspaces::ycbcr_bt601) {
    return;
  } else if (inp_img->colourspace() == seedimg::colourspaces::rgb) {
    if (type == seedimg::colourspaces::ycbcr_jpeg) {
      seedimg::utils::hrz_thread(rgb2ycbcr_worker, inp_img, res_img,
                                 ycbcr_jpeg_mat, ycbcr_jpeg_vec);
    } else if (type == seedimg::colourspaces::ycbcr_bt601) {
      seedimg::utils::hrz_thread(rgb2ycbcr_worker, inp_img, res_img,
                                 ycbcr_bt601_mat, ycbcr_bt601_vec);
    }
  } else if (inp_img->colourspace() == seedimg::colourspaces::hsv) {
    rgb(inp_img, res_img);
    ycbcr(res_img, res_img, type);
  }
  std::static_pointer_cast<seedimg::uimg>(res_img)->set_colourspace(type);
}
void ycbcr_i(simg &inp_img, seedimg::colourspaces type) {
  ycbcr(inp_img, inp_img, type);
}
} // namespace cconv
} // namespace seedimg::filters
