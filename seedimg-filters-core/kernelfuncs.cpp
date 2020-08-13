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

void box_blur_single(std::unique_ptr<seedimg::img> &inp_img,
                     unsigned int blur_level) {
  auto res_img = std::make_unique<seedimg::img>(*inp_img);
  simg_int upper = static_cast<simg_int>(
      std::min(static_cast<int>(blur_level),
               std::min(static_cast<int>((inp_img->width() - 1)) / 2,
                        static_cast<int>((inp_img->height() - 1)) / 2)));
  for (simg_int y = 0; y < inp_img->height(); ++y) {
    simg_int r = 0, g = 0, b = 0;
    // need to initialize with this pixel + upper
    for (simg_int i = 0; i <= upper; ++i) {
      auto pix = inp_img->pixel(i, y);
      r += pix.r;
      g += pix.g;
      b += pix.b;
    }
    res_img->pixel(0, y) = {static_cast<std::uint8_t>(r / (upper + 1)),
                            static_cast<std::uint8_t>(g / (upper + 1)),
                            static_cast<std::uint8_t>(b / (upper + 1)),
                            inp_img->pixel(0, y).a};
    // this is a separate loop because we don't remove anything, only add
    for (simg_int x = 1; x < upper; ++x) {
      simg_int sbl = x + upper + 1;
      auto pix = inp_img->pixel(x + upper, y);
      r += pix.r;
      g += pix.g;
      b += pix.b;
      res_img->pixel(x, y) = {static_cast<std::uint8_t>(r / sbl),
                              static_cast<std::uint8_t>(g / sbl),
                              static_cast<std::uint8_t>(b / sbl),
                              inp_img->pixel(x, y).a};
    }
    // queuing optimization start when all edge cases end
    for (simg_int x = upper; x < inp_img->width() - upper; ++x) {
      auto pix = inp_img->pixel(x + upper, y);
      auto prev = inp_img->pixel(x - upper, y);
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
    // final upper pixel loop
    for (simg_int x = inp_img->width() - upper; x < inp_img->width(); ++x) {
      simg_int ebl = inp_img->width() - x + upper + 1;
      auto pix = inp_img->pixel(x - upper - 1, y);
      r -= pix.r;
      g -= pix.g;
      b -= pix.b;
      res_img->pixel(x, y) = {static_cast<std::uint8_t>(r / ebl),
                              static_cast<std::uint8_t>(g / ebl),
                              static_cast<std::uint8_t>(b / ebl),
                              inp_img->pixel(x, y).a};
    }
  }
  for (simg_int y = 0; y < res_img->height(); ++y) {
    for (simg_int x = 0; x < res_img->width(); ++x) {
    }
  }
  inp_img.reset(res_img.release());
}

namespace seedimg::filters {
void blur(std::unique_ptr<seedimg::img> &inp_img, unsigned int blur_level,
          std::uint8_t it) {
  for (std::uint8_t i = 0; i < it; ++i) {
    box_blur_single(inp_img, blur_level);
  }
}
} // namespace seedimg::filters
