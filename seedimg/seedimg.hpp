#ifndef _SEEDIMG_CORE_H
#define _SEEDIMG_CORE_H

#include <vector>
#include <string>
#include <sstream>
#include <tuple>


namespace seedimg {
  typedef std::tuple<uint16_t, uint16_t, uint16_t, uint16_t> pixel;

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

  /**
   * @brief Internal modules that do I/O operations with file-formats providing unified interface to each.
   */
  namespace modules {};
}
#endif