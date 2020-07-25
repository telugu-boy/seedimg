#ifndef SEEDIMG_CORE_H
#define SEEDIMG_CORE_H
#include <cinttypes>
#include <vector>
#include <tuple>

typedef std::tuple<uint16_t, uint16_t, uint16_t, uint16_t> seedimg_pixel;

class seedimg_img {
public:
	uint32_t width;
	uint32_t height;
	//stored in row major order
	//width amount of pixels in a row
	//height amount of rows.
	std::vector<std::vector<seedimg_pixel> > data;
};

#endif