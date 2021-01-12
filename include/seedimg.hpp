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
  constexpr bool operator==(const point &other) const noexcept {
    return std::tie(x, y) == std::tie(other.x, other.y);
  }
  constexpr bool operator!=(const point &other) const noexcept {
    return !(*this == other);
  }
} point;

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

#ifdef SEEDIMG_SUBIMAGE_API
class img_view;
#endif

class img {
protected:
  colourspaces colourspace_;
  simg_int width_;
  simg_int height_;

  // unsigned 8-bit RGBA pixels.
  // stored in row major order.
  // width_ amount of pixels per row.
  // height_ amount of rows.
  seedimg::pixel *data_;
public:
  static constexpr std::uint8_t MIN_PIXEL_VALUE = 0;
  static constexpr std::uint8_t MAX_PIXEL_VALUE = UINT8_MAX;

  img(colourspaces space = colourspaces::rgb)
      : colourspace_{space},
        width_{0},
        height_{0},
        data_{nullptr} {}

  img(simg_int w, simg_int h, colourspaces space = colourspaces::rgb)
      : colourspace_{space},
        width_{w},
        height_{h}
  {
    data_ = static_cast<seedimg::pixel *>(std::malloc(
        static_cast<std::size_t>(height_ * width_) * sizeof(seedimg::pixel *)));
    if (data_ == nullptr)
      throw std::bad_alloc();
  }

  img(simg_int w, simg_int h, seedimg::pixel *u_data,
      colourspaces space = colourspaces::rgb)
      : colourspace_{space},
        width_{w},
        height_{h},
        data_{u_data} {}

  img(img const &img_)
      : img{img_.width_,
            img_.height_}
  {
    std::copy(img_.data_,
              img_.data_ + img_.width_ * img_.height_,
              data_);
  }

  img(img &&other) noexcept {
    width_       = other.width_;
    height_      = other.height_;
    data_        = other.data_;
    colourspace_ = other.colourspace_;

    other.width_  = 0;
    other.height_ = 0;
    other.data_   = nullptr;
  }

  ~img() { std::free(data_); }

  img &operator=(img other) noexcept {
    std::swap(data_,        other.data_);
    std::swap(width_,       other.width_);
    std::swap(height_,      other.height_);
    std::swap(colourspace_, other.colourspace_);

    return *this;
  }

  img &operator=(img &&other) noexcept {  
    if(&other != this) {
      std::free(data_);

      data_        = other.data_;
      width_       = other.width_;
      height_      = other.height_;
      colourspace_ = other.colourspace_;

      other.data_   = nullptr;
      other.width_  = 0;
      other.height_ = 0;
    }
    return *this;
  }

  seedimg::pixel& pixel(simg_int x, simg_int y) const noexcept {
    return data()[y * width() + x];
  }
  seedimg::pixel& pixel(seedimg::point p) const noexcept {
    return pixel(p.x, p.y);
  }
  seedimg::pixel& pixel(simg_int x) const noexcept {
    return pixel(x / width(), x % width());
  }

  seedimg::pixel& pixel_s(simg_int x, simg_int y) const {
    if (x >= width() || y >= height())
      throw std::out_of_range { "Coordinates out of range" };
    return data()[y * width() + x];
  }
  seedimg::pixel& pixel_s(seedimg::point p) const { return pixel_s(p.x, p.y); }
  seedimg::pixel& pixel_s(simg_int x) const {
    return pixel_s(x / width(), x % width());
  }

  seedimg::pixel* row(simg_int y) const noexcept {
    return data() + y * width();
  }
  seedimg::pixel* row_s(simg_int y) const {
    if (y >= height())
      throw std::out_of_range { "Row out of range" };
    return data() + y * width();
  }

  seedimg::pixel *data()        const noexcept { return data_;        }
  simg_int        width()       const noexcept { return width_;       }
  simg_int        height()      const noexcept { return height_;      }
  colourspaces    colourspace() const noexcept { return colourspace_; }

#ifdef SEEDIMG_SUBIMAGE_API
  inline img_view sub(simg_int x,
                      simg_int y,
                      simg_int w,
                      simg_int h) const noexcept;
#endif
};

class uimg : public img {
public:
  void set_data(seedimg::pixel *d)     noexcept { data_        = d; }
  void set_width(simg_int w)           noexcept { width_       = w; }
  void set_height(simg_int h)          noexcept { height_      = h; }
  void set_colourspace(colourspaces c) noexcept { colourspace_ = c; }
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
static inline std::unique_ptr<seedimg::img> make(simg_int width, simg_int height) {
  return std::make_unique<seedimg::img>(width, height);
}

static inline std::unique_ptr<seedimg::img> make(simg_int width, simg_int height, seedimg::pixel *data) {
  return std::make_unique<seedimg::img>(width, height, data);
}

static inline std::unique_ptr<seedimg::img> make(seedimg::img &&inp_img) {
  return std::make_unique<seedimg::img>(std::move(inp_img));
}

static inline std::unique_ptr<seedimg::img> make(const std::unique_ptr<seedimg::img> &inp_img) {
  return std::make_unique<seedimg::img>(*inp_img);
}

// Animations or for files storing more than one image GIF, TIFF, etc. APNG
// support might be added as a separate module.
class anim {
public:
  std::size_t framerate;

  anim()
      : framerate {0},
        data{} {}

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

  std::size_t size() const noexcept { return data.size(); }

  std::vector<simg>::iterator begin() noexcept { return data.begin(); }
  std::vector<simg>::iterator end() noexcept { return data.end(); }

private:
  std::vector<simg> data;
};
} // namespace seedimg
#endif
