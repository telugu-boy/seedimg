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
#include <fstream>
#include <iostream>

extern "C" {
#include <png.h>
}

#include "seedimg-png.hpp"

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

namespace seedimg::modules {
namespace png {
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

simg from(const std::string &filename) {
  simg res_img = nullptr;
  png_structp png_ptr = nullptr;
  png_infop info_ptr = nullptr;
  // chosen 127 as 0 is already taken as a type.
  uint8_t color_type = 127;
  uint8_t bit_depth = 0;
  uint8_t interlace_type = 127;
  int errcode = 0;

  auto fp = std::fopen(filename.c_str(), "rb");

  if (!fp) {
    std::cerr << "File " << filename << " could not be opened" << std::endl;
    errcode = -1;
    return nullptr;
  }

  uint8_t sig[8];

  fread(sig, sizeof(uint8_t), 8, fp);
  if (!png_check_sig(sig, 8)) {
    std::cerr << filename << " is not a valid PNG file" << std::endl;
    errcode = -1;
    goto finalise;
  }

  // validation done: initialize info structs.

  png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) {
    std::cerr << "Failed to create read struct for " << filename << std::endl;
    errcode = -1;
    goto finalise;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    std::cerr << "Failed to create info struct for " << filename << std::endl;
    errcode = -1;
    goto finalise;
  }

  // set jmp for errors, finalise is just for cleanup

  if (setjmp(png_jmpbuf(png_ptr))) {
    std::cerr << "Error during PNG processing for " << filename << std::endl;
    errcode = -1;
    goto finalise;
  }

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);
  png_read_info(png_ptr, info_ptr);

  png_set_interlace_handling(png_ptr);
  color_type = png_get_color_type(png_ptr, info_ptr);
  bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  interlace_type = png_get_interlace_type(png_ptr, info_ptr);
  res_img =
      std::make_unique<seedimg::img>(png_get_image_width(png_ptr, info_ptr),
                                     png_get_image_height(png_ptr, info_ptr));

  if (bit_depth == 16)
    png_set_strip_16(png_ptr);

  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png_ptr);

  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png_ptr);

  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png_ptr);

  if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png_ptr);

  png_read_update_info(png_ptr, info_ptr);

  // This will load the png into the vectors. (non interlaced pngs only)
  png_read_image(png_ptr, reinterpret_cast<png_bytepp>(res_img->data()));

finalise:
  if (fp != nullptr)
    fclose(fp);
  if (info_ptr != nullptr)
    png_destroy_info_struct(png_ptr, &info_ptr);
  if (png_ptr != nullptr)
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);

  if (errcode != 0) {
    return nullptr;
  } else {
    return res_img;
  }
}

bool to(const std::string &filename,
        const simg &inp_img) {
  png_structp png_ptr = nullptr;
  png_infop info_ptr = nullptr;
  int errcode = 0;

  auto fp = std::fopen(filename.c_str(), "wb");

  if (!fp) {
    std::cerr << "File " << filename << " could not be opened" << std::endl;
    errcode = -1;
    return false;
  }

  // validation done: initialize info structs.

  png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) {
    std::cerr << "Failed to create write struct for " << filename << std::endl;
    errcode = -1;
    goto finalise;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    std::cerr << "Failed to create info struct for " << filename << std::endl;
    errcode = -1;
    goto finalise;
  }

  // set jmp for errors, finalise is just for cleanup
  if (setjmp(png_jmpbuf(png_ptr))) {
    std::cerr << "Error during PNG processing for " << filename << std::endl;
    errcode = -1;
    goto finalise;
  }

  png_init_io(png_ptr, fp);

  // Output is 8bit depth, RGBA format.
  png_set_IHDR(png_ptr, info_ptr, static_cast<png_uint_32>(inp_img->width()),
               static_cast<png_uint_32>(inp_img->height()), 8,
               PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png_ptr, info_ptr);

  png_write_image(png_ptr, reinterpret_cast<png_bytepp>(inp_img->data()));

  png_write_end(png_ptr, nullptr);

finalise:
  if (fp != nullptr)
    fclose(fp);
  if (info_ptr != nullptr)
    png_destroy_info_struct(png_ptr, &info_ptr);
  if (png_ptr != nullptr)
    png_destroy_write_struct(&png_ptr, nullptr);

  return errcode == 0;
}
} // namespace png
} // namespace seedimg::modules
