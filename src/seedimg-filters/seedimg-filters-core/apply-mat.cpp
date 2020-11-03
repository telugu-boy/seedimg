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

void apply_mat_worker(simg& inp_img, simg& res_img, simg_int start, simg_int end, const seedimg::fsmat& mat) {
    for (; start < end; ++start) {
        for (simg_int x = 0; x < inp_img->width(); ++x) {
            seedimg::pixel pix         = inp_img->pixel(x, start);
            res_img->pixel(x, start).r = seedimg::utils::clamp(
                mat[0] * pix.r + mat[4] * pix.g + mat[8] * pix.b + mat[12], 0, seedimg::img::MAX_PIXEL_VALUE);
            res_img->pixel(x, start).g = seedimg::utils::clamp(
                mat[1] * pix.r + mat[5] * pix.g + mat[9] * pix.b + mat[13], 0, seedimg::img::MAX_PIXEL_VALUE);
            res_img->pixel(x, start).b = seedimg::utils::clamp(
                mat[2] * pix.r + mat[6] * pix.g + mat[10] * pix.b + mat[14], 0, seedimg::img::MAX_PIXEL_VALUE);
        }
    }
}

void apply_mat_lut_worker(simg&                               inp_img,
                          simg&                               res_img,
                          simg_int                            start,
                          simg_int                            end,
                          const seedimg::slut<seedimg::smat>& lut,
                          const std::array<float, 3>          vec) {
    for (; start < end; ++start) {
        for (simg_int x = 0; x < inp_img->width(); ++x) {
            seedimg::pixel pix         = inp_img->pixel(x, start);
            res_img->pixel(x, start).r = seedimg::utils::clamp(
                vec[0] + lut[0][pix.r] + lut[3][pix.g] + lut[6][pix.b], 0, seedimg::img::MAX_PIXEL_VALUE);
            res_img->pixel(x, start).g = seedimg::utils::clamp(
                vec[1] + lut[1][pix.r] + lut[4][pix.g] + lut[7][pix.b], 0, seedimg::img::MAX_PIXEL_VALUE);
            res_img->pixel(x, start).b = seedimg::utils::clamp(
                vec[2] + lut[2][pix.r] + lut[5][pix.g] + lut[8][pix.b], 0, seedimg::img::MAX_PIXEL_VALUE);
        }
    }
}

namespace seedimg::filters {
void apply_mat(simg& inp_img, simg& res_img, const fsmat& mat) {
    seedimg::utils::hrz_thread(apply_mat_worker, inp_img, res_img, mat);
}

void apply_mat_i(simg& inp_img, const fsmat& mat) { apply_mat(inp_img, inp_img, mat); }
void apply_mat(simg& inp_img, simg& res_img, const smat& mat) { apply_mat(inp_img, res_img, to_fsmat(mat)); }
void apply_mat_i(simg& inp_img, const smat& mat) { apply_mat(inp_img, inp_img, mat); }

void apply_mat_lut(simg& inp_img, simg& res_img, const seedimg::slut<seedimg::smat>& lut, const seedimg::lutvec& vec) {
    seedimg::utils::hrz_thread(apply_mat_lut_worker, inp_img, res_img, lut, vec);
}
void apply_mat_lut_i(simg& inp_img, const seedimg::slut<seedimg::smat>& lut, const seedimg::lutvec& vec) {
    apply_mat_lut(inp_img, inp_img, lut, vec);
}

// sepia
void sepia(simg& inp_img, simg& res_img) { apply_mat(inp_img, res_img, SEPIA_MAT); }
void sepia_i(simg& inp_img) { apply_mat_i(inp_img, SEPIA_MAT); }

void rotate_hue(simg& inp_img, simg& res_img, int angle) { apply_mat(inp_img, res_img, generate_hue_mat(angle)); }

void rotate_hue_i(simg& inp_img, int angle) { rotate_hue(inp_img, inp_img, angle); }
} // namespace seedimg::filters
