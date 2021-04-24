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
#ifndef SEEDIMG_JPEG_H
#define SEEDIMG_JPEG_H

#include <iostream>
#include <fstream>
#include <csetjmp>
#include <cstring>

extern "C" {
#include <jconfig.h>
#include <jpeglib.h>
}

#include <seedimg.hpp>

namespace seedimg {
namespace modules {
namespace jpeg {

namespace detail {
struct seedimg_jpeg_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf               setjmp_buffer;
};

static char jpeg_last_error_msg[JMSG_LENGTH_MAX];
[[noreturn]] static void jpeg_error_exit(j_common_ptr cinfo) {
  seedimg_jpeg_error_mgr *err = reinterpret_cast<seedimg_jpeg_error_mgr *>(cinfo->err);
  (*(cinfo->err->format_message))(cinfo, jpeg_last_error_msg);
  std::longjmp(err->setjmp_buffer, 1);
}
}

/**
 * @param quality quality of JPEG encoding (0-100)
 * @param progressive whether to make JPEG progresssive
 */
bool check(const std::string &filename) noexcept {
    // SOI + APP0 marker, both are mandatory.
    static const std::uint8_t JFIF_MAGICCODE[] = {0xFF, 0xD8, 0xFF, 0xE0};

    std::ifstream file(filename, std::ios::binary);
    std::uint8_t  magic[4];

    file.read(reinterpret_cast<char *>(magic), 4);
    return !std::memcmp(JFIF_MAGICCODE, magic, 4);
}

bool to(const std::string &filename, const simg &image, uint8_t quality = 100,
        bool progressive = false) {
    auto output = std::fopen(filename.c_str(), "wb");
    if (output == nullptr)
        return false;

    jpeg_compress_struct jenc;
    detail::seedimg_jpeg_error_mgr jerr;
    int errcode = 0;

    jenc.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = detail::jpeg_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
        std::cerr << detail::jpeg_last_error_msg << std::endl;
        errcode = -1;
        goto finalise;
    }

    jpeg_create_compress(&jenc);
    jpeg_stdio_dest(&jenc, output);

    jenc.image_width      = static_cast<JDIMENSION>(image->width());
    jenc.image_height     = static_cast<JDIMENSION>(image->height());
    jenc.input_components = 4;
    jenc.in_color_space   = JCS_EXT_RGBA;

    jpeg_set_defaults(&jenc);
    jpeg_set_quality(&jenc, quality, TRUE);
    if (progressive)
        jpeg_simple_progression(&jenc);
    jpeg_start_compress(&jenc, TRUE);

    for (simg_int y = 0; y < image->height(); ++y) {
        JSAMPROW row = reinterpret_cast<JSAMPLE*>(image->row(y));
        if (jpeg_write_scanlines(&jenc, &row, 1) != 1)
        return false;
    }

    finalise:
    jpeg_finish_compress(&jenc);
    jpeg_destroy_compress(&jenc);
    std::fclose(output);

    return errcode == 0;
}
simg from(const std::string &filename) {
      auto input = std::fopen(filename.c_str(), "rb");
  if (input == nullptr)
    return nullptr;

  jpeg_decompress_struct jdec;
  detail::seedimg_jpeg_error_mgr jerr;
  simg res_img;
  int errcode = 0;

  jdec.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = detail::jpeg_error_exit;

  if (setjmp(jerr.setjmp_buffer)) {
    std::cerr << detail::jpeg_last_error_msg << std::endl;
    errcode = -1;
    goto finalise;
  }

  jpeg_create_decompress(&jdec);
  jpeg_stdio_src(&jdec, input);
  jpeg_read_header(&jdec, TRUE);

  jdec.out_color_space      = JCS_EXT_RGBA;
  jdec.out_color_components = 4;

  jpeg_start_decompress(&jdec);

  res_img = seedimg::make(jdec.output_width, jdec.output_height);

  for (simg_int y = 0; y < res_img->height(); ++y) {
    JSAMPROW row = reinterpret_cast<JSAMPLE*>(res_img->row(y));
    if (jpeg_read_scanlines(&jdec, &row, 1) != 1)
      return nullptr;
  }
finalise:
  if (errcode != -1)
    jpeg_finish_decompress(&jdec);
  jpeg_destroy_decompress(&jdec);
  std::fclose(input);
  if (errcode == 0)
    return res_img;
  else
    return nullptr;
}
} // namespace seedimg::modules::jpeg
} // namespace seedimg::modules
} // namespace seedimg
#endif
