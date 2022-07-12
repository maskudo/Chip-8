#define SDL_MAIN_HANDLED
#include "chip8.hpp"
#include "graphics.hpp"
#include <iostream>
int main(int argc, char **argv) {
    const int SCALE = 10;
    const int FREQ = 500;
    const int FPS = 60;
    int delay = 1000 / FREQ;
    Graphics graphics("Chip8", SCALE * 64, SCALE * 32, 64, 32);
    chip8::Chip8 interpreter = chip8::Chip8();
    interpreter.Reset();
    if (!interpreter.LoadROM(argv[1])) {
        std::cerr << "Unable to load" << argv[1] << std::endl;
        return -1;
    }
    // number of bytes in a row
    int videoPitch = sizeof(interpreter.display[0]) * 64;

    bool quit = false;
    int nextFrame = SDL_GetTicks();
    while (!quit) {
        int start = SDL_GetTicks();
        quit = graphics.ProcessInput(interpreter.keypad);

        interpreter.Tick();
        if (SDL_GetTicks() >= nextFrame) {
            graphics.Update(interpreter.display, videoPitch);
            nextFrame = SDL_GetTicks() + (1000 / FPS);
            // std::cout << SDL_GetTicks() << std::endl;
        }

        int totalDelay = SDL_GetTicks() - start;
        if (totalDelay < delay)
            SDL_Delay(delay - totalDelay);
    }

    return 0;
}
