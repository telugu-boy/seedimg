// seedimg-filters-core.cpp : Defines the functions for the static library.
//

#include "seedimg-filters-core.hpp"

// TODO: This is an example of a library function
void seedimg::filters::grayscale(std::unique_ptr<seedimg::img> &inp_img,
                                 bool lightness) noexcept {
  // would rather check once if it's in lightness mode than width*height times.
  if (lightness) {
    for (auto &row : inp_img->get_data()) {
      for (auto &pix : row) {
        uint8_t linear = static_cast<uint8_t>((0.2126 * (pix.r / 255.0) +
                                               0.7152 * (pix.g / 255.0) +
                                               0.0722 * (pix.b / 255.0)) *
                                              255);
        pix = {linear, linear, linear, pix.a};
      }
    }
  } else {
    for (auto &row : inp_img->get_data()) {
      for (auto &pix : row) {
        uint8_t avg = (pix.r + pix.g + pix.b) / 3;
        pix = {avg, avg, avg, pix.a};
      }
    }
  }
}
