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
		std::string name = "violeur.png";
		auto a = seedimg_autodetect_from(name);
		seedimg::filters::grayscale(*a, false);
		if (a != std::nullopt) {
			bool b = seedimg_autodetect_to("boileur.jpg", *a);
		}
		std::cout << "done";
	}
}