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
#ifndef SEEDIMG_JPEG_H
#define SEEDIMG_JPEG_H

#include <memory>
#include <optional>
#include <seedimg/seedimg.hpp>

namespace seedimg::modules {
namespace jpeg {
/**
 * @param quality quality of JPEG encoding (0-100)
 * @param progressive whether to make JPEG progresssive
 */
bool check(const std::string &filename) noexcept;
bool to(const std::string &filename, const simg &image,
        uint8_t quality = 100, bool progressive = false);
simg from(const std::string &filename);
} // namespace jpeg
} // namespace seedimg::modules
#endif
