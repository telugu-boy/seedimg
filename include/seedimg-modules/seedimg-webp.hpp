/**********************************************************************
    seedimg - module based image manipulation library written in modern
                C++ Copyright(C) 2020 telugu-boy

    This program is free software : you can redistribute it and /
    or modify it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation,
    either version 3 of the License,
    or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/
#ifndef SEEDIMG_WEBP_H
#define SEEDIMG_WEBP_H

#include <cstring>
#include <filesystem>
#include <fstream>

#include <seedimg.hpp>

extern "C" {
#include <webp/decode.h>
#include <webp/demux.h>
#include <webp/encode.h>
#include <webp/mux.h>
}

namespace seedimg::modules {
namespace webp {
bool check(const std::string& filename) noexcept {
    std::ifstream   file(filename);
    std::error_code e;
    std::size_t     s = std::filesystem::file_size(filename, e);

    if (e != std::error_code() || s < 12) return false;

    std::uint8_t hdr[12];
    file.read(reinterpret_cast<char*>(hdr), 12);

    return !std::memcmp(hdr, "RIFF", 4) || !std::memcmp(hdr + 8, "WEBP", 4);
}

anim from(const std::string& filename, std::size_t max_frames) {
    anim          images;
    std::ifstream in(filename);

    struct {
        WebPData data;
        size_t   image_size;

        struct {
            WebPAnimDecoder* h;
            WebPAnimInfo     info;
            char             _[4];
        } anim;

        struct {
            int start;
            int end;
        } ts;
    } webp;

    // copy-in the contents from the iterator.
    std::string buf(std::istreambuf_iterator<char>{in}, std::istreambuf_iterator<char>());

    webp.data.bytes = reinterpret_cast<const uint8_t*>(buf.data());
    webp.data.size  = buf.size();

    webp.anim.h = WebPAnimDecoderNew(&webp.data, nullptr);
    WebPAnimDecoderGetInfo(webp.anim.h, &webp.anim.info);

    // amount of data to memcpy into seedimg::img buffer.
    webp.image_size = webp.anim.info.canvas_width * 4 * webp.anim.info.canvas_height;

    auto WebPPushFrame = [&]() -> int {
        uint8_t* buf;
        int      ts;

        if (!WebPAnimDecoderGetNext(webp.anim.h, &buf, &ts)) return -1;

        auto img = seedimg::make(webp.anim.info.canvas_width, webp.anim.info.canvas_height);

        std::memcpy(img->data(), buf, webp.image_size);
        images.add(std::move(img));

        return ts;
    };

    webp.ts.start = WebPPushFrame();
    webp.ts.end   = WebPPushFrame();

    // if atleast two consecutive frames were found; calculate the delta,
    // considering it a time step-factor calculate the framerate.
    images.framerate
        = webp.ts.start == -1 || webp.ts.end == -1 ? 0 : static_cast<std::size_t>(1000 / (webp.ts.end - webp.ts.start));

    max_frames = !max_frames ? webp.anim.info.frame_count : max_frames;

    // only decode the specified amount of frames, discard rest.
    for (std::size_t f = 0; f < max_frames; ++f)
        if (WebPPushFrame() == -1) break;

    WebPAnimDecoderDelete(webp.anim.h);
    return images;
}

bool to(const std::string& filename, const simg& img, float quality = 100.0, bool lossless = false) {
    std::ofstream file(filename);

    std::uint8_t* out;
    std::size_t   size;

    if (lossless)
        size = WebPEncodeRGBA(reinterpret_cast<std::uint8_t*>(img->data()),
                              static_cast<int>(img->width()),
                              static_cast<int>(img->height()),
                              static_cast<int>(img->width() * sizeof(seedimg::pixel)),
                              quality,
                              &out);
    else
        size = WebPEncodeLosslessRGBA(reinterpret_cast<std::uint8_t*>(img->data()),
                                      static_cast<int>(img->width()),
                                      static_cast<int>(img->height()),
                                      static_cast<int>(img->width() * sizeof(seedimg::pixel)),
                                      &out);

    try {
        file.write(reinterpret_cast<char*>(out), static_cast<std::streamsize>(size));
    } catch (std::ios_base::failure) { return false; }
    file.close();

    return size; // size == 0 ? false : true
}

bool to(const std::string&                    filename,
        const anim&                           images,
        std::pair<std::uint8_t, std::uint8_t> quality  = {100, 100},
        bool                                  lossless = false,
        bool                                  loop     = false) {
    if (!images.size()) return true;

    // since the effect is same and many decoders don't support
    // ANIM and ANMF chunks, they count it as invalid file.
    else if (images.size() == 1)
        to(filename, images[0], quality.first, lossless);

    std::ofstream out(filename);

    struct {
        WebPData   data;
        WebPConfig cfg;

        char        _[4];
        WebPPicture pic;
        struct {
            WebPAnimEncoder*       h;
            WebPAnimEncoderOptions opts;
            char                   _[4];
        } anim;

        struct {
            int counter = 0;
            int step;
        } ts;
    } webp;

    if (!WebPAnimEncoderOptionsInit(&webp.anim.opts) || !WebPConfigInit(&webp.cfg) || !WebPPictureInit(&webp.pic))
        return false;

    webp.cfg.lossless      = lossless;
    webp.cfg.quality       = quality.first;
    webp.cfg.alpha_quality = quality.second;

    webp.pic.width  = static_cast<int>(images[0]->width());
    webp.pic.height = static_cast<int>(images[0]->height());

    if (!WebPPictureAlloc(&webp.pic)) return false;

    webp.anim.opts.anim_params.loop_count = !loop;
    webp.ts.step                          = 1000 / images.framerate;

    webp.anim.h = WebPAnimEncoderNew(
        static_cast<int>(images[0]->width()), static_cast<int>(images[0]->height()), &webp.anim.opts);

    for (const auto& frame : images) {
        // only rescale picture-buffer if needed.
        if (frame->width() != static_cast<simg_int>(webp.pic.width)
            || frame->height() != static_cast<simg_int>(webp.pic.height))
            WebPPictureRescale(&webp.pic, static_cast<int>(frame->width()), static_cast<int>(frame->height()));
        WebPPictureImportRGBA(&webp.pic,
                              reinterpret_cast<std::uint8_t*>(frame->data()),
                              static_cast<int>(frame->width() * sizeof(seedimg::pixel)));

        if (!WebPAnimEncoderAdd(webp.anim.h, &webp.pic, webp.ts.counter, &webp.cfg)) break;
        webp.ts.counter += webp.ts.step;
    }

    WebPPictureFree(&webp.pic);
    if (!WebPAnimEncoderAssemble(webp.anim.h, &webp.data)) return false;
    WebPAnimEncoderDelete(webp.anim.h);

    try {
        out.write(reinterpret_cast<char*>(const_cast<std::uint8_t*>(webp.data.bytes)),
                  static_cast<int>(webp.data.size));
        free(const_cast<std::uint8_t*>(webp.data.bytes));
    } catch (std::iostream::failure) { return false; }
    return true;
}

} // namespace webp
} // namespace seedimg::modules

#endif
