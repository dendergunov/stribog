cmake_minimum_required(VERSION 3.10)

#set the project name and version
project(StribogFT VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#add the executable
add_executable(StribogFT main.cpp stribog.cpp)

target_include_directories(StribogFT PUBLIC 
							"${PROJECT_BINARY_DIR}"
							)

