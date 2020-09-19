#include <seedimg-filters/seedimg-filters-core.hpp>

namespace seedimg::filters {
void scanline(simg& input, simg& output, simg_int rows) {
    if(rows == 0) return;

    // if to alter group of scanlines or not.
    bool alter = false;

    for(simg_int y = 0; y < input->height(); ++y) {
        // once hit the boundary invert its state.
        if(y % rows == 0)
            alter = !alter;

        if(alter) {
            for(simg_int x = 0; x < input->width(); ++x) {
                auto& p = input->pixel(x, y);

                output->pixel(x, y) = seedimg::pixel{
                    static_cast<std::uint8_t>(p.r + 24),
                    static_cast<std::uint8_t>(p.g + 24),
                    static_cast<std::uint8_t>(p.b + 24),
                    p.a};
            }
        }
    }
}

// TODO: implement nearest neighbour scaling.
}
