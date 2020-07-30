#include <iostream>

#include "jpeg-test.hpp"
#include "png-test.hpp"
#include "webp-test.hpp"

#include <seedimg/seedimg.hpp>

int main() {
	{
		//test_png_module();
		//test_jpeg_module();
		test_webp_module();
	}
	{
		auto a = seedimg::modules::png::from("violeur.png");
		if (a != std::nullopt) {
			bool b = seedimg::modules::jpeg::to("bioleur.jpg", *a, 1);
		}
		std::cout << "done";
	}
}