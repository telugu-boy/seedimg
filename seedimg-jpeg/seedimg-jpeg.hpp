#ifndef _SEEDIMG_JPEG
#define _SEEDIMG_JPEG
#pragma warning(disable:4996)

#include <seedimg/seedimg.hpp>

#include <optional>
#include <memory>
#include <string>

namespace seedimg::modules {
  namespace jpeg {
    std::optional<std::unique_ptr<seedimg::img> > from(std::string filename) noexcept;

    /**
      * @param quality quality of JPEG encoding (0-100)
      * @param progressive whether to make JPEG progresssive
      */
    bool to(std::string filename, std::unique_ptr<seedimg::img>& image, uint8_t quality = 100, bool progressive = false) noexcept;
  }
}
#endif
