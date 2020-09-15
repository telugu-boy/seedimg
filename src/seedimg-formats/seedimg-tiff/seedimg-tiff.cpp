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
// seedimg-png.cpp : Defines the functions for the static library.
//

#include <algorithm>
#include <cstring>
#include <fstream>
#include <vector>

extern "C" {
#include <tiffconf.h>

#include <tiffio.h>
}

#include <seedimg-formats/seedimg-tiff.hpp>

namespace seedimg::modules {
namespace tiff {
bool check(const std::string &filename) noexcept {
  std::ifstream input(filename);
  std::uint8_t intel[4] = {0x49, 0x49, 0x2A, 0x00};
  std::uint8_t motorola[4] = {0x4D, 0x4D, 0x00, 0x2A};
  char sig[4];

  try {
    input.read(sig, 4);
  } catch (std::iostream::failure) {
    return false;
  }
  return std::memcmp(sig, intel, 4) == 0 || std::memcmp(sig, motorola, 4) == 0;
}

bool to(const std::string &filename, const anim &inp_anim) {
  uint16 out[1] = {EXTRASAMPLE_ASSOCALPHA};
  TIFF *img = TIFFOpen(filename.c_str(), "w");
  if (img) {
    for (std::size_t i = 0; i < inp_anim.size(); ++i) {
      if (inp_anim[i]->width() > UINT32_MAX ||
          inp_anim[i]->height() > UINT32_MAX)
        return false;
      TIFFSetField(img, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
      TIFFSetField(img, TIFFTAG_IMAGEWIDTH, inp_anim[i]->width());
      TIFFSetField(img, TIFFTAG_IMAGELENGTH, inp_anim[i]->height());
      TIFFSetField(img, TIFFTAG_BITSPERSAMPLE, 8);
      TIFFSetField(img, TIFFTAG_SAMPLESPERPIXEL, 4);
      TIFFSetField(img, TIFFTAG_EXTRASAMPLES, 1, &out);

      TIFFSetField(img, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
      TIFFSetField(img, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

      unsigned char *buf =
          static_cast<unsigned char *>(_TIFFmalloc(static_cast<tmsize_t>(
              inp_anim[i]->width() * sizeof(seedimg::pixel))));

      for (simg_int y = 0; y < inp_anim[i]->height(); ++y) {
        std::copy(inp_anim[i]->row(y), inp_anim[i]->row(y + 1),
                  reinterpret_cast<seedimg::pixel *>(buf));
        if (TIFFWriteScanline(img, buf, static_cast<uint32>(y), 0) < 0) {
          return false;
        }
      }

      _TIFFfree(buf);
      TIFFWriteDirectory(img);
    }
  }
  TIFFClose(img);
  return true;
}

bool to(const std::string &filename, const simg &inp_img) {
  uint16 out[1] = {EXTRASAMPLE_ASSOCALPHA};
  TIFF *img = TIFFOpen(filename.c_str(), "w");
  TIFFSetField(img, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
  TIFFSetField(img, TIFFTAG_IMAGEWIDTH, inp_img->width());
  TIFFSetField(img, TIFFTAG_IMAGELENGTH, inp_img->height());
  TIFFSetField(img, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(img, TIFFTAG_SAMPLESPERPIXEL, 4);
  TIFFSetField(img, TIFFTAG_EXTRASAMPLES, 1, &out);

  TIFFSetField(img, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(img, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

  unsigned char *buf = static_cast<unsigned char *>(_TIFFmalloc(
      static_cast<tmsize_t>(inp_img->width() * sizeof(seedimg::pixel))));

  for (simg_int y = 0; y < inp_img->height(); ++y) {
    std::copy(inp_img->row(y), inp_img->row(y + 1),
              reinterpret_cast<seedimg::pixel *>(buf));
    if (TIFFWriteScanline(img, buf, static_cast<uint32>(y), 0) < 0) {
      return false;
    }
  }
  _TIFFfree(buf);
  return true;
}

anim from(const std::string &filename, std::size_t max_frames) {
  anim res{};
  std::size_t cnt = 0;
  TIFF *img = TIFFOpen(filename.c_str(), "r");
  if (img) {
    do {
      uint32 w, h;
      TIFFGetField(img, TIFFTAG_IMAGEWIDTH, &w);
      TIFFGetField(img, TIFFTAG_IMAGELENGTH, &h);
      simg decompressed = seedimg::make(w, h);
      TIFFReadRGBAImage(img, w, h,
                        reinterpret_cast<uint32 *>(decompressed->data()), 0);
      seedimg::pixel *row = new seedimg::pixel[w];
      // TIFFReadRGBAImage reads the image in reverse. need to mirror it
      // vertically
      for (simg_int y = 0; y < h / 2; ++y) {
        std::copy(decompressed->row(y), decompressed->row(y + 1), row);
        std::copy(decompressed->row(h - y - 1), decompressed->row(h - y),
                  decompressed->row(y));
        std::copy(row, row + w, decompressed->row(h - y - 1));
      }
      delete[] row;
      res.add(std::move(decompressed));
    } while (TIFFReadDirectory(img) && ++cnt < max_frames);
  }
  TIFFClose(img);
  return res;
}
} // namespace tiff
} // namespace seedimg::modules
