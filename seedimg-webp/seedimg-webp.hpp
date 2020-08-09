﻿#ifndef _SEEDIMG_WEBP_H
#define _SEEDIMG_WEBP_H

#include <memory>
#include <optional>
#include <seedimg/seedimg.hpp>

namespace seedimg::modules {
namespace webp {
bool check(const std::string &filename) noexcept;
bool to(const std::string &filename, std::unique_ptr<seedimg::img> &inp_img, float quality = 100.0) noexcept;
std::optional<std::unique_ptr<seedimg::img>>
from(const std::string &filename) noexcept;

namespace animated {
std::unique_ptr<std::vector<seedimg::img>> from(const std::string& filename) noexcept;
}
} // namespace webp
} // namespace seedimg::modules

#endif
