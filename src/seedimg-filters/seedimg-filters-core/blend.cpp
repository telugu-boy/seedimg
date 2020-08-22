/***********************************************************************
seedimg - module based image manipulation library written in modern C++
Copyright (C) 2020 tripulse

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

#include <seedimg-filters/seedimg-filters-core.hpp>

#define CLAMP(x, a, b) ((x) < (a) ? (a) : (x) > (b) ? (b) : (x))

namespace seedimg::filters {
<<<<<<< HEAD:src/seedimg-filters/seedimg-filters-core/blend.cpp
void pixel_add_worker(simg &image, simg &other, simg_int start_row,
                      simg_int end_row) {
  for (; start_row < end_row; ++start_row) {
    for (simg_int x = 0; x < image->width(); ++x) {
      auto &pix = image->pixel(x, start_row);
      auto &opix = other->pixel(x, start_row);

      pix.r = CLAMP(pix.r + opix.r, 0, 255);
      pix.g = CLAMP(pix.g + opix.g, 0, 255);
      pix.b = CLAMP(pix.b + opix.b, 0, 255);
      pix.a = CLAMP(pix.a + opix.a, 0, 255);
=======
  void pixel_add_worker(simg& image, simg& other, 
                        simg_int start_row,
                        simg_int end_row) {
    for(; start_row < end_row; ++start_row) {
      for(simg_int x = 0; x < image->width(); ++x) {
        auto& pix = image->pixel(x,start_row);
        auto& opix = other->pixel(x,start_row);

        pix.r = CLAMP(pix.r + opix.r, 0,255);
        pix.g = CLAMP(pix.g + opix.g, 0,255);
        pix.b = CLAMP(pix.b + opix.b, 0,255);
        pix.a = CLAMP(pix.a + opix.a, 0,255);
      }
>>>>>>> master:seedimg-filters/seedimg-filters-core/blend.cpp
    }
  }

<<<<<<< HEAD:src/seedimg-filters/seedimg-filters-core/blend.cpp
void blend(std::pair<simg &, const std::uint8_t> input,
           std::pair<seedimg::img, const std::uint8_t> other, simg &output) {
  if (input.first->width() != other.first.width() ||
      input.first->height() != other.first.height())
    return;

  auto other_img = std::make_unique<seedimg::img>(other.first);

  // reduce the image gain as needed.
  brightness_a(input.first, output, input.second);
  brightness_a_i(other_img, other.second);

  auto start_end = input.first->start_end_rows();
  std::vector<std::thread> workers(start_end.size());

  for (std::size_t i = 0; i < workers.size(); i++)
    workers.at(i) =
        std::thread(pixel_add_worker, std::ref(output), std::ref(other_img),
                    start_end.at(i).first, start_end.at(i).second);
  for (std::size_t i = 0; i < workers.size(); ++i)
    workers.at(i).join();
}

void blend_i(std::pair<simg &, const std::uint8_t> input,
             std::pair<seedimg::img, const std::uint8_t> other) {
  blend(input, other, input.first);
}
=======
  void blend(std::pair<simg&,        const std::uint8_t> input,
             std::pair<seedimg::img, const std::uint8_t> other,
             simg&                                       output) {
    if (input.first->width() != other.first.width() ||
       input.first->height() != other.first.height())
      return;

    auto other_img = std::make_unique<seedimg::img>(other.first);

    // reduce the image gain as needed.
    brightness_a(input.first, output, input.second);
    brightness_a_i(other_img, other.second);

    auto start_end = input.first->start_end_rows();
    std::vector<std::thread> workers(start_end.size());

    for (std::size_t i = 0; i < workers.size(); i++)
      workers.at(i) =
          std::thread(pixel_add_worker,
                      std::ref(output), std::ref(other_img),
                      start_end.at(i).first, start_end.at(i).second);
    for (std::size_t i = 0; i < workers.size(); ++i)
      workers.at(i).join();
  }

  void blend_i(std::pair<simg &,       const std::uint8_t> input,
               std::pair<seedimg::img, const std::uint8_t> other)
  { blend(input, other, input.first); }
>>>>>>> master:seedimg-filters/seedimg-filters-core/blend.cpp

}