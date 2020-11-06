/**********************************************************************
    seedimg - module based image manipulation library written in modern
                C++ Copyright(C) 2020 telugu-boy, tripulse

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
#ifndef SEEDIMG_IRDUMP_HPP
#define SEEDIMG_IRDUMP_HPP

#include <istream>
#include <ostream>

#include <seedimg-modules/modules-abc.hpp>
#include <seedimg-utils.hpp>

namespace seedimg::modules {
namespace irdump {
class decoder : public input_abc {
private:
  simg_int width_;
  simg_int height_;
  simg_int scline = 0;

  std::istream &in;

public:
  decoder(std::istream &input) : in(input) {
    input.read(reinterpret_cast<char *>(&width_), 4);
    input.read(reinterpret_cast<char *>(&height_), 4);

    width_ = seedimg::utils::endian::from_u32_big(
        reinterpret_cast<uint8_t *>(&width_));
    height_ = seedimg::utils::endian::from_u32_big(
        reinterpret_cast<uint8_t *>(&height_));
  }

  simg_int width() const noexcept { return width_; }
  simg_int height() const noexcept { return height_; }

  bool read(pixel *to) noexcept {
    if (scline == height_)
      return false;

    in.read(reinterpret_cast<char *>(to),
            static_cast<std::streamsize>(4 * width_));

    ++scline;
    return true;
  }

  ~decoder();
};

decoder::~decoder(){};

class encoder : public output_abc {
private:
  simg_int width;
  simg_int height;
  simg_int scline = 0;

  std::ostream &out;

public:
  encoder(std::ostream &output, simg_int width, simg_int height)
      : width(width), height(height), out(output) {
    std::uint8_t w[4], h[4];

    seedimg::utils::endian::to_u32_big(width, w);
    seedimg::utils::endian::to_u32_big(height, h);

    out.write(reinterpret_cast<char *>(w), 4);
    out.write(reinterpret_cast<char *>(h), 4);
  }

  bool write(const pixel *const from) noexcept {
    if (scline == height)
      return false;

    out.write(reinterpret_cast<const char *>(from),
              static_cast<std::streamsize>(4 * width));

    ++scline;
    return true;
  }
  void flush() { out.flush(); }

  ~encoder();
};
encoder::~encoder() { out.flush(); }
} // namespace irdump
} // namespace seedimg::modules
#endif
