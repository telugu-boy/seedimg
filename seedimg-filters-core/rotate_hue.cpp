#include "seedimg-filters-core.hpp"
#include <cmath>
#include <memory>
#include <seedimg/seedimg.hpp>
#include <thread>

const double PI = 4 * std::atan(1);

double clamp(double num) {
  if (num < 0.0) {
    return 0.0;
  } else if (num > 255.0) {
    return 255.0;
  } else {
    return num;
  }
}

void rotate_hue_worker(std::unique_ptr<seedimg::img> &inp_img,
                       std::unique_ptr<seedimg::img> &res_img, simg_int start,
                       simg_int end, const double hue_kernel[9]) {

  for (; start < end; start++) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      auto inp_pix = inp_img->pixel(x, start);
      auto &res_pix = res_img->pixel(x, start);
      res_pix.r = static_cast<std::uint8_t>(clamp(hue_kernel[0] * inp_pix.r +
                                                  hue_kernel[1] * inp_pix.g +
                                                  hue_kernel[2] * inp_pix.b));
      res_pix.g = static_cast<std::uint8_t>(clamp(hue_kernel[3] * inp_pix.r +
                                                  hue_kernel[4] * inp_pix.g +
                                                  hue_kernel[5] * inp_pix.b));
      res_pix.b = static_cast<std::uint8_t>(clamp(hue_kernel[6] * inp_pix.r +
                                                  hue_kernel[7] * inp_pix.g +
                                                  hue_kernel[8] * inp_pix.b));
    }
  }
}

void get_hue_kernel(int angle, double hue_kernel[9]) {
  const double sinr = std::sin(angle * PI / 180);
  const double cosr = std::cos(angle * PI / 180);
  hue_kernel[0] = 0.213 + cosr * 0.787 - sinr * 0.213;
  hue_kernel[1] = 0.715 - cosr * 0.715 - sinr * 0.715;
  hue_kernel[2] = 0.072 - cosr * 0.072 + sinr * 0.928;
  hue_kernel[3] = 0.213 - cosr * 0.213 + sinr * 0.143;
  hue_kernel[4] = 0.715 + cosr * 0.285 + sinr * 0.140;
  hue_kernel[5] = 0.072 - cosr * 0.072 - sinr * 0.283;
  hue_kernel[6] = 0.213 - cosr * 0.213 - sinr * 0.787;
  hue_kernel[7] = 0.715 - cosr * 0.715 + sinr * 0.715;
  hue_kernel[8] = 0.072 + cosr * 0.928 + sinr * 0.072;
}

namespace seedimg::filters {
void rotate_hue(std::unique_ptr<seedimg::img> &inp_img,
                std::unique_ptr<seedimg::img> &res_img, int angle) {
  double hue_kernel[9];
  get_hue_kernel(angle, hue_kernel);
  auto start_end = inp_img->start_end_rows();
  std::vector<std::thread> workers(start_end.size());
  for (std::size_t i = 0; i < workers.size(); i++) {
    workers.at(i) =
        std::thread(rotate_hue_worker, std::ref(inp_img), std::ref(res_img),
                    start_end.at(i).first, start_end.at(i).second, hue_kernel);
  }
  for (std::size_t i = 0; i < workers.size(); ++i)
    workers.at(i).join();
}

void rotate_hue_i(std::unique_ptr<seedimg::img> &inp_img, int angle) {
  double hue_kernel[9];
  get_hue_kernel(angle, hue_kernel);
}
} // namespace seedimg::filters
