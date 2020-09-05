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
// seedimg-jpeg.cpp : Defines the functions for the static library.
//

#include <csetjmp>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

extern "C" {
#include <jconfig.h>

#include <jpeglib.h>
}

#include <seedimg-formats/seedimg-jpeg.hpp>

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

struct seedimg_jpeg_error_mgr {
  /* "public" fields */
  struct jpeg_error_mgr pub;
  /* for return to caller */
  jmp_buf setjmp_buffer;
};
static char jpeg_last_error_msg[JMSG_LENGTH_MAX];
[[noreturn]] void jpeg_error_exit(j_common_ptr cinfo) {
  seedimg_jpeg_error_mgr *err =
      reinterpret_cast<seedimg_jpeg_error_mgr *>(cinfo->err);
  (*(cinfo->err->format_message))(cinfo, jpeg_last_error_msg);
  std::longjmp(err->setjmp_buffer, 1);
}

bool seedimg::modules::jpeg::check(const std::string &filename) noexcept {
  std::error_code ec;
  std::size_t size = std::filesystem::file_size(filename, ec);
  if (ec != std::error_code{} || size < 3)
    return false;

  std::ifstream file(filename, std::ios::binary);
  std::uint8_t cmp[3] = {0xFF, 0xD8, 0xFF};
  std::uint8_t header[3] = {};
  file.read(reinterpret_cast<char *>(header), 3);
  return !std::memcmp(cmp, header, 3);
}

simg seedimg::modules::jpeg::from(const std::string &filename) {
  auto input = std::fopen(filename.c_str(), "rb");
  if (input == nullptr)
    return nullptr;

  jpeg_decompress_struct jdec;
  seedimg_jpeg_error_mgr jerr;
  simg res_img;
  JSAMPROW rowbuffer = nullptr;
  int errcode = 0;

  jdec.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = jpeg_error_exit;

  if (setjmp(jerr.setjmp_buffer)) {
    std::cerr << jpeg_last_error_msg << std::endl;
    errcode = -1;
    goto finalise;
  }

  jpeg_CreateDecompress(
      &jdec, JPEG_LIB_VERSION,
      static_cast<size_t>(sizeof(struct jpeg_decompress_struct)));
  jpeg_stdio_src(&jdec, input);
  jpeg_read_header(&jdec, TRUE);
  jpeg_start_decompress(&jdec);

  res_img =
      std::make_shared<seedimg::img>(jdec.output_width, jdec.output_height);

  // libjpeg doesn't allow colorspace conversion while decoding for some weird
  // reason, it's set static to produce an RGB image at the end, thus conversion
  // is done manually.
  rowbuffer = new JSAMPLE[jdec.output_width * 3];

  for (simg_int y = 0; y < res_img->height(); ++y) {
    if (jpeg_read_scanlines(&jdec, &rowbuffer, 1) != 1)
      return nullptr;

    for (simg_int x = 0; x < res_img->width(); ++x) {
      res_img->pixel(x, y) = {rowbuffer[3 * x], rowbuffer[3 * x + 1],
                              rowbuffer[3 * x + 2], 0xFF};
    }
  }

finalise:
  if (errcode != -1)
    jpeg_finish_decompress(&jdec);
  jpeg_destroy_decompress(&jdec);
  std::fclose(input);
  if (rowbuffer != nullptr)
    delete[] rowbuffer;
  if (errcode == 0)
    return res_img;
  else
    return nullptr;
}

/**
 * @param quality quality of JPEG encoding (0-100)
 * @param progressive whether to make JPEG progresssive
 */
// quality default param = 100, progressive = false
bool seedimg::modules::jpeg::to(const std::string &filename, const simg &image,
                                uint8_t quality, bool progressive) {

  auto output = std::fopen(filename.c_str(), "wb");
  if (output == nullptr)
    return false;

  jpeg_compress_struct jenc;
  seedimg_jpeg_error_mgr jerr;
  JSAMPROW rowbuffer = nullptr;
  int errcode = 0;

  jenc.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = jpeg_error_exit;

  if (setjmp(jerr.setjmp_buffer)) {
    std::cerr << jpeg_last_error_msg << std::endl;
    errcode = -1;
    goto finalise;
  }

  jpeg_CreateCompress(&jenc, JPEG_LIB_VERSION,
                      static_cast<size_t>(sizeof(struct jpeg_compress_struct)));
  jpeg_stdio_dest(&jenc, output);

  jenc.image_width = static_cast<JDIMENSION>(image->width());
  jenc.image_height = static_cast<JDIMENSION>(image->height());
  jenc.input_components = 3;
  jenc.in_color_space = JCS_RGB;

  jpeg_set_defaults(&jenc);
  jpeg_set_quality(&jenc, quality, TRUE);
  if (progressive)
    jpeg_simple_progression(&jenc);
  jpeg_start_compress(&jenc, TRUE);

  rowbuffer = new JSAMPLE[jenc.image_width *
                          static_cast<simg_int>(jenc.input_components)];

  for (simg_int y = 0; y < image->height(); ++y) {
    for (simg_int x = 0; x < image->width(); ++x) {
      auto pix = image->pixel(x, y);
      rowbuffer[3 * x] = pix.r;
      rowbuffer[3 * x + 1] = pix.g;
      rowbuffer[3 * x + 2] = pix.b;
    }
    if (jpeg_write_scanlines(&jenc, &rowbuffer, 1) != 1)
      return false;
  }

finalise:
  jpeg_finish_compress(&jenc);
  jpeg_destroy_compress(&jenc);
  std::fclose(output);

  if (rowbuffer != nullptr)
    delete[] rowbuffer;
  return errcode == 0;
}
