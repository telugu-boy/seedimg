#include <seedimg-png/seedimg-png.hpp>
#include <iostream>

int main() {
	auto a = seedimg::modules::png::from("violeur.png");
	if (a != std::nullopt) {
		a.value()->to("rawviol.seedimg");
		auto seedimage = seedimg::from("rawviol.seedimg");
		if (seedimage != std::nullopt) {
			for (int i = 0; i < a.value()->height(); i++) {
				for (int j = 0; j < a.value()->width(); j++) {
					if ((*seedimage)->data[j][i] == (*a)->data[j][i]) {
						std::cout << j << ' ' << i << std::endl;
					}
				}
			}
			seedimg::modules::png::to("seedviol.png", seedimage.value());
		}
		bool success = seedimg::modules::png::to("seedviol.png", a.value());
		std::cout << a.value()->height() << ' ' << a.value()->width() << std::endl;
	}
}