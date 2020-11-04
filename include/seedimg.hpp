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

#define SIMG_WRAP_IN_QUOTES(exp) #exp
#define SIMG_EXPAND_WRAP_IN_QUOTES(exp) SIMG_WRAP_IN_QUOTES(exp)

#include <array>
#include <cinttypes>
#include <functional>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

typedef std::size_t simg_int;

namespace seedimg {

typedef struct point {
  simg_int x;
  simg_int y;
  inline bool operator==(const point &other) const noexcept {
    return std::tie(x, y) == std::tie(other.x, other.y);
  }
  inline bool operator!=(const point &other) const noexcept {
    return !(*this == other);
  }
} point;

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

enum class colourspaces : std::size_t { rgb, hsv, ycbcr_jpeg, ycbcr_bt601 };

struct pixel {
  // instead of an anonymous struct of unions.
  // unnamed unions are standard
  union {
    std::uint8_t r;
    std::uint8_t h;
    std::uint8_t y;
  };
  union {
    std::uint8_t g;
    std::uint8_t s;
    std::uint8_t cb;
  };
  union {
    std::uint8_t b;
    std::uint8_t v;
    std::uint8_t cr;
  };
  std::uint8_t a;
  constexpr bool operator==(const pixel &other) const noexcept {
    return std::tie(r, g, b, a) == std::tie(other.r, other.g, other.b, other.a);
  }
  constexpr bool operator!=(const pixel &other) const noexcept {
    return !(*this == other);
  }
};

class img {
protected:
  colourspaces colourspace_;
  simg_int width_;
  simg_int height_;
  // stored in row major order
  // width amount of pixels in a row
  // height amount of rows.
  seedimg::pixel *data_;
  std::function<void()> deleter = [this] { std::free(data_); };

public:
  static constexpr std::uint8_t MAX_PIXEL_VALUE = UINT8_MAX;

  img(colourspaces space = colourspaces::rgb)
      : colourspace_{space}, width_{0}, height_{0}, data_{nullptr} {}

  img(simg_int w, simg_int h, colourspaces space = colourspaces::rgb)
      : colourspace_{space}, width_{w}, height_{h} {
    data_ = static_cast<seedimg::pixel *>(std::malloc(
        static_cast<std::size_t>(height_ * width_) * sizeof(seedimg::pixel *)));
    if (data_ == nullptr)
      throw std::bad_alloc();
  }

  img(simg_int w, simg_int h, seedimg::pixel *u_data,
      colourspaces space = colourspaces::rgb)
      : colourspace_{space}, width_{w}, height_{h}, data_{u_data} {}

  img(img const &img_) : img(img_.width_, img_.height_) {
    std::copy(img_.data(), img_.data() + img_.width_ * img_.height_,
              this->data());
  }

  img(img &&other) noexcept {
    this->width_ = other.width_;
    this->height_ = other.height_;
    this->data_ = other.data_;
    this->colourspace_ = other.colourspace_;
    other.width_ = 0;
    other.height_ = 0;
    other.data_ = nullptr;
  }

  ~img() { std::invoke(deleter); }

  img &operator=(img other) noexcept {
    std::swap(this->data_, other.data_);
    std::swap(this->width_, other.width_);
    std::swap(this->height_, other.height_);
    std::swap(this->colourspace_, other.colourspace_);
    return *this;
  }
  img &operator=(img &&other) noexcept {
    if (&other != this) {
      std::free(this->data_);
      this->data_ = other.data_;
      other.data_ = nullptr;
      width_ = other.width_;
      height_ = other.height_;
      this->colourspace_ = other.colourspace_;
    }
    return *this;
  }

  void set_deleter(std::function<void()> deleter_) { deleter = deleter_; }

  seedimg::pixel &pixel(simg_int x, simg_int y) const noexcept {
    return data()[y * width() + x];
  }

  seedimg::pixel &pixel(seedimg::point p) const noexcept {
    return pixel(p.x, p.y);
  }

  seedimg::pixel &pixel(simg_int x) const noexcept {
    return pixel(x / width(), x % width());
  }

  seedimg::pixel &pixel_s(simg_int x, simg_int y) const {
    if (x >= width() || y >= height())
      throw std::out_of_range("Coordinates out of range");
    return data()[y * width() + x];
  }
  seedimg::pixel &pixel_s(seedimg::point p) const { return pixel(p.x, p.y); }
  seedimg::pixel &pixel_s(simg_int x) const {
    return pixel(x / width(), x % width());
  }

  seedimg::pixel *row(simg_int y) const noexcept {
    return data() + y * width();
  }

  seedimg::pixel *row_s(simg_int y) const {
    if (y >= height())
      throw std::out_of_range("Row out of range");
    return data() + y * width();
  }

  seedimg::pixel *data() const noexcept { return data_; }
  simg_int width() const noexcept { return width_; }
  simg_int height() const noexcept { return height_; }

  std::vector<std::pair<simg_int, simg_int>> start_end_rows() const noexcept {
    std::vector<std::pair<simg_int, simg_int>> res;
    auto processor_count =
        std::min(this->height(),
                 static_cast<simg_int>(std::thread::hardware_concurrency()));
    if (processor_count == 0)
      processor_count = 1;
    res.reserve(static_cast<std::size_t>(processor_count));
    simg_int rows_per_thread = this->height() / processor_count;
    for (simg_int i = 0; i < processor_count * rows_per_thread;
         i += rows_per_thread)
      res.push_back({i, i + rows_per_thread});
    res[res.size() - 1].second += this->height() % processor_count;
    return res;
  }

  std::vector<std::pair<simg_int, simg_int>> start_end_cols() const noexcept {
    std::vector<std::pair<simg_int, simg_int>> res;
    auto processor_count =
        std::min(this->width(),
                 static_cast<simg_int>(std::thread::hardware_concurrency()));
    if (processor_count == 0)
      processor_count = 1;
    res.reserve(static_cast<std::size_t>(processor_count));
    simg_int cols_per_thread = this->width() / processor_count;
    for (simg_int i = 0; i < processor_count * cols_per_thread;
         i += cols_per_thread)
      res.push_back({i, i + cols_per_thread});
    res[res.size() - 1].second += this->width() % processor_count;
    return res;
  }

  colourspaces colourspace() const noexcept { return this->colourspace_; }
};

class uimg : public img {
public:
  void set_data(seedimg::pixel *data) noexcept { this->data_ = data; }
  void set_width(simg_int w) noexcept { this->width_ = w; }
  void set_height(simg_int h) noexcept { this->height_ = h; }
  void set_colourspace(seedimg::colourspaces colourspace) noexcept {
    this->colourspace_ = colourspace;
  }
};
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

typedef std::unique_ptr<seedimg::img> simg;
typedef std::unique_ptr<seedimg::uimg> suimg;

namespace seedimg {
static inline std::unique_ptr<seedimg::img> make(simg_int width,
                                                 simg_int height) {
  return std::make_unique<seedimg::img>(width, height);
}

static inline std::unique_ptr<seedimg::img>
make(simg_int width, simg_int height, seedimg::pixel *data) {
  return std::make_unique<seedimg::img>(width, height, data);
}

static inline std::unique_ptr<seedimg::img> make(seedimg::img &&inp_img) {
  return std::make_unique<seedimg::img>(std::move(inp_img));
}

static inline std::unique_ptr<seedimg::img>
make(const std::unique_ptr<seedimg::img> &inp_img) {
  return std::make_unique<seedimg::img>(*inp_img);
}

// Animations or for files storing more than one image GIF, TIFF, etc. APNG
// support might be added as a separate module.
class anim {
public:
  std::size_t framerate;
  anim() : framerate{0}, data{} {}

  anim(std::size_t size, std::size_t framerate) {
    this->framerate = framerate;
    data = std::vector<simg>(size);
  }

  anim(anim const &anim_) {
    data = std::vector<simg>();
    data.reserve(anim_.data.size());
  }
  anim(anim &&other) {
    framerate = other.framerate;
    other.framerate = 0;
    data = std::move(other.data);
  }

  anim &operator=(anim other) {
    this->data.swap(other.data);
    return *this;
  }
  anim &operator=(anim &&other) {
    if (&other != this) {
      this->data = std::move(other.data);
    }
    return *this;
  }

  anim(simg &&img...) : anim() { this->add(std::move(img)); }

  simg &operator[](std::size_t i) { return data[i]; }
  const simg &operator[](std::size_t i) const { return data[i]; }

  void add(simg &&img) { data.push_back(std::move(img)); }
  bool insert(simg &&img, std::size_t index) {
    if (index >= data.size())
      return false;
    data.insert(data.begin() + static_cast<int>(index), std::move(img));
    return true;
  }
  bool remove(std::size_t index) {
    if (index >= data.size())
      return false;
    data.erase(data.begin() + static_cast<int>(index));
    return true;
  }
  bool trim(std::size_t start, std::size_t end) {
    if (start > end || end >= data.size())
      return false;
    data.erase(data.begin(), data.begin() + static_cast<int>(start));
    data.erase(data.begin() + static_cast<int>(end - start), data.end());
    return true;
  }

  auto begin() { return data.begin(); }
  auto end() { return data.end(); }

  auto size() { return data.size(); }

protected:
  std::vector<simg> data;
};

namespace modules {};
namespace filters {};

/** Some miscallenious utilities. */
namespace utils {};
namespace extras {};
} // namespace seedimg

#endif
