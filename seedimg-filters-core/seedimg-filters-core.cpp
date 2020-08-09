// seedimg-filters-core.cpp : Defines the functions for the static library.
//

#include "seedimg-filters-core.hpp"
#include <algorithm>
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
    for (auto &row : inp_img->data()) {
      for (auto &pix : row) {
        uint8_t linear = static_cast<uint8_t>((0.2126 * (pix.r / 255.0) +
                                               0.7152 * (pix.g / 255.0) +
                                               0.0722 * (pix.b / 255.0)) *
                                              255);
        pix = {linear, linear, linear, pix.a};
      }
    }
  } else {
    for (auto &row : inp_img->data()) {
      for (auto &pix : row) {
        uint8_t avg = (pix.r + pix.g + pix.b) / 3;
        pix = {avg, avg, avg, pix.a};
      }
    }
  }
}

void invert(std::unique_ptr<seedimg::img> &inp_img,
            bool invert_alpha) noexcept {
  if (invert_alpha) {
    for (auto &row : inp_img->data()) {
      for (auto &pix : row) {
        pix = {
            static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.r),
            static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.g),
            static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.b),
            static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.a)};
      }
    }
  } else {
    for (auto &row : inp_img->data()) {
      for (auto &pix : row) {
        pix = {static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.r),
               static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.g),
               static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.b),
               pix.a};
      }
    }
  }
}

bool crop(std::unique_ptr<seedimg::img> &inp_img, seedimg::point p1,
          seedimg::point p2) noexcept {
  if (!(is_on_rect({0, 0}, {inp_img->width, inp_img->height}, p1) &&
        is_on_rect({0, 0}, {inp_img->width, inp_img->height}, p2)))
    return false;
  auto ordered_crop_x = std::minmax(p1.first, p2.first);
  auto ordered_crop_y = std::minmax(p1.second, p2.second);
  auto cropped_img = std::make_unique<seedimg::img>(
      ordered_crop_x.second - ordered_crop_x.first,
      ordered_crop_y.second - ordered_crop_y.first);
  // ordered_crop_points.first is the minimum distance from the left, so we
  // offset the memcpy src by it
  for (std::size_t y = 0; y < (ordered_crop_y.second - ordered_crop_y.first);
       ++y) {
    std::memcpy(
        cropped_img->row(static_cast<uint32_t>(y)).data(),
        inp_img->row(static_cast<uint32_t>(y + ordered_crop_y.first))
                .data() +
            (ordered_crop_x.first * sizeof(seedimg::pixel)),
        (ordered_crop_x.second - ordered_crop_x.first) *
            sizeof(seedimg::pixel));
  }
  inp_img = std::move(cropped_img);
  return true;
}
} // namespace seedimg::filters
