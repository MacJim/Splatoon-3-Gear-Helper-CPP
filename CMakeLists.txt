cmake_minimum_required(VERSION 3.20)

project(Splatoon-3-Gear-Helper-CPP)

set(CMAKE_CXX_STANDARD 17)

add_executable(Splatoon-3-Gear-Helper-CPP main.cpp seed_helper.cpp data/abilities.cpp)

add_subdirectory(tests)
