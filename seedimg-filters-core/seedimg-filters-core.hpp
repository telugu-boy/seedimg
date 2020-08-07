#ifndef _SEEDIMG_FILTERS_CORE_H
#define _SEEDIMG_FILTERS_CORE_H

#include <seedimg/seedimg.hpp>

namespace seedimg::filters {
void grayscale(std::unique_ptr<seedimg::img> &inp_img,
               bool lightness = false) noexcept;
void invert(std::unique_ptr<seedimg::img> &inp_img,
            bool invert_alpha = false) noexcept;
bool crop(std::unique_ptr<seedimg::img> &inp_img, seedimg::point p1,
          seedimg::point p2) noexcept;
} // namespace seedimg::filters

#endif
