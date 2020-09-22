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

/**
 * @brief Column-major iteration. As the buffer internally is row-major,
 * and at certain conditions one requires column-major ordering.
 *
 * @note Not all features are implemented, it is exclusively built for
 * the seedimg::extras::resize_bilin function.
 */
class column_iterator {
private:
    simg&       img;
    std::size_t col;
//    std::size_t pos;
public:
    column_iterator(simg& img, std::size_t col = 0) :
        img(img), col(col) {}

//    seedimg::pixel& operator* () noexcept {
//        // formula for indexing pixels in column major order is:
//        //    index = height * pos + col
//        return img->data()[img->height()*pos + col];
//    }
//
//    seedimg::pixel* operator++() noexcept {
//        // pos is bounded to [0..height), when it overflows
//        // increment 'col' and reset 'pos' to zero.
//        if(++pos == img->height())
//            (void)++col, pos = 0;
//        return img->data();
//    }
//
//    seedimg::pixel* operator--() noexcept {
//        if(pos == 0)
//            (void)--col, pos = img->height() - 1;
//        else --pos;
//        return img->data();
//    }

    seedimg::pixel& operator[](std::size_t i) const noexcept {
        return img->data()[img->width()*i + col];
    }
    void set_column(std::size_t c) noexcept { col = c; }
};

} // namespace seedimg::utils
#endif
