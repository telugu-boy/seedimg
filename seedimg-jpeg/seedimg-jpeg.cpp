// seedimg-jpeg.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"

#include <iostream>
#include <jpeglib.h>

#include "seedimg-jpeg.hpp"

namespace seedimg {
  namespace modules {
    namespace jpeg {
      std::optional<std::unique_ptr<seedimg::img>> from(std::string filename) {
        auto input = std::fopen(filename.c_str(), "rb");
        if (input == NULL) return {};

        jpeg_decompress_struct jdec;
        jpeg_error_mgr jerr;

        jdec.err = jpeg_std_error(&jerr);

        jpeg_create_decompress(&jdec);
        jpeg_stdio_src(&jdec, input);
        jpeg_read_header(&jdec, TRUE);
        jpeg_start_decompress(&jdec);

        seedimg::img image(jdec.output_width, jdec.output_height);

        // libjpeg doesn't allow colorspace conversion while decoding for some weird reason,
        // it's set static to produce an RGB image at the end, thus conversion is done manually.
        JSAMPROW rowbuffer = new JSAMPLE[jdec.output_width * 3];

        for (size_t y = 0; y < image.height(); ++y) {
          if (jpeg_read_scanlines(&jdec, &rowbuffer, 1) != 1)
            return {};

          for (size_t x = 0; x < image.width(); ++x) {
            #define P(ch) rowbuffer[3*x + ch]
            image.data[y][x] = pixel { P(0), P(1), P(2), 255 };
          }
        }

        jpeg_finish_decompress(&jdec);
        jpeg_destroy_decompress(&jdec);
        std::fclose(input);

        return std::make_unique<seedimg::img>(image);
      }

      /**
       * @param quality quality of JPEG encoding (0-100)
       * @param progressive whether to make JPEG progresssive
       */
      bool to(std::string filename, std::unique_ptr<seedimg::img>& image, uint8_t quality, bool progressive) {
        auto output = std::fopen(filename.c_str(), "wb");
        if (output == NULL) return {};

        jpeg_compress_struct jenc;
        jpeg_error_mgr jerr;

        jenc.err = jpeg_std_error(&jerr);

        jpeg_create_compress(&jenc);
        jpeg_stdio_dest(&jenc, output);

        jenc.image_width = image->width();
        jenc.image_height = image->height();
        jenc.input_components = 4;
        jenc.in_color_space = JCS_EXT_RGBA;

        jpeg_set_defaults(&jenc);
        jpeg_set_quality(&jenc, quality, TRUE);
        if (progressive)
          jpeg_simple_progression(&jenc);
        jpeg_start_compress(&jenc, TRUE);

        JSAMPROW rowbuffer = new JSAMPLE[jenc.image_width * 4];

        for (size_t y = 0; y < image->height(); ++y) {
          for (size_t x = 0; x < image->width(); ++x) {
            auto pix = image->data[y][x];
            #define I(ch) (4 * x + ch)

            rowbuffer[I(0)] = pix.r, rowbuffer[I(1)] = pix.g,
            rowbuffer[I(2)] = pix.g, rowbuffer[I(3)] = pix.b;
          }

          if (jpeg_write_scanlines(&jenc, &rowbuffer, 1) != 1)
            return false;
        }

        jpeg_finish_compress(&jenc);
        jpeg_destroy_compress(&jenc);
        std::fclose(output);

        return true;
      }
    }
  }
}