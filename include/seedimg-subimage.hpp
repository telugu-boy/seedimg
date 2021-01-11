#ifndef SEEDIMG_SUBIMAGE_API
#define SEEDIMG_SUBIMAGE_API

#include <seedimg.hpp>
#include <seedimg-utils.hpp>

namespace seedimg {
/**
 * @brief Provides a mutable view over an already exisiting seedimg::img,
 * this points to the same resource as the object that is being viewed.
 */
class img_view {
protected:
    seedimg::pixel* data_;
    simg_int        stride_;
    simg_int        x0_, y0_;
    simg_int        x1_, y1_;
    colourspaces    cs_;
public:
    /**
     * @brief view Constructs a subimage, bounding it to [(x,y)..(x+w-1,x+h-1)] range.
     *
     * @param img sequence of pixels representing a 2D plane, the memory associated with
     *            this data is not freed when the destructor runs.
     *
     * @param stride number of pixels to skip to reach to the next line, this is usually
     *               the old width (and some padding perhaps).
     *
     * @param x X-coordinate offset from top-left.
     * @param y Y-coordinate offset from top-left.
     * @param w Width of the view.
     * @param h Height of the view.
     */
    img_view(seedimg::pixel* img,
             simg_int        stride,
             simg_int        x,
             simg_int        y,
             simg_int        w,
             simg_int        h,
             colourspaces    cs = colourspaces::rgb)
        : data_(img),
          stride_(stride),
          x0_(x),
          y0_(y),
          x1_(x+w),
          y1_(y+h),
          cs_(cs)
    {
        if(x1_ > stride)
            std::out_of_range { "Width illegally exceeds stride" };
    }

    /**
     * @brief Creates a view of the pointed pixel sequence bounded by the
     *        regions of this view.
     *
     * @param x X-coordinate offset from top-left.
     * @param y Y-coordinate offset from top-left.
     * @param w Width of the view.
     * @param h Height of the view.
     */
    inline img_view sub(simg_int x,
                        simg_int y,
                        simg_int w,
                        simg_int h) const noexcept {
        if(x >= x1_ || y >= y1_)
            std::out_of_range { "Offset coordinates out of bounds" };

        using namespace seedimg::utils;

        return { data_, stride_, x, y,
                 clamp(x+w, x, x1_),
                 clamp(y+h, y, y1_),
                 cs_ };
    }

    inline simg_int width() const noexcept { return x1_; }
    inline simg_int height() const noexcept { return y1_; }
    inline colourspaces colourspace() const noexcept { return cs_; }

    /**
     * @brief Returns a lvalue reference to a pixel at a specified coordinate.
     * @param x X-coordinate of pixel location.
     * @param y Y-coordinate of pixel location.
     */
    inline seedimg::pixel& pixel(simg_int x, simg_int y) noexcept {
        return data_[(y + y0_) * stride_ + x + x0_];
    }

    /**
     * @brief Same as pixel(simg_int x, simg_int y).
     * @param p the point at which the pixel is located.
     */
    inline seedimg::pixel& pixel(point p) noexcept { return pixel(p.x, p.y); }

    inline seedimg::pixel& operator[](point p) noexcept { return pixel(p); }

    /**
     * @brief Returns the pointer to the n-th row of the image.
     * @param n zero-based index of the row.
     */
    inline seedimg::pixel* row(simg_int n) noexcept {
        return data_ + (n + y0_) * stride_ + x0_;
    }

    /**
     * @brief Returns the pointer to the sequence of pixels passed to constructor
     *        though the "img" parameter, no offseting is done on the data.
     */
    inline seedimg::pixel* data() const noexcept { return data_; }

    ~img_view() = default;
};

// adaptor for seedimg::img for creating a view.
inline img_view img::sub(simg_int x,
                         simg_int y,
                         simg_int w,
                         simg_int h) const noexcept {
     if(x >= width_ || y >= height_)
         std::out_of_range { "Offset coordinates out of bounds" };

     using namespace seedimg::utils;

     return { data_, width_, x, y,
              clamp(x+w, x, width_),
              clamp(y+h, y, height_) };
}
}; // namespace seedimg
#endif // SEEDIMGSUBIMAGE_HPP
