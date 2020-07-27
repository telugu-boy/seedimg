#ifndef _SEEDIMG_CORE_H
#define _SEEDIMG_CORE_H

#include <cinttypes>
#include <vector>
#include <string>
#include <optional>
#include <memory>

namespace seedimg {
	struct pixel {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
		pixel() : r(0), g(0), b(0), a(0) {};
		pixel(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_)
			: r(r_), g(g_), b(b_), a(a_)
		{}
		bool operator==(const pixel& other) const {
			return std::tie(r, g, b, a) == std::tie(other.r, other.g, other.b, other.a);
		}
	};

	class img {
	public:
		img(uint32_t w, uint32_t h) : width_(w), height_(h) {};
		img() : width_(0), height_(0) {};
		uint32_t width() const { return width_; }
		uint32_t height() const { return height_; }
		//stored in row major order
		//width amount of pixels in a row
		//height amount of rows.
		std::vector<std::vector<pixel> > data;

		bool to(std::string filename);
	private:
		uint32_t width_;
		uint32_t height_;
	};

	std::optional<std::unique_ptr<seedimg::img> > from(std::string filename);

	namespace modules {};
}

#endif