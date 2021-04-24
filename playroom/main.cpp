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
#include <iostream>
//#include <iomanip>

//#include <seedimg-subimage.hpp>
#include <seedimg-formats/seedimg-png.hpp>
#include <seedimg-utils.hpp>
#include <cmath>

//void print_pixel(seedimg::pixel p) {
//    std::cout << '#'
//              << std::hex
//              << std::setw(2)

//              << int(p.r)
//              << int(p.g)
//              << int(p.b)
//              << int(p.a)
//              << std::endl;
//}


using namespace seedimg::utils;

// employs bucubic resampling for smooth value approximation.
void resize(const simg& in, simg& out) {
    auto w_cub = [](const float x) -> float {
        const float _x_ = std::fabs(x);

        return 1.f/6.f * ((0 <= _x_ && _x_ < 1)
                           ? (6.f * std::pow(_x_, 3.f) - 12.f * std::pow(_x_, 2.f) + 6.f)
                           :
                          (1 <= _x_ && _x_ < 2)
                           ? (-6.f * std::pow(_x_, 3.f) + 30.f * std::pow(_x_, 2.f) - 48.f * _x_ + 24.f)
                           : 0.f);
    };

    for(simg_int y = 0; y < out->height(); ++y) {
        for(simg_int x = 0; x < out->width(); ++x) {
            // project the coordinates of the scaled output image to the
            // original ones, this results in fractional indicies.
            //
            // resampling deals with just that, it finds the "missing" values
            // for those fractional indices.
            float x0 = map_range<float>(x, 0, out->width(), 0, in->width()),
                  y0 = map_range<float>(y, 0, out->height(), 0, in->height());

            float qr = 0,
                  qg = 0,
                  qb = 0,
                  qa = 0;

            for(simg_int j = 0; j < 3; ++j) {
                float v = y0 + j - 1;
                float pr = 0,
                      pg = 0,
                      pb = 0,
                      pa = 0;

                for(simg_int i = 0; i < 3; ++i) {
                    float u = x0 + i - 1;
                    const auto pix = in->pixel(simg_int(u), simg_int(v));

                    pr += pix.r * w_cub(x0 - u);
                    pg += pix.g * w_cub(x0 - u);
                    pb += pix.b * w_cub(x0 - u);
                    pa += pix.a * w_cub(x0 - u);
                }
                qr += pr * w_cub(y0 - v);
                qg += pg * w_cub(y0 - v);
                qb += pb * w_cub(y0 - v);
                qa += pa * w_cub(y0 - v);
            }

            out->pixel(x, y) = {{std::uint8_t(qr)},
                                {std::uint8_t(qg)},
                                {std::uint8_t(qb)},
                                 std::uint8_t(qa)};
        }
    }
}

int main() {
    auto im      = seedimg::modules::png::from("cat.png");
//    auto im_view = im->sub(0, 0, 4, 4)
//                      .sub(0, 0, 2, 2)
//                      .sub(0, 0, 1, 1);

//    print_pixel(im_view.pixel(1, 1));
//    print_pixel(im->pixel(1, 1));

    auto im2    = seedimg::make(im->width() * 2, im->height() * 2);

    resize(im, im2);

    seedimg::modules::png::to("libblr.so.png", im2);
}
