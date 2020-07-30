// seedimg-webp.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"

#include <filesystem>
#include <fstream>

extern "C" {
#include <webp/encode.h>
#include <webp/decode.h>
}

#include "seedimg-webp.hpp"

// TODO: This is an example of a library function
bool seedimg::modules::webp::to(std::string filename, std::unique_ptr<seedimg::img>& inp_img, uint8_t quality) noexcept {
	uint8_t* output = NULL;
	uint8_t* data = new uint8_t[inp_img->height * inp_img->width * sizeof(seedimg::pixel)];
	for (size_t y = 0; y < inp_img->height; y++) {
		std::memcpy(data + y * inp_img->width * sizeof(seedimg::pixel), inp_img->get_row(y).data(), inp_img->width * sizeof(seedimg::pixel));
	}
	// this is the amount of bytes output has been allocated, 0 if failure '''''-'''''
	auto success = WebPEncodeRGBA(data, inp_img->width, inp_img->height, inp_img->width * sizeof(seedimg::pixel), quality, &output);
	delete[] data; data = NULL;
	if (success == 0) return false;
	std::ofstream file(filename, std::ios::binary);
	file.write(reinterpret_cast<char*>(output), success);
	WebPFree(output);
	return true;
}
std::optional<std::unique_ptr<seedimg::img> > seedimg::modules::webp::from(std::string filename) noexcept {
	std::error_code ec;
	size_t size = std::filesystem::file_size(filename, ec);
	if (ec != std::error_code{})
		return std::nullopt;
	int width, height;
	auto data = std::make_unique<uint8_t[]>(size);

	// read into data
	std::ifstream file(filename, std::ios::binary);
	if (!file.read(reinterpret_cast<char*>(data.get()), size)) return std::nullopt;

	int success = WebPGetInfo(data.get(), size, &width, &height);
	if (!success) return std::nullopt;
	auto res_img = std::make_unique<seedimg::img>(width, height);
	uint8_t* inp_img = WebPDecodeRGBA(data.get(), size, &width, &height);
	for (size_t y = 0; y < height; y++) {
		std::memcpy(res_img->get_row(y).data(), inp_img + y * width * sizeof(seedimg::pixel), width * sizeof(seedimg::pixel));
	}
	WebPFree(inp_img);
	return res_img;
}
