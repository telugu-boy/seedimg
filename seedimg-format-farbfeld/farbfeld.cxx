#include "farbfeld.hxx"

#include <fstream>

static std::uint16_t fu16be(uint8_t *cb) {
    return cb[1] << 8 | cb[0];
}
static void tu16be(std::uint8_t* out, std::uint16_t n) {
    out[0] = n >> 8 & 0xff;
    out[1] = n & 0xff;
}
static std::uint32_t fu32be(uint8_t *cb) {
    return cb[0] << 24 | cb[1] << 16 | cb[2] << 8 | cb[3];
}
static std::uint8_t* tu32be(std::uint32_t n) {
    auto out = (uint8_t*)malloc(4);
    out[0] = (n >> 24) & 0xff;
    out[1] = (n >> 16) & 0xff;
    out[2] = (n >> 8) & 0xff;
    out[3] = n & 0xff;
    return out;
}

// https://stackoverflow.com/a/41762509/10978642
static std::uint8_t b16_8(std::uint16_t n) { return n >> 8; }
// https://stackoverflow.com/a/6436100/10978642
// NOTE: simple bitshifting could've solved but would
//       be quite noisy, so an addition put.
static std::uint16_t b8_16(std::uint8_t n) { return (n << 8) + n; }

namespace seedimg::modules::farbfeld {
bool check(const std::string& filename) noexcept {
    std::ifstream input(filename);
    char sig[8];

    try { input.read(sig, 8); }
    catch(std::iostream::failure) { return false; }
    return std::memcmp(sig, "farbfeld", 8) == 0;
}

std::optional<std::unique_ptr<seedimg::img>> from(const std::string& filename) noexcept {
    std::ifstream input(filename);
    struct {
        char sig[8];
        uint8_t width[4];
        uint8_t height[4];
    } rawinfo;

    try {
        input.read(rawinfo.sig, 8)
             .read(reinterpret_cast<char*>(rawinfo.width), 4)
             .read(reinterpret_cast<char*>(rawinfo.height), 4);
    } catch(std::ios_base::failure) { return std::nullopt; }

    // siganture match is mandatory else invalid Farbfeld file.
    if(std::memcmp(rawinfo.sig, "farbfeld", 8) != 0)
        return std::nullopt;

    auto result = std::make_unique<seedimg::img>(fu32be(rawinfo.width), fu32be(rawinfo.height));

    uint8_t rawpixel[8];
    for(std::uint32_t y = 0; y < result->height; ++y) {
        for(std::uint32_t x = 0; x < result->width; ++x ) {
            try { input.read(reinterpret_cast<char*>(rawpixel), 8); }
            catch(std::ios_base::failure) { return std::nullopt; }

            result->pixel(x,y) = {b16_8(fu16be(rawpixel)),     b16_8(fu16be((rawpixel+2))),
                                  b16_8(fu16be((rawpixel+4))), b16_8(fu16be((rawpixel+6)))};
        }
    }

    return result;
}

bool to(const std::string& filename, const std::unique_ptr<seedimg::img>& image) noexcept {
    std::ofstream output(filename);

    try {
        output.write("farbfeld", 8)
              .write(reinterpret_cast<char*>(tu32be(image->width)), 4)
              .write(reinterpret_cast<char*>(tu32be(image->height)), 4);
    } catch (std::ios_base::failure) { return false; }

    uint8_t rawpixel[8];
    for(std::uint32_t y = 0; y < image->height; ++y) {
        for(std::uint32_t x = 0; x < image->width; ++x ) {
            auto px = image->pixel(x, y);

            tu16be(rawpixel, b8_16(px.r)); tu16be(rawpixel+2, b8_16(px.g));
            tu16be(rawpixel, b8_16(px.b)); tu16be(rawpixel+6, b8_16(px.a));

            try { output.write(reinterpret_cast<char*>(rawpixel), 8); }
            catch(std::ios_base::failure) { return false; }
        }
    }

    return true;
}
}
