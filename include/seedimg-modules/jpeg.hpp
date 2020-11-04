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
#include <ostream>

#include <seedimg-modules/modules-abc.hpp>
#include <seedimg-utils.hpp>

extern "C" {
#include <jerror.h>
#include <jpeglib.h>
}

namespace seedimg::modules {
namespace jpeg {
namespace impl {
struct simg_jpeg_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf               buf;
};

static char              jpeg_last_error_msg[JMSG_LENGTH_MAX];
[[noreturn]] static void jpeg_error_exit(j_common_ptr j) {
    simg_jpeg_error_mgr* err = reinterpret_cast<simg_jpeg_error_mgr*>(j->err);
    (*(j->err->format_message))(j, jpeg_last_error_msg);
    std::longjmp(err->buf, 1);
}

constexpr const static auto J_BUF_SIZE = 8 * 1024;
}; // namespace impl

/**
 * @brief Subsampling ratios/schemes to be used in JPEG encoding.
 */
enum subsampling {
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
    bool per_row; /** Treat rows as units, otherwise MCUs. */
    int  val;     /** Put a marker after (val) units. */
};

class decoder : public input_abc {
    impl::simg_jpeg_error_mgr err;
    struct istream_jsrcm {
        jpeg_source_mgr pub;

        JOCTET* buffer; // track of jpeg buffer start.
        bool    SOF;    // start-of-file indicator.
    };

    jpeg_decompress_struct d;
    std::istream&          in;

    static void    init_source(j_decompress_ptr) {}
    static boolean fill_input_buffer(j_decompress_ptr j) {
        auto self = reinterpret_cast<decoder*>(j->client_data);
        auto src  = reinterpret_cast<istream_jsrcm*>(j->src);

        try {
            self->in.read(reinterpret_cast<char*>(src->buffer), impl::J_BUF_SIZE);
        } catch (std::ios_base::failure) { return FALSE; /* fatal IO error */ }
        if (self->in.gcount() <= 0) {
            if (src->SOF) {
                j->err->msg_code = JERR_INPUT_EMPTY;
                (*j->err->error_exit)(reinterpret_cast<j_common_ptr>(j));
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

  public:
    /**
     * @param method DCT (discrete cosine transform) compuation method to use.
     */
    decoder(std::istream& input, J_DCT_METHOD method = JDCT_DEFAULT)
        : in(input) {
        d.err              = jpeg_std_error(&err.pub);
        err.pub.error_exit = impl::jpeg_error_exit;

        if (setjmp(err.buf)) {
            jpeg_destroy_decompress(&d);
            throw input_failure{impl::jpeg_last_error_msg};
        }

        jpeg_create_decompress(&d);

        auto src = reinterpret_cast<istream_jsrcm*>(
            d.mem->alloc_small(reinterpret_cast<j_common_ptr>(&d), JPOOL_PERMANENT, sizeof(istream_jsrcm)));

        d.client_data = this;
        d.src         = reinterpret_cast<jpeg_source_mgr*>(src);

        src->buffer = reinterpret_cast<JOCTET*>(
            d.mem->alloc_small(reinterpret_cast<j_common_ptr>(&d), JPOOL_PERMANENT, impl::J_BUF_SIZE * sizeof(JOCTET)));

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

    simg_int width() const noexcept { return d.image_width; }
    simg_int height() const noexcept { return d.image_height; }

    bool read(pixel* to) {
        if (d.output_scanline >= d.output_height) return false;

        auto row = reinterpret_cast<JSAMPROW>(to);
        return jpeg_read_scanlines(&d, &row, 1) == 1;
    }

    ~decoder();
};

decoder::~decoder() {
    jpeg_finish_decompress(&d);
    jpeg_destroy_decompress(&d);
}

class encoder : public output_abc {
    jpeg_compress_struct      c;
    impl::simg_jpeg_error_mgr err;

    struct ostream_jdstm {
        jpeg_destination_mgr pub;

        JOCTET* buffer; // track of jpeg buffer start.
    };

    static void init_destination(j_compress_ptr j) {
        auto dest = reinterpret_cast<ostream_jdstm*>(j->dest);

        dest->buffer = reinterpret_cast<JOCTET*>(
            (*j->mem->alloc_small)(reinterpret_cast<j_common_ptr>(j), JPOOL_IMAGE, impl::J_BUF_SIZE * sizeof(JOCTET)));

        dest->pub.next_output_byte = dest->buffer;
        dest->pub.free_in_buffer   = impl::J_BUF_SIZE;
    }
    static boolean empty_output_buffer(j_compress_ptr j) {
        auto dest = reinterpret_cast<ostream_jdstm*>(j->dest);

        try {
            reinterpret_cast<encoder*>(j->client_data)
                ->out.write(reinterpret_cast<char*>(dest->buffer), impl::J_BUF_SIZE);
        } catch (std::ios_base::failure) { return FALSE; }

        dest->pub.next_output_byte = dest->buffer;
        dest->pub.free_in_buffer   = impl::J_BUF_SIZE;

        return TRUE;
    }
    static void term_destination(j_compress_ptr j) {
        auto dest = reinterpret_cast<ostream_jdstm*>(j->dest);
        auto self = reinterpret_cast<encoder*>(j->client_data);

        try {
            self->out.write(reinterpret_cast<char*>(dest->buffer),
                            static_cast<std::streamsize>(impl::J_BUF_SIZE - dest->pub.free_in_buffer));
            self->out.flush();
        } catch (std::ios_base::failure) {
            j->err->msg_code = JERR_FILE_WRITE;
            (*j->err->error_exit)(reinterpret_cast<j_common_ptr>(j));
        }
    }

    std::ostream& out;

  public:
    /**
     * @param quality   IJG quality scale (0..100], though qualities below 25
     * yield the same result as 25 due to forced baseline constraint.
     *
     * @param sumbsamp  Subsampling ratio of luma and chroma channels.
     * @param optimize  Generate optimistic huffman tables for better compression.
     * @param arith     Use compression-wise superior arithmetic coding over Huffman.
     * @param smoothing Smoothing of input image [0..100].
     * @param method    DCT (discrete cosine transform) compuation method to use.
     * @param res_int   Restart interval of MCU blocks or rows.
     */
    encoder(std::ostream& output,
            simg_int      width,
            simg_int      height,
            int           quality     = 100,
            subsampling   subsamp     = YCbCr_444,
            bool          progressive = false,
            bool          optimize    = false,
            bool          arithcoding = false,
            int           smoothing   = 0,
            J_DCT_METHOD  dct_method  = JDCT_DEFAULT,
            restart_int   res_int     = {false, 0})
        : out(output) {
        c.err              = jpeg_std_error(&err.pub);
        err.pub.error_exit = impl::jpeg_error_exit;

        if (setjmp(err.buf)) {
            jpeg_destroy_compress(&c);
            throw output_failure{impl::jpeg_last_error_msg};
        }

        jpeg_create_compress(&c);

        c.client_data = this;
        c.dest        = reinterpret_cast<jpeg_destination_mgr*>(
            c.mem->alloc_small(reinterpret_cast<j_common_ptr>(&c), JPOOL_PERMANENT, sizeof(ostream_jdstm)));

        c.dest->init_destination    = init_destination;
        c.dest->empty_output_buffer = empty_output_buffer;
        c.dest->term_destination    = term_destination;

        c.in_color_space   = JCS_EXT_RGBA;
        c.input_components = 4;

        jpeg_set_defaults(&c);

        c.image_width  = static_cast<JDIMENSION>(width);
        c.image_height = static_cast<JDIMENSION>(height);

        jpeg_set_quality(&c, quality, TRUE);

        switch (subsamp) {
            case YCbCr_444: // all planes have same resolution.
                c.comp_info[0].h_samp_factor = 1;
                c.comp_info[0].v_samp_factor = 1;
                c.comp_info[1].h_samp_factor = 1;
                c.comp_info[1].v_samp_factor = 1;
                c.comp_info[2].h_samp_factor = 1;
                c.comp_info[2].v_samp_factor = 1;
                break;

            case YCbCr_422: // both chroma channels are halved horizonally.
                c.comp_info[0].h_samp_factor = 2;
                c.comp_info[0].v_samp_factor = 2;
                c.comp_info[1].h_samp_factor = 1;
                c.comp_info[1].v_samp_factor = 2;
                c.comp_info[2].h_samp_factor = 1;
                c.comp_info[2].v_samp_factor = 2;
                break;

            case YCbCr_411: // both chroma channels are quartered horizontally.
                c.comp_info[0].h_samp_factor = 4;
                c.comp_info[0].v_samp_factor = 4;
                c.comp_info[1].h_samp_factor = 1;
                c.comp_info[1].v_samp_factor = 4;
                c.comp_info[2].h_samp_factor = 1;
                c.comp_info[2].v_samp_factor = 4;
                break;

            case YCbCr_420: // both chroma channels are halved vertically and horizontally.
                c.comp_info[0].h_samp_factor = 2;
                c.comp_info[0].v_samp_factor = 2;
                c.comp_info[1].h_samp_factor = 1;
                c.comp_info[1].v_samp_factor = 1;
                c.comp_info[2].h_samp_factor = 1;
                c.comp_info[2].v_samp_factor = 1;
                break;
        }

        if (progressive) jpeg_simple_progression(&c);

        c.optimize_coding  = optimize;
        c.arith_code       = arithcoding;
        c.smoothing_factor = smoothing;
        c.dct_method       = dct_method;

        if (res_int.per_row)
            c.restart_in_rows = res_int.val;
        else
            c.restart_interval = seedimg::utils::clamp(res_int.val, 0, std::numeric_limits<int>::max());
        jpeg_start_compress(&c, TRUE);
    }

    bool write(const pixel* const from) {
        auto row = reinterpret_cast<JSAMPROW>(const_cast<pixel*>(from));
        return jpeg_write_scanlines(&c, &row, 1) == 1;
    }

    void flush() { jpeg_finish_compress(&c); }
    ~encoder();
};

encoder::~encoder() {
    flush();
    jpeg_destroy_compress(&c);
}
}; // namespace jpeg
}; // namespace seedimg::modules
#endif
