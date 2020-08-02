// seedimg-jpeg.cpp : Defines the functions for the static library.
//




#include <iostream>
#include <fstream>
#include <csetjmp>
#include <filesystem>

extern "C" {
#include <jpeglib.h>
}

#include "seedimg-jpeg.hpp"

#pragma warning(disable:4996)

struct seedimg_jpeg_error_mgr {
	/* "public" fields */
	struct jpeg_error_mgr pub;
	/* for return to caller */
	jmp_buf setjmp_buffer;
};
char jpegLastErrorMsg[JMSG_LENGTH_MAX];
void jpegErrorExit(j_common_ptr cinfo) {
	seedimg_jpeg_error_mgr* err = (seedimg_jpeg_error_mgr*)cinfo->err;
	(*(cinfo->err->format_message))(cinfo, jpegLastErrorMsg);
	std::longjmp(err->setjmp_buffer, 1);
}

bool seedimg::modules::jpeg::check(const std::string& filename) noexcept {
	std::error_code ec;
	std::size_t size = std::filesystem::file_size(filename, ec);
	if (ec != std::error_code{} || size < 3) return false;
	
	std::ifstream file(filename, std::ios::binary);
	std::uint8_t cmp[3] = { 0xFF, 0xD8, 0xFF };
	std::uint8_t header[3] = {};
	file.read(reinterpret_cast<char*>(header), 3);
	return !std::memcmp(cmp, header, 3);
}

std::optional<std::unique_ptr<seedimg::img> > seedimg::modules::jpeg::from(const std::string& filename) noexcept {
	auto input = std::fopen(filename.c_str(), "rb");
	if (input == NULL) return std::nullopt;

	jpeg_decompress_struct jdec;
	seedimg_jpeg_error_mgr jerr;
	std::unique_ptr<seedimg::img> image;
	JSAMPROW rowbuffer = NULL;
	int errcode = 0;

	jdec.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = jpegErrorExit;

	if (std::setjmp(jerr.setjmp_buffer)) {
		std::cerr << jpegLastErrorMsg << std::endl;
		errcode = -1;
		goto finalise;
	}

	jpeg_create_decompress(&jdec);
	jpeg_stdio_src(&jdec, input);
	jpeg_read_header(&jdec, TRUE);
	jpeg_start_decompress(&jdec);

	image = std::make_unique<seedimg::img>(jdec.output_width, jdec.output_height);

	// libjpeg doesn't allow colorspace conversion while decoding for some weird reason,
	// it's set static to produce an RGB image at the end, thus conversion is done manually.
	rowbuffer = new JSAMPLE[jdec.output_width * 3];

	for (std::size_t y = 0; y < image->height; ++y) {
		if (jpeg_read_scanlines(&jdec, &rowbuffer, 1) != 1)
			return std::nullopt;

		for (std::size_t x = 0; x < image->width; ++x) {
#define P(ch) rowbuffer[3*x + ch]
			image->get_pixel(x, y) = { P(0), P(1), P(2), 0xFF };
		}
	}

finalise:
	if(errcode!=-1)
		jpeg_finish_decompress(&jdec);
	jpeg_destroy_decompress(&jdec);
	std::fclose(input);
	if(rowbuffer!=NULL)
		delete[] rowbuffer;
	if (errcode == 0)
		return image;
	else return std::nullopt;
}

/**
  * @param quality quality of JPEG encoding (0-100)
  * @param progressive whether to make JPEG progresssive
  */
  // quality default param = 100, progressive = false
bool seedimg::modules::jpeg::to(const std::string& filename, std::unique_ptr<seedimg::img>& image, uint8_t quality, bool progressive) noexcept {

	auto output = std::fopen(filename.c_str(), "wb");
	if (output == NULL) return false;

	jpeg_compress_struct jenc;
	seedimg_jpeg_error_mgr jerr;
	JSAMPROW rowbuffer = NULL;
	int errcode = 0;

	jenc.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = jpegErrorExit;

	if (setjmp(jerr.setjmp_buffer)) {
		std::cerr << jpegLastErrorMsg << std::endl;
		errcode = -1;
		goto finalise;
	}

	jpeg_create_compress(&jenc);
	jpeg_stdio_dest(&jenc, output);

	jenc.image_width = image->width;
	jenc.image_height = image->height;
	jenc.input_components = 3;
	jenc.in_color_space = JCS_RGB;

	jpeg_set_defaults(&jenc);
	jpeg_set_quality(&jenc, quality, TRUE);
	if (progressive) jpeg_simple_progression(&jenc);
	jpeg_start_compress(&jenc, TRUE);

	 rowbuffer = new JSAMPLE[jenc.image_width * jenc.input_components];

	for (std::size_t y = 0; y < image->height; ++y) {
		for (std::size_t x = 0; x < image->width; ++x) {
#define P(ch) rowbuffer[3*x + ch]
			auto pix = image->get_pixel(x, y);
			P(0) = pix.r;
			P(1) = pix.g;
			P(2) = pix.b;
		}
		if (jpeg_write_scanlines(&jenc, &rowbuffer, 1) != 1)
			return false;
	}

finalise:
	jpeg_finish_compress(&jenc);
	jpeg_destroy_compress(&jenc);
	std::fclose(output);
	
	if(rowbuffer!=NULL)
		delete[] rowbuffer;
	return errcode == 0;
}
