/***********************************************************************
    seedimg - module based image manipulation library written in modern C++
    Copyright (C) 2020 telugu-boy, tripulse

    This program is free software : you can redistribute it and /
    or modify it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation,
    either version 3 of the License,
    or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/
#ifndef SEEDIMG_FILTERS_CORE_H
#define SEEDIMG_FILTERS_CORE_H

#define SEEDIMG_CROP_I_FRIEND

#include <array>
#include <cmath>
#include <cstring>
#include <functional>
#include <seedimg-utils.hpp>
#include <seedimg.hpp>

const float PI = 4 * std::atan(1.0f);

namespace seedimg::utils {
template <typename T = smat,
          std::size_t MaxPV = seedimg::img::MAX_PIXEL_VALUE + 1,
          std::size_t Amt = sizeof(T) / sizeof(typename T::value_type)>
constexpr auto gen_lut(const T &mat) {
  std::array<std::array<float, MaxPV>, Amt> res{};
  for (std::size_t elem = 0; elem < Amt; ++elem) {
    for (std::size_t i = 0; i < MaxPV; ++i) {
      res[elem][i] = i * mat[elem];
    }
  }
  return res;
}
} // namespace seedimg::utils

namespace seedimg::filters {
inline smat generate_hue_mat(float angle) {
  const float sinr = static_cast<float>(std::sin(angle * PI / 180));
  const float cosr = static_cast<float>(std::cos(angle * PI / 180));
  return {0.213f + cosr * 0.787f - sinr * 0.213f,
          0.213f - cosr * 0.213f + sinr * 0.143f,
          0.213f - cosr * 0.213f - sinr * 0.787f,
          0.715f - cosr * 0.715f - sinr * 0.715f,
          0.715f + cosr * 0.285f + sinr * 0.140f,
          0.715f - cosr * 0.715f + sinr * 0.715f,
          0.072f - cosr * 0.072f + sinr * 0.928f,
          0.072f - cosr * 0.072f - sinr * 0.283f,
          0.072f + cosr * 0.928f + sinr * 0.072f};
}

template <typename T> constexpr fsmat compose_fsmats(const T &mats) {
  fsmat res = mats[0];
  for (std::size_t l = 1; l < mats.size(); l++) {
    for (std::size_t i = 0; i < 4; i++) {
      for (std::size_t j = 0; j < 4; j++) {
        float sum = 0.0;
        for (std::size_t k = 0; k < 4; k++)
          sum += mats[l][i * 4 + k] * res[k * 4 + j];
        res[i * 4 + j] = sum;
      }
    }
  }
  return res;
}

template <typename T> constexpr smat compose_smats(const T &mats) {
  smat res = mats[0];
  for (std::size_t l = 1; l < mats.size(); l++) {
    for (std::size_t i = 0; i < 3; i++) {
      for (std::size_t j = 0; j < 3; j++) {
        float sum = 0.0;
        for (std::size_t k = 0; k < 3; k++)
          sum += mats[l][i * 3 + k] * res[k * 3 + j];
        res[i * 3 + j] = sum;
      }
    }
  }
  return res;
}

// matrix manipulation related functions, used for building
// matrices, mostly for the apply_mat filter.
constexpr fsmat scalar_mat_mul(const fsmat &mat, float sc) {
  fsmat res{};
  for (std::size_t i = 0; i < 16; i++) {
    res[i] = mat[i] * sc;
  }
  return res;
}

constexpr smat scalar_mat_mul(const smat &mat, float sc) {
  smat res{};
  for (std::size_t i = 0; i < 9; i++) {
    res[i] = mat[i] * sc;
  }
  return res;
}

constexpr fsmat to_fsmat(const smat &mat) {
  return {
      mat[0], mat[1], mat[2], 0.0f, mat[3], mat[4], mat[5], 0.0f,
      mat[6], mat[7], mat[8], 0.0f, 0.0f,   0.0f,   0.0f,   1.0f,
  };
}
} // namespace seedimg::filters

namespace simgdetails {
inline void apply_mat_worker(simg &inp_img, simg &res_img, simg_int start,
                             simg_int end, const seedimg::fsmat &mat) {
  for (; start < end; ++start) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start);
      res_img->pixel(x, start).r =
          static_cast<std::uint8_t>(seedimg::utils::clamp(
              mat[0] * pix.r + mat[4] * pix.g + mat[8] * pix.b + mat[12], 0,
              seedimg::img::MAX_PIXEL_VALUE));
      res_img->pixel(x, start).g =
          static_cast<std::uint8_t>(seedimg::utils::clamp(
              mat[1] * pix.r + mat[5] * pix.g + mat[9] * pix.b + mat[13], 0,
              seedimg::img::MAX_PIXEL_VALUE));
      res_img->pixel(x, start).b =
          static_cast<std::uint8_t>(seedimg::utils::clamp(
              mat[2] * pix.r + mat[6] * pix.g + mat[10] * pix.b + mat[14], 0,
              seedimg::img::MAX_PIXEL_VALUE));
    }
  }
}

inline void apply_mat_lut_worker(simg &inp_img, simg &res_img, simg_int start,
                                 simg_int end,
                                 const seedimg::slut<seedimg::smat> &lut,
                                 const std::array<float, 3> vec) {
  for (; start < end; ++start) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start);
      res_img->pixel(x, start).r =
          static_cast<std::uint8_t>(seedimg::utils::clamp(
              vec[0] + lut[0][pix.r] + lut[3][pix.g] + lut[6][pix.b], 0,
              seedimg::img::MAX_PIXEL_VALUE));
      res_img->pixel(x, start).g =
          static_cast<std::uint8_t>(seedimg::utils::clamp(
              vec[1] + lut[1][pix.r] + lut[4][pix.g] + lut[7][pix.b], 0,
              seedimg::img::MAX_PIXEL_VALUE));
      res_img->pixel(x, start).b =
          static_cast<std::uint8_t>(seedimg::utils::clamp(
              vec[2] + lut[2][pix.r] + lut[5][pix.g] + lut[8][pix.b], 0,
              seedimg::img::MAX_PIXEL_VALUE));
    }
  }
}

inline void grayscale_worker_luminosity(simg &inp_img, simg &res_img,
                                        simg_int start_row, simg_int end_row) {
  simg_int w = inp_img->width();
  for (; start_row < end_row; ++start_row) {
    for (simg_int x = 0; x < w; ++x) {
      seedimg::pixel &pix = inp_img->pixel(x, start_row);
      uint8_t linear = static_cast<uint8_t>(
          (0.2126f *
               (pix.r / static_cast<float>(seedimg::img::MAX_PIXEL_VALUE)) +
           0.7152f *
               (pix.g / static_cast<float>(seedimg::img::MAX_PIXEL_VALUE)) +
           0.0722f *
               (pix.b / static_cast<float>(seedimg::img::MAX_PIXEL_VALUE))) *
          seedimg::img::MAX_PIXEL_VALUE);
      res_img->pixel(x, start_row) = {{linear}, {linear}, {linear}, pix.a};
    }
  };
}

inline void grayscale_worker_average(simg &inp_img, simg &res_img,
                                     simg_int start_row, simg_int end_row) {
  simg_int w = inp_img->width();
  for (; start_row < end_row; ++start_row) {
    for (simg_int x = 0; x < w; ++x) {
      seedimg::pixel &pix = inp_img->pixel(x, start_row);
      uint8_t avg = (pix.r + pix.g + pix.b) / 3;
      res_img->pixel(x, start_row) = {{avg}, {avg}, {avg}, pix.a};
    }
  }
}

inline void invert_worker(simg &inp_img, simg &res_img, simg_int start_row,
                          simg_int end_row) {
  simg_int w = inp_img->width();
  for (; start_row < end_row; ++start_row) {
    for (simg_int x = 0; x < w; ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start_row);
      res_img->pixel(x, start_row) = {
          {static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.r)},
          {static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.g)},
          {static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.b)},
          pix.a};
    }
  }
}

inline void invert_worker_alpha(simg &inp_img, simg &res_img,
                                simg_int start_row, simg_int end_row) {
  simg_int w = inp_img->width();
  for (; start_row < end_row; ++start_row) {
    for (simg_int x = 0; x < w; ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start_row);
      res_img->pixel(x, start_row) = {
          {static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.r)},
          {static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.g)},
          {static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.b)},
          static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.a)};
    }
  }
}

inline void invert_worker_alpha_only(simg &inp_img, simg &res_img,
                                     simg_int start_row, simg_int end_row) {
  simg_int w = inp_img->width();
  for (; start_row < end_row; ++start_row) {
    for (simg_int x = 0; x < w; ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start_row);
      res_img->pixel(x, start_row).a =
          static_cast<std::uint8_t>(seedimg::img::MAX_PIXEL_VALUE - pix.a);
    }
  }
}

inline void pixel_add_worker(simg &image, simg &other, simg_int start_row,
                             simg_int end_row) {
  for (; start_row < end_row; ++start_row) {
    for (simg_int x = 0; x < image->width(); ++x) {
      auto &pix = image->pixel(x, start_row);
      auto &opix = other->pixel(x, start_row);

      pix.r = static_cast<std::uint8_t>(seedimg::utils::clamp(
          pix.r + opix.r, 0, seedimg::img::MAX_PIXEL_VALUE));
      pix.g = static_cast<std::uint8_t>(seedimg::utils::clamp(
          pix.g + opix.g, 0, seedimg::img::MAX_PIXEL_VALUE));
      pix.b = static_cast<std::uint8_t>(seedimg::utils::clamp(
          pix.b + opix.b, 0, seedimg::img::MAX_PIXEL_VALUE));
      pix.a = static_cast<std::uint8_t>(seedimg::utils::clamp(
          pix.a + opix.a, 0, seedimg::img::MAX_PIXEL_VALUE));
    }
  }
}

inline void horizontal_blur_i_single_worker(simg &inp_img, simg &res_img,
                                            simg_int start, simg_int end,
                                            unsigned int blur_level) {
  for (simg_int y = start; y < end; ++y) {
    simg_int r = 0, g = 0, b = 0;
    for (simg_int i = 0; i <= blur_level; ++i) {
      auto pix = inp_img->pixel(i, y);
      r += pix.r;
      g += pix.g;
      b += pix.b;
    }
    res_img->pixel(0, y) = {{static_cast<std::uint8_t>(r / (blur_level + 1))},
                            {static_cast<std::uint8_t>(g / (blur_level + 1))},
                            {static_cast<std::uint8_t>(b / (blur_level + 1))},
                            inp_img->pixel(0, y).a};
    for (simg_int x = 1; x < blur_level; ++x) {
      simg_int sbl = x + blur_level + 1;
      auto pix = inp_img->pixel(x + blur_level, y);
      r += pix.r;
      g += pix.g;
      b += pix.b;
      res_img->pixel(x, y) = {{static_cast<std::uint8_t>(r / sbl)},
                              {static_cast<std::uint8_t>(g / sbl)},
                              {static_cast<std::uint8_t>(b / sbl)},
                              inp_img->pixel(x, y).a};
    }
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

      res_img->pixel(x, y) = {{static_cast<std::uint8_t>(r / (2 * blur_level))},
                              {static_cast<std::uint8_t>(g / (2 * blur_level))},
                              {static_cast<std::uint8_t>(b / (2 * blur_level))},
                              inp_img->pixel(x, y).a};
    }
    for (simg_int x = inp_img->width() - blur_level; x < inp_img->width();
         ++x) {
      simg_int ebl = inp_img->width() - x + blur_level - 1;
      auto pix = inp_img->pixel(x - blur_level, y);
      r -= pix.r;
      g -= pix.g;
      b -= pix.b;
      res_img->pixel(x, y) = {{static_cast<std::uint8_t>(r / ebl)},
                              {static_cast<std::uint8_t>(g / ebl)},
                              {static_cast<std::uint8_t>(b / ebl)},
                              inp_img->pixel(x, y).a};
    }
  }
}

inline void vertical_blur_i_single_worker(simg &inp_img, simg &res_img,
                                          simg_int start, simg_int end,
                                          unsigned int blur_level) {
  for (simg_int x = start; x < end; ++x) {
    simg_int r = 0, g = 0, b = 0;
    for (simg_int i = 0; i <= blur_level; ++i) {
      auto pix = inp_img->pixel(x, i);
      r += pix.r;
      g += pix.g;
      b += pix.b;
    }
    res_img->pixel(x, 0) = {{static_cast<std::uint8_t>(r / (blur_level + 1))},
                            {static_cast<std::uint8_t>(g / (blur_level + 1))},
                            {static_cast<std::uint8_t>(b / (blur_level + 1))},
                            inp_img->pixel(x, 0).a};
    for (simg_int y = 1; y < blur_level; ++y) {
      simg_int sbl = y + blur_level + 1;
      auto pix = inp_img->pixel(x, y + blur_level);
      r += pix.r;
      g += pix.g;
      b += pix.b;
      res_img->pixel(x, y) = {{static_cast<std::uint8_t>(r / sbl)},
                              {static_cast<std::uint8_t>(g / sbl)},
                              {static_cast<std::uint8_t>(b / sbl)},
                              inp_img->pixel(x, y).a};
    }
    for (simg_int y = blur_level; y < inp_img->height() - blur_level; ++y) {
      auto pix = inp_img->pixel(x, y + blur_level);
      auto prev = inp_img->pixel(x, y - blur_level);
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

      res_img->pixel(x, y) = {{static_cast<std::uint8_t>(r / (2 * blur_level))},
                              {static_cast<std::uint8_t>(g / (2 * blur_level))},
                              {static_cast<std::uint8_t>(b / (2 * blur_level))},
                              inp_img->pixel(x, y).a};
    }
    for (simg_int y = inp_img->height() - blur_level; y < inp_img->height();
         ++y) {
      simg_int ebl = inp_img->height() - y + blur_level - 1;
      auto pix = inp_img->pixel(x, y - blur_level);
      r -= pix.r;
      g -= pix.g;
      b -= pix.b;
      res_img->pixel(x, y) = {{static_cast<std::uint8_t>(r / ebl)},
                              {static_cast<std::uint8_t>(g / ebl)},
                              {static_cast<std::uint8_t>(b / ebl)},
                              inp_img->pixel(x, y).a};
    }
  }
}

inline void horizontal_blur_single_i(simg &inp_img, simg &res_img,
                                     unsigned int blur_level) {
  seedimg::utils::hrz_thread(horizontal_blur_i_single_worker, inp_img, res_img,
                             blur_level);
}

inline void vertical_blur_single_i(simg &inp_img, simg &res_img,
                                   unsigned int blur_level) {
  seedimg::utils::vrt_thread(vertical_blur_i_single_worker, inp_img, res_img,
                             blur_level);
}

inline void box_blur_single(simg &inp_img, simg &res_img,
                            unsigned int blur_level) {
  horizontal_blur_single_i(inp_img, res_img, blur_level);
  vertical_blur_single_i(res_img, inp_img, blur_level);
}

inline unsigned int clamped_blur_level(unsigned int blur_level, simg_int width,
                                       simg_int height) {
  return static_cast<unsigned int>(
      std::min(static_cast<int>(blur_level),
               std::min(static_cast<int>((width - 1)) / 2,
                        static_cast<int>((height - 1)) / 2)));
}

inline void brightness_alpha_worker(simg &input, simg &output,
                                    simg_int start_row, simg_int end_row,
                                    std::uint8_t intensity) {
  for (; start_row < end_row; ++start_row) {
    for (simg_int x = 0; x < input->width(); ++x) {
      auto &pix = input->pixel(x, start_row);

      output->pixel(x, start_row) = {
          {static_cast<std::uint8_t>(pix.r + intensity)},
          {static_cast<std::uint8_t>(pix.g + intensity)},
          {static_cast<std::uint8_t>(pix.b + intensity)},
          static_cast<std::uint8_t>(pix.a + intensity)};
    }
  }
}
void saturation_worker(simg &inp_img, simg &res_img, simg_int start,
                       simg_int end, float mul) {

  for (; start < end; ++start) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      res_img->pixel(x, start).s =
          seedimg::utils::clamp(inp_img->pixel(x, start).s * mul, 0, 100);
    }
  }
}
} // namespace simgdetails

namespace seedimg::filters {

static constexpr smat const SEPIA_MAT = {0.393f, 0.349f, 0.272f, 0.769f, 0.686f,
                                         0.534f, 0.189f, 0.168f, 0.131f};
static constexpr smat const GRAYSCALE_LUM_MAT = {0.2126f, 0.2126f, 0.2126f,
                                                 0.7152f, 0.7152f, 0.7152f,
                                                 0.0722f, 0.0722f, 0.0722f};

// filters that exclusively use this functionality will go in apply-mat.cpp to
// save unnecessary files.
// Current list: sepia, rotate_hue, grayscale
inline void apply_mat(simg &inp_img, simg &res_img, const fsmat &mat) {
  seedimg::utils::hrz_thread(simgdetails::apply_mat_worker, inp_img, res_img,
                             mat);
}

inline void apply_mat_i(simg &inp_img, const fsmat &mat) {
  apply_mat(inp_img, inp_img, mat);
}
inline void apply_mat(simg &inp_img, simg &res_img, const smat &mat) {
  apply_mat(inp_img, res_img, to_fsmat(mat));
}
inline void apply_mat_i(simg &inp_img, const smat &mat) {
  apply_mat(inp_img, inp_img, mat);
}

inline void apply_mat_lut(simg &inp_img, simg &res_img,
                          const seedimg::slut<seedimg::smat> &lut,
                          const lutvec &vec = {0, 0, 0}) {
  seedimg::utils::hrz_thread(simgdetails::apply_mat_lut_worker, inp_img,
                             res_img, lut, vec);
}
inline void apply_mat_lut_i(simg &inp_img,
                            const seedimg::slut<seedimg::smat> &lut,
                            const lutvec &vec = {0, 0, 0}) {
  apply_mat_lut(inp_img, inp_img, lut, vec);
}

inline void grayscale(simg &inp_img, simg &res_img, bool luminosity = true) {
  if (luminosity) {
    seedimg::utils::hrz_thread(simgdetails::grayscale_worker_luminosity,
                               inp_img, res_img);
  } else {
    seedimg::utils::hrz_thread(simgdetails::grayscale_worker_average, inp_img,
                               res_img);
  }
}

inline void grayscale_i(simg &inp_img, bool luminosity = true) {
  grayscale(inp_img, inp_img, luminosity);
}

inline void invert(simg &inp_img, simg &res_img) {
  seedimg::utils::hrz_thread(simgdetails::invert_worker, inp_img, res_img);
}
inline void invert_a(simg &inp_img, simg &res_img,
                     bool invert_alpha_only = false) {
  if (invert_alpha_only) {
    seedimg::utils::hrz_thread(simgdetails::invert_worker_alpha_only, inp_img,
                               res_img);
  } else {
    seedimg::utils::hrz_thread(simgdetails::invert_worker_alpha, inp_img,
                               res_img);
  }
}
inline void invert_i(simg &inp_img) { invert(inp_img, inp_img); }
inline void invert_a_i(simg &inp_img, bool invert_alpha_only = false) {
  invert_a(inp_img, inp_img, invert_alpha_only);
}

inline void rotate_cw(simg &inp_img, simg &res_img) {
  for (simg_int y = 0; y < res_img->height(); ++y) {
    for (simg_int x = 0; x < res_img->width(); ++x) {
      res_img->pixel(x, y) = inp_img->pixel(res_img->height() - y - 1, x);
    }
  }
}
inline void rotate_180(simg &inp_img, simg &res_img) {
  for (simg_int y = 0; y < inp_img->height(); ++y) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      res_img->pixel(x, y) =
          inp_img->pixel(inp_img->width() - x - 1, inp_img->height() - y - 1);
    }
  }
}
inline void rotate_ccw(simg &inp_img, simg &res_img) {
  for (simg_int y = 0; y < res_img->height(); ++y) {
    for (simg_int x = 0; x < res_img->width(); ++x) {
      res_img->pixel(x, y) = inp_img->pixel(y, x);
    }
  }
}

inline void rotate_cw_i(simg &inp_img) {
  simg res_img = seedimg::make(inp_img->height(), inp_img->width());
  rotate_cw(inp_img, res_img);
  inp_img.reset(res_img.release());
}

inline void rotate_180_i(simg &inp_img) {
  // reverse each row
  std::reverse(inp_img->data(),
               inp_img->data() + inp_img->width() * inp_img->height());
}
inline void rotate_ccw_i(simg &inp_img) {
  simg res_img = seedimg::make(inp_img->height(), inp_img->width());
  rotate_ccw(inp_img, res_img);
  inp_img.reset(res_img.release());
}

inline void v_mirror(simg &inp_img, simg &res_img) {
  for (simg_int y = 0; y < inp_img->height(); ++y) {
    std::copy(inp_img->row(y), inp_img->row(y) + inp_img->width(),
              res_img->row(res_img->height() - y - 1));
  }
}
inline void h_mirror(simg &inp_img, simg &res_img) {
  for (simg_int y = 0; y < res_img->height(); ++y) {
    for (simg_int x = 0; x < res_img->width(); ++x) {
      res_img->pixel(inp_img->width() - x - 1, y) = inp_img->pixel(x, y);
    }
  }
}
inline void v_mirror_i(simg &inp_img) {
  simg_int h = inp_img->height();
  simg_int w = inp_img->width();
  seedimg::pixel *row = new seedimg::pixel[w];
  // swaps last row and first row.
  // then, swaps second last and second row
  // continue h/2 times. automatically floors h, because for odd height the
  // middle row must not be touched.
  for (simg_int y = 0; y < h / 2; ++y) {
    std::copy(inp_img->row(y), inp_img->row(y + 1), row);
    std::copy(inp_img->row(h - y - 1), inp_img->row(h - y), inp_img->row(y));
    std::copy(row, row + w, inp_img->row(h - y - 1));
  }

  delete[] row;
}
inline void h_mirror_i(simg &inp_img) {
  for (simg_int y = 0; y < inp_img->height(); ++y) {
    std::reverse(inp_img->row(y), inp_img->row(y) + inp_img->width());
  }
}

inline bool crop(simg &inp_img, simg &res_img, seedimg::point p1,
                 seedimg::point p2) {
  if (p1 == seedimg::point{0, 0} &&
      p2 == seedimg::point{inp_img->width(), inp_img->height()}) {
    return true;
  }
  if (!(seedimg::is_on_rect({0, 0}, {inp_img->width(), inp_img->height()},
                            p1) &&
        seedimg::is_on_rect({0, 0}, {inp_img->width(), inp_img->height()}, p2)))
    return false;
  auto least_crop_x = std::min(p1.x, p2.x);
  auto least_crop_y = std::min(p1.y, p2.y);

  for (simg_int y = 0; y < res_img->height(); ++y) {
    // apparently std::crop is faster, idk
    std::copy(inp_img->row(y + least_crop_y) + least_crop_x,
              inp_img->row(y + least_crop_y) + least_crop_x + res_img->width(),
              res_img->row(y));
    /*std::memcpy(res_img->row(y), inp_img->row(y + least_crop_y) +
     * least_crop_x,
                res_img->width() * sizeof(seedimg::pixel));*/
  }
  return true;
}

inline bool crop_i(simg &inp_img, seedimg::point p1, seedimg::point p2) {
  seedimg::uimg *unmanaged = static_cast<seedimg::uimg *>(inp_img.get());
  if (p1 == seedimg::point{0, 0} &&
      p2 == seedimg::point{unmanaged->width(), unmanaged->height()}) {
    return true;
  }
  if (!(seedimg::is_on_rect({0, 0}, {unmanaged->width(), unmanaged->height()},
                            p1) &&
        seedimg::is_on_rect({0, 0}, {unmanaged->width(), unmanaged->height()},
                            p2)))
    return false;

  auto least_crop_x = std::min(p1.x, p2.x);
  auto least_crop_y = std::min(p1.y, p2.y);

  auto dims = get_rect_dimensions(p1, p2);

  // width is dims.x, height is dims.y.
  for (simg_int y = 0; y < dims.y; ++y) {
    std::memmove(unmanaged->data() + y * dims.x,
                 unmanaged->row(y + least_crop_y) + least_crop_x,
                 dims.x * sizeof(seedimg::pixel));
  }

  unmanaged->set_width(dims.x);
  unmanaged->set_height(dims.y);

  auto *tmp = static_cast<seedimg::pixel *>(
      std::realloc(unmanaged->data(), unmanaged->width() * unmanaged->height() *
                                          sizeof(seedimg::pixel)));
  if (tmp == nullptr)
    throw std::bad_alloc{};
  unmanaged->set_data(tmp);
  return true;
}

inline void blur_i(simg &inp_img, unsigned int blur_level,
                   std::uint8_t it = 3) {
  if (blur_level == 0)
    return;
  blur_level = simgdetails::clamped_blur_level(blur_level, inp_img->width(),
                                               inp_img->height());
  auto res_img = seedimg::make(inp_img->width(), inp_img->height());
  for (std::uint8_t i = 0; i < it; ++i) {
    simgdetails::box_blur_single(inp_img, res_img, blur_level);
  }
}

inline void h_blur_i(simg &inp_img, unsigned int blur_level,
                     std::uint8_t it = 3) {
  if (blur_level == 0)
    return;
  blur_level = simgdetails::clamped_blur_level(blur_level, inp_img->width(),
                                               inp_img->height());
  auto res_img = seedimg::make(inp_img->width(), inp_img->height());
  for (std::uint8_t i = 0; i < it; ++i) {
    if (i % 2 == 0) {
      simgdetails::horizontal_blur_single_i(inp_img, res_img, blur_level);
    } else {
      simgdetails::horizontal_blur_single_i(res_img, inp_img, blur_level);
    }
  }
  if (it % 2 == 0 || it == 1)
    inp_img.reset(res_img.release());
}

inline void v_blur_i(simg &inp_img, unsigned int blur_level,
                     std::uint8_t it = 3) {
  if (blur_level == 0)
    return;
  blur_level = simgdetails::clamped_blur_level(blur_level, inp_img->width(),
                                               inp_img->height());

  auto res_img = seedimg::make(inp_img->width(), inp_img->height());
  for (std::uint8_t i = 0; i < it; ++i) {
    if (i % 2 == 0) {
      simgdetails::vertical_blur_single_i(inp_img, res_img, blur_level);
    } else {
      simgdetails::vertical_blur_single_i(res_img, inp_img, blur_level);
    }
  }

  if (it % 2 == 0 || it == 1)
    inp_img.reset(res_img.release());
}

/** Apply a square kernel convolution to an image.
 * NOTE: if weren't a square kernel, the image stays intact.
 * NOTE: alpha is passed-as it is, it's not convoluted.
 */
inline void convolution(simg &input, std::vector<std::vector<float>> kernel) {
  if (kernel.size() == 0 || kernel[0].size() == 0)
    return;

  simg_int kw = kernel[0].size();
  simg_int kh = kernel.size();

  float neg_sum = 0.0f, pos_sum = 0.0f;

  for (const auto &r : kernel)
    for (auto e : r)
      if (std::signbit(e))
        neg_sum -= e;
      else
        pos_sum += e;

  // approximated the center coordinate of kernel, for a symmetric one
  // the origin is the current pixel.
  simg_int ko_x = kw / 2, ko_y = kh / 2;

  // normalised kernel.
  std::vector<std::vector<float>> norm_kernel(kh, std::vector<float>(kw, 0.0));

  // flip the kernel both vertically and horizontally +
  // normalise all the elements.
  for (simg_int y = 0; y < kh; ++y)
    for (simg_int x = 0; x < kw; ++x)
      norm_kernel[kh - y - 1][kw - x - 1] =
          kernel[y][x] / (std::signbit(kernel[y][x]) ? neg_sum : pos_sum);

  for (simg_int y = 0; y < input->height(); ++y) {
    for (simg_int x = 0; x < input->width(); ++x) {
      struct {
        float r = 0, g = 0, b = 0, a = 0;
      } o;

      for (std::size_t dy = 0; dy < kh; ++dy) {
        for (std::size_t dx = 0; dx < kw; ++dx) {
          // When the 2D sliding window is at the edge of the image plane,
          // the edges of the kernel fall outside the bound, to conceal that
          // a mixture of wrapping and mirroring is used.
          auto pix = input->pixel(
              static_cast<simg_int>(
                  static_cast<unsigned long long>(std::llabs(
                      static_cast<long long>(x) + static_cast<long long>(dx) -
                      static_cast<long long>(ko_x))) %
                  input->width()),
              static_cast<simg_int>(
                  static_cast<unsigned long long>(std::llabs(
                      static_cast<long long>(y) + static_cast<long long>(dy) -
                      static_cast<long long>(ko_y))) %
                  input->height()));

          o.r += static_cast<float>(pix.r) * norm_kernel[dy][dx];
          o.g += static_cast<float>(pix.g) * norm_kernel[dy][dx];
          o.b += static_cast<float>(pix.b) * norm_kernel[dy][dx];
          // o.a += static_cast<float>(pix.a) * norm_kernel[dy][dx];
        }
      }

      using namespace seedimg::utils;

      input->pixel(x, y) = {{static_cast<std::uint8_t>(clamp(o.r, 0, 255))},
                            {static_cast<std::uint8_t>(clamp(o.g, 0, 255))},
                            {static_cast<std::uint8_t>(clamp(o.b, 0, 255))},
                            static_cast<std::uint8_t>(clamp(o.a, 0, 255))};
    }
  }
}

constexpr seedimg::fsmat generate_brightness_mat(float intensity) {
  return {1,         0,         0,         0,

          0,         1,         0,         0,

          0,         0,         1,         0,

          intensity, intensity, intensity, 1};
}

inline void brightness(simg &input, simg &output, float intensity) {
  apply_mat(input, output, generate_brightness_mat(intensity));
}
inline void brightness_i(simg &image, float intensity) {
  brightness(image, image, intensity);
}

inline void brightness_a(simg &input, simg &output, float intensity) {
  seedimg::utils::hrz_thread(simgdetails::brightness_alpha_worker, input,
                             output, intensity);
}
inline void brightness_a_i(simg &image, float intensity) {
  brightness_a(image, image, intensity);
}

inline void blend(std::pair<simg &, const std::uint8_t> input,
                  std::pair<simg &, const std::uint8_t> other, simg &output) {
  if (input.first->width() != other.first->width() ||
      input.first->height() != other.first->height())
    return;

  // reduce the image gain as needed.
  brightness_a(input.first, output, input.second);
  brightness_a_i(other.first, other.second);
  seedimg::utils::hrz_thread(simgdetails::pixel_add_worker, output,
                             other.first);
}
inline void blend_i(std::pair<simg &, const std::uint8_t> input,
                    std::pair<simg &, const std::uint8_t> other) {
  blend(input, other, input.first);
}

// apply_mat filters
inline void sepia(simg &inp_img, simg &res_img) {
  apply_mat(inp_img, res_img, SEPIA_MAT);
}
inline void sepia_i(simg &inp_img) { apply_mat_i(inp_img, SEPIA_MAT); }

inline void rotate_hue(simg &inp_img, simg &res_img, int angle) {
  apply_mat(inp_img, res_img, generate_hue_mat(angle));
}

inline void rotate_hue_i(simg &inp_img, int angle) {
  rotate_hue(inp_img, inp_img, angle);
}

constexpr seedimg::fsmat generate_contrast_mat(float intensity) {
  float t = (1.0f - intensity) / 2.0f;
  return {intensity, 0,         0,         0,

          0,         intensity, 0,         0,

          0,         0,         intensity, 0,

          t,         t,         t,         1};
}

inline void contrast(simg &input, simg &output, float intensity = 100.0) {
  apply_mat(input, output, generate_contrast_mat(intensity));
}
inline void contrast_i(simg &image, float intensity = 100.0) {
  contrast(image, image, intensity);
}

constexpr seedimg::fsmat generate_saturation_mat(float mul) {
  constexpr float rwgt = 0.2126f;
  constexpr float gwgt = 0.7152f;
  constexpr float bwgt = 0.0722f;
  float sr = (1.0f - mul) * rwgt;
  float sg = (1.0f - mul) * gwgt;
  float sb = (1.0f - mul) * bwgt;
  return {sr + mul, sr,       sr,       0,

          sg,       sg + mul, sg,       0,

          sb,       sb,       sb + mul, 0,

          0,        0,        0,        1};
}

// HSV colourspace filters
inline void saturation(simg &inp_img, simg &res_img, float mul) {
  if (inp_img->colourspace() == seedimg::colourspaces::hsv) {
    seedimg::utils::hrz_thread(simgdetails::saturation_worker, inp_img, res_img,
                               mul);
    static_cast<seedimg::uimg *>(res_img.get())
        ->set_colourspace(seedimg::colourspaces::hsv);
  } else {
    apply_mat(inp_img, res_img, generate_saturation_mat(mul));
  }
}
inline void saturation_i(simg &inp_img, float mul) {
  saturation(inp_img, inp_img, mul);
}
} // namespace seedimg::filters
namespace simgdetails {
#include "rh/from_ycbcr_jpeg_lut.rh"

inline void hsv2rgb_worker(simg &inp_img, simg &res_img, simg_int start,
                           simg_int end) {
  for (; start < end; start++) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix_src = inp_img->pixel(x, start);
      struct {
        float h;
        float s;
        float v;
      } pix{static_cast<float>(pix_src.h) * 2,
            static_cast<float>(pix_src.s) / 100,
            static_cast<float>(pix_src.v) / 100};
      float C = pix.v * pix.s;
      float X = C * (1 - std::fabs(std::fmod(pix.h / 60.0f, 2.0f) - 1));

      float m = pix.v - C;

      float componentsp[4] = {m, m, m, 0};
      switch (static_cast<int>(pix.h / 60)) {
      case 0: {
        componentsp[0] += C;
        componentsp[1] += X;
        componentsp[2] += 0;
      } break;
      case 1: {
        componentsp[0] += X;
        componentsp[1] += C;
        componentsp[2] += 0;
      } break;
      case 2: {
        componentsp[0] += 0;
        componentsp[1] += C;
        componentsp[2] += X;
      } break;
      case 3: {
        componentsp[0] += 0;
        componentsp[1] += X;
        componentsp[2] += C;
      } break;
      case 4: {
        componentsp[0] += X;
        componentsp[1] += 0;
        componentsp[2] += C;
      } break;
      case 5: {
        componentsp[0] += C;
        componentsp[1] += 0;
        componentsp[2] += X;
      } break;
      }
      res_img->pixel(x, start) = {
          {static_cast<std::uint8_t>(componentsp[0] * 255)},
          {static_cast<std::uint8_t>(componentsp[1] * 255)},
          {static_cast<std::uint8_t>(componentsp[2] * 255)},
          pix_src.a};
    }
  }
}

inline void ycbcr_jpeg2rgb_worker(simg &inp_img, simg &res_img, simg_int start,
                                  simg_int end) {
  for (; start < end; ++start) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start);
      // ry* is all 1. we do not need a lookup because it will all be the same
      // as pix.y
      // gcb1 and bcr3 are zero in the matrix.
      res_img->pixel(x, start).r =
          static_cast<std::uint8_t>(pix.y + 0 + jpeg_bcr1[pix.cr]);
      res_img->pixel(x, start).g = static_cast<std::uint8_t>(
          pix.y + jpeg_gcb2[pix.cb] + jpeg_bcr2[pix.cr]);
      res_img->pixel(x, start).b =
          static_cast<std::uint8_t>(pix.y + jpeg_gcb3[pix.cb] + 0);
    }
  }
}

inline float fmodulo(float x, float N) {
  return std::fmod(x, N) + (std::fmod(x, N) < 0) * N;
}

inline bool feq(float a, float b) { return std::fabs(a - b) < .0000001f; }

inline void rgb2hsv_worker(simg &inp_img, simg &res_img, simg_int start,
                           simg_int end) {
  for (; start < end; ++start) {
    for (simg_int x = 0; x < res_img->width(); ++x) {
      // the p suffix in this sense stands for prime. normally we use R' G' B'
      // to represent normalized colour.
      float rp = static_cast<float>(inp_img->pixel(x, start).r) /
                 static_cast<float>(seedimg::img::MAX_PIXEL_VALUE);
      float gp = static_cast<float>(inp_img->pixel(x, start).g) /
                 static_cast<float>(seedimg::img::MAX_PIXEL_VALUE);
      float bp = static_cast<float>(inp_img->pixel(x, start).b) /
                 static_cast<float>(seedimg::img::MAX_PIXEL_VALUE);
      float cmax = std::max(rp, std::max(gp, bp));
      float cmin = std::min(rp, std::min(gp, bp));
      float delta = cmax - cmin;

      std::uint8_t hue;
      std::uint8_t sat = 0;
      std::uint8_t val = static_cast<std::uint8_t>(cmax * 100.0f);

      if (feq(rp, cmax)) {
        hue = static_cast<std::uint8_t>(30 * fmodulo((gp - bp) / delta, 6));
      } else if (feq(gp, cmax)) {
        hue = static_cast<std::uint8_t>(60 * ((bp - rp) / delta + 2));
      } else {
        hue = static_cast<std::uint8_t>(60 * ((rp - gp) / delta + 4));
      }

      // saturation
      if (!feq(cmax, 0)) {
        sat = static_cast<std::uint8_t>((delta / cmax) * 100.0f);
      }
      res_img->pixel(x,
                     start) = {{hue}, {sat}, {val}, inp_img->pixel(x, start).a};
    }
  }
}
} // namespace simgdetails

// Colourspace conversion
namespace seedimg::filters::cconv {
static constexpr seedimg::smat const rgb_ycbcr_bt601_mat = {
    298.082f / 256.0f, 298.082f / 256.0f,  298.082f / 256.0f,
    0.0f / 256.0f,     -100.291f / 256.0f, 516.412f / 256.0f,
    408.583f / 256.0f, -208.120f / 256.0f, 0.0f / 256.0f};

static constexpr seedimg::lutvec const rgb_ycbcr_bt601_vec = {
    -222.921f, 135.576f, -276.836f};

static constexpr seedimg::slut<seedimg::smat> const rgb_ycbcr_bt601_lut =
    seedimg::utils::gen_lut(rgb_ycbcr_bt601_mat);

static constexpr seedimg::smat const ycbcr_jpeg_rgb_mat = {
    0.299f,     -0.168736f, 0.5f, 0.587f,    -0.331264f,
    -0.418688f, 0.114f,     0.5f, -0.081312f};

static constexpr seedimg::lutvec const ycbcr_jpeg_rgb_vec = {0, 128, 128};

static constexpr seedimg::slut<seedimg::smat> const ycbcr_jpeg_rgb_lut =
    seedimg::utils::gen_lut(ycbcr_jpeg_rgb_mat);

static constexpr seedimg::smat const ycbcr_bt601_rgb_mat = {
    65.738f / 256.0f,  -37.945f / 256.0f, 112.439f / 256.0f,
    129.057f / 256.0f, -74.494f / 256.0f, -94.154f / 256.0f,
    25.064f / 256.0f,  112.439f / 256.0f, -18.285f / 256.0f};

static constexpr seedimg::lutvec const ycbcr_bt601_rgb_vec = {16, 128, 128};

static constexpr seedimg::slut<seedimg::smat> const ycbcr_bt601_rgb_lut =
    seedimg::utils::gen_lut(ycbcr_bt601_rgb_mat);

inline void rgb(simg &inp_img, simg &res_img) {
  if (inp_img->colourspace() == seedimg::colourspaces::rgb) {
    return;
  } else if (inp_img->colourspace() == seedimg::colourspaces::hsv) {
    seedimg::utils::hrz_thread(simgdetails::hsv2rgb_worker, inp_img, res_img);
    // hsv2rgb_worker(inp_img, res_img, 0, inp_img->height());
  } else if (inp_img->colourspace() == seedimg::colourspaces::ycbcr_jpeg) {
    seedimg::utils::hrz_thread(simgdetails::ycbcr_jpeg2rgb_worker, inp_img,
                               res_img);
  } else if (inp_img->colourspace() == seedimg::colourspaces::ycbcr_bt601) {
    seedimg::filters::apply_mat_lut(inp_img, res_img, rgb_ycbcr_bt601_lut,
                                    rgb_ycbcr_bt601_vec);
  }
  static_cast<seedimg::uimg *>(res_img.get())
      ->set_colourspace(seedimg::colourspaces::rgb);
}

inline void rgb_i(simg &inp_img) { rgb(inp_img, inp_img); }

inline void hsv(simg &inp_img, simg &res_img) {
  if (inp_img->colourspace() == seedimg::colourspaces::hsv) {
    return;
  } else if (inp_img->colourspace() == seedimg::colourspaces::rgb) {
    seedimg::utils::hrz_thread(simgdetails::rgb2hsv_worker, inp_img, res_img);
    // rgb2hsv_worker(inp_img, res_img, 0, inp_img->height());
  } else if (inp_img->colourspace() == seedimg::colourspaces::ycbcr_jpeg ||
             inp_img->colourspace() == seedimg::colourspaces::ycbcr_bt601) {
    rgb(inp_img, res_img);
    hsv(res_img, res_img);
  }
  static_cast<seedimg::uimg *>(res_img.get())
      ->set_colourspace(seedimg::colourspaces::hsv);
}
inline void hsv_i(simg &inp_img) { hsv(inp_img, inp_img); }

inline void ycbcr(simg &inp_img, simg &res_img, seedimg::colourspaces type) {
  if (inp_img->colourspace() == seedimg::colourspaces::ycbcr_jpeg ||
      inp_img->colourspace() == seedimg::colourspaces::ycbcr_bt601) {
    return;
  } else if (inp_img->colourspace() == seedimg::colourspaces::rgb) {
    if (type == seedimg::colourspaces::ycbcr_jpeg) {
      seedimg::filters::apply_mat_lut(inp_img, res_img, ycbcr_jpeg_rgb_lut,
                                      ycbcr_jpeg_rgb_vec);
    } else if (type == seedimg::colourspaces::ycbcr_bt601) {
      seedimg::filters::apply_mat_lut(inp_img, res_img, ycbcr_bt601_rgb_lut,
                                      ycbcr_bt601_rgb_vec);
    }
  } else if (inp_img->colourspace() == seedimg::colourspaces::hsv) {
    rgb(inp_img, res_img);
    ycbcr(res_img, res_img, type);
  }
  static_cast<seedimg::uimg *>(res_img.get())->set_colourspace(type);
}
inline void ycbcr_i(simg &inp_img, seedimg::colourspaces type) {
  ycbcr(inp_img, inp_img, type);
}

} // namespace seedimg::filters::cconv

namespace seedimg::filters {

/**
 * @brief Lazily evaluated linear chain of filters wrapping I/O based
 * filter-functions. This is equavalient of wrapping multiple filter functions
 * in a single function, except it holds an underlying queue.
 *
 * @note Result of invoking a filter function is ignored.
 * @note Filters that change dimensions of image cause potential UB.
 */
class filterchain {
private:
  std::vector<std::function<void(simg &, simg &)>> filters;

public:
  /**
   * @brief Push a function to end of the queue that follows the idioms of
   * seedimg filter-function definitions. It will bind any number of additional
   * arguments to the function (if it requires and given).
   *
   * @note Due to limitation with C++ parameter packs, default arguments of
   * filter function also must be specified when adding.
   *
   * @param func a callable object.
   * @param args custom arguments to pass to callable.
   */
  template <class F, class... Args>
  filterchain &add(F &&func, Args &&... args) {
    filters.push_back(std::bind(func, std::placeholders::_1,
                                std::placeholders::_2,
                                std::forward<Args>(args)...));

    return *this;
  }

  /**
   * @brief Pop-off the most recently added filter in queue.
   */
  filterchain &pop() {
    filters.pop_back();
    return *this;
  }

  /**
   * @brief Apply enqueued filters accumulatively from start to end.
   * @note If queue was empty, it just copies image data.
   *
   * @param input image to apply filters on.
   * @param output output of the accumulated result.
   */
  filterchain &eval(const simg &in, simg &out) {
    // copy all pixels first into output, for "f(out, out)" to work.
    std::copy(in->data(), in->data() + in->width() * in->height(), out->data());

    for (const auto &f : filters)
      f(out, out);

    return *this;
  }

  filterchain &eval(simg &img) {
    eval(img, img);
    return *this;
  }

  /**
   * @brief Same effect as a single image but evalualtes on multiple frames,
   * and does it inplace to avoid temporary allocations.
   *
   * @remark Should we do it I/O based here?
   *
   * @param imgs images to transform (inplace).
   * @param start index to start with.
   * @param end index to end with (0 = imgs.size() - 1).
   */
  filterchain &eval(anim &imgs, simg_int start = 0, simg_int end = 0) {
    end = end ? end : imgs.size();
    if (end <= start) // if nothing to do or invalid range.
      return *this;

    for (simg_int i = start; i < end; ++i)
      eval(imgs[i]);
    return *this;
  }
};

/**
 * @brief Lazily evaluated linear chain of filters wrapping inplace
 * filter-functions. This is equavalient of wrapping multiple filter functions
 * in a single function, except it holds an underlying queue.
 *
 * @note Result of invoking a filter function is ignored.
 */
class filterchain_i {
private:
  std::vector<std::function<void(simg &)>> filters;

public:
  /**
   * @brief Push a function to end of the queue that follows the idioms of
   * seedimg filter-function definitions. It will bind any number of additional
   * arguments to the function (if it requires and given).
   *
   * @note Due to limitation with C++ parameter packs, default arguments of
   * filter function also must be specified when adding.
   *
   * @param func a callable object.
   * @param args custom arguments to pass to callable.
   */
  template <class F, class... Args>
  filterchain_i &add(F &&func, Args &&... args) {
    filters.push_back([&](simg &input) -> void {
      std::invoke(func, input, std::forward<Args>(args)...);
    });

    return *this;
  }

  /**
   * @brief Pop-off the most recently added filter in queue.
   */
  filterchain_i &pop() {
    filters.pop_back();
    return *this;
  }

  /**
   * @brief Apply enqueued filters accumulatively from start to end.
   * @note If queue was empty, it just copies image data.
   *
   * @param input image to apply filters on.
   */
  filterchain_i &eval(simg &img) {
    for (const auto &f : filters)
      std::invoke(f, img);

    return *this;
  }

  /**
   * @brief Same effect as a single image but evalualtes on multiple frames.
   *
   * @param imgs images to transform (inplace).
   * @param start index to start with.
   * @param end index to end with (0 = imgs.size() - 1).
   */
  filterchain_i &eval(anim &imgs, simg_int start = 0, simg_int end = 0) {
    if (end == 0 || end > imgs.size())
      end = imgs.size();
    if (end <= start) // if nothing to do or invalid range.
      return *this;

    for (simg_int i = start; i < end; ++i)
      eval(imgs[i]);
    return *this;
  }
};

} // namespace seedimg::filters

#endif
