add_library(seedimg-png SHARED seedimg-png.cpp)

find_package(libpng CONFIG REQUIRED)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(seedimg-png PRIVATE png_static seedimg)
else()
    target_link_libraries(seedimg-png PRIVATE png_static seedimg stdc++fs)
endif()
