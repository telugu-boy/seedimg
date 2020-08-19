add_library(seedimg-filters-ocl ${LIB_TYPE}
    rotate-hue-ocl.cpp
)

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})
target_link_libraries(seedimg-filters-ocl ${OpenCL_LIBRARY} seedimg)
target_include_directories(seedimg-filters-ocl PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    "${CMAKE_SOURCE_DIR}/seedimg-filters"
    seedimg)
