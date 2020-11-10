/**********************************************************************
    seedimg - module based image manipulation library written in modern C++
            Copyright(C) 2020 telugu-boy, tripulse

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

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

#define SIMG_OCL_LOCAL_WG_SIZE 64

#include <seedimg-autodetect.hpp>
#include <seedimg-filters/seedimg-filters-core.hpp>
#include <seedimg-filters/seedimg-filters-ocl.hpp>

#include <random>

auto main() -> decltype(
    std::remove_reference_t<decltype(std::declval<decltype(0)>())>{}) {
  using namespace seedimg::filters;
  std::cout << "Current path is " << std::filesystem::current_path()
            << std::endl;
  {
    // ocl::init_ocl_singleton(1, 0);
    auto a = seedimg::load("jat.jpg");
    if (a != nullptr) {
      // crop_i(a, {0, 0}, {100, 100});
      // grayscale_i(a, true);
      // invert_i(a);
      // blur_i(a, 10);
      // h_blur_i(a, 10);
      // v_blur_i(a, 100, 1);
      // convolution(a, {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}});
      // rotate_hue_i(a, 90);
      // v_mirror_i(a);
      // h_mirror_i(a);
      // ocl::rotate_hue_i(a, -90);
      // ocl::grayscale_i(a, true);
      // cconv::ycbcr_i(a);
      // cconv::hsv_i(a);
      // saturation_i(a, 3.5f);
      // cconv::rgb_i(a);
      // constexpr auto comp = compose_smats(std::array{SEPIA_MAT, SEPIA_MAT});
      // constexpr static auto sepia_lut = seedimg::utils::gen_lut(comp);
      // apply_mat_i(a, SEPIA_MAT);
      // apply_mat_lut_i(a, sepia_lut);
      // cconv::ycbcr_i(a);
      // cconv::rgb_i(a);
      // cconv::ycbcr_i(a, seedimg::colourspaces::ycbcr_bt601);
      // cconv::rgb_i(a);
      /*
              auto inp_img_buf =
                  new cl::Buffer{ocl::get_context(), CL_MEM_READ_WRITE,
                                 seedimg::utils::round_up(sizeof(seedimg::pixel)
      * a->width() * a->height(), SIMG_OCL_BUF_PADDING)}; auto res_img_buf = new
           cl::Buffer{ocl::get_context(), CL_MEM_READ_WRITE,
                                 seedimg::utils::round_up(sizeof(seedimg::pixel)
      * a->width() * a->height(), SIMG_OCL_BUF_PADDING)}; std::random_device rd;
              std::mt19937 mt(rd());
              std::uniform_real_distribution<float> sat(-2.5, 2.5);
              std::uniform_real_distribution<float> br(-1, 1);
              std::uniform_real_distribution<float> con(0.1f, 2.0f);
              std::filesystem::create_directory("bibs");
              auto b = seedimg::make(a->width(), a->height());
              auto &queue = ocl::get_queue();
              ocl::write_img_1d(queue, a, *inp_img_buf, true);*/
    }
  }
}
