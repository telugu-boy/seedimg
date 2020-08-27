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
// seedimg-webp.cpp : Defines the functions for the static library.
//

#include <cstring>
#include <filesystem>
#include <fstream>

extern "C" {
#include <webp/decode.h>
#include <webp/encode.h>
}

#include <seedimg-formats/seedimg-webp.hpp>

namespace seedimg::modules {
namespace webp {

// TODO: This is an example of a library function
bool check(const std::string &filename) noexcept {
  std::error_code ec;
  std::size_t size = std::filesystem::file_size(filename, ec);
  if (ec != std::error_code{} || size < 8)
    return false;
  // this does not check the full header as
  // the length is little endian and that would require more code
  std::uint8_t cmp[8] = {'R', 'I', 'F', 'F', 'W', 'E', 'B', 'P'};
  std::uint8_t header[8] = {};
  std::ifstream file(filename, std::ios::binary);
  file.read(reinterpret_cast<char *>(header), 4);
  file.ignore(4);
  file.read(reinterpret_cast<char *>(header + 4), 4);
  return !std::memcmp(cmp, header, 8);
}

bool to(const std::string &filename, const simg &inp_img, float quality) {
  std::uint8_t *output = nullptr;
  // this is the amount of bytes output has been allocated, 0 if failure
  // '''''-'''''
  std::size_t success = WebPEncodeRGBA(
      reinterpret_cast<std::uint8_t *>(inp_img->data()),
      static_cast<int>(inp_img->width()), static_cast<int>(inp_img->height()),
      static_cast<int>(inp_img->width() *
                       static_cast<simg_int>(sizeof(seedimg::pixel))),
      quality, &output);
  if (success == 0)
    return false;
  std::ofstream file(filename, std::ios::binary);
  file.write(reinterpret_cast<char *>(output),
             static_cast<std::streamsize>(success));
  WebPFree(output);
  return true;
}
simg from(const std::string &filename) {
  std::error_code ec;
  size_t size = std::filesystem::file_size(filename, ec);
  if (ec != std::error_code{})
    return nullptr;
  int width, height;
  auto data = std::make_shared<uint8_t[]>(size);

  // read into data
  std::ifstream file(filename, std::ios::binary);
  if (!file.read(reinterpret_cast<char *>(data.get()), static_cast<long>(size)))
    return nullptr;

  int success = WebPGetInfo(data.get(), size, &width, &height);
  if (!success)
    return nullptr;

  return std::make_shared<seedimg::img>(
      static_cast<simg_int>(width), static_cast<simg_int>(height),
      reinterpret_cast<seedimg::pixel *>(
          WebPDecodeRGBA(data.get(), size, &width, &height)));
}
} // namespace webp
} // namespace seedimg::modules
