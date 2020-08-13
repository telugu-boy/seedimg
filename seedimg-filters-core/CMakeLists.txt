add_library(seedimg-filters-core SHARED
    invert.cpp
    grayscale.cpp
    blur.cpp
    convolution.cpp)

target_link_libraries(seedimg-filters-core seedimg)
