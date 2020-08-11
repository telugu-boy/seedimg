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

void grayscale_worker_lightness(std::unique_ptr<seedimg::img> &inp_img,
                                int start_row, int end_row) {
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

void grayscale_worker_luminosity(std::unique_ptr<seedimg::img> &inp_img,
                                 int start_row, int end_row) {}

namespace seedimg::filters {
void grayscale(std::unique_ptr<seedimg::img> &inp_img,
               bool luminosity) noexcept {
  // would rather check once if it's in lightness mode than width*height times.
  std::vector<std::pair<int, int>> start_end_row;
  auto processor_count = static_cast<int>(std::thread::hardware_concurrency());
  if (processor_count == 0)
    processor_count = 1;
  int rows_per_thread = inp_img->height() / processor_count;
  for (int i = 0; i < processor_count; i += rows_per_thread)
    start_end_row.push_back({i, i + processor_count});
  start_end_row[start_end_row.size() - 1].second +=
      inp_img->height() % processor_count;

  if (luminosity) {
    for (int y = 0; y < inp_img->height(); ++y) {
      for (int x = 0; x < inp_img->width(); ++x) {
        seedimg::pixel &pix = inp_img->pixel(x, y);
        uint8_t linear = static_cast<uint8_t>((0.2126 * (pix.r / 255.0) +
                                               0.7152 * (pix.g / 255.0) +
                                               0.0722 * (pix.b / 255.0)) *
                                              255);
        pix = {linear, linear, linear, pix.a};
      }
    }
  } else {
    for (int y = 0; y < inp_img->height(); ++y) {
      for (int x = 0; x < inp_img->width(); ++x) {
        seedimg::pixel &pix = inp_img->pixel(x, y);
        uint8_t avg = (pix.r + pix.g + pix.b) / 3;
        pix = {avg, avg, avg, pix.a};
      }
    }
  }
}

void invert(std::unique_ptr<seedimg::img> &inp_img,
            bool invert_alpha) noexcept {
  if (invert_alpha) {
    for (int y = 0; y < inp_img->height(); ++y) {
      for (int x = 0; x < inp_img->width(); ++x) {
        seedimg::pixel &pix = inp_img->pixel(x, y);
        pix = {
            static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.r),
            static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.g),
            static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.b),
            static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.a)};
      }
    }
  } else {
    for (int y = 0; y < inp_img->height(); ++y) {
      for (int x = 0; x < inp_img->width(); ++x) {
        seedimg::pixel &pix = inp_img->pixel(x, y);
        pix = {static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.r),
               static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.g),
               static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.b),
               pix.a};
      }
    }
  }
}

void blur(std::unique_ptr<seedimg::img> &inp_img,
          std::uint8_t blur_level) noexcept {}
} // namespace seedimg::filters
