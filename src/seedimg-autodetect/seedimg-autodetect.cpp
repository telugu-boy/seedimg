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

#include <seedimg-utils.hpp>


constexpr const auto FMT_DETECTORS = seedimg::utils::make_array(
    #define F(fmt, size, funcb) std::make_tuple( \
            simg_imgfmt::fmt, \
            static_cast<std::size_t>(size), \
            [](const char* const h) funcb)

    #define EQ(a, b, s) (std::memcmp(a, b, s) == 0)

    /* format   sigsize           comparator */
    F(farbfeld,    8,   { return EQ(h, "farbfeld", 8); })
);

constexpr const auto MAX_SIGSIZE = std::get<1>(*std::max_element(
            std::begin(FMT_DETECTORS),
            std::end(FMT_DETECTORS),
            [](auto a, auto b) { return std::get<1>(a) < std::get<1>(b);
        }));


static std::unordered_map<std::string, simg_imgfmt> EXT2FMT = {
    { "ff", simg_imgfmt::farbfeld},
    {"sir", simg_imgfmt::irdump},
};


simg_imgfmt detect_format(std::istream& buf) {
    char sig[MAX_SIGSIZE];
    buf.read(sig, MAX_SIGSIZE);

    simg_imgfmt fmt = simg_imgfmt::unknown;

    for(auto fmtdet : FMT_DETECTORS) {
        if(std::get<2>(fmtdet)(sig)) {
            fmt = std::get<0>(fmtdet);
            break;
        }
    }

    for(size_t i = 0; i < MAX_SIGSIZE; ++i)
        buf.unget();  // unextract read chars.

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
        #define impli(fmt) case simg_imgfmt::fmt: return read_img<decoding::fmt>(buf)

        switch(detect_format(buf)) {
              impli  (farbfeld);
            default: return nullptr;
        }
    }

    simg load(const std::string& filename) {
        std::ifstream in(filename);
        return load(in);
    }

    bool save(std::ostream& buf, simg_imgfmt fmt, const simg& img) {
        #define implo(fmt) case simg_imgfmt::fmt: return write_img<encoding::fmt>(buf, img)
        switch(fmt) {
              implo  (farbfeld);
              implo  (irdump);
            default: return false;
        }
    }

    bool save(const std::string& filename, const simg& img) {
        std::ofstream out(filename);
        return save(out,
                    EXT2FMT[filename.substr(
                                filename.rfind('.') + 1
                    )], img);
    }
}
