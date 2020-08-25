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

#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <seedimg-formats/seedimg-bmp.hpp>

// http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm

constexpr unsigned ih_offset = 14;
constexpr unsigned ih_size = 4;

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

  static std::uint32_t data_offset;
  std::uint32_t width, height;
  unsigned padding;
  constexpr unsigned wh_offset = ih_offset + ih_size;

  data_offset =
      static_cast<std::uint32_t>(bytes[0 + 10] | (bytes[1 + 10] << 8) |
                                 (bytes[2 + 10] << 16) | (bytes[3 + 10] << 24));
  width = static_cast<std::uint32_t>(
      bytes[0 + wh_offset] | (bytes[1 + wh_offset] << 8) |
      (bytes[2 + wh_offset] << 16) | (bytes[3 + wh_offset] << 24));
  height = static_cast<std::uint32_t>(
      std::abs(bytes[4 + wh_offset] | (bytes[5 + wh_offset] << 8) |
               (bytes[6 + wh_offset] << 16) | (bytes[7 + wh_offset] << 24)));
  padding = width % 4;
  input.seekg(data_offset);
  for (simg_int y = 0; y < height; ++y) {
    try {
      i
    }
  }
}
} // namespace bmp
} // namespace seedimg::modules
