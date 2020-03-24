cmake_minimum_required(VERSION 3.15)
project(patterns VERSION 0.1 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
target_include_directories(${PROJECT_NAME}
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

add_subdirectory(tests)
add_subdirectory(examples)