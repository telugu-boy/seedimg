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
#include <filesystem>

#include <seedimg-farbfeld/seedimg-farbfeld.hpp>
#include <seedimg-irdump/irdump.hpp>
#include <seedimg-jpeg/seedimg-jpeg.hpp>
#include <seedimg-png/seedimg-png.hpp>
#include <seedimg-webp/seedimg-webp.hpp>
#include <seedimg/seedimg.hpp>

#include "seedimg-autodetect.hpp"

enum img_type seedimg_match_ext(const std::string &ext) noexcept {
  if (ext == "png")
    return img_type::png;
  if (ext == "jpeg" || ext == "jpg" || ext == "jfif")
    return img_type::jpeg;
  if (ext == "webp")
    return img_type::webp;
  if (ext == "ff" || ext == "farbfeld")
    return img_type::farbfeld;
  if (ext == "sir")
    return img_type::irdump;
  return img_type::unknown;
}

std::optional<enum img_type>
seedimg_imgtype(const std::string &filename) noexcept {
  if (!std::filesystem::exists(filename))
    return std::nullopt;
  if (seedimg::modules::png::check(filename))
    return img_type::png;
  if (seedimg::modules::jpeg::check(filename))
    return img_type::jpeg;
  if (seedimg::modules::webp::check(filename))
    return img_type::webp;
  if (seedimg::modules::farbfeld::check(filename))
    return img_type::farbfeld;
  return img_type::unknown;
}

simg seedimg_autodetect_from(const std::string &filename) {
  auto type = seedimg_imgtype(filename);
  if (type == std::nullopt)
    return nullptr;
  switch (*seedimg_imgtype(filename)) {
  case img_type::png:
    return seedimg::modules::png::from(filename);
  case img_type::jpeg:
    return seedimg::modules::jpeg::from(filename);
  case img_type::webp:
    return seedimg::modules::webp::from(filename);
  case img_type::farbfeld:
    return seedimg::modules::farbfeld::from(filename);
  default:
    return nullptr;
  }
}

bool seedimg_autodetect_to(const std::string &filename, const simg &image) {
  std::string extension_type = filename.substr(filename.rfind('.') + 1);
  switch (seedimg_match_ext(extension_type)) {
  case img_type::png:
    return seedimg::modules::png::to(filename, image);
  case img_type::jpeg:
    return seedimg::modules::jpeg::to(filename, image);
  case img_type::webp:
    return seedimg::modules::webp::to(filename, image);
  case img_type::farbfeld:
    return seedimg::modules::farbfeld::to(filename, image);
  case img_type::irdump:
    return seedimg::modules::irdump::to(filename, image);
  default:
    return false;
  }
}
