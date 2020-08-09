#ifndef FARBFELD_HXX
#define FARBFELD_HXX

#include <seedimg/seedimg.hpp>

namespace seedimg::modules {
namespace farbfeld {
bool check(const std::string& filename) noexcept;
std::optional<std::unique_ptr<seedimg::img>> from(const std::string& filename) noexcept;
bool to(const std::string& filename, const std::unique_ptr<seedimg::img>& image) noexcept;
}
}

#endif // FARBFELD_HXX
