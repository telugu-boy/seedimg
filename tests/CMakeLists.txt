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
