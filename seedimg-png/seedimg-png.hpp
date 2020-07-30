#ifndef _SEEDIMG_PNG_H
#define _SEEDIMG_PNG_H

#include "pch.h"
#include <seedimg/seedimg.hpp>

namespace seedimg::modules {
	namespace png {
		bool to(std::string filename, std::unique_ptr<seedimg::img>& inp_img) noexcept;
		std::optional<std::unique_ptr<seedimg::img> > from(std::string filename) noexcept;
	}
}

#endif