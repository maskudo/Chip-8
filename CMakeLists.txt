cmake_minimum_required(VERSION 3.15)
project(chip8)

#set c++ standard
set(CMAKE_CXX_STANDARD 17)
set(SDL2_DIR vcpkg_installed/x64-windows/share/sdl2/)
set(CMAKE_FIND_DEBUG_MODE 1)
find_package(SDL2 CONFIG REQUIRED)

include_directories(./include)

add_library(chip8 ./src/chip8.cpp)
add_executable(main ./src/main.cpp)

target_link_libraries(main PRIVATE SDL2::SDL2 SDL2::SDL2main)