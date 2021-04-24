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
#ifndef SEEDIMG_WEBP_H
#define SEEDIMG_WEBP_H

#include <seedimg.hpp>

namespace seedimg {
namespace modules {
namespace webp {
bool check(const std::string &filename) noexcept {
  std::uint8_t header[8] = {};
  std::ifstream file(filename, std::ios::binary);

  file.read(reinterpret_cast<char *>(header), 4);
  file.ignore(4);
  file.read(reinterpret_cast<char *>(header + 4), 4);
  
  return !std::memcmp("RIFFWEBP", header, 8);
}

bool to(const std::string &filename, const simg &inp_img, float quality = 100.0) {
  std::uint8_t *output = nullptr;
  // this is the amount of bytes output has been allocated, 0 if failure
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
  std::size_t size = std::filesystem::file_size(filename, ec);
  if (ec != std::error_code{})
    return nullptr;
  int width, height;
  std::uint8_t *data = new std::uint8_t[size];

  // read into data
  std::ifstream file(filename, std::ios::binary);
  if (!file.read(reinterpret_cast<char *>(data), static_cast<long>(size)))
    return nullptr;

  int success = WebPGetInfo(data, size, &width, &height);
  if (!success)
    return nullptr;

  auto *decoded = WebPDecodeRGBA(data, size, &width, &height);

  delete[] data;

  return std::make_unique<seedimg::img>(
      static_cast<simg_int>(width), static_cast<simg_int>(height),
      reinterpret_cast<seedimg::pixel *>(decoded));
}
} // namespace seedimg::modules::webp
} // namespace seedimg::modules
} // namespace seedimg
#endif
