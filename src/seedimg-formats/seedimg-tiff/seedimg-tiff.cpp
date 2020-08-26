/**********************************************************************
    seedimg - module based image manipulation library written in modern
                C++ Copyright(C) 2020 telugu-boy

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
// seedimg-png.cpp : Defines the functions for the static library.
//

#include <cstring>
#include <filesystem>
#include <fstream>
#include <vector>

extern "C" {
#include <tiffio.h>
}

#include <seedimg-formats/seedimg-tiff.hpp>

namespace seedimg::modules {
namespace tiff {
bool check(const std::string &filename) noexcept {
  bool res;
  TIFF *img = TIFFOpen(filename.c_str(), "r");
  if ((res = img))
    TIFFClose(img);
  return res;
}

bool to(const std::string &filename, const std::vector<seedimg::img> &inp_img) {
}

std::vector<seedimg::img> from(const std::string &filename,
                               std::size_t max_frames) {}

} // namespace tiff
} // namespace seedimg::modules
