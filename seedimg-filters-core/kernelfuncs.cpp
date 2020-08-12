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
                     std::uint8_t blur_level) {
  auto res_img = std::make_unique<seedimg::img>(*inp_img);
  if (!(blur_level & 1))
    blur_level++;
  int upper = std::min((blur_level - 1) / 2,
                       std::min(static_cast<int>((inp_img->width() - 1)) / 2,
                                static_cast<int>((inp_img->height() - 1)) / 2));
  int lower = -upper;
  for (simg_int y = 0; y < inp_img->height(); ++y) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      {
        auto pix = inp_img->pixel(x, y);
        simg_int r = 0, g = 0, b = 0;
        for (int i = lower; i <= upper; ++i) {
          if (x + i < upper || x + i > inp_img->width())
            continue;
          seedimg::pixel pix = inp_img->pixel(x + i - 1, y);
          r += pix.r;
          g += pix.g;
          b += pix.b;
        }
        res_img->pixel(x, y) = {static_cast<std::uint8_t>(r / blur_level),
                                static_cast<std::uint8_t>(g / blur_level),
                                static_cast<std::uint8_t>(b / blur_level),
                                inp_img->pixel(x, y).a};
      }
    }
  }
  for (simg_int y = 0; y < res_img->height(); ++y) {
    for (simg_int x = 0; x < res_img->width(); ++x) {
      {
        simg_int r = 0, g = 0, b = 0;
        for (int i = lower; i <= upper; ++i) {
          if (y + i < upper || y + i > res_img->height())
            continue;
          seedimg::pixel pix = res_img->pixel(x, y + i - 1);
          r += pix.r;
          g += pix.g;
          b += pix.b;
        }
        inp_img->pixel(x, y) = {static_cast<std::uint8_t>(r / blur_level),
                                static_cast<std::uint8_t>(g / blur_level),
                                static_cast<std::uint8_t>(b / blur_level),
                                res_img->pixel(x, y).a};
      }
    }
  }
}

namespace seedimg::filters {
void blur(std::unique_ptr<seedimg::img> &inp_img, std::uint8_t blur_level,
          std::uint8_t it) {
  for (std::uint8_t i = 0; i < it; ++i) {
    box_blur_single(inp_img, blur_level);
  }
}
} // namespace seedimg::filters
