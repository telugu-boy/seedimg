#ifndef _SEEDIMG_CORE_H
#define _SEEDIMG_CORE_H

#include <vector>
#include <string>
#include <optional>
#include <memory>


namespace seedimg {
  /**
   * @brief A pixel is a building-block of an image, which consists of RGBA components.
   */
  struct pixel {
    uint8_t r, g, b, a;

    pixel() : r(0), g(0), b(0), a(0) {}
    pixel(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) :
      r(_r), g(_g), b(_b), a(_a) {};
    bool operator==(const pixel& o) {
      return std::tie(r, g, b, a) == std::tie(o.r, o.g, o.b, o.a);
    }
  };

  /**
   * @brief An image is a collection of RGBA pixels in a 2D space, where the dimensions are:
   * horizontal and vertical.
   */
  struct img {
    /**
      * @brief A 2D plane of pixels which define a image, layout of this plane is row-major,
      * which means: this is a collection of rows, each row is a collection of pixels.
      */
    std::vector<std::vector<pixel>> data;

    /**
     * @param width number of horizontal pixels
     * @param height number of vertical pixels
    */
    img(uint32_t width, uint32_t height) {
      data.resize(height);
      for (auto& row : data)
        row.resize(width);
    }

    uint32_t width() {
      return data.size() ? data[0].size() : 0;
    }

    uint32_t height() {
      return data.size();
    }
  };

  std::optional<std::unique_ptr<img>> from(std::string filename);
  bool to(std::string, std::unique_ptr<img> image);

  /**
   * @brief Internal modules that do I/O operations with file-formats providing unified interface to each.
   */
  namespace modules {};
}
#endif
