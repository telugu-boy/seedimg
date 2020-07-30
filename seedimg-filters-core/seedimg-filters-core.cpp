// seedimg-filters-core.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"

#include "seedimg-filters-core.hpp"

// TODO: This is an example of a library function
void seedimg::filters::grayscale(std::unique_ptr<seedimg::img>& inp_img) noexcept {
	for (auto& row : inp_img->get_data()) {
		for (auto& pix : row) {
			uint8_t avg = (pix.r + pix.g + pix.b) / 3;
			pix = { avg, avg, avg, pix.a };
		}
	}
}
