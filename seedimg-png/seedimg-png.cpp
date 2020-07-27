// seedimg-png.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"

#include <iostream>

#include <libpng16/png.h>

#include "seedimg-png.hpp"

#pragma warning(disable:4996)

std::optional<std::unique_ptr<seedimg::img> > seedimg::modules::png::from(std::string filename) {

	std::unique_ptr<seedimg::img> res_img = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytepp row_pointers = NULL;
	// chosen 127 as 0 is already taken as a type.
	uint8_t color_type = 127;
	uint8_t bit_depth = 0;

	int errcode = 0;

	FILE* fp = fopen(filename.c_str(), "rb");

	if (!fp) {
		std::cerr << "File " << filename << " could not be opened" << std::endl;
		errcode = -1;
		return std::nullopt;
	}

	uint8_t sig[8];

	fread(sig, sizeof(uint8_t), 8, fp);
	if (!png_check_sig(sig, 8)) {
		std::cerr << filename << " is not a valid PNG file" << std::endl;
		errcode = -1;
		goto finalise;
	}

	//validation done: initialize info structs.

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
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

	//set jmp for errors, finalise is just for cleanup

	if (setjmp(png_jmpbuf(png_ptr))) {
		std::cerr << "Error during PNG processing for " << filename << std::endl;
		errcode = -1;
		goto finalise;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);

	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	res_img = std::make_unique<seedimg::img>(
		png_get_image_width(png_ptr, info_ptr),
		png_get_image_height(png_ptr, info_ptr)
		);
	res_img->data.resize(res_img->height());

	if (bit_depth == 16)
		png_set_strip_16(png_ptr);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	if (color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	row_pointers = (png_bytepp)malloc(sizeof(png_bytep) * res_img->width());
	for (size_t i = 0; i < res_img->height(); i++) {
		row_pointers[i] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
	}
	png_read_image(png_ptr, row_pointers);

	//This will load the png into the vectors.
	for (size_t y = 0; y < res_img->height(); y++) {
		png_bytep row = row_pointers[y];
		res_img->data[y].resize(res_img->width());
		for (size_t x = 0; x < res_img->width(); x++) {
			png_byte* pix = &(row[x * 4]);
			res_img->data[y][x] = { pix[0], pix[1], pix[2], pix[3] };
		}
	}

finalise:
	if (fp != NULL)
		fclose(fp);
	if (info_ptr != NULL)
		png_destroy_info_struct(png_ptr, &info_ptr);
	if (png_ptr != NULL)
		png_destroy_read_struct(&png_ptr, NULL, NULL);
	if (row_pointers != NULL) {
		for (size_t y = 0; y < res_img->height(); y++)
			free(row_pointers[y]);
		free(row_pointers);
	}

	if (errcode != 0 || res_img == NULL) {
		return std::nullopt;
	}
	else {
		return res_img;
	}
}

bool seedimg::modules::png::to(std::string filename, std::unique_ptr<seedimg::img>& inp_img) {

	if (inp_img->data.size() != inp_img->height() && inp_img->data[0].size() != inp_img->width()) {
		std::cerr << "Input seedimg dimensions do not match" << std::endl;
		return false;
	}

	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytepp row_pointers = NULL;

	int errcode = 0;

	FILE* fp = fopen(filename.c_str(), "wb");

	if (!fp) {
		std::cerr << "File " << filename << " could not be opened" << std::endl;
		errcode = -1;
		return false;
	}

	//validation done: initialize info structs.

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
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

	//set jmp for errors, finalise is just for cleanup
	if (setjmp(png_jmpbuf(png_ptr))) {
		std::cerr << "Error during PNG processing for " << filename << std::endl;
		errcode = -1;
		goto finalise;
	}

	png_init_io(png_ptr, fp);

	// Output is 8bit depth, RGBA format.
	png_set_IHDR(
		png_ptr,
		info_ptr,
		inp_img->width(), inp_img->height(),
		8,
		PNG_COLOR_TYPE_RGBA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);
	png_write_info(png_ptr, info_ptr);

	row_pointers = (png_bytepp)malloc(sizeof(png_bytep) * inp_img->width());

	for (size_t y = 0; y < inp_img->height(); y++) {
		png_bytep row = row_pointers[y] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
		for (size_t x = 0; x < inp_img->width(); x++) {
			png_byte* pix = &(row[x * 4]);
			pix[0] = inp_img->data[y][x].r;
			pix[1] = inp_img->data[y][x].g;
			pix[2] = inp_img->data[y][x].b;
			pix[3] = inp_img->data[y][x].a;
		}
	}

	png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, NULL);

finalise:
	if (fp != NULL)
		fclose(fp);
	if (info_ptr != NULL)
		png_destroy_info_struct(png_ptr, &info_ptr);
	if (png_ptr != NULL)
		png_destroy_write_struct(&png_ptr, NULL);
	if (row_pointers != NULL) {
		for (size_t y = 0; y < inp_img->height(); y++)
			free(row_pointers[y]);
		free(row_pointers);
	}
	return errcode == 0;
}