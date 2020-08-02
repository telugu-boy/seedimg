#ifndef _SEEDIMG_PNG_H
#define _SEEDIMG_PNG_H


#include "../seedimg/seedimg.hpp"

namespace seedimg::modules {
	namespace png {
		bool check(const std::string& filename) noexcept;
		bool to(const std::string& filename, std::unique_ptr<seedimg::img>& inp_img) noexcept;
		std::optional<std::unique_ptr<seedimg::img> > from(const std::string& filename) noexcept;
	}
}

#endif