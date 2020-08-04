set(APP_EXE example)

add_executable(${APP_EXE} main.cpp)

target_link_libraries(${APP_EXE}
    seedimg-png 
    seedimg-jpeg 
    seedimg-webp 
    seedimg-filters-core
)

include_directories(../)
