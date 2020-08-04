add_library(seedimg-webp SHARED seedimg-webp.cpp)

find_package(WebP CONFIG REQUIRED)
target_link_libraries(seedimg-webp PRIVATE WebP::webp WebP::webpdemux WebP::libwebpmux WebP::webpdecoder stdc++fs)
