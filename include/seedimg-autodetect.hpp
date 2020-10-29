/**********************************************************************
    seedimg - module based image manipulation library written in modern
                C++ Copyright(C) 2020 telugu-boy, tripulse

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
#ifndef SEEDIMG_AUTODETECT_H
#define SEEDIMG_AUTODETECT_H

#include <memory>
#include <optional>
#include <string>

#include <seedimg.hpp>

/**
 * @brief Formats that autodetect is capable of detecting.
 *
 *  O    cannot be loaded using load() but be saved using save().
 * IO    can    be loaded using load() and    saved using save().
 */
enum class simg_imgfmt : size_t {
    unknown,   // ..
    farbfeld,  // IO
    irdump     // .O
};

namespace seedimg {
simg load(const std::string&  filename);
simg load(      std::istream& buf);

bool save(const std::string& filename, const simg& img);
bool save(std::ostream& buf, simg_imgfmt fmt, const simg& img);
};
#endif
