#pragma once
#include "../seedimg-jpeg/seedimg-jpeg.hpp"

void test_jpeg_module() {
	auto img = seedimg::modules::jpeg::from("violeur.jpg");
	if (img != std::nullopt) {
		seedimg::modules::jpeg::to("violeur_enc.jpg", *img);
	}
	// IJG quantization table scale factor: 8
	// Web friendly progressive JPEG:       yes
}