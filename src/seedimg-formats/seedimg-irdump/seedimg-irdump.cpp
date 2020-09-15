#include <fstream>
#include <seedimg-formats/seedimg-irdump.hpp>
#include <seedimg.hpp>

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

namespace seedimg::modules::irdump {
bool to(const std::string &filename, const simg &input) {
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

simg from(const std::string &filename) {
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
} // namespace seedimg::modules::irdump
