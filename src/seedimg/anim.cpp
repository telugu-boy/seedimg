/***********************************************************************
    seedimg - module based image manipulation library written in modern C++
    Copyright (C) 2020 telugu-boy, <contributor name>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

#include <seedimg.hpp>

namespace seedimg {
class anim::anim_impl {
public:
  std::size_t framerate;

  anim_impl() { framerate = 0; }

  anim_impl(std::size_t size, std::size_t framerate) {
    this->framerate = framerate;
    data = std::vector<simg>(size);
  }

  anim_impl(anim_impl const &anim_) {
    data = std::vector<simg>();
    data.reserve(anim_.data.size());
  }
  anim_impl(anim_impl &&other) {
    framerate = other.framerate;
    other.framerate = 0;
    data = std::move(other.data);
  }

  anim_impl &operator=(anim_impl other) {
    this->data.swap(other.data);
    return *this;
  }
  anim_impl &operator=(anim_impl &&other) {
    if (&other != this) {
      this->data = std::move(other.data);
    }
    return *this;
  }

  anim_impl(simg &&img...) : anim_impl() { this->add(std::move(img)); }

  simg &operator[](std::size_t i) { return data[i]; }
  const simg &operator[](std::size_t i) const { return data[i]; }

  void add(simg &&img) { data.push_back(std::move(img)); }
  bool insert(simg &&img, std::size_t index) {
    if (index >= data.size())
      return false;
    data.insert(data.begin() + static_cast<int>(index), std::move(img));
    return true;
  }
  bool remove(std::size_t index) {
    if (index >= data.size())
      return false;
    data.erase(data.begin() + static_cast<int>(index));
    return true;
  }
  bool trim(std::size_t start, std::size_t end) {
    if (start > end || end >= data.size())
      return false;
    data.erase(data.begin(), data.begin() + static_cast<int>(start));
    data.erase(data.begin() + static_cast<int>(end - start), data.end());
    return true;
  }

  std::size_t size() const noexcept { return data.size(); }

private:
  std::vector<simg> data;
};

anim::anim() : impl{} {};

anim::anim(std::size_t size, std::size_t framerate)
    : impl{new anim_impl{size, framerate}} {}

anim::anim(seedimg::anim const &anim_) : impl{new anim_impl{*anim_.impl}} {}

anim::anim(seedimg::anim &&other) : impl{new anim_impl{*other.impl}} {}

anim::anim(simg &&imgs...) : impl{new anim_impl{std::move(imgs)}} {}

anim &anim::operator=(anim other) {
  *impl = *other.impl;
  return *this;
}
anim &anim::operator=(anim &&other) {
  *impl = *other.impl;
  return *this;
}

simg &anim::operator[](std::size_t i) { return (*impl)[i]; }
const simg &anim::operator[](std::size_t i) const { return (*impl)[i]; }

void anim::add(simg &&img) { impl->add(std::move(img)); }
bool anim::insert(simg &&img, std::size_t index) {
  return impl->insert(std::move(img), index);
}
bool anim::remove(std::size_t index) { return impl->remove(index); }
bool anim::trim(std::size_t start, std::size_t end) {
  return impl->trim(start, end);
}

std::size_t anim::size() const noexcept { return impl->size(); }

anim::~anim() = default;
} // namespace seedimg
