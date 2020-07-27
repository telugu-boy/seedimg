#ifndef _SEEDIMG_CORE_H
#define _SEEDIMG_CORE_H

#include <cinttypes>
#include <vector>
#include <tuple>

namespace seedimg {
	typedef std::tuple<uint16_t, uint16_t, uint16_t, uint16_t> pixel;

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