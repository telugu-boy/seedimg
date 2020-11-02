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
#include <filesystem>
#include <iostream>

#include <seedimg-autodetect.hpp>
#include <seedimg-filters/seedimg-filters-core.hpp>

using namespace seedimg::filters;


#define RELEASE_DATE    "November, 2020"
#define RELEASE_VERSION "v1.0-beta"


auto main(int argc, char** argv) -> int {
  fprintf(stderr, "               /--------------------\\             \n"
                  "           -===( CHUTIA REALISATION )===-          \n"
                  "               \\--------------------/             \n"
                  "                                                   \n"
                  "               [ The 5CP foundation ]              \n"
                  "                   " RELEASE_DATE "                \n"
                  "                      " RELEASE_VERSION "          \n"
                  "                                                   \n"
                  "                                                   \n"
                  "  USAGE:   %s IN OUT chutianess iterations         \n",
          argv[0]);

  if(argc < 5) {
    fprintf(stderr, "  ERROR:   no files were given, aborting!       \n");
    exit(ENOENT);  // idk if windows support ERRNO '-'
  }

  int lev  = atoi(argv[3]),
      iter = atoi(argv[4]);

  auto img  = seedimg::load(argv[1]);
  auto orig = seedimg::make(img);

  blur_i(img, static_cast<unsigned int>(lev),
              static_cast<std::uint8_t>(iter));
  difference_i(img, orig);

  seedimg::save(argv[2], img);
}
