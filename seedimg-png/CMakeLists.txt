add_library(seedimg-png SHARED seedimg-png.cpp)

find_package(libpng CONFIG REQUIRED)
target_link_libraries(seedimg-png PRIVATE png_static stdc++fs)
