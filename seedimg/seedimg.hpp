#ifndef SEEDIMG_CORE_H
#define SEEDIMG_CORE_H

#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace seedimg {

typedef std::pair<int, int> point;

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
  img(int w, int h) : width_(w), height_(h) {
    data_ = reinterpret_cast<seedimg::pixel **>(std::malloc(
        static_cast<std::size_t>(height_) * sizeof(seedimg::pixel *)));
    if (data_ == nullptr)
      exit(0);
    for (int r = 0; r < height_; ++r) {
      data_[r] = reinterpret_cast<seedimg::pixel *>(std::malloc(
          static_cast<std::size_t>(width_) * sizeof(seedimg::pixel)));
      if (data_[r] == nullptr)
        exit(0);
    }
  }
  img(seedimg::img const &img_) : width_(img_.width_), height_(img_.height_) {
    data_ = reinterpret_cast<seedimg::pixel **>(std::malloc(
        static_cast<std::size_t>(height_) * sizeof(seedimg::pixel *)));
    if (data_ == nullptr)
      exit(0);
    for (int r = 0; r < height_; ++r) {
      data_[r] = reinterpret_cast<seedimg::pixel *>(std::malloc(
          static_cast<std::size_t>(width_) * sizeof(seedimg::pixel)));
      if (data_[r] == nullptr)
        exit(0);
    }
    for (int y = 0; y < img_.height_; ++y) {
      std::memcpy(this->data_[y], img_.data_[y],
                  static_cast<std::size_t>(img_.width_) *
                      sizeof(seedimg::pixel));
    }
  }

  ~img() {
    if (data_ != nullptr) {
      for (int r = 0; r < height_; ++r)
        std::free(data_[r]);
      std::free(data_);
    }
  }
  seedimg::pixel &pixel(int x, int y) { return data_[y][x]; }
  seedimg::pixel &pixel(seedimg::point p) { return pixel(p.first, p.second); }
  seedimg::pixel *row(int y) { return data_[y]; }
  seedimg::pixel **data() { return data_; }
  int width() { return width_; }
  int height() { return height_; }

  // resizing image manipulation functions
  bool crop(seedimg::point p1, seedimg::point p2);

private:
  int width_;
  int height_;
  // stored in row major order
  // width amount of pixels in a row
  // height amount of rows.
  seedimg::pixel **data_;
};

bool to(const std::string &filename, std::unique_ptr<seedimg::img> &inp_img);
std::unique_ptr<seedimg::img> from(const std::string &filename);

namespace modules {};
namespace filters {};
} // namespace seedimg

#endif
