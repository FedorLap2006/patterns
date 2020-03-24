cmake_minimum_required(VERSION 3.15)
project(examples)

add_executable(simple simpleVisitorExample.cpp)
target_link_libraries(simple patterns)
target_compile_features(simple PRIVATE cxx_std_17)