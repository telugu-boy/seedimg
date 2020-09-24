/***********************************************************************
    seedimg - module based image manipulation library written in modern C++
    Copyright (C) 2020 tripulse

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
#ifndef SEEDIMG_FILTERS_FILTERCHAIN
#define SEEDIMG_FILTERS_FILTERCHAIN

#include <seedimg.hpp>
#include <vector>
#include <functional>

namespace seedimg::filters {
/**
 * @brief Lazily evaluated linear chain of filters wrapping I/O based filter-functions.
 * This is equavalient of wrapping multiple filter functions in a single function,
 * except it holds an underlying queue.
 *
 * @note Result of invoking a filter function is ignored.
 * @note Filters that change dimensions of image cause potential UB.
 */
class filterchain {
private:
    std::vector<std::function<void(simg&, simg&)>> filters;
public:
    /**
     * @brief Push a function to end of the queue that follows the idioms of seedimg
     * filter-function definitions. It will bind any number of additional arguments
     * to the function (if it requires and given).
     *
     * @note Due to limitation with C++ parameter packs, default arguments of filter
     * function also must be specified when adding.
     *
     * @param func a callable object.
     * @param args custom arguments to pass to callable.
     */
    template<class F, class... Args>
    filterchain& add(F&& func, Args&&... args) {
        filters.push_back([&](simg& input, simg& output) -> void {
            func(input, output, std::forward<Args>(args)...);
        });

        return *this;
    }

    /**
     * @brief Pop-off the most recently added filter in queue.
     */
    filterchain& pop() {
        filters.pop_back();
        return *this;
    }

    /**
     * @brief Apply enqueued filters accumulatively from start to end.
     * @note If queue was empty, it just copies image data.
     *
     * @param input image to apply filters on.
     * @param output output of the accumulated result.
     */
    filterchain& eval(const simg& in, simg& out) {
        // copy all pixels first into output, for "f(out, out)" to work.
        std::copy(in->data(),
                  in->data() + in->width() * in->height(),
                  out->data());

        for(const auto& f: filters)
          f(out, out);

        return *this;
    }

    filterchain& eval(simg& img) {
        eval(img, img);
        return *this;
    }

    /**
     * @brief Same effect as a single image but evalualtes on multiple frames,
     * and does it inplace to avoid temporary allocations.
     *
     * @remark Should we do it I/O based here?
     *
     * @param imgs images to transform (inplace).
     * @param start index to start with.
     * @param end index to end with (0 = imgs.size() - 1).
     */
    filterchain& eval(anim& imgs, simg_int start=0,
                                    simg_int end=0) {
        end = end ? end : imgs.size();
        if(end <= start)  // if nothing to do or invalid range.
            return *this;

        for(simg_int i = start; i < end; ++i)
          eval(imgs[i]);
        return *this;
    }
};

/**
 * @brief Lazily evaluated linear chain of filters wrapping inplace filter-functions.
 * This is equavalient of wrapping multiple filter functions in a single function,
 * except it holds an underlying queue.
 *
 * @note Result of invoking a filter function is ignored.
 */
class filterchain_i {
private:
    std::vector<std::function<void(simg&)>> filters;
public:
    /**
     * @brief Push a function to end of the queue that follows the idioms of seedimg
     * filter-function definitions. It will bind any number of additional arguments
     * to the function (if it requires and given).
     *
     * @note Due to limitation with C++ parameter packs, default arguments of filter
     * function also must be specified when adding.
     *
     * @param func a callable object.
     * @param args custom arguments to pass to callable.
     */
    template<class F, class... Args>
    filterchain_i& add(F&& func, Args&&... args) {
        filters.push_back([&](simg& input) -> void {
            func(input, std::forward<Args>(args)...);
        });

        return *this;
    }

    /**
     * @brief Pop-off the most recently added filter in queue.
     */
    filterchain_i& pop() {
        filters.pop_back();
        return *this;
    }

    /**
     * @brief Apply enqueued filters accumulatively from start to end.
     * @note If queue was empty, it just copies image data.
     *
     * @param input image to apply filters on.
     */
    filterchain_i& eval(simg& img) {
        for(const auto& f: filters)
          f(img);

        return *this;
    }

    /**
     * @brief Same effect as a single image but evalualtes on multiple frames.
     *
     * @param imgs images to transform (inplace).
     * @param start index to start with.
     * @param end index to end with (0 = imgs.size() - 1).
     */
    filterchain_i& eval(anim& imgs, simg_int start=0,
                                    simg_int end=0) {
        end = end ? end : imgs.size();
        if(end <= start)  // if nothing to do or invalid range.
            return *this;

        for(simg_int i = start; i < end; ++i)
          eval(imgs[i]);
        return *this;
    }
};
}
#endif
