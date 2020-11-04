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

template<typename T = std::size_t> constexpr T round_up(T inp, T mul) noexcept {
    if (mul == 0) return inp;

    T remainder = inp % mul;
    if (remainder == 0) return inp;

    return inp + mul - remainder;
}

template <typename I, typename MinT, typename MaxT>
constexpr auto clamp(I a, MinT min, MaxT max) {
  return a > max ? max : a < min ? min : a;
}

template<typename T, typename... Args> void hrz_thread(T&& func, simg& inp_img, simg& res_img, Args&... args) {
    auto                     start_end = inp_img->start_end_rows();
    std::vector<std::thread> workers(start_end.size());
    for (std::size_t i = 0; i < workers.size(); i++) {
        workers.at(i) = std::thread(std::forward<T>(func),
                                    std::ref(inp_img),
                                    std::ref(res_img),
                                    start_end.at(i).first,
                                    start_end.at(i).second,
                                    std::forward<Args>(args)...);
    }
    for (auto&& worker : workers) { worker.join(); }
}

template<typename T, typename... Args> void vrt_thread(T&& func, simg& inp_img, simg& res_img, Args&&... args) {
    auto&&                   start_end = inp_img->start_end_cols();
    std::vector<std::thread> workers(start_end.size());
    for (std::size_t i = 0; i < workers.size(); i++) {
        workers.at(i) = std::thread(std::forward<T>(func),
                                    std::ref(inp_img),
                                    std::ref(res_img),
                                    start_end.at(i).first,
                                    start_end.at(i).second,
                                    std::forward<Args>(args)...);
    }
    for (auto&& worker : workers) { worker.join(); }
}

// number falls between A and B
// transform to fall between C and D
template<typename T> auto map_range(T old_val, T a, T b, T c, T d) { return c + ((old_val - a) * (d - c)) / (b - a); }

template<typename IIter, typename OIter, typename T>
void map_range_iter(IIter start, IIter end, OIter output, T a, T b, T c, T d) {
    auto inelem  = start;
    auto outelem = output;

    while (inelem != end) {
        *outelem = map_range(*inelem, a, b, c, d);
        ++inelem;
        ++outelem;
    }
}

template<typename... Args> constexpr auto make_array(Args&&... args) {
    return std::array<std::decay_t<std::common_type_t<Args...>>, sizeof...(Args)>{std::forward<Args>(args)...};
}

namespace endian {
inline std::uint16_t from_u16_big(uint8_t* cb) {
    return static_cast<std::uint16_t>(cb[1] << 8) | static_cast<std::uint16_t>(cb[0]);
}
inline void to_u16_big(std::uint16_t n, std::uint8_t* out) {
    out[0] = n >> 8 & 0xff;
    out[1] = n & 0xff;
}

inline simg_int from_u32_big(uint8_t* cb) {
    return static_cast<simg_int>(cb[0] << 24) | static_cast<simg_int>(cb[1] << 16) | static_cast<simg_int>(cb[2] << 8)
           | static_cast<simg_int>(cb[3]);
}
inline void to_u32_big(simg_int n, std::uint8_t* out) {
    out[0] = n >> 24 & 0xff;
    out[1] = n >> 16 & 0xff;
    out[2] = n >> 8 & 0xff;
    out[3] = n & 0xff;
}
} // namespace endian
} // namespace seedimg::utils
#endif
