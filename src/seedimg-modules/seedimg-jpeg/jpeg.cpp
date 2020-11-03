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
#include <seedimg-modules/jpeg.hpp>
#include <seedimg-utils.hpp>

static char jpeg_last_error_msg[JMSG_LENGTH_MAX];
[[noreturn]] static void jpeg_error_exit(j_common_ptr j) {
  seedimg_jpeg_error_mgr *err =
      reinterpret_cast<seedimg_jpeg_error_mgr *>(j->err);
  (*(j->err->format_message))(j, jpeg_last_error_msg);
  std::longjmp(err->buf, 1);
}

namespace seedimg::modules::decoding {
jpeg::jpeg(std::istream& buf, J_DCT_METHOD method) : in(buf)
{
    d.err = jpeg_std_error(&err.pub);
    err.pub.error_exit = jpeg_error_exit;

    if(setjmp(err.buf)) {
        jpeg_destroy_decompress(&d);
        throw input_failure{jpeg_last_error_msg};
    }

    jpeg_create_decompress(&d);


    auto src = reinterpret_cast<istream_jsrcm*>(
                    d.mem->alloc_small(reinterpret_cast<j_common_ptr>(&d),
                                       JPOOL_PERMANENT,
                                       sizeof(istream_jsrcm)));
    d.client_data = this;
    d.src         = reinterpret_cast<jpeg_source_mgr*>(src);

    src->buffer = reinterpret_cast<JOCTET*>(
                            d.mem->alloc_small(reinterpret_cast<j_common_ptr>(&d),
                                               JPOOL_PERMANENT,
                                               J_BUF_SIZE * sizeof(JOCTET)));
    src->pub.init_source       = init_source;
    src->pub.fill_input_buffer = fill_input_buffer;
    src->pub.skip_input_data   = skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart;
    src->pub.term_source       = term_source;

    src->pub.next_input_byte = nullptr;
    src->pub.bytes_in_buffer = 0;
    src->SOF                 = true;

    jpeg_read_header(&d, FALSE);

    d.out_color_space = JCS_EXT_RGBA;
    d.dct_method      = method;

    jpeg_start_decompress(&d);
}

simg_int jpeg::width()  const noexcept { return d.image_width;  }
simg_int jpeg::height() const noexcept { return d.image_height; }

bool jpeg::read(pixel* to) {
    if(d.output_scanline >= d.output_height)
        return false;

    auto row = reinterpret_cast<JSAMPROW>(to);
    return jpeg_read_scanlines(&d, &row, 1) == 1;
}

jpeg::~jpeg() {
    jpeg_finish_decompress(&d);
    jpeg_destroy_decompress(&d);
}
}


