/**********************************************************************
    seedimg - module based image manipulation library written in modern C++
            Copyright(C) 2020 telugu-boy, tripulse

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
#include <fstream>

#include <seedimg.hpp>

#include <seedimg-streams.hpp>
#include <seedimg-modules/farbfeld.hpp>

#include <seedimg-filters/seedimg-filters-core.hpp>

using namespace seedimg;
using namespace seedimg::modules;
using namespace seedimg::filters;

int main() {
         std::ifstream i{"jat.ff"};
    decoding::farbfeld d{i};

         std::ofstream o{"chamar.ff"};
    encoding::farbfeld e{o, 512, 512};

    stream<false> proc {d, e};

    proc.chain.add(filters::invert);
    proc.chain.add(filters::grayscale, true);

    proc.eval_all();
}
