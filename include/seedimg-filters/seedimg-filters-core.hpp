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

static constexpr smat const SEPIA_MAT = {0.393f, 0.349f, 0.272f, 0.769f, 0.686f,
                                         0.534f, 0.189f, 0.168f, 0.131f};
static constexpr smat const GRAYSCALE_LUM_MAT = {0.2126f, 0.2126f, 0.2126f,
                                                 0.7152f, 0.7152f, 0.7152f,
                                                 0.0722f, 0.0722f, 0.0722f};

// filters that exclusively use this functionality will go in apply-mat.cpp to
// save unnecessary files.
// Current list: sepia, rotate_hue
void apply_mat(simg &inp_img, simg &res_img, const fsmat &mat);
void apply_mat_i(simg &inp_img, const fsmat &mat);
void apply_mat(simg &inp_img, simg &res_img, const smat &mat);
void apply_mat_i(simg &inp_img, const smat &mat);

void apply_mat_lut(simg &inp_img, simg &res_img,
                   const seedimg::slut<seedimg::smat> &lut,
                   const lutvec &vec = {0, 0, 0});
void apply_mat_lut_i(simg &inp_img, const seedimg::slut<seedimg::smat> &lut,
                     const lutvec &vec = {0, 0, 0});

void grayscale(simg &inp_img, simg &res_img, bool luminosity = true);
void grayscale_i(simg &inp_img, bool luminosity = true);

void invert(simg &inp_img, simg &res_img);
void invert_a(simg &inp_img, simg &res_img, bool invert_alpha_only = false);
void invert_i(simg &inp_img);
void invert_a_i(simg &inp_img, bool invert_alpha_only = false);

void rotate_cw(simg &inp_img, simg &res_img);
void rotate_180(simg &inp_img, simg &res_img);
void rotate_ccw(simg &inp_img, simg &res_img);
void rotate_cw_i(simg &inp_img);
void rotate_180_i(simg &inp_img);
void rotate_ccw_i(simg &inp_img);

void v_mirror(simg &inp_img, simg &res_img);
void h_mirror(simg &inp_img, simg &res_img);
void v_mirror_i(simg &inp_img);
void h_mirror_i(simg &inp_img);

bool crop(simg &inp_img, simg &res_img, seedimg::point p1, seedimg::point p2);
bool crop_i(simg &inp_img, seedimg::point p1, seedimg::point p2);

void blur_i(simg &inp_img, unsigned int blur_level, std::uint8_t it = 3);
void h_blur_i(simg &inp_img, unsigned int blur_level, std::uint8_t it = 3);
void v_blur_i(simg &inp_img, unsigned int blur_level, std::uint8_t it = 3);

/** Apply a square kernel convolution to an image.
 * NOTE: if weren't a square kernel, the image stays intact.
 * NOTE: alpha is passed-as it is, it's not convoluted.
 */
void convolution(simg &input, std::vector<std::vector<float>> kernel);

void brightness(simg &input, simg &output, std::uint8_t intensity);
void brightness_i(simg &image, std::uint8_t intensity);
void brightness_a(simg &input, simg &output, std::uint8_t intensity);
void brightness_a_i(simg &image, std::uint8_t intensity);

void blend(std::pair<simg &, const std::uint8_t> input,
           std::pair<simg &, const std::uint8_t> other, simg &output);
void blend_i(std::pair<simg &, const std::uint8_t> input,
             std::pair<simg &, const std::uint8_t> other);

// apply_mat filters
void sepia(simg &inp_img, simg &res_img);
void sepia_i(simg &inp_img);

void rotate_hue(simg &inp_img, simg &res_img, int angle);
void rotate_hue_i(simg &inp_img, int angle);

// HSV colourspace filters
void saturation(simg &inp_img, simg &res_img, float mul);
void saturation_i(simg &inp_img, float mul);

void contrast(simg &input, simg &output, float intensity = 100.0,
              bool alpha = false);
void contrast_i(simg &image, float intensity = 100.0, bool alpha = false);
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
  for (std::size_t i = 1; i < mats.size(); i++) {
    for (std::size_t j = 0; j < 4; j++) {
      for (std::size_t k = 0; k < 4; k++) {
        res[j * 4 + k] = res[j * 4] * mats[i][0 + j] +
                         res[j * 4 + 1] * mats[i][4 + j] +
                         res[j * 4 + 2] * mats[i][8 + j] + mats[i][12 + j];
      }
    }
  }
  return res;
}

template <typename T> constexpr smat compose_smats(const T &mats) {
  smat res = mats[0];
  for (std::size_t i = 1; i < mats.size(); i++) {
    for (std::size_t j = 0; j < 3; j++) {
      for (std::size_t k = 0; k < 3; k++) {
        res[j * 3 + k] = res[j * 3] * mats[i][0 + j] +
                         res[j * 3 + 1] * mats[i][3 + j] +
                         res[j * 3 + 2] * mats[i][6 + j];
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
  return fsmat{
      mat[0], mat[1], mat[2], 0.0f, mat[3], mat[4], mat[5], 0.0f,
      mat[6], mat[7], mat[8], 0.0f, 0.0f,   0.0f,   0.0f,   1.0f,
  };
}

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
