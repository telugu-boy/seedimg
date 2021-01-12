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
#ifndef SEEDIMG_WEBP_H
#define SEEDIMG_WEBP_H

#include <seedimg.hpp>

namespace seedimg {
namespace modules {
namespace webp {
bool check(const std::string &filename) noexcept;
bool to(const std::string &filename, const simg &inp_img, float quality = 100.0);
simg from(const std::string &filename);
} // namespace seedimg::modules::webp
} // namespace seedimg::modules
} // namespace seedimg
#endif
