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
#ifndef SEEDIMG_IRDUMP_HPP
#define SEEDIMG_IRDUMP_HPP

#include <istream>
#include <ostream>

#include <seedimg.hpp>
#include "formats.hpp"

namespace seedimg::modules::decoding {

class irdump : public input {
private:
    simg_int _width;
    simg_int _height;
    simg_int _scline = 0;  // current scanline.

    std::istream& in;
public:
    irdump(std::istream& input);

    simg_int width () const noexcept { return _width; }
    simg_int height() const noexcept { return _height; }

    bool read(pixel* to);
    bool read(simg& to) {
        if(to->width() != _width || to->height() != _height)
            return false;  // will not crop/resize.

        for(simg_int r = 0; r < to->height(); ++r)
            if(!read(to->row(r)))
                return false;

        return true;
    }
};

}

namespace seedimg::modules::encoding {

class irdump : public output {
private:
    simg_int _width;
    simg_int _height;
    simg_int _scline = 0;

    std::ostream& out;
public:
    irdump(std::ostream& output, simg_int width, simg_int height);
    irdump(std::ostream& output, seedimg::point dimensions) :
        irdump(output, dimensions.x, dimensions.y) {}

    bool write(const pixel* const from);
    bool write(const simg& to) {
        if(to->width() != _width || to->height() != _height)
            return false;  // will not crop/resize

        for(simg_int r = 0; r < to->height(); ++r)
            if(!write(to->row(r)))
                return false;

        return true;
    }

    void flush() { out.flush(); }
    ~irdump()    { out.flush(); }
};

}
#endif
