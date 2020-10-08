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

#include <cstring>
#include <fstream>
#include <seedimg-formats/seedimg-farbfeld.hpp>

// from unsigned 16 big endian
static inline std::uint16_t from_u16_big_endian(uint8_t *cb) {
  return static_cast<std::uint16_t>(cb[1] << 8) |
         static_cast<std::uint16_t>(cb[0]);
}
static inline void to_u16_big_endian(std::uint16_t n, std::uint8_t *out) {
  out[0] = n >> 8 & 0xff;
  out[1] = n & 0xff;
}
static inline simg_int from_u32_big_endien(uint8_t *cb) {
  return static_cast<simg_int>(cb[0] << 24) |
         static_cast<simg_int>(cb[1] << 16) |
         static_cast<simg_int>(cb[2] << 8) | static_cast<simg_int>(cb[3]);
}
static inline void to_u32_big_endian(simg_int n, std::uint8_t *out) {
  out[0] = (n >> 24) & 0xff;
  out[1] = (n >> 16) & 0xff;
  out[2] = (n >> 8) & 0xff;
  out[3] = n & 0xff;
}

// https://stackoverflow.com/a/41762509/10978642
static inline std::uint8_t trunc_8b(std::uint16_t n) { return n >> 8; }
// https://stackoverflow.com/a/6436100/10978642
// NOTE: simple bitshifting could've solved but would
//       be quite noisy, so an addition put.
static inline std::uint16_t dup_8_to_16b(std::uint8_t n) {
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
  if (!check(filename))
    return nullptr;

  std::ifstream input(filename);
  struct {
    char sig[8];
    uint8_t width[4];
    uint8_t height[4];
  } rawinfo;

  auto result = seedimg::make(from_u32_big_endien(rawinfo.width),
                              from_u32_big_endien(rawinfo.height));

  uint8_t rawpixel[8];
  for (simg_int y = 0; y < result->height(); ++y) {
    for (simg_int x = 0; x < result->width(); ++x) {
      try {
        input.read(reinterpret_cast<char *>(rawpixel), 8);
      } catch (std::ios_base::failure) {
        return nullptr;
      }

      result->pixel(x, y) = {{trunc_8b(from_u16_big_endian(rawpixel))},
                             {trunc_8b(from_u16_big_endian((rawpixel + 2)))},
                             {trunc_8b(from_u16_big_endian((rawpixel + 4)))},
                             trunc_8b(from_u16_big_endian((rawpixel + 6)))};
    }
  }

  return result;
}

bool to(const std::string &filename, const simg &inp_img) {
  std::ofstream output(filename);

  std::uint8_t width_ser[4];
  std::uint8_t height_ser[4];
  to_u32_big_endian(inp_img->width(), width_ser);
  to_u32_big_endian(inp_img->height(), height_ser);
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

      to_u16_big_endian(dup_8_to_16b(px.r), rawpixel);
      to_u16_big_endian(dup_8_to_16b(px.g), rawpixel + 2);
      to_u16_big_endian(dup_8_to_16b(px.b), rawpixel + 4);
      to_u16_big_endian(dup_8_to_16b(px.a), rawpixel + 6);

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
