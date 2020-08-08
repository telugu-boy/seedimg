#include <filesystem>
#include <iostream>

#include <seedimg-autodetect/seedimg-autodetect.hpp>
#include <seedimg-filters-core/seedimg-filters-core.hpp>

int main() {
  std::cout << "Current path is " << std::filesystem::current_path()
            << std::endl;
  {
    auto a = seedimg_autodetect_from("violeur-int.png");
    if (a != std::nullopt) {
      seedimg::filters::grayscale(*a, true);
      seedimg::filters::invert(*a);
      seedimg::filters::crop(*a, {50, 20}, {250, 183});
      bool b = seedimg_autodetect_to("bioleur.jpg", *a);
    }
    std::cout << "done" << std::endl;
  }
}
