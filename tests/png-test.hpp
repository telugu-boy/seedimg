#pragma once
#include <seedimg-png/seedimg-png.hpp>

void test_png_module() {
	auto a = seedimg::modules::png::from("violeur.png");
	if (a != std::nullopt) {
		bool b = seedimg::modules::png::to("seedviol.png", *a);
	}
}