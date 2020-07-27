#include <seedimg-png/seedimg-png.hpp>
#include <iostream>

int main() {
	auto a = seedimg::modules::png::from("violeur.jpg");
	if (a != std::nullopt) {
		bool success = seedimg::modules::png::to("seedviol.png", a.value());
		std::cout << a.value()->height() << ' ' << a.value()->width() << std::endl;
	}
}