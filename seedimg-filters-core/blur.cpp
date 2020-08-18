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
#include "seedimg-filters-core.hpp"
#include <cmath>
#include <cstdint>
// #include <immintrin.h>
#include <algorithm>
#include <memory>
#include <seedimg-png/seedimg-png.hpp>
#include <seedimg/seedimg.hpp>
#include <thread>
#include <vector>

void horizontal_blur_i_single_worker(simg &inp_img,
                                     simg &res_img,
                                     simg_int start, simg_int end,
                                     unsigned int blur_level) {
  for (simg_int y = start; y < end; ++y) {
    simg_int r = 0, g = 0, b = 0;
    for (simg_int i = 0; i <= blur_level; ++i) {
      auto pix = inp_img->pixel(i, y);
      r += pix.r;
      g += pix.g;
      b += pix.b;
    }
    res_img->pixel(0, y) = {static_cast<std::uint8_t>(r / (blur_level + 1)),
                            static_cast<std::uint8_t>(g / (blur_level + 1)),
                            static_cast<std::uint8_t>(b / (blur_level + 1)),
                            inp_img->pixel(0, y).a};
    for (simg_int x = 1; x < blur_level; ++x) {
      simg_int sbl = x + blur_level + 1;
      auto pix = inp_img->pixel(x + blur_level, y);
      r += pix.r;
      g += pix.g;
      b += pix.b;
      res_img->pixel(x, y) = {static_cast<std::uint8_t>(r / sbl),
                              static_cast<std::uint8_t>(g / sbl),
                              static_cast<std::uint8_t>(b / sbl),
                              inp_img->pixel(x, y).a};
    }
    for (simg_int x = blur_level; x < inp_img->width() - blur_level; ++x) {
      auto pix = inp_img->pixel(x + blur_level, y);
      auto prev = inp_img->pixel(x - blur_level, y);
      r = static_cast<simg_int>(std::max(
          static_cast<int_fast64_t>(0), static_cast<int_fast64_t>(r) +
                                            static_cast<int_fast64_t>(pix.r) -
                                            static_cast<int_fast64_t>(prev.r)));
      g = static_cast<simg_int>(std::max(
          static_cast<int_fast64_t>(0), static_cast<int_fast64_t>(g) +
                                            static_cast<int_fast64_t>(pix.g) -
                                            static_cast<int_fast64_t>(prev.g)));
      b = static_cast<simg_int>(std::max(
          static_cast<int_fast64_t>(0), static_cast<int_fast64_t>(b) +
                                            static_cast<int_fast64_t>(pix.b) -
                                            static_cast<int_fast64_t>(prev.b)));

      res_img->pixel(x, y) = {static_cast<std::uint8_t>(r / (2 * blur_level)),
                              static_cast<std::uint8_t>(g / (2 * blur_level)),
                              static_cast<std::uint8_t>(b / (2 * blur_level)),
                              inp_img->pixel(x, y).a};
    }
    for (simg_int x = inp_img->width() - blur_level; x < inp_img->width();
         ++x) {
      simg_int ebl = inp_img->width() - x + blur_level - 1;
      auto pix = inp_img->pixel(x - blur_level, y);
      r -= pix.r;
      g -= pix.g;
      b -= pix.b;
      res_img->pixel(x, y) = {static_cast<std::uint8_t>(r / ebl),
                              static_cast<std::uint8_t>(g / ebl),
                              static_cast<std::uint8_t>(b / ebl),
                              inp_img->pixel(x, y).a};
    }
  }
}

void vertical_blur_i_single_worker(simg &inp_img,
                                   simg &res_img,
                                   simg_int start, simg_int end,
                                   unsigned int blur_level) {
  for (simg_int x = start; x < end; ++x) {
    simg_int r = 0, g = 0, b = 0;
    for (simg_int i = 0; i <= blur_level; ++i) {
      auto pix = inp_img->pixel(x, i);
      r += pix.r;
      g += pix.g;
      b += pix.b;
    }
    res_img->pixel(x, 0) = {static_cast<std::uint8_t>(r / (blur_level + 1)),
                            static_cast<std::uint8_t>(g / (blur_level + 1)),
                            static_cast<std::uint8_t>(b / (blur_level + 1)),
                            inp_img->pixel(x, 0).a};
    for (simg_int y = 1; y < blur_level; ++y) {
      simg_int sbl = y + blur_level + 1;
      auto pix = inp_img->pixel(x, y + blur_level);
      r += pix.r;
      g += pix.g;
      b += pix.b;
      res_img->pixel(x, y) = {static_cast<std::uint8_t>(r / sbl),
                              static_cast<std::uint8_t>(g / sbl),
                              static_cast<std::uint8_t>(b / sbl),
                              inp_img->pixel(x, y).a};
    }
    for (simg_int y = blur_level; y < inp_img->height() - blur_level; ++y) {
      auto pix = inp_img->pixel(x, y + blur_level);
      auto prev = inp_img->pixel(x, y - blur_level);
      r = static_cast<simg_int>(std::max(
          static_cast<int_fast64_t>(0), static_cast<int_fast64_t>(r) +
                                            static_cast<int_fast64_t>(pix.r) -
                                            static_cast<int_fast64_t>(prev.r)));
      g = static_cast<simg_int>(std::max(
          static_cast<int_fast64_t>(0), static_cast<int_fast64_t>(g) +
                                            static_cast<int_fast64_t>(pix.g) -
                                            static_cast<int_fast64_t>(prev.g)));
      b = static_cast<simg_int>(std::max(
          static_cast<int_fast64_t>(0), static_cast<int_fast64_t>(b) +
                                            static_cast<int_fast64_t>(pix.b) -
                                            static_cast<int_fast64_t>(prev.b)));

      res_img->pixel(x, y) = {static_cast<std::uint8_t>(r / (2 * blur_level)),
                              static_cast<std::uint8_t>(g / (2 * blur_level)),
                              static_cast<std::uint8_t>(b / (2 * blur_level)),
                              inp_img->pixel(x, y).a};
    }
    for (simg_int y = inp_img->height() - blur_level; y < inp_img->height();
         ++y) {
      simg_int ebl = inp_img->height() - y + blur_level - 1;
      auto pix = inp_img->pixel(x, y - blur_level);
      r -= pix.r;
      g -= pix.g;
      b -= pix.b;
      res_img->pixel(x, y) = {static_cast<std::uint8_t>(r / ebl),
                              static_cast<std::uint8_t>(g / ebl),
                              static_cast<std::uint8_t>(b / ebl),
                              inp_img->pixel(x, y).a};
    }
  }
}

void horizontal_blur_single_i(simg &inp_img,
                              simg &res_img,
                              unsigned int blur_level) {
  auto start_end = inp_img->start_end_rows();
  std::vector<std::thread> workers(start_end.size());
  for (std::size_t i = 0; i < workers.size(); i++) {
    workers.at(i) = std::thread(
        horizontal_blur_i_single_worker, std::ref(inp_img), std::ref(res_img),
        start_end.at(i).first, start_end.at(i).second, blur_level);
  }
  for (std::size_t i = 0; i < workers.size(); ++i)
    workers.at(i).join();
}

void vertical_blur_single_i(simg &inp_img,
                            simg &res_img,
                            unsigned int blur_level) {
  auto start_end = inp_img->start_end_cols();
  std::vector<std::thread> workers(start_end.size());
  for (std::size_t i = 0; i < workers.size(); i++) {
    workers.at(i) = std::thread(
        vertical_blur_i_single_worker, std::ref(inp_img), std::ref(res_img),
        start_end.at(i).first, start_end.at(i).second, blur_level);
  }
  for (std::size_t i = 0; i < workers.size(); ++i)
    workers.at(i).join();
}

void box_blur_single(simg &inp_img,
                     simg &res_img,
                     unsigned int blur_level) {
  horizontal_blur_single_i(inp_img, res_img, blur_level);
  vertical_blur_single_i(res_img, inp_img, blur_level);
}

unsigned int clamped_blur_level(unsigned int blur_level, simg_int width,
                                simg_int height) {
  return static_cast<unsigned int>(
      std::min(static_cast<int>(blur_level),
               std::min(static_cast<int>((width - 1)) / 2,
                        static_cast<int>((height - 1)) / 2)));
}

namespace seedimg::filters {
void h_blur_i(simg &inp_img, unsigned int blur_level,
              std::uint8_t it) {
  if (blur_level == 0)
    return;
  blur_level =
      clamped_blur_level(blur_level, inp_img->width(), inp_img->height());
  auto res_img =
      std::make_unique<seedimg::img>(inp_img->width(), inp_img->height());
  for (std::uint8_t i = 0; i < it; ++i) {
    if (i % 2 == 0) {
      horizontal_blur_single_i(inp_img, res_img, blur_level);
    } else {
      horizontal_blur_single_i(res_img, inp_img, blur_level);
    }
  }
  if (it % 2 == 0)
    inp_img.reset(res_img.release());
}
void v_blur_i(simg &inp_img, unsigned int blur_level,
              std::uint8_t it) {
  if (blur_level == 0)
    return;
  blur_level =
      clamped_blur_level(blur_level, inp_img->width(), inp_img->height());
  auto res_img =
      std::make_unique<seedimg::img>(inp_img->width(), inp_img->height());
  for (std::uint8_t i = 0; i < it; ++i) {
    if (i % 2 == 0) {
      vertical_blur_single_i(inp_img, res_img, blur_level);
    } else {
      vertical_blur_single_i(res_img, inp_img, blur_level);
    }
  }
  if (it % 2 == 0)
    inp_img.reset(res_img.release());
}
void blur_i(simg &inp_img, unsigned int blur_level,
            std::uint8_t it) {
  if (blur_level == 0)
    return;
  blur_level =
      clamped_blur_level(blur_level, inp_img->width(), inp_img->height());
  auto res_img =
      std::make_unique<seedimg::img>(inp_img->width(), inp_img->height());
  for (std::uint8_t i = 0; i < it; ++i) {
    box_blur_single(inp_img, res_img, blur_level);
  }
}
} // namespace seedimg::filters
