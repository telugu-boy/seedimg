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

#include <cstring>
#include <fstream>
#include <seedimg-modules/irdump.hpp>

static inline simg_int from_u32_big_endian(uint8_t *cb) {
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

namespace seedimg::modules::decoding {

irdump::irdump(std::istream& input) : in(input) {
    input.read(reinterpret_cast<char*>(&_width), 4);
    input.read(reinterpret_cast<char*>(&_height), 4);

    _width  = from_u32_big_endian(reinterpret_cast<uint8_t*>(&_width));
    _height = from_u32_big_endian(reinterpret_cast<uint8_t*>(&_height));
}

bool irdump::read(pixel* to) {
    if(_scline == _height)
        return false;

    in.read(reinterpret_cast<char*>(to), static_cast<std::streamsize>(4 * _width));

    ++_scline;
    return true;
}

}

namespace seedimg::modules::encoding {

irdump::irdump(std::ostream& output, simg_int width, simg_int height)
    : _width(width), _height(height), out(output) {
    std::uint8_t w[4], h[4];

    to_u32_big_endian(width, w);
    to_u32_big_endian(height, h);

    out.write(reinterpret_cast<char*>(w), 4);
    out.write(reinterpret_cast<char*>(h), 4);
}

bool irdump::write(const pixel* const from) {
    if(_scline == _height)
        return false;

    out.write(reinterpret_cast<const char*>(from), static_cast<std::streamsize>(4 * _width));

    ++_scline;
    return true;
}

}
