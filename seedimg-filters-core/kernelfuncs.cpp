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
  if (blur_level == 0)
    return;
  auto res_img = std::make_unique<seedimg::img>(*inp_img);
  blur_level = static_cast<unsigned int>(
      std::min(static_cast<int>(blur_level),
               std::min(static_cast<int>((inp_img->width() - 1)) / 2,
                        static_cast<int>((inp_img->height() - 1)) / 2)));
  for (simg_int y = 0; y < inp_img->height(); ++y) {
    simg_int r = 0, g = 0, b = 0;
    // need to initialize with this pixel +  blur_level
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
    // this is a separate loop because we don't remove anything, only add
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
    // queuing optimization start when all edge cases end
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
    // final  blur_level pixel loop
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
  for (simg_int x = 0; x < res_img->width(); ++x) {
    simg_int r = 0, g = 0, b = 0;
    // need to initialize with this pixel +  blur_level
    for (simg_int i = 0; i <= blur_level; ++i) {
      auto pix = res_img->pixel(x, i);
      r += pix.r;
      g += pix.g;
      b += pix.b;
    }
    inp_img->pixel(x, 0) = {static_cast<std::uint8_t>(r / (blur_level + 1)),
                            static_cast<std::uint8_t>(g / (blur_level + 1)),
                            static_cast<std::uint8_t>(b / (blur_level + 1)),
                            res_img->pixel(x, 0).a};
    // this is a separate loop because we don't remove anything, only add
    for (simg_int y = 1; y < blur_level; ++y) {
      simg_int sbl = y + blur_level + 1;
      auto pix = res_img->pixel(x, y + blur_level);
      r += pix.r;
      g += pix.g;
      b += pix.b;
      inp_img->pixel(x, y) = {static_cast<std::uint8_t>(r / sbl),
                              static_cast<std::uint8_t>(g / sbl),
                              static_cast<std::uint8_t>(b / sbl),
                              res_img->pixel(x, y).a};
    }
    // queuing optimization start when all edge cases end
    for (simg_int y = blur_level; y < res_img->height() - blur_level; ++y) {
      auto pix = res_img->pixel(x, y + blur_level);
      auto prev = res_img->pixel(x, y - blur_level);
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

      inp_img->pixel(x, y) = {static_cast<std::uint8_t>(r / (2 * blur_level)),
                              static_cast<std::uint8_t>(g / (2 * blur_level)),
                              static_cast<std::uint8_t>(b / (2 * blur_level)),
                              res_img->pixel(x, y).a};
    }
    // final  blur_level pixel loop
    for (simg_int y = res_img->height() - blur_level; y < res_img->height();
         ++y) {
      simg_int ebl = res_img->height() - y + blur_level - 1;
      auto pix = res_img->pixel(x, y - blur_level);
      r -= pix.r;
      g -= pix.g;
      b -= pix.b;
      inp_img->pixel(x, y) = {static_cast<std::uint8_t>(r / ebl),
                              static_cast<std::uint8_t>(g / ebl),
                              static_cast<std::uint8_t>(b / ebl),
                              res_img->pixel(x, y).a};
    }
  }
}

namespace seedimg::filters {
void blur(std::unique_ptr<seedimg::img> &inp_img, unsigned int blur_level,
          std::uint8_t it) {
  for (std::uint8_t i = 0; i < it; ++i) {
    box_blur_single(inp_img, blur_level);
  }
}
} // namespace seedimg::filters
