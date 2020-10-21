/**********************************************************************
    seedimg - module based image manipulation library written in modern C++
            Copyright(C) 2020 tripulse

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

#include <seedimg-modules/formats.hpp>
#include <seedimg-filters/seedimg-filters-core.hpp>

namespace seedimg {

using input  = seedimg::modules::decoding::input;
using output = seedimg::modules::encoding::output;

template<bool Inplace>
using filterchain = seedimg::filters::filterchain<Inplace>;

/**
 * @brief Holds input, output streams, a filterchain. Reads rows progressively from
 * input and applies the filterchain to it and writes to the output.
 *
 * @tparam Inplace whether to use an inplace filterchain.
 */
template<bool Inplace>
class stream {
private:
    input& i;
    output& o;

    simg img;      // internal
    simg img_out;  // when not inplace.
public:
    /**
     * @brief A chain of filters that will be applied on input image and be written to output,
     * filters that change dimensions, are non-linear, are stateful are subject to potential
     * UB (undefined behaviour).
     */
    filterchain<Inplace> chain;

    stream(input& in, output& out)
        : i(in), o(out),
          img                        (seedimg::make(i.width(), 1)),
          img_out(Inplace ? nullptr : seedimg::make(i.width(), 1)) {}

    /**
     * @brief Evaluate a single row. Reads a row, applies filterchain, writes it.
     * @return true if evaluation successful, false otherwise.
     */
    bool eval() {
        i.read(img->data());  // img->data() = img->row(0)

        bool result;

        if constexpr(Inplace) {
            chain.eval(img);
            result = o.write(img->data());
        }
        else {
            chain.eval(img, img_out);
            result = o.write(img_out->data());
        }

        return result;
    }

    /**
     * @brief Evaluate all rows, equavalient to calling eval() to "in.height()" times.
     * @return true if evaluation successful, false otherwise.
     */
    bool eval_all() {
        for(simg_int r = 0; r < i.height(); ++r)
            if(eval() == false)
                return false;

        return true;
    }
};
}; // namespace seedimg
