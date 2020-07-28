// #include <seedimg-jpeg/seedimg-jpeg.hpp>
#include <seedimg/seedimg.hpp>

int main() {
  // auto img = seedimg::modules::jpeg::from("violeur.jpg").value();

  // IJG quantization table scale factor: 8
  // Web friendly progressive JPEG:       yes
  // seedimg::modules::jpeg::to("violeur_enc.jpg", img);

  auto image = std::make_unique<seedimg::img>(1024, 512);
  seedimg::to("blank_image.simg", image);
}