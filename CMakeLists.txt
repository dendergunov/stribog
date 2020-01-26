cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(StribogFT LANGUAGES CXX)

add_executable(${PROJECT_NAME} main.cpp stribog.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

