#ifndef SEEDIMG_PNG_H
#define SEEDIMG_PNG_H

#include <memory>
#include <libpng16/png.h>
#include "seedimg-core.h"

std::unique_ptr<seedimg_img> seedimg_from_png(png_structp);
std::unique_ptr<seedimg_img> seedimg_from_png_filename(std::string);

#endif