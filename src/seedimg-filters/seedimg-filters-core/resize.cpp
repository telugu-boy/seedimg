#include <seedimg-filters/seedimg-filters-core.hpp>

namespace seedimg::filters {
void resize(simg& input, simg& output) {
    if(input->width()  == output->width() &&
       input->height() == output->height()) {
        output = seedimg::make(input);
    }

    // find out the nearest pixel coordinate in input image,
    // from the output's coordinates. this is a very fast
    // and easy algorithm but yields poor quality.
    for(float y = 0; y < output->height(); ++y) {
        for(float x = 0; x < output->width(); ++x) {
            output->pixel(static_cast<simg_int>(x),
                          static_cast<simg_int>(y)) = input->pixel(
                static_cast<simg_int>(x/output->width()  * input->width()),
                static_cast<simg_int>(y/output->height() * input->height()));
        }
    }
}
}
