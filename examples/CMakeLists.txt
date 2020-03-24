cmake_minimum_required(VERSION 3.15)
project(examples)

add_executable(simple simple.cpp)
target_link_libraries(simple patterns)
target_compile_features(simple PRIVATE cxx_std_17)

add_executable(advanced advanced.cpp)
target_link_libraries(advanced patterns)
target_compile_features(advanced PRIVATE cxx_std_17)