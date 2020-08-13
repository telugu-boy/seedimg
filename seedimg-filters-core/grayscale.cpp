/*********************************************************************** 
seedimg - module based image manipulation library written in modern C++ 
Copyright (C) 2020 telugu-boy 
 
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
#include "seedimg-filters-core.hpp"
#include <cstddef>
#include <memory>
#include <seedimg/seedimg.hpp>
#include <thread>
#include <vector>

void grayscale_worker_luminosity(std::unique_ptr<seedimg::img> &inp_img,
                                 simg_int start_row, simg_int end_row) noexcept {
  simg_int w = inp_img->width();
  for (; start_row < end_row; ++start_row) {
    for (simg_int x = 0; x < w; ++x) {
      seedimg::pixel &pix = inp_img->pixel(x, start_row);
      uint8_t linear = static_cast<uint8_t>((0.2126 * (pix.r / 255.0) +
                                             0.7152 * (pix.g / 255.0) +
                                             0.0722 * (pix.b / 255.0)) *
                                            255);
      pix = {linear, linear, linear, pix.a};
    }
  }
}

void grayscale_worker_average(std::unique_ptr<seedimg::img> &inp_img,
                              simg_int start_row, simg_int end_row) noexcept {
  simg_int w = inp_img->width();
  for (; start_row < end_row; ++start_row) {
    for (simg_int x = 0; x < w; ++x) {
      seedimg::pixel &pix = inp_img->pixel(x, start_row);
      uint8_t avg = (pix.r + pix.g + pix.b) / 3;
      pix = {avg, avg, avg, pix.a};
    }
  }
}

namespace seedimg::filters {
void grayscale(std::unique_ptr<seedimg::img> &inp_img, bool luminosity) {
  auto start_end = inp_img->start_end_rows();
  std::vector<std::thread> workers(start_end.size());
  if (luminosity) {
    for (std::size_t i = 0; i < workers.size(); i++) {
      workers.at(i) =
          std::thread(grayscale_worker_luminosity, std::ref(inp_img),
                      start_end.at(i).first, start_end.at(i).second);
    }
  } else {
    for (std::size_t i = 0; i < workers.size(); i++) {
      workers.at(i) =
          std::thread(grayscale_worker_average, std::ref(inp_img),
                      start_end.at(i).first, start_end.at(i).second);
    }
  }
  for (std::size_t i = 0; i < workers.size(); ++i)
    workers.at(i).join();
}
} // namespace seedimg::filters
