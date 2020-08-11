#include <filesystem>
#include <iostream>

#include <seedimg-autodetect/seedimg-autodetect.hpp>
#include <seedimg-filters-core/seedimg-filters-core.hpp>
#include <seedimg/seedimg.hpp>

int main() {
  std::cout << "Current path is " << std::filesystem::current_path()
            << std::endl;
  {
    auto a = seedimg_autodetect_from("cat.png");
    if (a != nullptr) {
      // bool f = a->crop({100, 100}, {250, 250});
      seedimg::filters::grayscale(a, true);
      // seedimg::filters::invert(a);
      bool b = seedimg_autodetect_to("boileur.jpg", a);
    }
    std::cout << "done" << std::endl;
  }
}
