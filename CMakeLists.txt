cmake_minimum_required(VERSION 3.15)
project(chip8)

#set c++ standard
set(CMAKE_CXX_STANDARD 17)
set(SDL2_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/deps/SDL2/include/SDL2)
set(SDL2_LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/deps/SDL2//lib)

link_directories(${SDL2_LIB_DIR})
include_directories(./include ${SDL2_INCLUDE_DIR})

add_library(chip8 ./src/chip8.cpp)
add_library(graphics ./src/graphics.cpp)
add_executable(main ./src/main.cpp)

target_link_libraries(main chip8 graphics SDL2main SDL2) 