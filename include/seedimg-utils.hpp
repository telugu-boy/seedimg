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
#include <thread>

namespace seedimg::utils {

template <typename T, typename I, typename MinT, typename MaxT>
inline T clamp(I a, MinT min, MaxT max) {
  return a > max ? max : a < min ? min : static_cast<T>(a);
}

namespace {
template <std::size_t... I>
static constexpr inline std::array<float, sizeof...(I)>
gen_dot_arr(std::index_sequence<I...>, float const elem) {
  return std::array<float, sizeof...(I)>{elem * I...};
}
template <typename T, std::size_t MaxPV, std::size_t... Amt>
static constexpr inline std::array<std::array<typename T::value_type, MaxPV>,
                                   sizeof...(Amt)>
group_dots_lut(std::index_sequence<Amt...>, const T &mat) {
  constexpr auto len = std::make_index_sequence<MaxPV>{};
  return {gen_dot_arr(len, mat[Amt])...};
}
} // namespace
// need to add 1 to MAX_PIXEL_VALUE because 256 values can be represented
template <typename T, std::size_t MaxPV = seedimg::img::MAX_PIXEL_VALUE + 1,
          std::size_t Amt = sizeof(T) / sizeof(typename T::value_type)>
constexpr auto gen_lut(const T &mat) {
  return group_dots_lut<T, MaxPV>(std::make_index_sequence<Amt>{}, mat);
}

template <typename T, typename... Args>
void hrz_thread(T &&func, simg &inp_img, simg &res_img, Args &... args) {
  auto start_end = inp_img->start_end_rows();
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
  auto &&start_end = inp_img->start_end_cols();
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
} // namespace seedimg::utils
#endif
