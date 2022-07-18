#define SDL_MAIN_HANDLED
#include "chip8.hpp"
#include "graphics.hpp"
#include <iostream>
int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <ROM> <Scale> <Delay>\n";
        std::exit(EXIT_FAILURE);
    }

    const int SCALE = std::stoi(argv[2]);
    const int FREQ = std::stoi(argv[3]);
    int delay = 1000 / FREQ;

    Graphics *graphics = new Graphics("Chip8", SCALE * 64, SCALE * 32, 64, 32);
    chip8::Chip8 interpreter = chip8::Chip8();

    interpreter.Reset();
    if (!interpreter.LoadROM(argv[1])) {
        std::cerr << "Unable to load" << argv[1] << std::endl;
        return -1;
    }

    // number of bytes in a row
    int videoPitch = sizeof(interpreter.display[0]) * 64;
    bool quit = false;
    auto framestart = SDL_GetTicks();
    while (!quit) {
        int start = SDL_GetTicks();
        quit = graphics->ProcessInput(interpreter.keypad);

        interpreter.Tick(graphics);
        auto framedelay = SDL_GetTicks() - framestart;
        if (framedelay >= 16) {
            interpreter.TickTimer();
            framestart = SDL_GetTicks();
        }
        int totalDelay = SDL_GetTicks() - start;
        if (totalDelay < delay)
            SDL_Delay(delay - totalDelay);
    }
    delete graphics;
    return 0;
}
