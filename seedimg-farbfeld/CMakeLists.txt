add_library(seedimg-farbfeld SHARED seedimg-farbfeld.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(seedimg-farbfeld PRIVATE seedimg)
else()
    target_link_libraries(seedimg-farbfeld PRIVATE stdc++fs seedimg)
endif()
