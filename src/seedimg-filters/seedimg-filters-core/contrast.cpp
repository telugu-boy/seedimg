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
void contrast(simg& input, simg& output, float intensity, bool alpha) {
    intensity = seedimg::utils::clamp(intensity, 0, 100);

    float minv = 127.0f - intensity / 100.0f * 127.0f, maxv = 127.0f + intensity / 100.0f * 127.0f;

    using namespace seedimg::utils;

    if (alpha) {
        for (simg_int y = 0; y < input->height(); ++y) {
            for (simg_int x = 0; x < input->width(); ++x) {
                const auto& p = input->pixel(x, y);

                output->pixel(x, y) = {{static_cast<std::uint8_t>(map_range<float>(p.r, 0, 255, minv, maxv))},
                                       {static_cast<std::uint8_t>(map_range<float>(p.g, 0, 255, minv, maxv))},
                                       {static_cast<std::uint8_t>(map_range<float>(p.b, 0, 255, minv, maxv))},
                                       static_cast<std::uint8_t>(map_range<float>(p.a, 0, 255, minv, maxv))};
            }
        }
    } else {
        for (simg_int y = 0; y < input->height(); ++y) {
            for (simg_int x = 0; x < input->width(); ++x) {
                const auto& p = input->pixel(x, y);

                output->pixel(x, y) = {
                    {static_cast<std::uint8_t>(map_range<float>(p.r, 0, 255, minv, maxv))},
                    {static_cast<std::uint8_t>(map_range<float>(p.g, 0, 255, minv, maxv))},
                    {static_cast<std::uint8_t>(map_range<float>(p.b, 0, 255, minv, maxv))},
                    p.a // preserve alpha?
                };
            }
        }
    }
}

void contrast_i(simg& input, float intensity, bool alpha) { contrast(input, input, intensity, alpha); }
} // namespace seedimg::filters
