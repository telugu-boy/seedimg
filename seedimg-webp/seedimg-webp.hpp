#ifndef _SEEDIMG_WEBP_H
#define _SEEDIMG_WEBP_H

#include <seedimg/seedimg.hpp>

namespace seedimg::modules {
	namespace webp {
		bool to(std::string filename, std::unique_ptr<seedimg::img>& inp_img, uint8_t quality = 100) noexcept;
		std::optional<std::unique_ptr<seedimg::img> > from(std::string filename) noexcept;
	}
}

#endif