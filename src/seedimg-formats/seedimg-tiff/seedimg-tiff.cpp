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
// seedimg-png.cpp : Defines the functions for the static library.
//

#include <cstring>
#include <filesystem>
#include <ifstream>
#include <vector>

extern "C" {
#include <tiff.h>
}

#include <seedimg-formats/seedimg-tiff.hpp>

namespace seedimg::modules {
namespace tiff {
bool check(const std::string &filename) noexcept {
  std::error_code ec;
  std::size_t size = std::filesystem::file_size(filename, ec);
  if (ec != std::error_code{} || size < 8)
    return false;

  std::ifstream file(filename, std::ios::binary);
  std::uint8_t cmp[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
  std::uint8_t header[8] = {};
  file.read(reinterpret_cast<char *>(header), 8);
  return !std::memcmp(cmp, header, 8);
}

std::vector<simg> from(const std::string &filename) {}

bool to(const std::string &filename, const std::vector<simg> &inp_img) {}
} // namespace tiff
} // namespace seedimg::modules
