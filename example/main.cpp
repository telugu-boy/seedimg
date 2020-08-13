#include <filesystem>
#include <iostream>

#include <seedimg-autodetect/seedimg-autodetect.hpp>
#include <seedimg-filters-core/seedimg-filters-core.hpp>
#include <seedimg-jpeg/seedimg-jpeg.hpp>
#include <seedimg/seedimg.hpp>

int main() {
  std::cerr << "Current path is " << std::filesystem::current_path()
            << std::endl;
  {
    auto a = seedimg_autodetect_from("cat.png");
    if (a != nullptr) {
      // a->crop({32, 115}, {315, 472});
      // seedimg::filters::grayscale(a, true);
      // seedimg::filters::invert(a);
      seedimg::filters::blur(a, 10);
      // seedimg::filters::h_blur(a, 10);
      // seedimg::filters::v_blur(a, 10);
      bool b = seedimg_autodetect_to("biol.jpg", a);
      // bool b = seedimg::modules::jpeg::to("biol.jpg", a, 1);
    } else {
      std::cerr << "failed" << std::endl;
    }
    std::cerr << "done" << std::endl;
  }
}
