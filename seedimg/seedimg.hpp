#ifndef _SEEDIMG_CORE_H
#define _SEEDIMG_CORE_H

#include <cinttypes>
#include <vector>
#include <tuple>

namespace seedimg {
	typedef std::tuple<uint16_t, uint16_t, uint16_t, uint16_t> pixel;

	class img {
	public:
		uint32_t width;
		uint32_t height;
		//stored in row major order
		//width amount of pixels in a row
		//height amount of rows.
		std::vector<std::vector<pixel> > data;
	};

	namespace modules {};
}

#endif