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
// seedimg.cpp : Defines the functions for the static library.
//

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>

#include "seedimg.hpp"

namespace seedimg {
std::unique_ptr<seedimg::img> from(const std::string &filename) {
  std::ifstream infile(filename, std::ios::binary);
  if (infile.fail())
    return nullptr;

  std::size_t width, height;
  infile.read(reinterpret_cast<char *>(&width), sizeof(width));
  infile.read(reinterpret_cast<char *>(&height), sizeof(height));

  std::error_code ec;
  auto size = std::filesystem::file_size(filename, ec);

  // check if the data size is equal to the retrieved rectangular area.
  // if not, then just don't return any results, partial data isn't supported.
  if (ec != std::error_code{} ||
      size != sizeof(seedimg::pixel) * width * height + sizeof(width) +
                  sizeof(height))
    return nullptr;

  auto image = std::make_unique<seedimg::img>(width, height);
  const auto stride =
      sizeof(seedimg::pixel) * static_cast<std::size_t>(image->width());

  for (simg_int y = 0; y < image->height(); ++y)
    infile.read(reinterpret_cast<char *>(image->row(y)),
                static_cast<long>(stride));

  return image;
}

bool to(const std::string &filename,
        const std::unique_ptr<seedimg::img> &image) {
  std::ofstream outfile(filename, std::ios::binary);
  if (outfile.fail())
    return false;

  outfile.write(reinterpret_cast<const char *>(image->width()),
                sizeof(image->width()));
  outfile.write(reinterpret_cast<const char *>(image->height()),
                sizeof(image->height()));

  const auto stride =
      sizeof(seedimg::pixel) * static_cast<std::size_t>(image->width());

  for (simg_int y = 0; y < image->height(); ++y)
    outfile.write(reinterpret_cast<const char *>(image->row(y)),
                  static_cast<long>(stride));

  return true;
}

std::unique_ptr<seedimg::img> make(simg_int width, simg_int height) {
  return std::make_unique<seedimg::img>(width, height);
}
} // namespace seedimg
