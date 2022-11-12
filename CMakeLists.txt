cmake_minimum_required(VERSION 3.20)

project(Splatoon-3-Gear-Helper-CPP)

set(CMAKE_CXX_STANDARD 17)

# 2 executables: `find`, `predict`
add_executable(find find.cpp seed_helper.cpp data/ability.cpp data/roll_sequence.cpp yaml/yaml_helper.cpp)

include(FetchContent)
FetchContent_Declare(
    yaml-cpp
    URL https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-0.7.0.zip
)
FetchContent_MakeAvailable(yaml-cpp)
target_link_libraries(find yaml-cpp)

# Tests.
add_subdirectory(tests EXCLUDE_FROM_ALL)
