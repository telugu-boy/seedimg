#include <seedimg-jpeg/seedimg-jpeg.hpp>

int main() {
  auto img = seedimg::modules::jpeg::from("violeur.jpg");

  // IJG quantization table scale factor: 75
  // Progressive JPEG:                    yes
  seedimg::modules::jpeg::to("violeur_enc.jpg", img.value(), 35, true);
}