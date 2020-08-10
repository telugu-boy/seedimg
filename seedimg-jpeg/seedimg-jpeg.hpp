#ifndef SEEDIMG_JPEG_H
#define SEEDIMG_JPEG_H

#include <memory>
#include <optional>
#include <seedimg/seedimg.hpp>

namespace seedimg::modules {
namespace jpeg {
/**
 * @param quality quality of JPEG encoding (0-100)
 * @param progressive whether to make JPEG progresssive
 */
bool check(const std::string &filename) noexcept;
bool to(const std::string &filename, std::unique_ptr<seedimg::img> &image,
        uint8_t quality = 100, bool progressive = false) noexcept;
std::optional<std::unique_ptr<seedimg::img>>
from(const std::string &filename) noexcept;
} // namespace jpeg
} // namespace seedimg::modules
#endif
