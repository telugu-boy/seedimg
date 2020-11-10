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
#ifndef SEEDIMG_FARBFELD_HPP
#define SEEDIMG_FARBFELD_HPP

#include <istream>
#include <ostream>

#include <seedimg-modules/modules-abc.hpp>
#include <seedimg-utils.hpp>

namespace seedimg::modules {
namespace farbfeld {
class decoder : public input_abc {
  private:
    simg_int width_;
    simg_int height_;
    simg_int scline = 0;

    std::istream& in;

  public:
    decoder(std::istream& input)
        : in(input) {
        char signature[8];
        in.read(signature, 8);

        if (!std::equal(signature, std::end(signature), "farbfeld")) throw input_failure{"invalid Farbfeld signature"};

        in.read(reinterpret_cast<char*>(&width_), 4);
        in.read(reinterpret_cast<char*>(&height_), 4);

        width_  = seedimg::utils::endian::from_u32_big(reinterpret_cast<uint8_t*>(&width_));
        height_ = seedimg::utils::endian::from_u32_big(reinterpret_cast<uint8_t*>(&height_));
    }

    simg_int width() const noexcept { return width_; }
    simg_int height() const noexcept { return height_; }

    bool read(pixel* to) noexcept {
        if (scline == height_) return false;

        for (simg_int i = 0; i < width_; ++i) {
            std::uint8_t p[4 /*RGBA*/ * 2 /*16-bit*/];

            in.read(reinterpret_cast<char*>(p), sizeof p);

            to[i] = {{static_cast<uint8_t>(seedimg::utils::endian::from_u16_big(p) >> 8)},
                     {static_cast<uint8_t>(seedimg::utils::endian::from_u16_big(p + 2) >> 8)},
                     {static_cast<uint8_t>(seedimg::utils::endian::from_u16_big(p + 4) >> 8)},
                     static_cast<uint8_t>(seedimg::utils::endian::from_u16_big(p + 6) >> 8)};
        }

        ++scline;
        return true;
    }

    ~decoder();
};

decoder::~decoder() = default;

class encoder : public output_abc {
  private:
    simg_int width;
    simg_int height;
    simg_int scline = 0;

    std::ostream& out;

  public:
    encoder(std::ostream& output, simg_int width, simg_int height)
        : out(output) {
        std::uint8_t w[4];
        std::uint8_t h[4];

        seedimg::utils::endian::to_u32_big(width, w);
        seedimg::utils::endian::to_u32_big(height, h);

        out.write("farbfeld", 8);
        out.write(reinterpret_cast<char*>(w), 4);
        out.write(reinterpret_cast<char*>(h), 4);
    }

    bool write(const pixel* const from) noexcept {
        if (scline == height) return false;

        for (simg_int i = 0; i < width; ++i) {
            std::uint8_t p[4 * 2];

            seedimg::utils::endian::to_u16_big(static_cast<std::uint16_t>(from[i].r << 8), p);
            seedimg::utils::endian::to_u16_big(static_cast<std::uint16_t>(from[i].g << 8), p + 2);
            seedimg::utils::endian::to_u16_big(static_cast<std::uint16_t>(from[i].b << 8), p + 4);
            seedimg::utils::endian::to_u16_big(static_cast<std::uint16_t>(from[i].a << 8), p + 6);

            out.write(reinterpret_cast<char*>(p), 8);
        }

        ++scline;
        return true;
    }

    void flush() { out.flush(); }
    ~encoder();
};

encoder::~encoder() { flush(); }

} // namespace farbfeld
} // namespace seedimg::modules
#endif
