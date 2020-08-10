// seedimg-filters-core.cpp : Defines the functions for the static library.
//

#include "seedimg-filters-core.hpp"
#include <cmath>
#include <cstdint>
#include <memory>
#include <seedimg/seedimg.hpp>

// TODO: This is an example of a library function

inline bool is_on_rect(seedimg::point xy1, seedimg::point xy2,
                       seedimg::point point) {
  return xy1.first <= point.first && point.first <= xy2.first &&
         xy1.second <= point.second && point.second <= xy2.second;
}

namespace seedimg::filters {
void grayscale(std::unique_ptr<seedimg::img> &inp_img,
               bool lightness) noexcept {
  // would rather check once if it's in lightness mode than width*height times.
  if (lightness) {
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
