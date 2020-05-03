cmake_minimum_required(VERSION 3.5)

project(FT-stribog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(FT-stribog
    main.cpp
    ft_stribog.cpp ft_stribog.hpp
    stribog.cpp stribog.hpp
    stribog_constants.hpp)
