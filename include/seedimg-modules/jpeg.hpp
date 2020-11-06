/**********************************************************************
    seedimg - module based image manipulation library written in modern
                C++ Copyright(C) 2020 tripulse

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

#include <csetjmp>
#include <istream>
#include <memory>
#include <ostream>

#include <seedimg-modules/modules-abc.hpp>
#include <seedimg-utils.hpp>

extern "C" {
#include <jerror.h>
#include <jpeglib.h>
}

namespace seedimg::modules {
namespace jpeg {
/**
 * @brief Subsampling ratios/schemes to be used in JPEG encoding.
 */
enum class subsampling {
  YCbCr_444, /* Lossless                              */
  YCbCr_422, /* Cb, Cr horizontally halved            */
  YCbCr_411, /* Cb, Cr horizontally quartered         */
  YCbCr_420  /* Cb, Cr horizontally, verically halved */
};

/**
 * @brief JFIF restart marker interval in the bytestream, used in
 * transmission protocols where data can be corrupt.
 */
struct restart_int {
  int per_row = 1; /** 0 for rows as units, otherwise MCUs. */
  int val;         /** Put a marker after (val) units. */
};

struct istream_jsrcm {
  jpeg_source_mgr pub;

  JOCTET *buffer; // track of jpeg buffer start.
  bool SOF;       // start-of-file indicator.
};

static char simg_jpeg_error[JMSG_LENGTH_MAX];

[[noreturn]] static void simg_jpeg_error_exit(j_common_ptr cinfo) {
  (*(cinfo->err->format_message))(cinfo, simg_jpeg_error);
  throw std::runtime_error(simg_jpeg_error);
}

class decoder : public input_abc {
private:
  jpeg_decompress_struct cinfo;
  jpeg_error_mgr jerr_mgr;
  std::istream &in;
  std::streamsize buffer_size;

public:
  decoder(std::istream &stream, std::streamsize buffer_size_ = 8192,
          J_COLOR_SPACE out_color_space = JCS_EXT_RGBA,
          J_DCT_METHOD method = JDCT_DEFAULT)
      : in{stream}, buffer_size{buffer_size_} {
    jerr_mgr.error_exit = simg_jpeg_error_exit;
    cinfo.err = jpeg_std_error(&jerr_mgr);

    jpeg_create_decompress(&cinfo);

    auto src = static_cast<istream_jsrcm *>(
        cinfo.mem->alloc_small(reinterpret_cast<j_common_ptr>(&cinfo),
                               JPOOL_PERMANENT, sizeof(istream_jsrcm)));

    cinfo.src = reinterpret_cast<jpeg_source_mgr *>(src);
    cinfo.client_data = this;

    src->buffer = static_cast<JOCTET *>(cinfo.mem->alloc_small(
        reinterpret_cast<j_common_ptr>(&cinfo), JPOOL_PERMANENT,
        static_cast<std::size_t>(buffer_size) * sizeof(JOCTET)));

    src->pub.init_source = [](j_decompress_ptr) {};
    src->pub.fill_input_buffer = fill_input_buffer;
    src->pub.skip_input_data = skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart;
    src->pub.term_source = term_source;

    src->pub.next_input_byte = nullptr;
    src->pub.bytes_in_buffer = 0;
    src->SOF = true;

    jpeg_read_header(&cinfo, FALSE);

    cinfo.out_color_space = JCS_EXT_RGBA;
    cinfo.dct_method = method;

    jpeg_start_decompress(&cinfo);
  }

  simg_int width() const noexcept override { return cinfo.image_width; }
  simg_int height() const noexcept override { return cinfo.image_height; }

  bool read(pixel *to) override {}
  ~decoder() override;

private:
  static boolean fill_input_buffer(j_decompress_ptr cinfo) {
    auto self = reinterpret_cast<decoder *>(cinfo->client_data);
    auto src = reinterpret_cast<istream_jsrcm *>(cinfo->src);

    /* Official libjpeg docs:
     *  A FALSE return should only be used when I/O
        suspension is desired
     * i'll just let the user deal with the read failing
     */
    self->in.read(reinterpret_cast<char *>(src->buffer), self->buffer_size);

    if (self->in.gcount() == 0) {
      // if this is the first call to this function and we couldn't read
      // anything, call the error_exit which will just throw an exception
      if (src->SOF == true) {
        cinfo->err->msg_code = JERR_INPUT_EMPTY;
        (*****************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************cinfo
                                                                                                                                                                                                                                                                                                                                                                                                                  ->err
                                                                                                                                                                                                                                                                                                                                                                                                                  ->error_exit)(
            reinterpret_cast<j_common_ptr>(cinfo));
      }
    } else {
      src->pub.bytes_in_buffer = static_cast<std::size_t>(self->in.gcount());
    }

    src->pub.next_input_byte = src->buffer;
    src->SOF = false;

    return TRUE;
  }

  static void skip_input_data(j_decompress_ptr j, long num_bytes) {
    if (num_bytes > 0) {
      while (num_bytes > static_cast<long>(j->src->bytes_in_buffer)) {
        num_bytes -= static_cast<long>(j->src->bytes_in_buffer);
        (void)(*j->src->fill_input_buffer)(j);
      }
      j->src->next_input_byte += static_cast<size_t>(num_bytes);
      j->src->bytes_in_buffer -= static_cast<size_t>(num_bytes);
    }
  }

  static void term_source(j_decompress_ptr) {}
};

decoder::~decoder() {}

class encoder : public output_abc {

  bool write(const pixel *const from) noexcept {}

  void flush() {}
  ~encoder();
};

encoder::~encoder() {}
} // namespace jpeg
} // namespace seedimg::modules
#endif
