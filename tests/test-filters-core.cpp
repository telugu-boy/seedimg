﻿/***********************************************************************
    seedimg - module based image manipulation library written in modern C++
    Copyright (C) 2020 telugu-boy

    This program is free software : you can redistribute it and /
    or modify it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation,
    either version 3 of the License,
    or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/
#include <cstdio>
#include <filesystem>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

#ifdef SEEDIMG_TESTS_OCL
#include <seedimg-filters/seedimg-filters-ocl.hpp>
#endif
#include <seedimg-autodetect.hpp>
#include <seedimg-filters/seedimg-filters-core.hpp>
#ifdef SEEDIMG_TESTS_OCL
#include <seedimg-filters/seedimg-filters-ocl.hpp>
#endif

enum class filter_functions {
  GRAYSCALE_LUM,
  GRAYSCALE_AVG,
  INVERT,
  INVERT_A,
  INVERT_AO,
  CROP,
  BLUR,
  H_BLUR,
  V_BLUR,
  KERNEL_CONVOLUTION,
  ROTATE_HUE,
  BRIGHTNESS,
  BRIGHTNESS_A,
  BLEND,
  BLEND_A,
  ROTATE_CW,
  ROTATE_180,
  ROTATE_CCW,
  V_MIRROR,
  H_MIRROR,
  SATURATION_HSV,
  SATURATION_RGB,
  SEPIA,
  DIFF,
#ifdef SEEDIMG_FILTERS_OCL_H
  ROTATE_HUE_OCL,
  GRAYSCALE_LUM_OCL,
  SEPIA_OCL,
  SATURATION_HSV_OCL,
  SATURATION_RGB_OCL,
  CONTRAST_OCL,
  BRIGHTNESS_OCL,
  BRIGHTNESS_A_OCL,
#endif
};

static const std::unordered_map<std::string, filter_functions> filter_mapping =
    {
        {"grayscale_lum", filter_functions::GRAYSCALE_LUM},
        {"grayscale_avg", filter_functions::GRAYSCALE_AVG},
        {"invert", filter_functions::INVERT},
        {"invert_alpha", filter_functions::INVERT_A},
        {"invert_alpha_only", filter_functions::INVERT_AO},
        {"crop", filter_functions::CROP},
        {"blur", filter_functions::BLUR},
        {"h_blur", filter_functions::H_BLUR},
        {"v_blur", filter_functions::V_BLUR},
        {"kernel_convolution", filter_functions::KERNEL_CONVOLUTION},
        {"rotate_hue", filter_functions::ROTATE_HUE},
        {"brightness", filter_functions::BRIGHTNESS},
        {"brightness_alpha", filter_functions::BRIGHTNESS_A},
        {"blend", filter_functions::BLEND},
        {"blend_alpha", filter_functions::BLEND_A},
        {"rotate_cw", filter_functions::ROTATE_CW},
        {"rotate_180", filter_functions::ROTATE_180},
        {"rotate_ccw", filter_functions::ROTATE_CCW},
        {"v_mirror", filter_functions::V_MIRROR},
        {"h_mirror", filter_functions::H_MIRROR},
        {"saturation_hsv", filter_functions::SATURATION_HSV},
        {"saturation_rgb", filter_functions::SATURATION_RGB},
        {"sepia", filter_functions::SEPIA},
        {"diff", filter_functions::DIFF},
#ifdef SEEDIMG_FILTERS_OCL_H
        {"rotate_hue_ocl", filter_functions::ROTATE_HUE_OCL},
        {"grayscale_lum_ocl", filter_functions::GRAYSCALE_LUM_OCL},
        {"sepia_ocl", filter_functions::SEPIA_OCL},
        {"saturation_hsv_ocl", filter_functions::SATURATION_HSV_OCL},
        {"saturation_rgb_ocl", filter_functions::SATURATION_RGB_OCL},
        {"contrast_ocl", filter_functions::CONTRAST_OCL},
        {"brightness_ocl", filter_functions::BRIGHTNESS_OCL},
        {"brightness_a_ocl", filter_functions::BRIGHTNESS_A_OCL},
#endif
};

int main(int, char *argv[]) {
  std::cout << argv[1] << std::endl;
  std::string res_dir = "tests_output/filters/";

  auto img = seedimg::load("test_image.jpg");
  filter_functions filter;
  try {
    filter = filter_mapping.at(argv[1]);
  } catch (const std::exception &) {
    std::cout << "Test not found (is OpenCL disabled?)";
    exit(0);
  }
  switch (filter) {
  case filter_functions::GRAYSCALE_LUM:
    seedimg::filters::grayscale_i(img, true);
    break;
  case filter_functions::GRAYSCALE_AVG:
    seedimg::filters::grayscale_i(img, false);
    break;
  case filter_functions::INVERT:
    seedimg::filters::invert_i(img);
    break;
  case filter_functions::INVERT_A:
    seedimg::filters::invert_a_i(img, false);
    break;
  case filter_functions::INVERT_AO:
    seedimg::filters::invert_a_i(img, true);
    break;
  case filter_functions::CROP:
    seedimg::filters::crop_i(img, {img->width() / 3, img->height() / 3},
                             {2 * img->width() / 3, 2 * img->height() / 3});
    break;
  case filter_functions::BLUR:
    seedimg::filters::blur_i(img, 10);
    break;
  case filter_functions::H_BLUR:
    seedimg::filters::h_blur_i(img, 10);
    break;
  case filter_functions::V_BLUR:
    seedimg::filters::v_blur_i(img, 10);
    break;
  // applies a 3x3 gaussian blur kernel.
  case filter_functions::KERNEL_CONVOLUTION:
    seedimg::filters::convolution(img, {{0.0625, 0.125, 0.0625},
                                        {0.125, 0.25, 0.125},
                                        {0.0625, 0.125, 0.0625}});
    break;
  case filter_functions::ROTATE_HUE:
    seedimg::filters::rotate_hue_i(img, 180);
    break;
  case filter_functions::BRIGHTNESS:
    // values in the second argument are percentages.
    seedimg::filters::brightness_i(img, 20);
    break;
  case filter_functions::BRIGHTNESS_A:
    seedimg::filters::brightness_a_i(img, 40);
    break;
  case filter_functions::BLEND: {
    auto another_img = seedimg::make(img);
    seedimg::filters::blend_i({img, 50}, {another_img, 50});
  } break;
  case filter_functions::BLEND_A: {
    auto another_img = seedimg::make(img);
    seedimg::filters::blend_i({img, 50}, {another_img, 50});
  } break;
  case filter_functions::ROTATE_CW:
    seedimg::filters::rotate_cw_i(img);
    break;
  case filter_functions::ROTATE_180:
    seedimg::filters::rotate_180_i(img);
    break;
  case filter_functions::ROTATE_CCW:
    seedimg::filters::rotate_ccw_i(img);
    break;
  case filter_functions::V_MIRROR:
    seedimg::filters::v_mirror_i(img);
    break;
  case filter_functions::H_MIRROR:
    seedimg::filters::h_mirror_i(img);
    break;
  case filter_functions::SATURATION_HSV: {
    seedimg::filters::cconv::hsv_i(img);
    seedimg::filters::saturation_i(img, 5.0f);
  } break;
  case filter_functions::SATURATION_RGB:
    seedimg::filters::saturation_i(img, 5.0f);
    break;
  case filter_functions::SEPIA:
    seedimg::filters::sepia_i(img);
    break;
  case filter_functions::DIFF: {
    auto other = seedimg::make(img);
    seedimg::filters::blur_i(other, 5);
    seedimg::filters::difference_i(img, other);
  } break;
#ifdef SEEDIMG_FILTERS_OCL_H
  case filter_functions::ROTATE_HUE_OCL:
    seedimg::filters::ocl::rotate_hue_i(img, 180);
    break;
  case filter_functions::GRAYSCALE_LUM_OCL:
    seedimg::filters::ocl::grayscale_i(img);
    break;
  case filter_functions::SEPIA_OCL:
    seedimg::filters::ocl::sepia_i(img);
    break;
  case filter_functions::SATURATION_HSV_OCL: {
    seedimg::filters::ocl::cconv::hsv_i(img);
    seedimg::filters::ocl::saturation_i(img, 5.0f);
  } break;
  case filter_functions::SATURATION_RGB_OCL:
    seedimg::filters::ocl::saturation_i(img, 5.0f);
    break;
  case filter_functions::CONTRAST_OCL:
    seedimg::filters::ocl::contrast_i(img, 2.0f);
    break;
  case filter_functions::BRIGHTNESS_OCL:
    seedimg::filters::ocl::brightness_i(img, -10);
    break;
  case filter_functions::BRIGHTNESS_A_OCL:
    seedimg::filters::ocl::brightness_a_i(img, -10);
    break;
#endif
  }

  seedimg::filters::cconv::rgb_i(img);
  char name_buf[256];

  std::filesystem::create_directories(res_dir + "/jpg/");
  std::filesystem::create_directories(res_dir + "/png/");
  std::filesystem::create_directories(res_dir + "/webp/");
  std::filesystem::create_directories(res_dir + "/tiff/");
  std::filesystem::create_directories(res_dir + "/farbfeld/");

  std::snprintf(name_buf, 256, "tests_output/filters/jpg/result-%s.jpg",
                argv[1]);
  seedimg::save(name_buf, img);

  std::snprintf(name_buf, 256, "tests_output/filters/png/result-%s.png",
                argv[1]);
  seedimg::save(name_buf, img);
  std::snprintf(name_buf, 256, "tests_output/filters/webp/result-%s.webp",
                argv[1]);
  seedimg::save(name_buf, img);
  std::snprintf(name_buf, 256, "tests_output/filters/tiff/result-%s.tiff",
                argv[1]);
  seedimg::save(name_buf, img);
  std::snprintf(name_buf, 256, "tests_output/filters/farbfeld/result-%s.ff",
                argv[1]);
  seedimg::save(name_buf, img);
  std::cout << "SUCCESS";
}
