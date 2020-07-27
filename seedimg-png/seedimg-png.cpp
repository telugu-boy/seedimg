// seedimg-png.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"

/*#include <iostream>

#include <libpng16/png.h>

#include "../seedimg/seedimg.hpp"
#include <optional>

#pragma warning(disable:4996)

std::optional<std::unique_ptr<seedimg::img> > seedimg::modules::png::from(std::string filename) {

	auto res_img = std::make_unique<seedimg::img>();
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytepp row_pointers = NULL;
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
		std::cerr << "Error during PNG creation for " << filename << std::endl;
		errcode = -1;
		goto finalise;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);

	if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA) {
		std::cerr << "File " << filename << " must be PNG_COLOR_TYPE_RGBA" << std::endl;
		errcode = -1;
		goto finalise;
	}

	res_img->width = png_get_image_width(png_ptr, info_ptr);
	res_img->height = png_get_image_height(png_ptr, info_ptr);
	res_img->data.resize(res_img->height);

	png_set_palette_to_rgb(png_ptr);
	png_set_expand_16(png_ptr);

	row_pointers = (png_bytepp)png_malloc(png_ptr, sizeof(png_bytepp) * res_img->width);
	for (int i = 0; i < res_img->height; i++) {
		row_pointers[i] = (png_bytep)png_malloc(png_ptr, res_img->width * 8);
	}
	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_read_image(png_ptr, row_pointers);

	//This will load the png into the vectors.
	for (size_t y = 0; y < res_img->height; y++) {
		png_bytep row = row_pointers[y];
		res_img->data[y].reserve(res_img->width);
		for (size_t x = 0; x < res_img->width; x++) {
			png_byte* ptr = &(row[x * 4]);
			//as we've reserved space, this doesnt do any reallocation also its faster.
			res_img->data[y].emplace_back(ptr[0], ptr[1], ptr[2], ptr[3]);
		}
	}

finalise:
	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_read_struct(&png_ptr, NULL, NULL);

	if (errcode != 0) {
		return std::nullopt;
	}
	else {
		return res_img;
	}
}
*/