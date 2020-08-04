#include <iostream>

#include "jpeg-test.hpp"
#include "png-test.hpp"
#include "webp-test.hpp"

#include <seedimg-filters-core/seedimg-filters-core.hpp>

#include "seedimg-autodetect.hpp"

int main() {
	{
		//test_png_module();
		//test_jpeg_module();
		//test_webp_module();
	}
	{
		auto a = seedimg_autodetect_from("violeur.png");
		if (a != std::nullopt) {
			seedimg::filters::grayscale(*a, true);
			bool b = seedimg_autodetect_to("boileur.jpg", *a);
		}
		std::cout << "done";
	}
}