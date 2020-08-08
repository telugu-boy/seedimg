set(TESTS_EXE tests)

add_executable(${TESTS_EXE} test-filters-core.cpp)
target_link_libraries(tests seedimg-filters-core seedimg-autodetect stdc++fs)
file(COPY violeur.png DESTINATION "${CMAKE_BINARY_DIR}/${TESTS_EXE}/")

include_directories(../)

add_test(grayscale_test ${TESTS_EXE} grayscale)
set_tests_properties(grayscale_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(invert_test ${TESTS_EXE} invert)
set_tests_properties(invert_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(crop_test ${TESTS_EXE} crop)
set_tests_properties(crop_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")
