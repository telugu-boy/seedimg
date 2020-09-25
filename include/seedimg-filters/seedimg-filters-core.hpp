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

#include <cmath>
#include <functional>
#include <seedimg.hpp>

const float PI = 4 * std::atan(1.0f);
namespace seedimg {
// Seedimg MATrix
typedef std::array<float, 9> smat;
// Full size Seedimg MATrix
typedef std::array<float, 16> fsmat;
// LookUp Table (additonal) Vector
typedef std::array<float, 3> lutvec;
// Seedimg LookUp Table
template <typename T>
using slut = std::array<std::array<float, seedimg::img::MAX_PIXEL_VALUE + 1>,
                        sizeof(T) / sizeof(typename T::value_type)>;
} // namespace seedimg

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

namespace cconv {};
namespace ocl {}
} // namespace seedimg::filters

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
    filters.push_back([&](simg &input, simg &output) -> void {
      func(input, output, std::forward<Args>(args)...);
    });

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
    filters.push_back(
        [&](simg &input) -> void { func(input, std::forward<Args>(args)...); });

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
      f(img);

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
