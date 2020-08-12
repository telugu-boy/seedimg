add_library(seedimg-jpeg SHARED seedimg-jpeg.cpp)

find_package(JPEG REQUIRED)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(seedimg-jpeg PRIVATE ${JPEG_LIBRARIES} seedimg)
else()
    target_link_libraries(seedimg-jpeg PRIVATE ${JPEG_LIBRARIES} seedimg stdc++fs)
endif()

target_include_directories(seedimg-jpeg PRIVATE ${JPEG_INCLUDE_DIR} seedimg)

