/**********************************************************************
    seedimg - module based image manipulation library written in modern
                C++ Copyright(C) 2020 tripulse

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
#ifndef SEEDIMG_FORMATS_HPP
#define SEEDIMG_FORMATS_HPP

#include <seedimg.hpp>

namespace seedimg::modules {
namespace decoding {

/**
 * @brief An input reads an image progressively from a given readable-stream.
 * The unit of reading data is a single row.
 */
class input_abc {
public:
    /**
     * @brief Maximum (X,Y) coordinates aka. size of the image.
     */
    point dimensions() const noexcept { return {width(), height()}; }

    virtual simg_int width() const noexcept = 0;
    virtual simg_int height() const noexcept = 0;

    /**
     * @brief Read a row of pixels from input to a buffer.
     * @param to buffer to read in, its size must be atleast (4 * width)
     * @return 'true' if succeeded reading a row, 'false' otherwise
     */
    virtual bool read(pixel* to) = 0;
    bool   operator>>(pixel* to) { return read(to); }

    virtual ~input_abc();
};

input_abc::~input_abc() {}  // sequelch -Wweak-vtables.


class input_init_failure : std::runtime_error {
public:
    input_init_failure(const char* m) : std::runtime_error(m) {}
    ~input_init_failure();
};

input_init_failure::~input_init_failure() {}

};


namespace encoding {

/**
 * @brief An output writes an image progressively to a given writable-stream.
 */
class output_abc {
public:
    /**
     * @brief Write a row of pixels from input to a buffer.
     * @param from buffer to read from, its size must be atleast (4 * width)
     * @return 'true' if succeeded writing a row, 'false' otherwise
     */
    virtual bool write(const pixel* const from) = 0;
    bool    operator<<(const pixel* const from) { return write(from); }

    /**
     * @brief Ensure that data is written to the destination, automatically called
     * by the destructor (RAII idiom).
     */
    virtual void flush() = 0;

    virtual ~output_abc();
};

output_abc::~output_abc() {}  // sequelch -Wweak-vtables.


class output_init_failure : std::runtime_error {
public:
    output_init_failure(const char* m) : std::runtime_error(m) {}
    ~output_init_failure();
};

output_init_failure::~output_init_failure() {}

};
};
#endif
