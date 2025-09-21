# tests/CMakeLists.txt
add_executable(atm_tests
  test_examples.cpp
)

target_link_libraries(atm_tests
  PRIVATE
    atm_core
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(atm_tests)
