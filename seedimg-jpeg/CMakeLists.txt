add_library(seedimg-jpeg SHARED seedimg-jpeg.cpp)

find_package(JPEG REQUIRED)
include_directories(${JPEG_INCLUDE_DIR})
target_link_libraries(seedimg-jpeg seedimg ${JPEG_LIBRARY} stdc++fs)
