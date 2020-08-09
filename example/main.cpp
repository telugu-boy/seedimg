#include <filesystem>
#include <iostream>

#include <seedimg-autodetect/seedimg-autodetect.hpp>
#include <seedimg-filters-core/seedimg-filters-core.hpp>

#include <seedimg-format-farbfeld/farbfeld.hxx>

int main() {
  std::cout << "Current path is " << std::filesystem::current_path()
            << std::endl;
  {
    auto a = seedimg_autodetect_from("violeur.png");
    // auto a = seedimg::modules::farbfeld::from("boileur.ff");
    if (a != std::nullopt) {
      // seedimg::filters::grayscale(*a, true);
      // seedimg::filters::invert(*a);
      // seedimg::filters::crop(*a, {50, 20}, {250, 183});
      // seedimg_autodetect_to("bioleur.jpg", *a);

      // seedimg::modules::farbfeld::to("boileur_1.ff", *a);
      seedimg::modules::farbfeld::to("boileur.ff", *a);
    }
    std::cout << "done" << std::endl;
  }
}
