/**********************************************************************
    seedimg - module based image manipulation library written in modern
                C++ Copyright(C) 2020 telugu-boy

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
#ifndef SEEDIMG_CORE_H
#define SEEDIMG_CORE_H

#include <algorithm>
#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <thread>
#include <vector>

typedef std::size_t simg_int;

namespace seedimg {

typedef std::pair<simg_int, simg_int> point;

struct pixel {
  std::uint8_t r;
  std::uint8_t g;
  std::uint8_t b;
  std::uint8_t a;
  inline bool operator==(const pixel &other) const noexcept {
    return std::tie(r, g, b, a) == std::tie(other.r, other.g, other.b, other.a);
  }
  inline bool operator!=(const pixel &other) const noexcept {
    return !(*this == other);
  }
};

class img {
public:
  static constexpr std::uint8_t MAX_PIXEL_VALUE = UINT8_MAX;
  img();
  img(simg_int w, simg_int h);
  img(simg_int w, simg_int h, seedimg::pixel *u_data);
  img(seedimg::img const &img_);
  img(seedimg::img &&other);

  ~img();

  std::vector<std::pair<simg_int, simg_int>> start_end_rows();

  std::vector<std::pair<simg_int, simg_int>> start_end_cols();

  seedimg::pixel &pixel(simg_int x, simg_int y) noexcept;
  seedimg::pixel &pixel(seedimg::point p) noexcept;
  seedimg::pixel &pixel(simg_int x);
  seedimg::pixel *row(simg_int y) noexcept;
  seedimg::pixel *data() noexcept;
  simg_int width() noexcept;
  simg_int height() noexcept;

private:
  simg_int width_;
  simg_int height_;
  // stored in row major order
  // width amount of pixels in a row
  // height amount of rows.
  seedimg::pixel *data_;
};

std::shared_ptr<seedimg::img> make(simg_int width, simg_int height);
std::shared_ptr<seedimg::img>
make(const std::shared_ptr<seedimg::img> &inp_img);

namespace modules {};
namespace filters {};

/** Some miscallenious utilities. */
namespace utils {};
} // namespace seedimg

typedef std::shared_ptr<seedimg::img> simg;

#endif
