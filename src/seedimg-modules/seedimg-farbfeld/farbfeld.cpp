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
#include <seedimg-modules/farbfeld.hpp>

// from unsigned 16 big endian
static inline std::uint16_t from_u16_big_endian(uint8_t *cb) {
  return static_cast<std::uint16_t>(cb[1] << 8) |
         static_cast<std::uint16_t>(cb[0]);
}
static inline void to_u16_big_endian(std::uint16_t n, std::uint8_t *out) {
  out[0] = n >> 8 & 0xff;
  out[1] = n & 0xff;
}
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

// https://stackoverflow.com/a/41762509/10978642
static inline std::uint8_t trunc_8b(std::uint16_t n) { return n >> 8; }
// https://stackoverflow.com/a/6436100/10978642
static inline std::uint16_t dup_8_to_16b(std::uint8_t n) {
  return static_cast<std::uint16_t>(n << 8);
}

namespace seedimg::modules::decoding {

farbfeld::farbfeld(std::istream& input) : in(input) {
    char signature[8];
    input.read(signature, 8);

    if(!std::equal(signature, std::end(signature), "farbfeld"))
        throw input_failure {"invalid Farbfeld signature"};

    input.read(reinterpret_cast<char*>(&_width), 4);
    input.read(reinterpret_cast<char*>(&_height), 4);

    _width  = from_u32_big_endian(reinterpret_cast<uint8_t*>(&_width));
    _height = from_u32_big_endian(reinterpret_cast<uint8_t*>(&_height));
}

bool farbfeld::read(pixel* to) {
    if(_scline == _height)
        return false;

    for(simg_int i = 0; i < _width; ++i) {
        std::uint8_t p[4/*RGBA*/ * 2/*16-bit*/];

        in.read(reinterpret_cast<char*>(p), sizeof p);

        to[i] = {{trunc_8b(from_u16_big_endian(p))},
                 {trunc_8b(from_u16_big_endian(p + 2))},
                 {trunc_8b(from_u16_big_endian(p + 4))},
                  trunc_8b(from_u16_big_endian(p + 6))};
     }

     ++_scline;
     return true;
}

}

namespace seedimg::modules::encoding {

farbfeld::farbfeld(std::ostream& output, simg_int width, simg_int height)
    : _width(width), _height(height), out(output) {
    std::uint8_t ff_width[4];
    std::uint8_t ff_height[4];

    to_u32_big_endian(width, ff_width);
    to_u32_big_endian(height, ff_height);

    out.write("farbfeld", 8);
    out.write(reinterpret_cast<char*>(ff_width), 4);
    out.write(reinterpret_cast<char*>(ff_height), 4);
}

bool farbfeld::write(const pixel* const from) {
    if(_scline == _height)
        return false;

    for(simg_int i = 0; i < _width; ++i) {
        std::uint8_t p[4 * 2];

        to_u16_big_endian(dup_8_to_16b(from[i].r), p);
        to_u16_big_endian(dup_8_to_16b(from[i].g), p+2);
        to_u16_big_endian(dup_8_to_16b(from[i].b), p+4);
        to_u16_big_endian(dup_8_to_16b(from[i].a), p+6);

        out.write(reinterpret_cast<char*>(p), 8);
     }

     ++_scline;
     return true;
}

}
