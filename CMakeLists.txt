cmake_minimum_required(VERSION 3.13)
project(seedimg_full VERSION 1.0 DESCRIPTION "seedimg module based image manipulation library")

set(CMAKE_CXX_STANDARD 17)

# core
add_subdirectory(seedimg)

# image modules
add_subdirectory(seedimg-jpeg)
add_subdirectory(seedimg-png)
add_subdirectory(seedimg-webp)

# filter modules
add_subdirectory(seedimg-filters-core)

# dependent project
add_subdirectory(example)
