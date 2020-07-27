// seedimg.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "seedimg.hpp"

std::optional<std::unique_ptr<seedimg::img> > seedimg::from(std::string filename) {
	std::ifstream infile(filename, std::ios::binary);

	if (infile.fail()){
		std::cerr << "File " << filename << " could not be opened" << std::endl;
		return std::nullopt;
	}

	uint32_t w, h;
	infile.read(reinterpret_cast<char*>(&w), sizeof(w));
	infile.read(reinterpret_cast<char*>(&h), sizeof(h));

	//checks if valid file
	std::error_code ec{};
	auto size = std::filesystem::file_size(filename, ec);
	if (ec != std::error_code{})
		return std::nullopt;
	if (size != 4 * w * h + sizeof(w) + sizeof(h)) {
		std::cerr << filename << " is not a valid SEEDIMG file" << std::endl;
		return std::nullopt;
	}

	auto res_img = std::make_unique<seedimg::img>(w, h);
	res_img->data.resize(h);
	for (size_t y = 0; y < h; y++) {
		res_img->data[y].resize(w);
		for (size_t x = 0; x < w; x++) {
			seedimg::pixel& pix = res_img->data[y][x];
			infile >> pix.r >> pix.g >> pix.b >> pix.a;
		}
	}
	return res_img;
}

bool seedimg::img::to(std::string filename) {
	std::ofstream outfile(filename, std::ios::binary);
	if (outfile.fail())
		return false;

	outfile.write(reinterpret_cast<const char*>(&width_), sizeof(width_));
	outfile.write(reinterpret_cast<const char*>(&height_), sizeof(height_));
	for (auto row : data) {
		for (auto pix : row) {
			outfile << pix.r << pix.g << pix.b << pix.a;
		}
	}
	outfile.close();
	return true;
}