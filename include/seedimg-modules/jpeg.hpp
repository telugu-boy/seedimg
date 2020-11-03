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

#include <iostream>
#include <istream>
#include <csetjmp>
#include <cassert>

#include <seedimg-modules/modules-abc.hpp>

extern "C" {
#include <jpeglib.h>
#include <jerror.h>
}

// https://github.com/python/cpython/blob/master/Lib/_pyio.py#L27
#define J_BUF_SIZE (8 * 1024)

struct seedimg_jpeg_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf               buf;
};

namespace seedimg::modules::decoding {
class jpeg : public input_abc {
    seedimg_jpeg_error_mgr err;
    struct istream_jsrcm {
        jpeg_source_mgr pub;

        JOCTET* buffer;     // track of jpeg buffer start.
        bool    SOF;        // start-of-file indicator.
    };

    jpeg_decompress_struct d;
    std::istream&          in;

    static void    init_source       (j_decompress_ptr                  ) {}
    static boolean fill_input_buffer (j_decompress_ptr j                ) {
        auto self = reinterpret_cast<jpeg*>(j->client_data);
        auto src = reinterpret_cast<istream_jsrcm*>(j->src);

        try {
            self->in.read(reinterpret_cast<char*>(src->buffer), J_BUF_SIZE);
        } catch(std::ios_base::failure) {
            return FALSE;   /* fatal IO error */
        }
        if(self->in.gcount() <= 0) {
            if(src->SOF) {
                j->err->msg_code   = JERR_INPUT_EMPTY;
              (*j->err->error_exit) (reinterpret_cast<j_common_ptr>(j));
            }

            src->buffer[0] = 0xFF;
            src->buffer[1] = JPEG_EOI;

            src->pub.bytes_in_buffer = 2;
        } else
            src->pub.bytes_in_buffer = static_cast<std::size_t>(self->in.gcount());

        src->SOF                 = false;
        src->pub.next_input_byte = src->buffer;

        return TRUE;
    }
    static void    skip_input_data   (j_decompress_ptr j, long num_bytes) {
        if (num_bytes > 0) {
            while (num_bytes > static_cast<long>(j->src->bytes_in_buffer)) {
              num_bytes -= static_cast<long>(j->src->bytes_in_buffer);
              (void) (*j->src->fill_input_buffer)(j);
            }
            j->src->next_input_byte += static_cast<size_t>(num_bytes);
            j->src->bytes_in_buffer -= static_cast<size_t>(num_bytes);
        }
    }
    static void    term_source       (j_decompress_ptr                  ) {}
public:
    /**
     * @param method    DCT (discrete cosine transform) compuation method to use.
     */
    jpeg(std::istream& buf, J_DCT_METHOD method = JDCT_DEFAULT);

    simg_int width()  const noexcept;
    simg_int height() const noexcept;

    bool read(pixel *to);

    ~jpeg();
};
}

namespace seedimg::modules::encoding {
enum jpeg_subsampling {
    YCbCr_444,
    YCbCr_422,
    YCbCr_411,
    YCbCr_420
};

struct jpeg_restarts {
    bool per_row;
    int  val;
};

class jpeg : public output_abc {
    jpeg_compress_struct c;
    seedimg_jpeg_error_mgr err;

    struct ostream_jdstm {
        jpeg_destination_mgr pub;

        JOCTET* buffer;  // track of jpeg buffer start.
    };

    static void    init_destination   (j_compress_ptr j) {
      auto dest = reinterpret_cast<ostream_jdstm*>(j->dest);

      dest->buffer = reinterpret_cast<JOCTET*>(
        (*j->mem->alloc_small) (reinterpret_cast<j_common_ptr>(j),
                                JPOOL_IMAGE,
                                J_BUF_SIZE * sizeof(JOCTET)));

      dest->pub.next_output_byte = dest->buffer;
      dest->pub.free_in_buffer   = J_BUF_SIZE;
    }
    static boolean empty_output_buffer(j_compress_ptr j) {
      auto dest = reinterpret_cast<ostream_jdstm*>(j->dest);

      try {
        reinterpret_cast<jpeg*>(j->client_data)
                  ->out.write(reinterpret_cast<char*>(dest->buffer), J_BUF_SIZE);
      } catch(std::ios_base::failure) {
          return FALSE;
      }

      dest->pub.next_output_byte = dest->buffer;
      dest->pub.free_in_buffer   = J_BUF_SIZE;

      return TRUE;
    }
    static void    term_destination   (j_compress_ptr j) {
      auto dest = reinterpret_cast<ostream_jdstm*>(j->dest);
      auto self = reinterpret_cast<jpeg*>(j->client_data);

      try {
        self->out.write(reinterpret_cast<char*>(dest->buffer),
                        static_cast<std::streamsize>(J_BUF_SIZE - dest->pub.free_in_buffer));
        self->out.flush();
      } catch(std::ios_base::failure) {
            j->err->msg_code   = JERR_FILE_WRITE;
          (*j->err->error_exit) (reinterpret_cast<j_common_ptr>(j));
      }
    }

    std::ostream& out;
    simg_int      cur_scline = 0;
public:
    /**
     * @param quality   IJG quality scale (0..100], though qualities below 25 yield
     *                  the same result as 25 due to forced baseline constraint.
     *
     * @param sumbsamp  Subsampling ratio of luma and chroma channels.
     * @param optimize  Generate optimistic huffman tables for better compression.
     * @param arith     Use compression-wise superior arithmetic coding over Huffman.
     * @param smoothing Smoothing of input image [0..100].
     * @param method    DCT (discrete cosine transform) compuation method to use.
     * @param res_int   Restart interval of MCU blocks or rows.
     */
    jpeg(std::ostream& buf, simg_int         width,
                            simg_int         height,
                            int              quality     = 100,
                            jpeg_subsampling subsamp     = YCbCr_444,
                            bool             progressive = false,
                            bool             optimize    = false,
                            bool             arithcoding = false,
                            int              smoothing   = 0,
                            J_DCT_METHOD     dct_method  = JDCT_DEFAULT,
                            jpeg_restarts    res_int     = {false, 0});

    bool write(const pixel* const from);
    void flush();
    ~jpeg();
};
}
#endif
