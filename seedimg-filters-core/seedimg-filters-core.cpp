// seedimg-filters-core.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"

#include "seedimg-filters-core.hpp"

// TODO: This is an example of a library function
void seedimg::filters::grayscale(std::unique_ptr<seedimg::img>& inp_img, bool lightness) noexcept {
	// would rather check once if it's in lightness mode than width*height times.
	if (lightness) {
		for (auto& row : inp_img->get_data()) {
			for (auto& pix : row) {
				pix = {
					static_cast<uint8_t>(0.299 * pix.r),
					static_cast<uint8_t>(0.587 * pix.g),
					static_cast<uint8_t>(0.114 * pix.b),
					pix.a
				};
			}
		}
	}
	else {
		for (auto& row : inp_img->get_data()) {
			for (auto& pix : row) {
				uint8_t avg = (pix.r + pix.g + pix.b) / 3;
				pix = { avg, avg, avg, pix.a };
			}
		}
	}
}
