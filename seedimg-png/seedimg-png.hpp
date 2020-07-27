#ifndef _SEEDIMG_PNG_H
#define _SEEDIMG_PNG_H

#include <string>
#include <memory>
#include <optional>

#include <seedimg/seedimg.hpp>

namespace seedimg::modules {
	namespace png {
		bool to(std::string filename);
		std::optional<std::unique_ptr<seedimg::img> > from(std::string filename);
	}
}

#endif