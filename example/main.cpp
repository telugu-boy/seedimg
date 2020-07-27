#include <seedimg-png/seedimg-png.hpp>
#include <iostream>

int main() {
	auto a(seedimg::modules::png::from("violeur.png"));
	a.reset();
	//std::cout << a.value()->height << ' ' << a.value()->width << std::endl;
	int fgd = 3;
	std::cout << fgd;
}