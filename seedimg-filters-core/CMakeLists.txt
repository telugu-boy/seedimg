add_library(seedimg-filters-core SHARED seedimg-filters-core.cpp invert.cpp grayscale.cpp)

target_link_libraries(seedimg-filters-core seedimg)
