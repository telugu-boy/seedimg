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
#include <seedimg-filters/seedimg-filters-core.hpp>
#include <seedimg-utils.hpp>

static constexpr seedimg::smat const ycbcr_jpeg_rgb_mat
    = {0.299f, -0.168736f, 0.5f, 0.587f, -0.331264f, -0.418688f, 0.114f, 0.5f, -0.081312f};

static constexpr seedimg::lutvec const ycbcr_jpeg_rgb_vec = {0, 128, 128};

static constexpr seedimg::slut<seedimg::smat> const ycbcr_jpeg_rgb_lut = seedimg::utils::gen_lut(ycbcr_jpeg_rgb_mat);

static constexpr seedimg::smat const ycbcr_bt601_rgb_mat
    = {65.738f / 256.0f,  -37.945f / 256.0f, 112.439f / 256.0f, 129.057f / 256.0f, -74.494f / 256.0f,
       -94.154f / 256.0f, 25.064f / 256.0f,  112.439f / 256.0f, -18.285f / 256.0f};

static constexpr seedimg::lutvec const ycbcr_bt601_rgb_vec = {16, 128, 128};

static constexpr seedimg::slut<seedimg::smat> const ycbcr_bt601_rgb_lut = seedimg::utils::gen_lut(ycbcr_bt601_rgb_mat);

namespace seedimg::filters {
namespace cconv {
void ycbcr(simg& inp_img, simg& res_img, seedimg::colourspaces type) {
    if (inp_img->colourspace() == seedimg::colourspaces::ycbcr_jpeg
        || inp_img->colourspace() == seedimg::colourspaces::ycbcr_bt601) {
        return;
    } else if (inp_img->colourspace() == seedimg::colourspaces::rgb) {
        if (type == seedimg::colourspaces::ycbcr_jpeg) {
            seedimg::filters::apply_mat_lut(inp_img, res_img, ycbcr_jpeg_rgb_lut, ycbcr_jpeg_rgb_vec);
        } else if (type == seedimg::colourspaces::ycbcr_bt601) {
            seedimg::filters::apply_mat_lut(inp_img, res_img, ycbcr_bt601_rgb_lut, ycbcr_bt601_rgb_vec);
        }
    } else if (inp_img->colourspace() == seedimg::colourspaces::hsv) {
        rgb(inp_img, res_img);
        ycbcr(res_img, res_img, type);
    }
    static_cast<seedimg::uimg*>(res_img.get())->set_colourspace(type);
}
void ycbcr_i(simg& inp_img, seedimg::colourspaces type) { ycbcr(inp_img, inp_img, type); }
} // namespace cconv
} // namespace seedimg::filters
