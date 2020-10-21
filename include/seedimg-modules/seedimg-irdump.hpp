#ifndef SEEDIMG_IRDUMP_H
#define SEEDIMG_IRDUMP_H

#include <filesystem>
#include <fstream>
#include <seedimg.hpp>

namespace simgdetails {
static inline simg_int from_u32be(uint8_t *cb) {
  return static_cast<simg_int>(cb[0] << 24) |
         static_cast<simg_int>(cb[1] << 16) |
         static_cast<simg_int>(cb[2] << 8) | static_cast<simg_int>(cb[3]);
}

static inline void to_u32be(simg_int n, std::uint8_t *out) {
  out[0] = (n >> 24) & 0xff;
  out[1] = (n >> 16) & 0xff;
  out[2] = (n >> 8) & 0xff;
  out[3] = n & 0xff;
}
} // namespace simgdetails

namespace seedimg::modules {
/**
 * Seedimg IR dump format is a custom seedimg-specific format,
 * which is CPU-endianness agnostic made to represent exact
 * core-contents of the IR for debugging and interchange.
 *
 * NOTE: there's no signature of this format, thus no way to
 * predict its identity, currently only this library does
 * support this format.
 */
namespace irdump {
/**
 * @brief Encode a given image in the Seedimg IR dump format to a filepath.
 * @param a valid name for the image (with/wihout filextension).
 * @param input the given image to encode.
 * @return true on success, false on failure.
 */
inline bool to(const std::string &filename, const simg &input) {
  using namespace simgdetails;
  std::ofstream output(filename);

  struct {
    char width[4];
    char height[4];
  } rawinfo;

  to_u32be(input->width(), reinterpret_cast<std::uint8_t *>(rawinfo.width));
  to_u32be(input->height(), reinterpret_cast<std::uint8_t *>(rawinfo.height));

  try {
    output.write(rawinfo.width, 4).write(rawinfo.height, 4);
  } catch (std::fstream::failure) {
    return false;
  }

  const auto rowstride = input->width() * sizeof(pixel);
  for (simg_int r = 0; r < input->height(); ++r) {
    try {
      output.write(reinterpret_cast<char *>(input->row(r)),
                   static_cast<std::streamsize>(rowstride));
    } catch (std::fstream::failure) {
      return false;
    }
  }

  return true;
}

/**
 * @brief Decode a image in the Seedimg IR dump format from a filepath.
 * @param filepath a valid filepath (with/without filextension).
 * @return a non-null image on success, null on failure.
 */
inline simg from(const std::string &filename) {
  using namespace simgdetails;
  std::ifstream input(filename);

  struct {
    char width[4];
    char height[4];
  } rawinfo;

  input.read(rawinfo.width, 4).read(rawinfo.height, 4);

  auto image = seedimg::make(
      from_u32be(reinterpret_cast<std::uint8_t *>(rawinfo.width)),
      from_u32be(reinterpret_cast<std::uint8_t *>(rawinfo.height)));

  const auto rowstride = image->width() * sizeof(pixel);
  for (simg_int r = 0; r < image->height(); ++r) {
    try {
      input.read(reinterpret_cast<char *>(image->row(r)),
                 static_cast<std::streamsize>(rowstride));
    } catch (std::fstream::failure) {
      return nullptr;
    }
  }

  return image;
}
} // namespace irdump
} // namespace seedimg::modules

#endif // IRDUMP_H
