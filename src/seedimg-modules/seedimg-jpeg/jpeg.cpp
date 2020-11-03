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
#include <seedimg-modules/jpeg.hpp>
#include <seedimg-utils.hpp>

static char jpeg_last_error_msg[JMSG_LENGTH_MAX];
[[noreturn]] static void jpeg_error_exit(j_common_ptr j) {
  seedimg_jpeg_error_mgr *err =
      reinterpret_cast<seedimg_jpeg_error_mgr *>(j->err);
  (*(j->err->format_message))(j, jpeg_last_error_msg);
  std::longjmp(err->buf, 1);
}
