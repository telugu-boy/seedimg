add_library(seedimg-webp SHARED seedimg-webp.cpp)

find_package(WebP CONFIG REQUIRED)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(seedimg-webp PRIVATE WebP::webp WebP::webpdemux WebP::libwebpmux WebP::webpdecoder seedimg)
else()
    target_link_libraries(seedimg-webp PRIVATE WebP::webp WebP::webpdemux WebP::libwebpmux WebP::webpdecoder seedimg stdc++fs)
endif()
