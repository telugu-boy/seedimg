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

#include <seedimg-filters/seedimg-filters-core.hpp>
#include <seedimg-utils.hpp>

namespace seedimg::filters {
void resize(simg& input, simg& output) {
    if(input->width()  == output->width() &&
       input->height() == output->height()) {
        output = seedimg::make(input);
    }

    // find out the nearest pixel coordinate in input image,
    // from the output's coordinates. this is a very fast
    // and easy algorithm but yields poor quality.
    for(simg_int y = 0; y < output->height(); ++y) {
        for(simg_int x = 0; x < output->width(); ++x) {
            output->pixel(static_cast<simg_int>(x),
                          static_cast<simg_int>(y)) = input->pixel(
                static_cast<simg_int>(static_cast<float>(x)/output->width()  * input->width()),
                static_cast<simg_int>(static_cast<float>(y)/output->height() * input->height()));
        }
    }
}

template<class IOIter>
void interpolate_1d(
        IOIter input, IOIter output,
        float delta, simg_int outsize)
{
    for(simg_int i = 0; i < outsize; ++i) {
        float intgr,
              frac = modff(static_cast<float>(i*delta), &intgr);

        auto inp = input[static_cast<simg_int>(intgr)];
        output[i] = {
            {{static_cast<std::uint8_t>((1-frac)*inp.r + frac*inp.r),
              static_cast<std::uint8_t>((1-frac)*inp.g + frac*inp.g),
              static_cast<std::uint8_t>((1-frac)*inp.b + frac*inp.b)}},
              static_cast<std::uint8_t>((1-frac)*inp.a + frac*inp.a),
        };
    }
}

void resize_bilin(simg& input, simg& output) {
    float w_delta =   static_cast<float>(input->width() - 1)
                    / static_cast<float>(output->width() - 1);

    float h_delta =   static_cast<float>(input->height() - 1)
                    / static_cast<float>(output->height() - 1);

    // copy data to output, if size was identical.
    if(input->width()  == output->width() ||
       input->height() == output->height())
        std::copy(input->data(),
                  input->data() + input->width() * input->height(),
                  output->data());

    // TODO: make this algorithm to not have to internally allocate like this.
    // this will hold row-passed data to be column passed.
    simg tmp_img = seedimg::make(output->width(), input->height());
    seedimg::utils::column_iterator cols_tmp(tmp_img), cols_out(output);

    for(simg_int r = 0; r < input->height(); ++r)  // row pass.
        interpolate_1d(input->row(r), tmp_img->row(r), w_delta, output->width());

    for(simg_int c = 0; c < output->width(); ++c) {  // column pass.
        cols_tmp.set_column(c);
        cols_out.set_column(c);

        interpolate_1d(cols_tmp, cols_out, h_delta, output->height());
    }
}
}
