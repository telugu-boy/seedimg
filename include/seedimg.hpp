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

#include <cinttypes>
#include <initializer_list>
#include <memory>
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

simg_int round_up(simg_int inp, simg_int mul) noexcept;

bool is_on_rect(seedimg::point xy1, seedimg::point xy2,
                seedimg::point point) noexcept;

point get_rect_dimensions(seedimg::point p1, seedimg::point p2) noexcept;

enum class colourspaces : std::size_t { rgb, hsv, ycbcr };

typedef struct pixel {
  union {
    struct {
      std::uint8_t r;
      std::uint8_t g;
      std::uint8_t b;
    };
    struct {
      std::uint8_t h;
      std::uint8_t s;
      std::uint8_t v;
    };
    struct {
      std::uint8_t y;
      std::uint8_t cb;
      std::uint8_t cr;
    };
  };
  std::uint8_t a;
  inline bool operator==(const pixel &other) const noexcept {
    return std::tie(r, g, b, a) == std::tie(other.r, other.g, other.b, other.a);
  }
  inline bool operator!=(const pixel &other) const noexcept {
    return !(*this == other);
  }
} pixel;

class img {
public:
  static constexpr std::uint8_t MAX_PIXEL_VALUE = UINT8_MAX;
  img();
  img(simg_int w, simg_int h);
  img(simg_int w, simg_int h, seedimg::pixel *u_data);
  img(seedimg::img const &img_);
  img(seedimg::img &&other);
  img &operator=(img other);
  img &operator=(img &&other);

  ~img();

  std::vector<std::pair<simg_int, simg_int>> start_end_rows() const noexcept;

  std::vector<std::pair<simg_int, simg_int>> start_end_cols() const noexcept;

  // Functions suffixed in `s` have a branch in them and will throw exceptions
  // if something is out of range.

  seedimg::pixel &pixel(simg_int x, simg_int y) const noexcept;
  seedimg::pixel &pixel(seedimg::point p) const noexcept;
  seedimg::pixel &pixel(simg_int x) const noexcept;
  seedimg::pixel &pixel_s(simg_int x, simg_int y) const;
  seedimg::pixel &pixel_s(seedimg::point p) const;
  seedimg::pixel &pixel_s(simg_int x) const;
  seedimg::pixel *row(simg_int y) const noexcept;
  seedimg::pixel *row_s(simg_int y) const;
  seedimg::pixel *data() const noexcept;
  simg_int width() const noexcept;
  simg_int height() const noexcept;

  colourspaces colourspace() const noexcept;

protected:
  class img_impl;
  std::unique_ptr<img_impl> impl;
};

class uimg : public img {
public:
  using img::img;
  void set_width(simg_int w) noexcept;
  void set_height(simg_int h) noexcept;
  void set_data(seedimg::pixel *data) noexcept;
};

} // namespace seedimg

typedef std::shared_ptr<seedimg::img> simg;
typedef std::shared_ptr<seedimg::uimg> suimg;

namespace seedimg {
std::shared_ptr<seedimg::img> make(simg_int width, simg_int height);
std::shared_ptr<seedimg::img>
make(const std::shared_ptr<seedimg::img> &inp_img);

// Animations or for files storing more than one image GIF, TIFF, etc. APNG
// support might be added as a separate module.
class anim {
public:
  std::size_t framerate;

  anim();
  anim(std::size_t size, std::size_t framerate = 30);
  anim(simg images...);
  anim(std::initializer_list<simg> images, std::size_t framerate = 0);
  anim(seedimg::anim const &anim_);
  anim(seedimg::anim &&other);
  anim &operator=(anim other);
  anim &operator=(anim &&other);

  simg &operator[](std::size_t i) const;

  void add(simg img);
  bool insert(simg img, std::size_t index);
  bool remove(std::size_t index);
  bool trim(std::size_t start, std::size_t end);

  std::size_t size() const noexcept;

  ~anim();

protected:
  class anim_impl;
  std::unique_ptr<anim_impl> impl;
};

namespace modules {};
namespace filters {};

/** Some miscallenious utilities. */
namespace utils {};
} // namespace seedimg

#endif
