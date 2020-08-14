#include <algorithm>
#include <cmath>
#include <numeric>

#include <seedimg-filters-core/seedimg-filters-core.hpp>
#include <seedimg/seedimg.hpp>

void seedimg::filters::convolution(std::unique_ptr<seedimg::img> &input,
                                   std::vector<std::vector<float>> kernel) {
  seedimg::point kernel_dims{kernel.size() ? kernel[0].size() : 0,
                             kernel.size()};
  // NOTE: only square aka. NxN kernels are allowed as they're symmetric and
  //       thus very easy to implement, if not then just ignore.
  if (kernel_dims.first != kernel_dims.second || kernel_dims.first == 0)
    return;

  {
    // will be used as the divisor to normalize all the kernel elements.
    auto kernmatrix_normdiv = std::abs(std::accumulate(
        kernel.begin(), kernel.end(), 0.0, [](float s, auto r) -> float {
          return s + std::accumulate(r.begin(), r.end(), 0.0);
        }));

    if (kernmatrix_normdiv == 0) // prevent zero-division UB,
      kernmatrix_normdiv = 1;    // it being zero doesn't mean discard.

    for (simg_int r = 0; r < kernel_dims.second; ++r)
      std::transform(
          kernel[r].begin(), kernel[r].end(), kernel[r].begin(),
          [&](auto elem) -> float { return elem / kernmatrix_normdiv; });
  }

  // the centerpoint of the kernel, where the current pixel is located.
  // kernel { y-axis: [-a..+a], x-axis: [-b..b] }, (a) and (b) are half
  // of kernel's (width) and (height).
  seedimg::point kernel_origin{kernel_dims.first / 2, kernel_dims.second / 2};

  for (ssize_t y = 0; y < static_cast<ssize_t>(input->height()); ++y) {
    for (ssize_t x = 0; x < static_cast<ssize_t>(input->width()); ++x) {
      // output value to be used as an accumulator.
      // this will be resulting pixel in the kernel's origin.
      seedimg::pixel outpix{0, 0, 0, input->pixel(x, y).a};

      for (ssize_t dy = 0; dy < static_cast<ssize_t>(kernel_dims.second);
           ++dy) {
        for (ssize_t dx = 0; dx < static_cast<ssize_t>(kernel_dims.first);
             ++dx) {
          // NOTE: for edge handling a mixture of mirroring and wrapping is
          // done. wrapping when it reaches the end, mirroring when it reaches
          // the start.
          auto pix = input->pixel(
              labs(x + dx - static_cast<ssize_t>(kernel_origin.first)) %
                  input->width(),
              labs(y + dy - static_cast<ssize_t>(kernel_origin.second)) %
                  input->height());

          // TODO: alpha isn't altered, need to add an option for it.
          outpix.r += pix.r * kernel[dy][dx];
          outpix.g += pix.g * kernel[dy][dx];
          outpix.b += pix.b * kernel[dy][dx];
        }
      }
      input->pixel(x, y) = outpix;
    }
  }
}
