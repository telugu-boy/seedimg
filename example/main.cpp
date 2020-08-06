#include <iostream>

#include <seedimg-filters-core/seedimg-filters-core.hpp>
#include <seedimg-autodetect/seedimg-autodetect.hpp>

int main() {
	{
		auto a = seedimg_autodetect_from("violeur.png");
		if (a != std::nullopt) {
			seedimg::filters::grayscale(*a, false);
			bool b = seedimg_autodetect_to("boileur.jpg", *a);
		}
		std::cout << "done";
	}
}