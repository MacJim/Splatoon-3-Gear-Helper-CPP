project(googletest)


# Fetch external dependencies.
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
    yaml-cpp
    URL https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-0.7.0.zip
)
FetchContent_MakeAvailable(yaml-cpp)


# Tests.
enable_testing()

add_executable(seed_helper_test seed_helper_test.cpp ../seed_helper.cpp ../data/abilities.cpp ../data/roll_sequence.cpp)
target_link_libraries(seed_helper_test GTest::gtest_main)

add_executable(roll_sequence_test roll_sequence_test.cpp roll_randomizer.cpp ../data/roll_sequence.cpp)
target_link_libraries(roll_sequence_test GTest::gtest_main)

add_executable(yaml_helper_test yaml_helper_test.cpp ../yaml/yaml_helper.cpp roll_randomizer.cpp ../data/ability.cpp)
target_link_libraries(yaml_helper_test yaml-cpp GTest::gtest_main)

add_executable(ability_helper_test ability_helper_test.cpp ../data/ability.cpp)
target_link_libraries(ability_helper_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(seed_helper_test)
gtest_discover_tests(roll_sequence_test)
#gtest_discover_tests(yaml_helper_test)
