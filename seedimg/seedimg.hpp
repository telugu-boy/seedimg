#ifndef _SEEDIMG_CORE_H
#define _SEEDIMG_CORE_H

#include <cinttypes>
#include <vector>
#include <string>
#include <cstring> // just for memset '-'
#include <optional>
#include <memory>

namespace seedimg {

	template <typename T>
	class vector_fixed {
	public:
		vector_fixed(std::size_t l = 0) : vec_(l) {}
		vector_fixed(std::size_t l, T elem) : vec_(l, elem) {}
		std::size_t size(void) const noexcept { return vec_.size(); }

		T& operator[](std::size_t e) {
			return vec_[e];
		}
		T* data(void) {
			return vec_.data();
		}
		auto begin() { return vec_.begin(); }
		auto end() { return vec_.end(); }
	protected:
		std::vector<T> vec_;
	};

	struct pixel {
		std::uint8_t r;
		std::uint8_t g;
		std::uint8_t b;
		std::uint8_t a;
		bool operator==(const pixel& other) const noexcept {
			return std::tie(r, g, b, a) == std::tie(other.r, other.g, other.b, other.a);
		}
	};

	class img {
	public:
		std::size_t const width;
		std::size_t const height;
		img(std::size_t w = 0, std::size_t h = 0) noexcept : width(w), height(h), data(h, seedimg::vector_fixed<seedimg::pixel>(w)) {
			for (auto& row : data) std::memset(row.data(), 0, sizeof(seedimg::pixel) * row.size());
		}
		seedimg::pixel& get_pixel(std::size_t x, std::size_t y) { return data[y][x]; }
		auto& get_row(std::size_t y) { return data[y]; }
		auto& get_data(void) { return data; }
	private:
		//stored in row major order
		//width amount of pixels in a row
		//height amount of rows.
		seedimg::vector_fixed<seedimg::vector_fixed<seedimg::pixel> > data;
	};

	bool to(const std::string& filename, std::unique_ptr<seedimg::img>& inp_img) noexcept;
	std::optional<std::unique_ptr<seedimg::img> > from(const std::string& filename) noexcept;

	namespace modules {};
	namespace filters {};
}

#endif