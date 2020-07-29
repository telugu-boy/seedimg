#include "jpeg-test.hpp"
#include "png-test.hpp"
#include <iostream>

int main() {
	{
		test_png_module();
		test_jpeg_module();
	}
	std::cout << "done";
}