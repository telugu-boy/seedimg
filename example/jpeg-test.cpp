#include <seedimg-jpeg/seedimg-jpeg.hpp>

int main() {
  auto img = seedimg::modules::jpeg::from("violeur.jpg");

  // IJG quantization table scale factor: 8
  // Web friendly progressive JPEG:       yes
  seedimg::modules::jpeg::to("violeur_enc.jpg", img.value(), 8, true);
}