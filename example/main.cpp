#include "jpeg-test.hpp"
#include "png-test.hpp"
#include <iostream>

int main() {
	{
		//test_png_module();
		//test_jpeg_module();
	}
	auto pngimg = seedimg::modules::png::from("violeur.png");
	bool convjpg = seedimg::modules::jpeg::to("biolejpg.jpg", *pngimg, 1);
	auto jpegimg = seedimg::modules::jpeg::from("biolejpg.jpg");
	bool convpng = seedimg::modules::png::to("violfromjpg.png", *jpegimg);
	std::cout << "done";
}