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
target_link_libraries(tests seedimg-filters-core seedimg-autodetect stdc++fs)
file(COPY test_image.png DESTINATION "${CMAKE_BINARY_DIR}/${TESTS_EXE}/")

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

add_test(blur_test ${TESTS_EXE} blur)
set_tests_properties(blur_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(kernel_convolution_test ${TESTS_EXE} kernel_convolution)
set_tests_properties(kernel_convolution_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(rotate_hue_test ${TESTS_EXE} rotate_hue)
set_tests_properties(rotate_hue_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")
