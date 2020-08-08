add_executable(tests test-filters-core.cpp)
target_link_libraries(tests seedimg-filters-core seedimg-autodetect stdc++fs)
file(COPY violeur.png DESTINATION "${CMAKE_BINARY_DIR}/tests/")

include_directories(../)

add_test(grayscale_test tests grayscale)
set_tests_properties(grayscale_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(invert_test tests invert)
set_tests_properties(invert_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(crop_test tests crop)
set_tests_properties(crop_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")
