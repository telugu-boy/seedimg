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

	std::size_t width, height;
	infile.read(reinterpret_cast<char*>(&width), sizeof(width));
	infile.read(reinterpret_cast<char*>(&height), sizeof(height));

	std::error_code ec;
	auto size = std::filesystem::file_size(filename, ec);

	// check if the data size is equal to the retrieved rectangular area.
	// if not, then just don't return any results, partial data isn't supported.
	if (ec != std::error_code{} || size != 4 * width * height + sizeof(width) + sizeof(height))
		return {};

	auto image = std::make_unique<seedimg::img>(width, height);
	const auto stride = sizeof(seedimg::pixel) * (size_t)image->width();

	for (std::size_t y = 0; y < image->height(); ++y)
		infile.read(reinterpret_cast<char*>(image->get(y).data()), stride);

	infile.close();
	return image;
}

bool seedimg::to(std::string filename, std::unique_ptr<seedimg::img>& image) {
	std::ofstream outfile(filename, std::ios::binary);
	if (outfile.fail())
		return false;

	outfile.write(reinterpret_cast<const char*>(image->width()), sizeof(image->width()));
	outfile.write(reinterpret_cast<const char*>(image->height()), sizeof(image->height()));

	const auto stride = sizeof(seedimg::pixel) * image->width();

	for (std::size_t y = 0; y < image->height(); ++y)
		outfile.write(reinterpret_cast<const char*>(image->get(y).data()), stride);

	outfile.close();
	return true;
}
