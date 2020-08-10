#include <cstdio>
#include <filesystem>
#include <functional>
#include <iostream>
#include <unordered_map>

#include <seedimg-autodetect/seedimg-autodetect.hpp>
#include <seedimg-filters-core/seedimg-filters-core.hpp>

enum class filter_functions { GRAYSCALE, INVERT, CROP };
static const std::unordered_map<std::string, filter_functions> filter_mapping =
    {{"grayscale", filter_functions::GRAYSCALE},
     {"invert", filter_functions::INVERT},
     {"crop", filter_functions::CROP}};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "FAIL";
    exit(1);
  }
  std::cout << argv[1] << std::endl;
  std::string res_dir = "tests_output/filters/";
  std::filesystem::create_directories(res_dir);
  auto img = seedimg_autodetect_from("violeur.png");
  switch (filter_mapping.at(argv[1])) {
  case filter_functions::GRAYSCALE:
    seedimg::filters::grayscale(*img);
    break;
  case filter_functions::INVERT:
    seedimg::filters::invert(*img);
    break;
  case filter_functions::CROP:
    img->crop({50, 25}, {250, 183});
    break;
  }
  char name_buf[256];
  std::snprintf(name_buf, 256, "tests_output/filters/result-%s.png", argv[1]);
  seedimg_autodetect_to(name_buf, *img);
  std::cout << "SUCCESS";
}
