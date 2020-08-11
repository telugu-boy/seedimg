add_library(seedimg-autodetect SHARED seedimg-autodetect.cpp)

target_link_libraries(seedimg-autodetect
    seedimg-png 
    seedimg-jpeg 
    seedimg-webp
    seedimg-farbfeld
    seedimg-filters-core
)
