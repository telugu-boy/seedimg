// seedimg.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "seedimg.hpp"

std::optional<std::unique_ptr<seedimg::img> > seedimg::from(std::string filename) noexcept {
	std::ifstream infile(filename, std::ios::binary);

	if (infile.fail()){
		std::cerr << "File " << filename << " could not be opened" << std::endl;
		return std::nullopt;
	}

	std::size_t w, h;
	infile.read(reinterpret_cast<char*>(&w), sizeof(w));
	infile.read(reinterpret_cast<char*>(&h), sizeof(h));

	//checks if valid file
	std::error_code ec{};
	auto size = std::filesystem::file_size(filename, ec);
	if (ec != std::error_code{})
		return std::nullopt;
	// stupid compiler warnings '-'
	if (size != 4 * w * unsigned long long(h) + unsigned long long(sizeof(w) + sizeof(h))) {
		std::cerr << filename << " is not a valid SEEDIMG file" << std::endl;
		return std::nullopt;
	}

	auto res_img = std::make_unique<seedimg::img>(w, h);
	for (std::size_t y = 0; y < h; y++) {
		infile.read(reinterpret_cast<char*>(res_img->get_row(y).data()), sizeof(seedimg::pixel) * w);
	}
	return res_img;
}

bool seedimg::to(std::string filename, std::unique_ptr<seedimg::img>& inp_img) noexcept {
	std::ofstream outfile(filename, std::ios::binary);
	if (outfile.fail())
		return false;
	uint32_t w = inp_img->width, h = inp_img->height;
	outfile.write(reinterpret_cast<const char*>(&w), sizeof(w));
	outfile.write(reinterpret_cast<const char*>(&h), sizeof(h));
	auto data = inp_img->get_data();
	for (std::size_t y = 0; y < inp_img->height; y++) {
		outfile.write(reinterpret_cast<const char*>(inp_img->get_row(y).data()), std::size_t(sizeof(seedimg::pixel)) * inp_img->width);
	}
	outfile.close();
	return true;
}