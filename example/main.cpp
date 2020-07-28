#include <seedimg-png/seedimg-png.hpp>
#include <iostream>

int main() {
	auto a = seedimg::modules::png::from("violeur.png");
	//bool b = seedimg::modules::png::to("seedviol.png", *a);
	if (a != std::nullopt) {
		seedimg::to("rawviol.seedimg", *a);
		auto seedimage = seedimg::from("rawviol.seedimg");
		if (seedimage != std::nullopt) {
			seedimg::modules::png::to("seedviol.png", *seedimage);
		}
		// bool success = seedimg::modules::png::to("seedviol.png", a.value());
		std::cout << (*a)->height << ' ' << (*a)->width << std::endl;
	}
}