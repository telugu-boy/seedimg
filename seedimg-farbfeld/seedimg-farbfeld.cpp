#include "seedimg-farbfeld.hpp"

#include <fstream>

static inline std::uint16_t fu16be(uint8_t *cb) {
  return static_cast<std::uint16_t>(cb[1] << 8) |
         static_cast<std::uint16_t>(cb[0]);
}
static inline void tu16be(std::uint16_t n, std::uint8_t *out) {
  out[0] = n >> 8 & 0xff;
  out[1] = n & 0xff;
}
static inline simg_int fu32be(uint8_t *cb) {
  return static_cast<simg_int>(cb[0] << 24) |
         static_cast<simg_int>(cb[1] << 16) |
         static_cast<simg_int>(cb[2] << 8) | static_cast<simg_int>(cb[3]);
}
static inline void tu32be(simg_int n, std::uint8_t *out) {
  out[0] = (n >> 24) & 0xff;
  out[1] = (n >> 16) & 0xff;
  out[2] = (n >> 8) & 0xff;
  out[3] = n & 0xff;
}

// https://stackoverflow.com/a/41762509/10978642
static inline std::uint8_t b16_8(std::uint16_t n) { return n >> 8; }
// https://stackoverflow.com/a/6436100/10978642
// NOTE: simple bitshifting could've solved but would
//       be quite noisy, so an addition put.
static inline std::uint16_t b8_16(std::uint8_t n) {
  return static_cast<std::uint16_t>(n << 8) + static_cast<std::uint16_t>(n);
}

namespace seedimg::modules::farbfeld {
bool check(const std::string &filename) noexcept {
  std::ifstream input(filename);
  char sig[8];

  try {
    input.read(sig, 8);
  } catch (std::iostream::failure) {
    return false;
  }
  return std::memcmp(sig, "farbfeld", 8) == 0;
}

std::unique_ptr<seedimg::img> from(const std::string &filename) {
  std::ifstream input(filename);
  struct {
    char sig[8];
    uint8_t width[4];
    uint8_t height[4];
  } rawinfo;

  try {
    input.read(rawinfo.sig, 8)
        .read(reinterpret_cast<char *>(rawinfo.width), 4)
        .read(reinterpret_cast<char *>(rawinfo.height), 4);
  } catch (std::ios_base::failure) {
    return nullptr;
  }

  // siganture match is mandatory else invalid Farbfeld file.
  if (std::memcmp(rawinfo.sig, "farbfeld", 8) != 0)
    return nullptr;

  auto result = std::make_unique<seedimg::img>(fu32be(rawinfo.width),
                                               fu32be(rawinfo.height));

  uint8_t rawpixel[8];
  for (simg_int y = 0; y < result->height(); ++y) {
    for (simg_int x = 0; x < result->width(); ++x) {
      try {
        input.read(reinterpret_cast<char *>(rawpixel), 8);
      } catch (std::ios_base::failure) {
        return nullptr;
      }

      result->pixel(x, y) = {
          b16_8(fu16be(rawpixel)), b16_8(fu16be((rawpixel + 2))),
          b16_8(fu16be((rawpixel + 4))), b16_8(fu16be((rawpixel + 6)))};
    }
  }

  return result;
}

bool to(const std::string &filename,
        const std::unique_ptr<seedimg::img> &image) {
  std::ofstream output(filename);

  std::uint8_t width_ser[4];
  std::uint8_t height_ser[4];
  tu32be(image->width(), width_ser);
  tu32be(image->height(), height_ser);
  try {
    output.write("farbfeld", 8)
        .write(reinterpret_cast<char *>(width_ser), 4)
        .write(reinterpret_cast<char *>(height_ser), 4);
  } catch (std::ios_base::failure) {
    return false;
  }

  uint8_t rawpixel[8];
  for (simg_int y = 0; y < image->height(); ++y) {
    for (simg_int x = 0; x < image->width(); ++x) {
      auto px = image->pixel(x, y);

      tu16be(b8_16(px.r), rawpixel);
      tu16be(b8_16(px.g), rawpixel + 2);
      tu16be(b8_16(px.b), rawpixel + 4);
      tu16be(b8_16(px.a), rawpixel + 6);

      try {
        output.write(reinterpret_cast<char *>(rawpixel), 8);
      } catch (std::ios_base::failure) {
        return false;
      }
    }
  }

  return true;
}
} // namespace seedimg::modules::farbfeld
