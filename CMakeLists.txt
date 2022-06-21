cmake_minimum_required(VERSION 3.1)
project(chip8)

#set c++ standard
set(CMAKE_CXX_STANDARD 17)

include_directories(./include)

add_library(chip8 ./src/chip8.cpp)
add_executable(main ./src/main.cpp)

target_link_libraries(main chip8)