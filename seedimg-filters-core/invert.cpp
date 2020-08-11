#include "seedimg-filters-core.hpp"
#include <cstdint>
#include <memory>
#include <seedimg/seedimg.hpp>
#include <thread>
#include <vector>

namespace seedimg::filters {
void invert(std::unique_ptr<seedimg::img> &inp_img) noexcept {
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

void invert_a(std::unique_ptr<seedimg::img> &inp_img,
              bool invert_alpha_only) noexcept {
  if (invert_alpha_only) {
    for (int y = 0; y < inp_img->height(); ++y) {
      for (int x = 0; x < inp_img->width(); ++x) {
        seedimg::pixel &pix = inp_img->pixel(x, y);
        pix.a =
            static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.a);
      }
    }
  } else {
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
  }
}
} // namespace seedimg::filters
