#ifndef _SEEDIMG_CORE_H
#define _SEEDIMG_CORE_H

#include <cinttypes>
#include <vector>
#include <tuple>

namespace seedimg {
	struct pixel {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
		pixel() {};
		pixel(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_)
			: r(r_), g(g_), b(b_), a(a_)
		{}
	};

	class img {
	public:
		img(uint32_t w, uint32_t h) : width_(w), height_(h) {};
		uint32_t width() const { return width_; }
		uint32_t height() const { return height_; }
		//stored in row major order
		//width amount of pixels in a row
		//height amount of rows.
		std::vector<std::vector<pixel> > data;
	private:
		uint32_t width_;
		uint32_t height_;
	};

	namespace modules {};
}

#endif