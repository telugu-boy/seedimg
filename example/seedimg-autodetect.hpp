#ifndef _SEEDIMG_AUTODETECT_H
#define _SEEDIMG_AUTODETECT_H

#include <string>
#include <optional>
#include <memory>
#include "../seedimg/seedimg.hpp"

enum img_type { unknown = 0, png, jpeg, webp };

enum img_type seedimg_match_ext(const std::string& ext) noexcept;

std::optional<enum img_type> seedimg_imgtype(const std::string& filename) noexcept;

std::optional<std::unique_ptr<seedimg::img> > seedimg_autodetect_from(const std::string& filename) noexcept;

bool seedimg_autodetect_to(const std::string& filename, std::unique_ptr<seedimg::img>& image) noexcept;

#endif