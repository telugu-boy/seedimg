#pragma once
#include "../seedimg-webp/seedimg-webp.hpp"

void test_webp_module() {
	auto a = seedimg::modules::webp::from("tripulse.webp");
	if (a != std::nullopt) {
		bool b = seedimg::modules::webp::to("tribulse.webp", *a);
	}
}