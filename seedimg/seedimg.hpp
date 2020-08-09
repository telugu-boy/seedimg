#ifndef _SEEDIMG_CORE_H
#define _SEEDIMG_CORE_H

#include <cinttypes>
#include <cstring>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace seedimg {
/**
 * Represents a point in a 2D dimensional space, having two axes:
 * horizontal and vertical (.first = horizontal, .second = vertical).
 */
typedef std::pair<std::uint32_t, std::uint32_t> point;

/**
 * A version of std::vector that is fixed to its original size since its construction.
 * This is a workaround as std::vector propogates const everywhere.
 */
template <typename T> class vector_fixed {
public:
  vector_fixed(std::size_t size = 0)     : buffer(size) {}
  vector_fixed(std::size_t size, T elem) : buffer(size, elem) {}

  std::size_t size() const noexcept { return buffer.size();  }
  T &operator[](std::size_t index)  { return buffer[index];  }
  T *data()                         { return buffer.data();  }

  auto begin()                      { return buffer.begin(); }
  auto end()                        { return buffer.end();   }

protected:
  std::vector<T> buffer;
};

/**
 * A pixel is an element which is a essential building block of an image.
 * This is in RGBA colorspace with unsigned 8-bit precision.
 */
struct pixel {
  std::uint8_t r,g,b,a;
  bool operator==(const pixel &o) const noexcept {
    return std::tie(r,g,b,a) == std::tie(o.r,o.g,o.b,o.a);
  }
};

/**
 * A fixed size 2D plane that is made out of seedimg::pixel (aka. RGBA pixels)
 * which all the seedimg libraries use as their core mechanism.
 */
class img {
public:
    // stupid compiler errors, don't know why it's here, ask multi#3304.
    static constexpr std::uint8_t MAX_PIXEL_VALUE = UINT8_MAX;

    const uint32_t width;
    const uint32_t height;

    img(std::uint32_t w = 0, std::uint32_t h = 0) noexcept :
        width(w), height(h), buffer(h, vector_fixed<seedimg::pixel>(w)) {
        const auto stride = sizeof(seedimg::pixel) * w;
        for (auto &row: buffer)
            std::memset(row.data(), 0, stride);
    }

    auto &row(std::uint32_t row)                  { return buffer[row];               }
    auto &pixel(point p)                          { return buffer[p.second][p.first]; }
    auto &pixel(std::uint32_t x, std::uint32_t y) { return buffer[y][x];              }
    auto &data()                                  { return buffer;                    }

private:
    /**
     * Underlying buffer that'd be used for holding image data.
     *
     * Pixels are stored in row-major order and stack by
     * top-to-bottom order.
     */
    vector_fixed<vector_fixed<seedimg::pixel>> buffer;
};

// Raw-dump loading/doing functions, non-interchangable between systems.
// Because of the differences in endianness.

/**
 * @brief Does a raw-dump of the image, can be loaded back using from().
 */
bool to(const std::string &filename, std::unique_ptr<img> &image) noexcept;

/**
 * @brief Loads a raw-dump of the image, this exists only for debugging purposes.
 *        No gurantees of portability is expressed or implied.
 */
std::optional<std::unique_ptr<img>> from(const std::string &filename) noexcept;

namespace modules {};
namespace filters {};
}

#endif
