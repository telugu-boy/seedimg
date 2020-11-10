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
#ifndef SEEDIMG_AUTODETECT_H
#define SEEDIMG_AUTODETECT_H

#include <cstring>
#include <fstream>
#include <functional>
#include <string>

#include <seedimg-modules/modules-abc.hpp>

#include <seedimg-modules/farbfeld.hpp>
#include <seedimg-modules/irdump.hpp>
#include <seedimg-modules/jpeg.hpp>

#include <seedimg-utils.hpp>

namespace seedimg {
/**
 * @brief Formats that autodetect is capable of detecting.
 *
 * Attributes:
 *   .O    cannot be loaded using load() but be saved using save().
 *   IO    can    be loaded using load() and    saved using save().
 */
enum class simg_imgfmt : std::size_t {
  unknown,  // ..
  farbfeld, // IO
  irdump,   // .O,
  jpeg      // IO
};

/* format   sigsize  comparator */
struct simg_imgfmt_entry {
  simg_imgfmt fmt;
  std::size_t sig_size;
  std::function<bool(const char *const)> sig_check;
};

namespace impl {
static const simg_imgfmt_entry farbfeld_det{
    simg_imgfmt::farbfeld, 8,
    [](const char *const h) { return std::memcmp(h, "farbfeld", 8) == 0; }};

static const simg_imgfmt_entry jpeg_det{
    simg_imgfmt::jpeg, 4, [](const char *const h) {
      return std::memcmp(h, "\xFF\xD8\xFF\xE0", 4) == 0;
    }};
static const std::array AUTODETECT_FMT_DETECTORS{farbfeld_det, jpeg_det};

static constexpr std::size_t AUTODETECT_MAX_SIGSIZE = 8;

static std::unordered_map<std::string, simg_imgfmt> AUTODETECT_EXT_TO_FMTTYPE =
    {
        {"ff", simg_imgfmt::farbfeld}, {"sir", simg_imgfmt::irdump},
        {"jpg", simg_imgfmt::jpeg},    {"jpeg", simg_imgfmt::jpeg},
        {"jpe", simg_imgfmt::jpeg},    {"jif", simg_imgfmt::jpeg},
        {"jfif", simg_imgfmt::jpeg},   {"jfi", simg_imgfmt::jpeg},
};

simg_imgfmt autodetect_format(std::istream &buf) {
  char signature[AUTODETECT_MAX_SIGSIZE];
  buf.read(
      signature,
      static_cast<long>(
          AUTODETECT_MAX_SIGSIZE)); // read to size of most lengthy signature.

  auto fmt = simg_imgfmt::unknown;
  for (const auto &fmtdet : AUTODETECT_FMT_DETECTORS) {
    bool b = fmtdet.sig_check(signature);
    if (b) {
      fmt = fmtdet.fmt;
      break;
    }
  }

  for (std::size_t i = 0; i < AUTODETECT_MAX_SIGSIZE; ++i)
    buf.unget();

  return fmt;
}

template <typename I> simg autodetect_read_img(std::istream &buf) {
  I reader{buf};
  auto img = seedimg::make(reader.width(), reader.height());

  for (simg_int r = 0; r < img->height(); ++r)
    if (!reader.read(img->row(r)))
      return nullptr;
  return img;
}

template <typename O>
bool autodetect_write_img(std::ostream &buf, const simg &img) {
  O writer{buf, img->width(), img->height()};

  for (simg_int r = 0; r < img->height(); ++r)
    if (!writer.write(img->row(r)))
      return false;
  return true;
}
} // namespace impl

simg load(std::istream &buf) {

  switch (impl::autodetect_format(buf)) {
  case simg_imgfmt::farbfeld:
    return impl::autodetect_read_img<seedimg::modules::farbfeld::decoder>(buf);
  case simg_imgfmt::jpeg:
    return impl::autodetect_read_img<seedimg::modules::jpeg::decoder>(buf);

  default:
    return nullptr;
  }
}

simg load(const std::string &filename) {
  std::ifstream in(filename);
  return load(in);
}

bool save(std::ostream &buf, simg_imgfmt fmt, const simg &img) {
  if (img == nullptr)
    return false;

#define OUT_FORMAT(fmt)                                                        \
  case simg_imgfmt::fmt:                                                       \
    return impl::autodetect_write_img<seedimg::modules::fmt::encoder>(buf, img)

  switch (fmt) {
    OUT_FORMAT(farbfeld);
    OUT_FORMAT(irdump);
    OUT_FORMAT(jpeg);

  default:
    return false;
  }
#undef OUT_FORMAT
}

bool save(const std::string &fn, const simg &img) {
  std::ofstream out(fn);
  return save(
      out, impl::AUTODETECT_EXT_TO_FMTTYPE[fn.substr(fn.rfind('.') + 1)], img);
}
} // namespace seedimg
#endif
