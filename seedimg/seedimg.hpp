#ifndef SEEDIMG_CORE_H
#define SEEDIMG_CORE_H

#include <cinttypes>
#include <cstdlib>
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
  img(int w = 0, int h = 0) : width_(w), height_(h) {
    data_ = reinterpret_cast<seedimg::pixel **>(std::malloc(
        static_cast<std::size_t>(height_) * sizeof(seedimg::pixel *)));
    for (int r = 0; r < height_; ++r) {
      data_[r] = reinterpret_cast<seedimg::pixel *>(std::malloc(
          static_cast<std::size_t>(width_) * sizeof(seedimg::pixel)));
    }
  }

  ~img() {
    for (int r = 0; r < height_; ++r)
      std::free(data_[r]);
    std::free(data_);
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

bool to(const std::string &filename, seedimg::img &inp_img);
std::optional<seedimg::img> from(const std::string &filename);

namespace modules {};
namespace filters {};
} // namespace seedimg

#endif
