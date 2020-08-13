#ifndef SEEDIMG_FILTERS_CORE_H
#define SEEDIMG_FILTERS_CORE_H

#include <seedimg/seedimg.hpp>

namespace seedimg::filters {
void grayscale(std::unique_ptr<seedimg::img> &inp_img, bool luminosity = true);
void invert(std::unique_ptr<seedimg::img> &inp_img);
void invert_a(std::unique_ptr<seedimg::img> &inp_img,
              bool invert_alpha_only = false);
bool crop(std::unique_ptr<seedimg::img> &inp_img, seedimg::point p1,
          seedimg::point p2) noexcept;
void blur(std::unique_ptr<seedimg::img> &inp_img, std::uint8_t blur_level);

// this just used for the convolution matrix.
// TODO: find a viable way of doing this.
template<class T>
using matrix2d = std::vector<std::vector<T>>;

/** Apply a convolution kernel to an image. */
void convolution(std::unique_ptr<seedimg::img>& input, matrix2d<float> kernel);
} // namespace seedimg::filters

#endif
