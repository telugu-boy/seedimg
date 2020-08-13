#include <filesystem>
#include <iostream>

#include <seedimg-autodetect/seedimg-autodetect.hpp>
#include <seedimg-filters-core/seedimg-filters-core.hpp>
#include <seedimg/seedimg.hpp>

int main() {
  std::cout << "current path is " << std::filesystem::current_path()
            << std::endl;

  auto a = seedimg_autodetect_from("cat.png");
  if (a != nullptr) {
    // a->crop({100, 100}, {250, 250});
    // seedimg::filters::grayscale(a, true);
    // seedimg::filters::invert(a);

    // applies a approixmate gaussian blurring matrix retrieved from:
    // https://en.wikipedia.org/wiki/Kernel_(image_processing)#Details
    seedimg::filters::convolution(a, {{  0.0625, 0.125, 0.0625 },
                                      {  0.125,  0.25,  0.125  },
                                      {  0.0625, 0.125, 0.0625 }});
    seedimg_autodetect_to("boileur_gblur.png", a);
  } else
    std::cout << "failed" << std::endl;
  std::cout << "done" << std::endl;
}
