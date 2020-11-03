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
#include <cstdint>
#include <memory>
#include <seedimg-filters/seedimg-filters-core.hpp>
#include <seedimg.hpp>
#include <thread>
#include <vector>

void invert_worker(simg& inp_img, simg& res_img, simg_int start_row, simg_int end_row) {
    simg_int w = inp_img->width();
    for (; start_row < end_row; ++start_row) {
        for (simg_int x = 0; x < w; ++x) {
            seedimg::pixel pix           = inp_img->pixel(x, start_row);
            res_img->pixel(x, start_row) = {{static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.r)},
                                            {static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.g)},
                                            {static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.b)},
                                            pix.a};
        }
    }
}

void invert_worker_alpha(simg& inp_img, simg& res_img, simg_int start_row, simg_int end_row) {
    simg_int w = inp_img->width();
    for (; start_row < end_row; ++start_row) {
        for (simg_int x = 0; x < w; ++x) {
            seedimg::pixel pix           = inp_img->pixel(x, start_row);
            res_img->pixel(x, start_row) = {{static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.r)},
                                            {static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.g)},
                                            {static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.b)},
                                            static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.a)};
        }
    }
}

void invert_worker_alpha_only(simg& inp_img, simg& res_img, simg_int start_row, simg_int end_row) {
    simg_int w = inp_img->width();
    for (; start_row < end_row; ++start_row) {
        for (simg_int x = 0; x < w; ++x) {
            seedimg::pixel pix             = inp_img->pixel(x, start_row);
            res_img->pixel(x, start_row).a = static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.a);
        }
    }
}

namespace seedimg::filters {

void invert(simg& inp_img, simg& res_img) {
    auto                     start_end = inp_img->start_end_rows();
    std::vector<std::thread> workers(start_end.size());
    for (std::size_t i = 0; i < workers.size(); i++) {
        workers.at(i) = std::thread(
            invert_worker, std::ref(inp_img), std::ref(res_img), start_end.at(i).first, start_end.at(i).second);
    }
    for (std::size_t i = 0; i < workers.size(); ++i) workers.at(i).join();
};

void invert_a(simg& inp_img, simg& res_img, bool invert_alpha_only) {
    auto                     start_end = inp_img->start_end_rows();
    std::vector<std::thread> workers(start_end.size());
    if (invert_alpha_only) {
        for (std::size_t i = 0; i < workers.size(); i++) {
            workers.at(i) = std::thread(invert_worker_alpha_only,
                                        std::ref(inp_img),
                                        std::ref(res_img),
                                        start_end.at(i).first,
                                        start_end.at(i).second);
        }
    } else {
        for (std::size_t i = 0; i < workers.size(); i++) {
            workers.at(i) = std::thread(invert_worker_alpha,
                                        std::ref(inp_img),
                                        std::ref(res_img),
                                        start_end.at(i).first,
                                        start_end.at(i).second);
        }
    }
    for (std::size_t i = 0; i < workers.size(); ++i) workers.at(i).join();
};

void invert_i(simg& inp_img) { invert(inp_img, inp_img); }

void invert_a_i(simg& inp_img, bool invert_alpha_only) { invert_a(inp_img, inp_img, invert_alpha_only); }
} // namespace seedimg::filters
