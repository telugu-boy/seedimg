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
    pixel(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) : r(_r), g(_g), b(_b), a(_a) {};
    bool operator==(const pixel& o) { return std::tie(r, g, b, a) == std::tie(o.r, o.g, o.b, o.a); }
  };

  /**
   * @brief A variant of std::vector whose elements mutable but size is fixed.
   * @tparam T type of element to initialise the vector with.
   */
  template <typename T>
  class vector_fixed {
  public:
    vector_fixed(std::size_t length) : memory(length) {}
    vector_fixed(std::size_t length, T value) : memory(length, value) {}
    
    std::size_t size(void) const noexcept { return memory.size(); }
    T& operator[](std::size_t index)      { return memory[index]; }
    T* data() noexcept                    { return memory.data(); }

    // TODO: find out why can't use a range thing here.
    // auto begin() const noexcept { return memory.begin(); }
    // auto end()   const noexcept { return memory.end(); }
  protected:
    std::vector<T> memory;
  };

  /**
   * @brief An image is a collection of RGBA pixels in a 2D space, where the dimensions are:
   * horizontal and vertical.
   */
  struct img {
  public:
    /**
     * @param width number of horizontal pixels
     * @param height number of vertical pixels
    */
    img(std::size_t width=0, std::size_t height=0) noexcept : buffer(height, vector_fixed<pixel>(width)) {}
    img(img&& image) noexcept : buffer(image.height(), vector_fixed<pixel>(image.width())) {}
    // please please make this constructor not weird, im begging yuo.

    std::size_t width() { return buffer.size() ? buffer[0].size() : 0; }
    std::size_t height() { return buffer.size(); }

    /**
      * @brief A 2D plane of pixels which define a image, layout of this plane is row-major,
      * which means: this is a collection of rows, each row is a collection of pixels.
      */
    auto& data() { return buffer; }

    /**
     * @brief Get a row of horizontal pixels by index.
     * @param row index of row
     * @return 
    */
    auto& get(std::size_t row)              { return buffer[row];  }

    /**
     * @brief Get a pixel at a 2D coordinate from the plane.
     * @param x horizontal axis
     * @param y vertical axis
     * @return 
    */
    auto& get(std::size_t x, std::size_t y) { return buffer[y][x]; }
  private:
    vector_fixed<vector_fixed<pixel>> buffer;
  };

  std::optional<std::unique_ptr<img>> from(std::string filename);
  bool to(std::string, std::unique_ptr<img>& image);

  /**
   * @brief Internal modules that do I/O operations with file-formats providing unified interface to each.
   */
  namespace modules {};
}
#endif