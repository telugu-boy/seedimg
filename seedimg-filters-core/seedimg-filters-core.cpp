// seedimg-filters-core.cpp : Defines the functions for the static library.
//

#include "seedimg-filters-core.hpp"
#include <cmath>
#include <cstdint>
#include <memory>
#include <seedimg/seedimg.hpp>
#include <thread>
#include <vector>

// TODO: This is an example of a library function

inline bool is_on_rect(seedimg::point xy1, seedimg::point xy2,
                       seedimg::point point) {
  return xy1.first <= point.first && point.first <= xy2.first &&
         xy1.second <= point.second && point.second <= xy2.second;
}

void grayscale_worker_luminosity(std::unique_ptr<seedimg::img> &inp_img,
                                 int start_row, int end_row) noexcept {
  int w = inp_img->width();
  for (; start_row < end_row; ++start_row) {
    for (int x = 0; x < w; ++x) {
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
                              int start_row, int end_row) noexcept {
  int w = inp_img->width();
  for (; start_row < end_row; ++start_row) {
    for (int x = 0; x < w; ++x) {
      seedimg::pixel &pix = inp_img->pixel(x, start_row);
      uint8_t avg = (pix.r + pix.g + pix.b) / 3;
      pix = {avg, avg, avg, pix.a};
    }
  }
}

namespace seedimg::filters {
void grayscale(std::unique_ptr<seedimg::img> &inp_img,
               bool luminosity) noexcept {
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

void blur(std::unique_ptr<seedimg::img> &inp_img,
          std::uint8_t blur_level) noexcept {}
} // namespace seedimg::filters
