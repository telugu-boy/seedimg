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
/**
 * @brief An input reads an image progressively from a given readable-stream.
 * The unit of reading data is a single row.
 */
class input_abc {
  public:
    virtual simg_int width() const noexcept  = 0;
    virtual simg_int height() const noexcept = 0;

    /**
     * @brief Read a row of pixels from input to a buffer.
     * @param to buffer to read in, its size must be atleast (4 * width)
     * @return 'true' if succeeded reading a row, 'false' otherwise
     */
    virtual bool read(pixel* to) = 0;

    virtual ~input_abc();
};

input_abc::~input_abc() {} // sequelch -Wweak-vtables.

struct input_failure : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class output_abc {
  public:
    /**
     * @brief Write a row of pixels from input to a buffer.
     * @param from buffer to read from, its size must be atleast (4 * width)
     * @return 'true' if succeeded writing a row, 'false' otherwise
     */
    virtual bool write(const pixel* const from) = 0;

    /**
     * @brief Ensure that data is written to the destination, automatically
     * called by the destructor (RAII idiom).
     */
    virtual void flush() = 0;

    virtual ~output_abc();
};

output_abc::~output_abc() {} // sequelch -Wweak-vtables.

class output_failure : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

}; // namespace seedimg::modules
#endif
