add_library(seedimg-filters-ocl ${LIB_TYPE}
    ocl-singleton.cpp
    init-ocl-singleton.cpp
    rotate-hue-ocl.cpp
    grayscale-ocl.cpp
    hsv-ocl.cpp
    rgb-ocl.cpp
)

find_package(OpenCL REQUIRED)
link_directories(${OpenCL_LIBRARY})
target_link_libraries(seedimg-filters-ocl ${OpenCL_LIBRARY} seedimg)
target_include_directories(seedimg-filters-ocl PRIVATE
    ${OpenCL_INCLUDE_DIRS}
    seedimg)
