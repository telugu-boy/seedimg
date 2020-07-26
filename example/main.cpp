#include <seedimg-png/seedimg-png.hpp>
#include <iostream>

int main() {
	{
		for (int i = 0; i < 10; i++) {
			auto a(seedimg::modules::png::from("violeur.png"));
			std::cout << a.value()->height << ' ' << a.value()->width << std::endl;
		}
	}
	int fgd = 3;
	std::cout << fgd;
}