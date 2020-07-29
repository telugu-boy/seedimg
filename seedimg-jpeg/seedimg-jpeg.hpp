#ifndef _SEEDIMG_JPEG_H
#define _SEEDIMG_JPEG_H

#include <seedimg/seedimg.hpp>

namespace seedimg::modules {
	namespace jpeg {
		/**
		  * @param quality quality of JPEG encoding (0-100)
		  * @param progressive whether to make JPEG progresssive
		  */
		bool to(std::string filename, std::unique_ptr<seedimg::img>& image, uint8_t quality = 100, bool progressive = false) noexcept;
		std::optional<std::unique_ptr<seedimg::img> > from(std::string filename) noexcept;
	}
}
#endif
