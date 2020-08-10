#ifndef SEEDIMG_AUTODETECT_H
#define SEEDIMG_AUTODETECT_H

#include <memory>
#include <optional>
#include <string>

#include <seedimg/seedimg.hpp>

enum class img_type { unknown = 0, png, jpeg, webp };

enum img_type seedimg_match_ext(const std::string &ext) noexcept;

std::optional<enum img_type>
seedimg_imgtype(const std::string &filename) noexcept;

std::unique_ptr<seedimg::img>
seedimg_autodetect_from(const std::string &filename);

bool seedimg_autodetect_to(const std::string &filename,
                           std::unique_ptr<seedimg::img> &image);

#endif
