/***********************************************************************
    seedimg - module based image manipulation library written in modern C++
    Copyright (C) 2020 tripulse, telugu-boy

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

#ifndef SEEDIMG_UTILS_HPP
#define SEEDIMG_UTILS_HPP

#include <array>
#include <seedimg.hpp>
#include <algorithm>
#include <thread>

namespace seedimg {
namespace utils {
constexpr bool is_on_rect(seedimg::point xy1, seedimg::point xy2,
                          seedimg::point point) noexcept {
  return xy1.x <= point.x && point.x <= xy2.x && xy1.y <= point.y &&
         point.y <= xy2.y;
}

constexpr seedimg::point get_rect_dimensions(seedimg::point p1,
                                             seedimg::point p2) noexcept {
  auto ordered_x = std::minmax(p1.x, p2.x);
  auto ordered_y = std::minmax(p1.y, p2.y);
  // width, height
  return {ordered_x.second - ordered_x.first,
          ordered_y.second - ordered_y.first};
}

template <typename T = std::size_t>
constexpr T round_up(T inp, T mul) noexcept {
  if (mul == 0)
    return inp;

  T remainder = inp % mul;
  if (remainder == 0)
    return inp;

  return inp + mul - remainder;
}

template <typename I, typename MinT, typename MaxT>
constexpr auto clamp(I a, MinT min, MaxT max) {
    return a > max ? max : a < min ? min : a;
}

std::vector<std::pair<simg_int, simg_int>> start_end_rows(const simg& inp_img) noexcept {
  std::vector<std::pair<simg_int, simg_int>> res;
  auto processor_count =
      std::min(inp_img->height(),
               static_cast<simg_int>(std::thread::hardware_concurrency()));
  if (processor_count == 0)
    processor_count = 1;
  res.reserve(static_cast<std::size_t>(processor_count));
  simg_int rows_per_thread = inp_img->height() / processor_count;
  for (simg_int i = 0; i < processor_count * rows_per_thread;
       i += rows_per_thread)
    res.push_back({i, i + rows_per_thread});
  res[res.size() - 1].second += inp_img->height() % processor_count;
  return res;
}

std::vector<std::pair<simg_int, simg_int>> start_end_cols(const simg& inp_img) noexcept {
  std::vector<std::pair<simg_int, simg_int>> res;
  auto processor_count =
      std::min(inp_img->width(),
               static_cast<simg_int>(std::thread::hardware_concurrency()));
  if (processor_count == 0)
    processor_count = 1;
  res.reserve(static_cast<std::size_t>(processor_count));
  simg_int cols_per_thread = inp_img->width() / processor_count;
  for (simg_int i = 0; i < processor_count * cols_per_thread;
       i += cols_per_thread)
    res.push_back({i, i + cols_per_thread});
  res[res.size() - 1].second += inp_img->width() % processor_count;
  return res;
}

template <typename T, typename... Args>
void hrz_thread(T &&func, simg &inp_img, simg &res_img, Args &... args) {
  auto start_end = start_end_rows(inp_img);
  std::vector<std::thread> workers(start_end.size());
  for (std::size_t i = 0; i < workers.size(); i++) {
    workers.at(i) =
        std::thread(std::forward<T>(func), std::ref(inp_img), std::ref(res_img),
                    start_end.at(i).first, start_end.at(i).second,
                    std::forward<Args>(args)...);
  }
  for (auto &&worker : workers) {
    worker.join();
  }
}

template <typename T, typename... Args>
void vrt_thread(T &&func, simg &inp_img, simg &res_img, Args &&... args) {
  auto &&start_end = start_end_cols(inp_img);
  std::vector<std::thread> workers(start_end.size());
  for (std::size_t i = 0; i < workers.size(); i++) {
    workers.at(i) =
        std::thread(std::forward<T>(func), std::ref(inp_img), std::ref(res_img),
                    start_end.at(i).first, start_end.at(i).second,
                    std::forward<Args>(args)...);
  }
  for (auto &&worker : workers) {
    worker.join();
  }
}

// number falls between A and B
// transform to fall between C and D
template <typename T> auto map_range(T old_val, T a, T b, T c, T d) {
  return c + ((old_val - a) * (d - c)) / (b - a);
}

template <typename IIter, typename OIter, typename T>
void map_range_iter(IIter start, IIter end, OIter output, T a, T b, T c, T d) {
  auto inelem = start;
  auto outelem = output;

  while (inelem != end) {
    *outelem = map_range(*inelem, a, b, c, d);
    ++inelem;
    ++outelem;
  }
}
} // namespace seedimg::utils
} // namespace seedimg
#endif
