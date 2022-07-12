#define SDL_MAIN_HANDLED
#include "chip8.hpp"
#include "graphics.hpp"
#include <iostream>
int main(int argc, char **argv) {
    const int SCALE = 10;
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

    while (!quit) {
        quit = graphics.ProcessInput(interpreter.keypad);

        interpreter.Tick();

        graphics.Update(interpreter.display, videoPitch);
        SDL_Delay(2);
    }

    return 0;
}
