cmake_minimum_required(VERSION 3.5)

project(xor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(xor main.cpp ft_xor.cpp ft_xor.hpp)
