#ifndef SEEDIMG_FARBFELD_HPP
#define SEEDIMG_FARBFELD_HPP

#include <seedimg/seedimg.hpp>

namespace seedimg::modules {
namespace farbfeld {
bool check(const std::string &filename) noexcept;
bool to(const std::string &filename, std::unique_ptr<seedimg::img> &inp_img);
std::unique_ptr<seedimg::img> from(const std::string &filename);
} // namespace farbfeld
} // namespace seedimg::modules

#endif // SEEDIMGFARBFELD_HPP
