#include <filesystem>
#include <iostream>

#include <seedimg-autodetect/seedimg-autodetect.hpp>
#include <seedimg-filters-core/seedimg-filters-core.hpp>
#include <seedimg/seedimg.hpp>

int main() {
  std::cout << "Current path is " << std::filesystem::current_path()
            << std::endl;
  {
    auto a = seedimg_autodetect_from("violeur.png");
    if (a != nullptr) {
      // a->crop({50, 20}, {250, 183});
      seedimg::filters::grayscale(a, true);
      seedimg::filters::invert(a);
      bool b = seedimg_autodetect_to("bioleur.jpg", a);
    }
    std::cout << "done" << std::endl;
  }
}
