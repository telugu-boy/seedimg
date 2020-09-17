#[[ 
seedimg - module based image manipulation library written in modern C++ 
Copyright (C) 2020 telugu-boy 
 
This program is free software: you can redistribute it and/or modify 
it under the terms of the GNU Lesser General Public License as published by 
the Free Software Foundation, either version 3 of the License, or 
(at your option) any later version. 
 
This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
GNU Lesser General Public License for more details. 
 
You should have received a copy of the GNU Lesser General Public License 
along with this program.  If not, see <http://www.gnu.org/licenses/>. 
]]
set(TESTS_EXE tests)

add_executable(${TESTS_EXE} test-filters-core.cpp)

target_link_libraries(${TESTS_EXE} seedimg-filters-core seedimg-filters-cconv seedimg-autodetect)

if(OPENCL_ENABLED)
    target_link_libraries(${TESTS_EXE} seedimg-filters-ocl)
    target_compile_definitions(${TESTS_EXE} -DSEEDIMG_TESTS_OCL)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(${TESTS_EXE} stdc++fs)
endif()

target_include_directories(${TESTS_EXE} PRIVATE "${CMAKE_SOURCE_DIR}/seedimg-filters")
file(COPY test_image.jpg DESTINATION "${CMAKE_BINARY_DIR}/${TESTS_EXE}/")

include_directories(../)

add_test(grayscale_lum_test ${TESTS_EXE} grayscale_lum)
set_tests_properties(grayscale_lum_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(grayscale_avg_test ${TESTS_EXE} grayscale_avg)
set_tests_properties(grayscale_avg_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(invert_test ${TESTS_EXE} invert)
set_tests_properties(invert_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(invert_alpha_test ${TESTS_EXE} invert_alpha)
set_tests_properties(invert_alpha_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(invert_alpha_only_test ${TESTS_EXE} invert_alpha_only)
set_tests_properties(invert_alpha_only_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(crop_test ${TESTS_EXE} crop)
set_tests_properties(crop_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(blur_test ${TESTS_EXE} blur)
set_tests_properties(blur_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(h_blur_test ${TESTS_EXE} h_blur)
set_tests_properties(h_blur_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(v_blur_test ${TESTS_EXE} v_blur)
set_tests_properties(v_blur_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(kernel_convolution_test ${TESTS_EXE} kernel_convolution)
set_tests_properties(kernel_convolution_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(rotate_hue_test ${TESTS_EXE} rotate_hue)
set_tests_properties(rotate_hue_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(brightness_test ${TESTS_EXE} brightness)
set_tests_properties(brightness_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(brightness_alpha_test ${TESTS_EXE} brightness_alpha)
set_tests_properties(brightness_alpha_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(blend_test ${TESTS_EXE} blend)
set_tests_properties(blend_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(blend_alpha_test ${TESTS_EXE} blend_alpha)
set_tests_properties(blend_alpha_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(rotate_cw_test ${TESTS_EXE} rotate_cw)
set_tests_properties(rotate_cw_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(rotate_180_test ${TESTS_EXE} rotate_180)
set_tests_properties(rotate_180_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(rotate_ccw_test ${TESTS_EXE} rotate_ccw)
set_tests_properties(rotate_ccw_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(v_mirror_test ${TESTS_EXE} v_mirror)
set_tests_properties(v_mirror_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(h_mirror_test ${TESTS_EXE} h_mirror)
set_tests_properties(h_mirror_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(saturation_test ${TESTS_EXE} saturation)
set_tests_properties(saturation_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(sepia_test ${TESTS_EXE} sepia)
set_tests_properties(sepia_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(rotate_hue_ocl_test ${TESTS_EXE} rotate_hue_ocl)
set_tests_properties(rotate_hue_ocl_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(grayscale_lum_ocl_test ${TESTS_EXE} grayscale_lum_ocl)
set_tests_properties(grayscale_lum_ocl_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(grayscale_avg_ocl_test ${TESTS_EXE} grayscale_avg_ocl)
set_tests_properties(grayscale_avg_ocl_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(sepia_ocl_test ${TESTS_EXE} sepia_ocl)
set_tests_properties(sepia_ocl_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

