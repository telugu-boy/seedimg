#ifndef SEEDIMG_FILTERS_CORE_H
#define SEEDIMG_FILTERS_CORE_H

#include <seedimg/seedimg.hpp>

namespace seedimg::filters {
void grayscale(seedimg::img &inp_img, bool lightness = false) noexcept;
void invert(seedimg::img &inp_img, bool invert_alpha = false) noexcept;
bool crop(seedimg::img &inp_img, seedimg::point p1, seedimg::point p2) noexcept;
void blur(seedimg::img &inp_img, std::uint8_t blur_level) noexcept;
} // namespace seedimg::filters

#endif
