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
#ifndef SEEDIMG_TIFF_H
#define SEEDIMG_TIFF_H

#include <seedimg.hpp>

namespace seedimg {
namespace modules {
namespace tiff {
bool check(const std::string &filename) noexcept;
bool to(const std::string &filename, const anim &inp_anim);
bool to(const std::string &filename, const simg &inp_img);
anim from(const std::string &filename, std::size_t max_frames = 1);
} // namespace seedimg::modules::tiff
} // namespace seedimg::modules
} // namespace seedimg

#endif
