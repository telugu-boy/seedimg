#ifndef _SEEDIMG_FILTERS_CORE_H
#define _SEEDIMG_FILTERS_CORE_H

#include "pch.h"
#include <seedimg/seedimg.hpp>

namespace seedimg::filters {
	void grayscale(std::unique_ptr<seedimg::img>& inp_img, bool lightness = false) noexcept;
}

#endif