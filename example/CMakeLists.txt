set(APP_EXE example)

add_executable(${APP_EXE} main.cpp)

target_link_libraries(${APP_EXE} 
    seedimg-filters-core
    seedimg-autodetect
)

include_directories(../)
