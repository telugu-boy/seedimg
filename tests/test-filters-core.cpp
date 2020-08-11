#include <cstdio>
#include <filesystem>
#include <functional>
#include <iostream>
#include <unordered_map>

#include <seedimg-autodetect/seedimg-autodetect.hpp>
#include <seedimg-filters-core/seedimg-filters-core.hpp>

enum class filter_functions {
  GRAYSCALE_LUM,
  GRAYSCALE_AVG,
  INVERT,
  INVERT_A,
  INVERT_AO,
  CROP
};
static const std::unordered_map<std::string, filter_functions> filter_mapping =
    {{"grayscale_lum", filter_functions::GRAYSCALE_LUM},
     {"grayscale_avg", filter_functions::GRAYSCALE_AVG},
     {"invert", filter_functions::INVERT},
     {"invert_alpha", filter_functions::INVERT_A},
     {"invert_alpha_only", filter_functions::INVERT_AO},
     {"crop", filter_functions::CROP}};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "FAIL";
    exit(1);
  }
  std::cout << argv[1] << std::endl;
  std::string res_dir = "tests_output/filters/";
  std::filesystem::create_directories(res_dir + "/png/");
  std::filesystem::create_directories(res_dir + "/jpg/");
  std::filesystem::create_directories(res_dir + "/webp/");
  auto img = seedimg_autodetect_from("test_image.png");
  switch (filter_mapping.at(argv[1])) {
  case filter_functions::GRAYSCALE_LUM:
    seedimg::filters::grayscale(img, true);
    break;
  case filter_functions::GRAYSCALE_AVG:
    seedimg::filters::grayscale(img, false);
    break;
  case filter_functions::INVERT:
    seedimg::filters::invert(img);
    break;
  case filter_functions::INVERT_A:
    seedimg::filters::invert_a(img, false);
    break;
  case filter_functions::INVERT_AO:
    seedimg::filters::invert_a(img, true);
    break;
  case filter_functions::CROP:
    img->crop({img->width() / 3, img->height() / 3},
              {2 * img->width() / 3, 2 * img->height() / 3});
    break;
  }
  char name_buf[256];
  std::snprintf(name_buf, 256, "tests_output/filters/png/result-%s.png",
                argv[1]);
  seedimg_autodetect_to(name_buf, img);
  std::snprintf(name_buf, 256, "tests_output/filters/jpg/result-%s.jpg",
                argv[1]);
  seedimg_autodetect_to(name_buf, img);
  std::snprintf(name_buf, 256, "tests_output/filters/webp/result-%s.webp",
                argv[1]);
  seedimg_autodetect_to(name_buf, img);
  std::cout << "SUCCESS";
}
