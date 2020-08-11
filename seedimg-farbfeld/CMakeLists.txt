add_library(seedimg-farbfeld SHARED seedimg-farbfeld.cpp)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(seedimg-farbfeld seedimg)
else()
    target_link_libraries(seedimg-farbfeld stdc++fs seedimg)
endif()
