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


#include <seedimg-filters-core/seedimg-filters-core.hpp>
#define CLAMP(x, a,b) ((x) < (a) ? (a) : (x) > (b) ? (b) : (x))

namespace seedimg::filters {
  void brightness_worker(std::unique_ptr<seedimg::img>& input,
                         std::unique_ptr<seedimg::img>& output,
                         simg_int start_row,
                         simg_int end_row,
                         std::uint32_t intensity) {
    for(; start_row < end_row; ++start_row) {
      for(simg_int x = 0; x < input->width(); ++x) {
        auto& pix = input->pixel(x,start_row);

        output->pixel(x,start_row) = {static_cast<std::uint8_t>(pix.r * intensity/255),
                                      static_cast<std::uint8_t>(pix.g * intensity/255),
                                      static_cast<std::uint8_t>(pix.b * intensity/255),
                                      pix.a};
      }
    }
  }

  void brightness_alpha_worker(
      std::unique_ptr<seedimg::img>& input,
      std::unique_ptr<seedimg::img>& output,
      simg_int start_row,
      simg_int end_row,
      std::uint32_t intensity) {
    for(; start_row < end_row; ++start_row) {
      for(simg_int x = 0; x < input->width(); ++x) {
        auto& pix = input->pixel(x,start_row);

        output->pixel(x,start_row) = {static_cast<std::uint8_t>(pix.r * intensity/255),
                                      static_cast<std::uint8_t>(pix.g * intensity/255),
                                      static_cast<std::uint8_t>(pix.b * intensity/255),
                                      static_cast<std::uint8_t>(pix.a * intensity/255)};
      }
    }
  }

  void brightness(std::unique_ptr<seedimg::img>& input,
                  std::unique_ptr<seedimg::img>& output,
                  std::uint8_t intensity) {
    intensity = CLAMP(intensity, 0, 100) * 255/100;

    auto start_end = input->start_end_rows();
    std::vector<std::thread> workers(start_end.size());
    for (std::size_t i = 0; i < workers.size(); i++) {
      workers.at(i) =
          std::thread(brightness_worker, std::ref(input), std::ref(output),
                      start_end.at(i).first, start_end.at(i).second, intensity);
    }
    for (std::size_t i = 0; i < workers.size(); ++i)
      workers.at(i).join();
  }

  void brightness_a(std::unique_ptr<seedimg::img>& input,
                    std::unique_ptr<seedimg::img>& output,
                    std::uint8_t intensity) {
    intensity = CLAMP(intensity, 0, 100) * 255/100;

    auto start_end = input->start_end_rows();
    std::vector<std::thread> workers(start_end.size());
    for (std::size_t i = 0; i < workers.size(); i++) {
      workers.at(i) =
          std::thread(brightness_alpha_worker, std::ref(input), std::ref(output),
                      start_end.at(i).first, start_end.at(i).second, intensity);
    }
    for (std::size_t i = 0; i < workers.size(); ++i)
      workers.at(i).join();
  }

  void brightness_i(std::unique_ptr<seedimg::img>& image,
                    std::uint8_t intensity) {
    brightness(image, image, intensity);
  }

  void brightness_a_i(std::unique_ptr<seedimg::img>& image,
                      std::uint8_t intensity) {
    brightness_a(image, image, intensity);
  }
}
