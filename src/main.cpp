#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "chip8.hpp"
#include "graphics.hpp"
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
    SDL_Window *window = nullptr;
    const int SCALE = 10;
    Graphics g = Graphics();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Video initialization error: " << SDL_GetError() << std::endl;
    } else {
        window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCALE * 32, SCALE * 64, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            std::cerr << "Window creation error" << SDL_GetError() << std::endl;
        } else {
            SDL_UpdateWindowSurface(window);
            SDL_Delay(2000);
        }
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
