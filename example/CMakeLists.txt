set(APP_EXE example)

add_executable(${APP_EXE} main.cpp)

file(COPY cat.png DESTINATION "${CMAKE_BINARY_DIR}/${APP_EXE}/")

target_link_libraries(${APP_EXE} 
    seedimg-filters-core
    seedimg-autodetect
)

include_directories(../)
