/**********************************************************************
    seedimg - module based image manipulation library written in modern
                C++ Copyright(C) 2020 telugu-boy, tripulse

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
#include <seedimg-autodetect.hpp>

#include <functional>
#include <cstring>
#include <fstream>

#include <seedimg-modules/modules-abc.hpp>
#include <seedimg-modules/farbfeld.hpp>
#include <seedimg-modules/irdump.hpp>
#include <seedimg-modules/jpeg.hpp>

#include <seedimg-utils.hpp>

using namespace seedimg::utils;

const auto FMT_DETECTORS = seedimg::utils::make_array(
    #define entry(fmt, size, funcb) \
        std::make_tuple<simg_imgfmt, std::streamsize, \
                        std::function<bool(const char* const)>> \
            (fmt, \
             size, \
             [](const char* const h) funcb)
    #define eq(a, b, s) (std::memcmp(a, b, s) == 0)


        /* format   sigsize           comparator */
    entry(farbfeld,   8,   { return eq(h, "farbfeld",         8); }),
    entry(jpeg,       4,   { return eq(h, "\xFF\xD8\xFF\xE0", 4); })
);

const auto MAX_SIGSIZE =
        std::get<1>(
            *std::max_element(
                std::begin(FMT_DETECTORS),
                std::end(FMT_DETECTORS),
                [](auto a, auto b) {
                    return std::get<1>(a) < std::get<1>(b);
         }));

static std::unordered_map<std::string, simg_imgfmt> EXT_TO_FMT = {
    {"ff",   farbfeld},
    {"sir",  irdump  },
    {"jpg",  jpeg    },
    {"jpeg", jpeg    },
    {"jpe",  jpeg    },
    {"jif",  jpeg    },
    {"jfif", jpeg    },
    {"jfi",  jpeg    },
};


simg_imgfmt detect_format(std::istream& buf) {
    auto signature = new char[static_cast<std::size_t>(MAX_SIGSIZE)];
    buf.read(signature, MAX_SIGSIZE);  // read to size of most lengthy signature.

    simg_imgfmt fmt = unknown;
    for(auto fmtdet : FMT_DETECTORS) {
        if(std::get<2>(fmtdet)(signature)) {
            fmt = std::get<0>(fmtdet);
            break;
        }
    }

    for(std::streamsize i = 0; i < MAX_SIGSIZE; ++i)
        buf.unget();

    delete[] signature;
    return fmt;
}

template<typename I>
simg read_img(std::istream& buf) {
    I reader{buf};
    auto img = seedimg::make(reader.width(), reader.height());

    for(simg_int r = 0; r < img->height(); ++r)
        if(!reader.read(img->row(r)))
            return nullptr;
    return img;
}

template<typename O>
bool write_img(std::ostream& buf, const simg& img) {
    O writer{buf, img->width(), img->height()};

    for(simg_int r = 0; r < img->height(); ++r)
        if(!writer.write(img->row(r)))
            return false;
    return true;
}


namespace seedimg {
    using namespace seedimg::modules;

    simg load(std::istream& buf) {
        switch(detect_format(buf)) {
            case farbfeld: return read_img<decoding::farbfeld>(buf);
            case jpeg:     return read_img<decoding::jpeg>    (buf);
            default:       return nullptr;
        }
    }

    simg load(const std::string& filename) {
        std::ifstream in(filename);
        return load(in);
    }

    bool save(std::ostream& buf, simg_imgfmt fmt, const simg& img) {
        if(img == nullptr)
            return false;

        switch(fmt) {
            case farbfeld: return write_img<encoding::farbfeld>(buf, img);
            case irdump:   return write_img<encoding::irdump>  (buf, img);
            case jpeg:     return write_img<encoding::jpeg>    (buf, img);
            default:       return false;
        }
    }

    bool save(const std::string& filename, const simg& img) {
        std::ofstream out(filename);
        return save(out, EXT_TO_FMT[filename.substr(filename.rfind('.') + 1)], img);
    }
}
