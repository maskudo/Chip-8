#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "chip8.hpp"
#include <iostream>
int main(int argc, char **argv) {
    // chip8::Chip8 interpreter = chip8::Chip8();
    // interpreter.Reset();
    // if (!interpreter.LoadROM(argv[1])) {
    //     std::cerr << "Unable to load " << argv[1] << std::endl;
    //     return -1;
    // }
    // while (true) {
    //     interpreter.Tick();
    // }
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Quit();
    return 0;
}
