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
// seedimg.cpp : Defines the functions for the static library.
//

#include <algorithm>
#include <seedimg.hpp>

namespace seedimg {

bool is_on_rect(seedimg::point xy1, seedimg::point xy2, seedimg::point point) noexcept {
    return xy1.x <= point.x && point.x <= xy2.x && xy1.y <= point.y && point.y <= xy2.y;
}

seedimg::point get_rect_dimensions(seedimg::point p1, seedimg::point p2) noexcept {
    auto ordered_x = std::minmax(p1.x, p2.x);
    auto ordered_y = std::minmax(p1.y, p2.y);
    // width, height
    return {ordered_x.second - ordered_x.first, ordered_y.second - ordered_y.first};
}

// create shared ptrs from certain suitable params
std::unique_ptr<seedimg::img> make(simg_int width, simg_int height) {
    return std::make_unique<seedimg::img>(width, height);
}

std::unique_ptr<seedimg::img> make(simg_int width, simg_int height, seedimg::pixel* data) {
    return std::make_unique<seedimg::img>(width, height, data);
}

std::unique_ptr<seedimg::img> make(seedimg::img&& inp_img) {
    return std::make_unique<seedimg::img>(std::move(inp_img));
}

std::unique_ptr<seedimg::img> make(const std::unique_ptr<seedimg::img>& inp_img) {
    return std::make_unique<seedimg::img>(*inp_img);
}

} // namespace seedimg
