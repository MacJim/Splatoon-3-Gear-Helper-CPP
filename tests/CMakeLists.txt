# GoogleTest
project(googletest)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(seed_helper_test seed_helper_test.cpp ../seed_helper.cpp)
target_link_libraries(seed_helper_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(seed_helper_test)
