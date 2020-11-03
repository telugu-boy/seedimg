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

inline float fmodulo(float x, float N) { return std::fmod(x, N) + (std::fmod(x, N) < 0) * N; }

inline bool feq(float a, float b) { return std::fabs(a - b) < .0000001f; }

void rgb2hsv_worker(simg& inp_img, simg& res_img, simg_int start, simg_int end) {
    for (; start < end; ++start) {
        for (simg_int x = 0; x < res_img->width(); ++x) {
            // the p suffix in this sense stands for prime. normally we use R' G' B'
            // to represent normalized colour.
            float rp
                = static_cast<float>(inp_img->pixel(x, start).r) / static_cast<float>(seedimg::img::MAX_PIXEL_VALUE);
            float gp
                = static_cast<float>(inp_img->pixel(x, start).g) / static_cast<float>(seedimg::img::MAX_PIXEL_VALUE);
            float bp
                = static_cast<float>(inp_img->pixel(x, start).b) / static_cast<float>(seedimg::img::MAX_PIXEL_VALUE);
            float cmax  = std::max(rp, std::max(gp, bp));
            float cmin  = std::min(rp, std::min(gp, bp));
            float delta = cmax - cmin;

            std::uint8_t hue;
            std::uint8_t sat = 0;
            std::uint8_t val = static_cast<std::uint8_t>(cmax * 100.0f);

            if (feq(rp, cmax)) {
                hue = static_cast<std::uint8_t>(30 * fmodulo((gp - bp) / delta, 6));
            } else if (feq(gp, cmax)) {
                hue = static_cast<std::uint8_t>(60 * ((bp - rp) / delta + 2));
            } else {
                hue = static_cast<std::uint8_t>(60 * ((rp - gp) / delta + 4));
            }

            // saturation
            if (!feq(cmax, 0)) { sat = static_cast<std::uint8_t>((delta / cmax) * 100.0f); }
            res_img->pixel(x, start) = {{hue}, {sat}, {val}, inp_img->pixel(x, start).a};
        }
    }
}

namespace seedimg::filters {
namespace cconv {

void hsv(simg& inp_img, simg& res_img) {
    if (inp_img->colourspace() == seedimg::colourspaces::hsv) {
        return;
    } else if (inp_img->colourspace() == seedimg::colourspaces::rgb) {
        seedimg::utils::hrz_thread(rgb2hsv_worker, inp_img, res_img);
        // rgb2hsv_worker(inp_img, res_img, 0, inp_img->height());
    } else if (inp_img->colourspace() == seedimg::colourspaces::ycbcr_jpeg
               || inp_img->colourspace() == seedimg::colourspaces::ycbcr_bt601) {
        rgb(inp_img, res_img);
        hsv(res_img, res_img);
    }
    static_cast<seedimg::uimg*>(res_img.get())->set_colourspace(seedimg::colourspaces::hsv);
}
void hsv_i(simg& inp_img) { hsv(inp_img, inp_img); }
} // namespace cconv
} // namespace seedimg::filters
