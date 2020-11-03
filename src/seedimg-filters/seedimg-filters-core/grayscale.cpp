/**********************************************************************
    seedimg - module based image manipulation library written in modern
                C++ Copyright(C) 2020 telugu-boy

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
#include <cstddef>
#include <memory>
#include <seedimg-filters/seedimg-filters-core.hpp>
#include <seedimg.hpp>
#include <thread>
#include <vector>

void grayscale_worker_luminosity(simg& inp_img, simg& res_img, simg_int start_row, simg_int end_row) {
    simg_int w = inp_img->width();
    for (; start_row < end_row; ++start_row) {
        for (simg_int x = 0; x < w; ++x) {
            seedimg::pixel& pix = inp_img->pixel(x, start_row);
            uint8_t         linear
                = static_cast<uint8_t>((0.2126f * (pix.r / static_cast<float>(seedimg::img::MAX_PIXEL_VALUE))
                                        + 0.7152f * (pix.g / static_cast<float>(seedimg::img::MAX_PIXEL_VALUE))
                                        + 0.0722f * (pix.b / static_cast<float>(seedimg::img::MAX_PIXEL_VALUE)))
                                       * seedimg::img::MAX_PIXEL_VALUE);
            res_img->pixel(x, start_row) = {{linear}, {linear}, {linear}, pix.a};
        }
    };
}

void grayscale_worker_average(simg& inp_img, simg& res_img, simg_int start_row, simg_int end_row) {
    simg_int w = inp_img->width();
    for (; start_row < end_row; ++start_row) {
        for (simg_int x = 0; x < w; ++x) {
            seedimg::pixel& pix          = inp_img->pixel(x, start_row);
            uint8_t         avg          = (pix.r + pix.g + pix.b) / 3;
            res_img->pixel(x, start_row) = {{avg}, {avg}, {avg}, pix.a};
        }
    }
}

namespace seedimg::filters {
void grayscale(simg& inp_img, simg& res_img, bool luminosity) {
    auto                     start_end = inp_img->start_end_rows();
    std::vector<std::thread> workers(start_end.size());
    if (luminosity) {
        for (std::size_t i = 0; i < workers.size(); i++) {
            workers.at(i) = std::thread(grayscale_worker_luminosity,
                                        std::ref(inp_img),
                                        std::ref(res_img),
                                        start_end.at(i).first,
                                        start_end.at(i).second);
        }
    } else {
        for (std::size_t i = 0; i < workers.size(); i++) {
            workers.at(i) = std::thread(grayscale_worker_average,
                                        std::ref(inp_img),
                                        std::ref(res_img),
                                        start_end.at(i).first,
                                        start_end.at(i).second);
        }
    }
    for (std::size_t i = 0; i < workers.size(); ++i) workers.at(i).join();
}

void grayscale_i(simg& inp_img, bool luminosity) { grayscale(inp_img, inp_img, luminosity); }
} // namespace seedimg::filters
