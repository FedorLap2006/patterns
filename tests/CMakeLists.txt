cmake_minimum_required(VERSION 3.15)
project(test)

add_executable(${PROJECT_NAME} test.cpp)
target_link_libraries(${PROJECT_NAME} patterns)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

find_package(GTest REQUIRED)
target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${GTEST_INCLUDE_DIRS})
enable_testing()
add_test(${PROJECT_NAME} "./TestVisitor")