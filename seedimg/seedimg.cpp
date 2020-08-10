// seedimg.cpp : Defines the functions for the static library.
//

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>

#include "seedimg.hpp"

inline bool is_on_rect(seedimg::point xy1, seedimg::point xy2,
                       seedimg::point point) {
  return xy1.first <= point.first && point.first <= xy2.first &&
         xy1.second <= point.second && point.second <= xy2.second;
}

std::unique_ptr<seedimg::img> seedimg::from(const std::string &filename) {
  std::ifstream infile(filename, std::ios::binary);
  if (infile.fail())
    return nullptr;

  std::size_t width, height;
  infile.read(reinterpret_cast<char *>(&width), sizeof(width));
  infile.read(reinterpret_cast<char *>(&height), sizeof(height));

  std::error_code ec;
  auto size = std::filesystem::file_size(filename, ec);

  // check if the data size is equal to the retrieved rectangular area.
  // if not, then just don't return any results, partial data isn't supported.
  if (ec != std::error_code{} ||
      size != sizeof(seedimg::pixel) * width * height + sizeof(width) +
                  sizeof(height))
    return nullptr;

  auto image = std::make_unique<seedimg::img>(width, height);
  const auto stride =
      sizeof(seedimg::pixel) * static_cast<std::size_t>(image->width());

  for (int y = 0; y < image->height(); ++y)
    infile.read(reinterpret_cast<char *>(image->row(y)),
                static_cast<long>(stride));

  return image;
}

bool seedimg::to(const std::string &filename,
                 std::unique_ptr<seedimg::img> &image) {
  std::ofstream outfile(filename, std::ios::binary);
  if (outfile.fail())
    return false;

  outfile.write(reinterpret_cast<const char *>(image->width()),
                sizeof(image->width()));
  outfile.write(reinterpret_cast<const char *>(image->height()),
                sizeof(image->height()));

  const auto stride =
      sizeof(seedimg::pixel) * static_cast<std::size_t>(image->width());

  for (int y = 0; y < image->height(); ++y)
    outfile.write(reinterpret_cast<const char *>(image->row(y)),
                  static_cast<long>(stride));

  return true;
}

bool seedimg::img::crop(seedimg::point p1, seedimg::point p2) {
  if (!(is_on_rect({0, 0}, {this->width_, this->height_}, p1) &&
        is_on_rect({0, 0}, {this->width_, this->height_}, p2)))
    return false;
  auto ordered_crop_x = std::minmax(p1.first, p2.first);
  auto ordered_crop_y = std::minmax(p1.second, p2.second);
  this->width_ = ordered_crop_x.second - ordered_crop_x.first;
  this->height_ = ordered_crop_y.second - ordered_crop_y.first;

  // ordered_crop_points.first is the minimum distance from the left, so we
  // offset the memcpy src by it
  for (int y = 0; y < this->height_; ++y) {
    std::memmove(this->row(y),
                 this->row(y) + static_cast<std::size_t>(ordered_crop_x.first) *
                                    sizeof(seedimg::pixel),
                 static_cast<std::size_t>(this->width_) *
                     sizeof(seedimg::pixel));
    this->data_[y] = reinterpret_cast<seedimg::pixel *>(
        std::realloc(this->row(y), static_cast<std::size_t>(this->width_) *
                                       sizeof(seedimg::pixel)));
  }
  this->data_ = reinterpret_cast<seedimg::pixel **>(
      std::realloc(this->data_, static_cast<std::size_t>(this->height_) *
                                    sizeof(seedimg::pixel)));
  return true;
}
