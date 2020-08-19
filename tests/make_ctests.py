header = """#[[ 
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
target_include_directories(tests PRIVATE "${CMAKE_SOURCE_DIR}/seedimg-filters")
file(COPY test_image.png DESTINATION "${CMAKE_BINARY_DIR}/${TESTS_EXE}/")

include_directories(../)

"""

template = """add_test({}_test ${{TESTS_EXE}} {})
set_tests_properties({}_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")\n\n"""

tests = ["grayscale_lum", "grayscale_avg", "invert", "invert_alpha","invert_alpha_only", "crop", "blur", "h_blur", "v_blur", "kernel_convolution", "rotate_hue", "brightness", "brightness_alpha", "blend", "blend_alpha","rotate_cw", "rotate_180", "rotate_ccw", "v_mirror", "h_mirror"]

if __name__ == "__main__":
    with open("CMakeLists.txt", "w") as f:
        f.write(header)
        for test in tests:
            f.write(template.format(test, test, test))
    
