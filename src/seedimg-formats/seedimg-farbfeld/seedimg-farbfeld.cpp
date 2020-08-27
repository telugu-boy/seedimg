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

#include <fstream>
#include <seedimg-formats/seedimg-farbfeld.hpp>

static inline std::uint16_t fu16be(uint8_t *cb) {
  return static_cast<std::uint16_t>(cb[1] << 8) |
         static_cast<std::uint16_t>(cb[0]);
}
static inline void tu16be(std::uint16_t n, std::uint8_t *out) {
  out[0] = n >> 8 & 0xff;
  out[1] = n & 0xff;
}
static inline simg_int fu32be(uint8_t *cb) {
  return static_cast<simg_int>(cb[0] << 24) |
         static_cast<simg_int>(cb[1] << 16) |
         static_cast<simg_int>(cb[2] << 8) | static_cast<simg_int>(cb[3]);
}
static inline void tu32be(simg_int n, std::uint8_t *out) {
  out[0] = (n >> 24) & 0xff;
  out[1] = (n >> 16) & 0xff;
  out[2] = (n >> 8) & 0xff;
  out[3] = n & 0xff;
}

// https://stackoverflow.com/a/41762509/10978642
static inline std::uint8_t b16_8(std::uint16_t n) { return n >> 8; }
// https://stackoverflow.com/a/6436100/10978642
// NOTE: simple bitshifting could've solved but would
//       be quite noisy, so an addition put.
static inline std::uint16_t b8_16(std::uint8_t n) {
  return static_cast<std::uint16_t>(n << 8) + static_cast<std::uint16_t>(n);
}

namespace seedimg::modules {
namespace farbfeld {

bool check(const std::string &filename) {
  std::ifstream input(filename);
  char sig[8];

  try {
    input.read(sig, 8);
  } catch (std::iostream::failure) {
    return false;
  }
  return std::memcmp(sig, "farbfeld", 8) == 0;
}

simg from(const std::string &filename) {
  std::ifstream input(filename);
  struct {
    char sig[8];
    uint8_t width[4];
    uint8_t height[4];
  } rawinfo;

  try {
    input.read(rawinfo.sig, 8)
        .read(reinterpret_cast<char *>(rawinfo.width), 4)
        .read(reinterpret_cast<char *>(rawinfo.height), 4);
  } catch (std::ios_base::failure) {
    return nullptr;
  }

  // siganture match is mandatory else invalid Farbfeld file.
  if (std::memcmp(rawinfo.sig, "farbfeld", 8) != 0)
    return nullptr;

  auto result = std::make_shared<seedimg::img>(fu32be(rawinfo.width),
                                               fu32be(rawinfo.height));

  uint8_t rawpixel[8];
  for (simg_int y = 0; y < result->height(); ++y) {
    for (simg_int x = 0; x < result->width(); ++x) {
      try {
        input.read(reinterpret_cast<char *>(rawpixel), 8);
      } catch (std::ios_base::failure) {
        return nullptr;
      }

      result->pixel(x, y) = {
          b16_8(fu16be(rawpixel)), b16_8(fu16be((rawpixel + 2))),
          b16_8(fu16be((rawpixel + 4))), b16_8(fu16be((rawpixel + 6)))};
    }
  }

  return result;
}

bool to(const std::string &filename, const simg &inp_img) {
  std::ofstream output(filename);

  std::uint8_t width_ser[4];
  std::uint8_t height_ser[4];
  tu32be(inp_img->width(), width_ser);
  tu32be(inp_img->height(), height_ser);
  try {
    output.write("farbfeld", 8)
        .write(reinterpret_cast<char *>(width_ser), 4)
        .write(reinterpret_cast<char *>(height_ser), 4);
  } catch (std::ios_base::failure) {
    return false;
  }

  uint8_t rawpixel[8];
  for (simg_int y = 0; y < inp_img->height(); ++y) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      auto px = inp_img->pixel(x, y);

      tu16be(b8_16(px.r), rawpixel);
      tu16be(b8_16(px.g), rawpixel + 2);
      tu16be(b8_16(px.b), rawpixel + 4);
      tu16be(b8_16(px.a), rawpixel + 6);

      try {
        output.write(reinterpret_cast<char *>(rawpixel), 8);
      } catch (std::ios_base::failure) {
        return false;
      }
    }
  }

  return true;
}
} // namespace farbfeld
} // namespace seedimg::modules
