/***********************************************************************
    seedimg - module based image manipulation library written in modern C++
    Copyright (C) 2020 telugu-boy, <contributor name>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

#include <cstring>
#include <filesystem>
#include <fstream>
#include <seedimg-formats/seedimg-bmp.hpp>

// http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm

constexpr unsigned ih_offset = 14;
constexpr unsigned ih_size = 4;

simg from_1_bpp(simg &img, std::ifstream &file) {
  simg_int offset = 0;
  const simg_int line_width = ((img->width() + 7) / 8 + 3) / 4 * 4;
  int pix;
  for (simg_int y = 0; y < img->height(); ++y) {
    for (simg_int x = 0; x < line_width; ++x) {
      file.read(reinterpret_cast<char *>(&pix), 1);
      if (x * 8 < img->width()) {
        for (simg_int b = 0; b < 8; ++b) {
          if (x * 8 + b < img->width()) {
            *(reinterpret_cast<std::uint8_t *>(img->data()) + offset) =
                static_cast<std::uint8_t>((pix & (1 << (7 - b))) ? 1 : 0);
          }
        }
      }
    }
  }
}

namespace seedimg::modules {
namespace bmp {
bool check(const std::string &filename) {
  std::ifstream input(filename);
  char sig[2];

  try {
    input.read(sig, 2);
  } catch (std::iostream::failure) {
    return false;
  }

  return std::memcmp(sig, "BM", 2) == 0;
}
bool to(const std::string &filename, const simg &inp_img) {}
simg from(const std::string &filename) {
  std::ifstream input(filename);
  char bytes[54];

  try {
    input.read(bytes, 54);
  } catch (std::iostream::failure) {
    return nullptr;
  }

  std::uint32_t width, height;
  std::uint16_t bpp;
  constexpr unsigned wh_offset = ih_offset + ih_size;
  constexpr unsigned bpp_offset = wh_offset + 10;

  width = static_cast<std::uint32_t>(
      bytes[0 + wh_offset] | (bytes[1 + wh_offset] << 8) |
      (bytes[2 + wh_offset] << 16) | (bytes[3 + wh_offset] << 24));
  height = static_cast<std::uint32_t>(
      bytes[4 + wh_offset] | (bytes[5 + wh_offset] << 8) |
      (bytes[6 + wh_offset] << 16) | (bytes[7 + wh_offset] << 24));
  bpp = static_cast<std::uint16_t>(bytes[0 + bpp_offset] |
                                   (bytes[1 + bpp_offset] << 8));

  auto res = seedimg::make(width, height);
  switch (bpp) { case 1: }
}
} // namespace bmp
} // namespace seedimg::modules
