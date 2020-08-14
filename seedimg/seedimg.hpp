/**********************************************************************
seedimg -
        module based image manipulation library written in modern
            C++ Copyright(C) 2020 telugu
        -
        boy

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
  img() : width_(0), height_(0), data_(nullptr) {}
  img(simg_int w, simg_int h) : width_{w}, height_{h} {
    data_ = reinterpret_cast<seedimg::pixel **>(std::malloc(
        static_cast<std::size_t>(height_) * sizeof(seedimg::pixel *)));
    if (data_ == nullptr)
      std::terminate();
    for (simg_int r = 0; r < height_; ++r) {
      data_[r] = reinterpret_cast<seedimg::pixel *>(std::malloc(
          static_cast<std::size_t>(width_) * sizeof(seedimg::pixel)));
      if (data_[r] == nullptr)
        std::terminate();
    }
  }
  img(seedimg::img const &img_) : img(img_.width_, img_.height_) {
    for (simg_int y = 0; y < img_.height_; ++y) {
      std::memcpy(this->data_[y], img_.data_[y],
                  static_cast<std::size_t>(img_.width_) *
                      sizeof(seedimg::pixel));
    }
  }

  ~img() {
    if (data_ != nullptr) {
      for (simg_int r = 0; r < height_; ++r)
        std::free(data_[r]);
      std::free(data_);
    }
  }

  std::vector<std::pair<simg_int, simg_int>> start_end_rows() {
    std::vector<std::pair<simg_int, simg_int>> res;
    auto processor_count =
        std::min(this->height(),
                 static_cast<simg_int>(std::thread::hardware_concurrency()));
    if (processor_count == 0)
      processor_count = 1;
    res.reserve(static_cast<std::size_t>(processor_count));
    simg_int rows_per_thread = this->height_ / processor_count;
    for (simg_int i = 0; i < processor_count * rows_per_thread;
         i += rows_per_thread)
      res.emplace_back(i, i + rows_per_thread);
    res[res.size() - 1].second += this->height_ % processor_count;
    return res;
  }

  std::vector<std::pair<simg_int, simg_int>> start_end_cols() {
    std::vector<std::pair<simg_int, simg_int>> res;
    auto processor_count =
        std::min(this->width(),
                 static_cast<simg_int>(std::thread::hardware_concurrency()));
    if (processor_count == 0)
      processor_count = 1;
    res.reserve(static_cast<std::size_t>(processor_count));
    simg_int cols_per_thread = this->width_ / processor_count;
    for (simg_int i = 0; i < processor_count * cols_per_thread;
         i += cols_per_thread)
      res.emplace_back(i, i + cols_per_thread);
    res[res.size() - 1].second += this->width_ % processor_count;
    return res;
  }

  seedimg::pixel &pixel(simg_int x, simg_int y) { return data_[y][x]; }
  seedimg::pixel &pixel(seedimg::point p) { return pixel(p.first, p.second); }
  seedimg::pixel &pixel(simg_int x) {
    if (x > this->width_ * this->height_ - 1)
      std::terminate();
    return pixel(x / this->width_, x % this->width_);
  }
  seedimg::pixel *row(simg_int y) { return data_[y]; }
  seedimg::pixel **data() { return data_; }
  simg_int width() { return width_; }
  simg_int height() { return height_; }

  // resizing image manipulation functions
  bool crop(seedimg::point p1, seedimg::point p2);

private:
  simg_int width_;
  simg_int height_;
  // stored in row major order
  // width amount of pixels in a row
  // height amount of rows.
  seedimg::pixel **data_;
}; // namespace seedimg

bool to(const std::string &filename,
        const std::unique_ptr<seedimg::img> &inp_img);
std::unique_ptr<seedimg::img> from(const std::string &filename);

namespace modules {};
namespace filters {};
} // namespace seedimg

#endif
