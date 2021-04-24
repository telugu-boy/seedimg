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
#ifndef SEEDIMG_AUTODETECT_H
#define SEEDIMG_AUTODETECT_H

#include <filesystem>

#include <seedimg-formats/seedimg-farbfeld.hpp>
#include <seedimg-formats/seedimg-irdump.hpp>
#include <seedimg-formats/seedimg-jpeg.hpp>
#include <seedimg-formats/seedimg-png.hpp>
#include <seedimg-formats/seedimg-tiff.hpp>
#include <seedimg-formats/seedimg-webp.hpp>
#include <seedimg.hpp>

enum class seedimg_img_type {
  unknown = 0,
  png,
  jpeg,
  webp,
  farbfeld,
  tiff,
  irdump
};

enum seedimg_img_type seedimg_match_ext(const std::string &ext) noexcept {
  if (ext == "png")
    return seedimg_img_type::png;
  if (ext == "jpeg" || ext == "jpg" || ext == "jfif")
    return seedimg_img_type::jpeg;
  if (ext == "webp")
    return seedimg_img_type::webp;
  if (ext == "ff" || ext == "farbfeld")
    return seedimg_img_type::farbfeld;
  if (ext == "tiff" || ext == "tif")
    return seedimg_img_type::tiff;
  if (ext == "sir")
    return seedimg_img_type::irdump;
  return seedimg_img_type::unknown;
}

std::optional<enum seedimg_img_type>
seedimg_imgtype(const std::string &filename) noexcept {
  if (!std::filesystem::exists(filename))
    return std::nullopt;
  if (seedimg::modules::png::check(filename))
    return seedimg_img_type::png;
  if (seedimg::modules::jpeg::check(filename))
    return seedimg_img_type::jpeg;
  if (seedimg::modules::webp::check(filename))
    return seedimg_img_type::webp;
  if (seedimg::modules::farbfeld::check(filename))
    return seedimg_img_type::farbfeld;
  if (seedimg::modules::tiff::check(filename))
    return seedimg_img_type::tiff;
  return seedimg_img_type::unknown;
}

namespace seedimg {
simg load(const std::string &filename) {
  auto type = seedimg_imgtype(filename);
  if (type == std::nullopt)
    return nullptr;
  switch (*type) {
  case seedimg_img_type::png:
    return seedimg::modules::png::from(filename);
  case seedimg_img_type::jpeg:
    return seedimg::modules::jpeg::from(filename);
  case seedimg_img_type::webp:
    return seedimg::modules::webp::from(filename);
  case seedimg_img_type::farbfeld:
    return seedimg::modules::farbfeld::from(filename);
  case seedimg_img_type::tiff: {
    // this will return the first one only. use the full function to get the
    // entire vector.
    return std::move(seedimg::modules::tiff::from(filename, 1)[0]);
  }
  default:
    return nullptr;
  }
}

bool save(const std::string &filename, const simg &image) {
  std::string extension_type{filename.substr(filename.rfind('.') + 1)};
  switch (seedimg_match_ext(extension_type)) {
  case seedimg_img_type::png:
    return seedimg::modules::png::to(filename, image);
  case seedimg_img_type::jpeg:
    return seedimg::modules::jpeg::to(filename, image);
  case seedimg_img_type::webp:
    return seedimg::modules::webp::to(filename, image);
  case seedimg_img_type::farbfeld:
    return seedimg::modules::farbfeld::to(filename, image);
  case seedimg_img_type::tiff:
    return seedimg::modules::tiff::to(filename, image);
  case seedimg_img_type::irdump:
    return seedimg::modules::irdump::to(filename, image);
  default:
    return false;
  }
}
} // namespace seedimg


#endif
