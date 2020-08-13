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
void blur(std::unique_ptr<seedimg::img> &inp_img, unsigned int blur_level,
          std::uint8_t it = 3);
void h_blur(std::unique_ptr<seedimg::img> &inp_img, unsigned int blur_level,
            std::uint8_t it = 3);
void v_blur(std::unique_ptr<seedimg::img> &inp_img, unsigned int blur_level,
            std::uint8_t it = 3);
} // namespace seedimg::filters

#endif
