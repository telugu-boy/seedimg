// seedimg.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "seedimg.hpp"


std::optional<std::unique_ptr<seedimg::img>> seedimg::from(std::string filename) {
	std::ifstream infile(filename, std::ios::binary);
	if (infile.fail()) return {};

	uint32_t width, height;
	infile.read(reinterpret_cast<char*>(&width), sizeof(width));
	infile.read(reinterpret_cast<char*>(&height), sizeof(height));

	std::error_code ec;
	auto size = std::filesystem::file_size(filename, ec);

	if (ec != std::error_code{} || size != 4 * width * height + sizeof(width) + sizeof(height))
		return {};

	seedimg::img image(width, height);
	for (auto& row : image.data)
		for (auto& pixel : row)
			infile >> pixel.r >> pixel.g >> pixel.b >> pixel.a;

	infile.close();
	return std::make_unique<seedimg::img>(image);
}

bool seedimg::to(std::string filename, std::unique_ptr<seedimg::img> image) {
	std::ofstream outfile(filename, std::ios::binary);
	if (outfile.fail())
		return false;

	outfile.write(reinterpret_cast<const char*>(image->width()), sizeof(image->width()));
	outfile.write(reinterpret_cast<const char*>(image->height()), sizeof(image->height()));

	for (auto row : image->data)
		for (auto pix : row)
			outfile << pix.r << pix.g << pix.b << pix.a;

	outfile.close();
	return true;
}