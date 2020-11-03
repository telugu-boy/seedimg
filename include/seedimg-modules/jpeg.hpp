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
#ifndef SEEDIMG_JPEG_H
#define SEEDIMG_JPEG_H

#include <iostream>
#include <istream>
#include <csetjmp>
#include <cassert>

#include <seedimg-modules/modules-abc.hpp>

extern "C" {
#include <jpeglib.h>
#include <jerror.h>
}

// https://github.com/python/cpython/blob/master/Lib/_pyio.py#L27
#define J_BUF_SIZE (8 * 1024)

struct seedimg_jpeg_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf               buf;
};
