#define SDL_MAIN_HANDLED
#include "chip8.hpp"
#include "graphics.hpp"
#include <chrono>
#include <iostream>
int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <ROM> <Scale> <Delay>\n";
        std::exit(EXIT_FAILURE);
    }

    const int SCALE = std::stoi(argv[2]);
    const float FREQ = std::stoi(argv[3]);
    const float FRAMERATE = 60;
    const float FRAMEDELAY = 1000 / FRAMERATE;
    const float CYCLEDELAY = 1000 / FREQ;

    Graphics graphics = Graphics("Chip8", SCALE * 64, SCALE * 32, 64, 32);
    chip8::Chip8 interpreter = chip8::Chip8();

    interpreter.Reset();
    if (!interpreter.LoadROM(argv[1])) {
        std::cerr << "Unable to load " << argv[1] << std::endl;
        return -1;
    }

    // number of bytes in a row
    int videoPitch = sizeof(interpreter.display[0]) * 64;
    bool quit = false;

    auto displayFunction = [&](){
        graphics.Update(interpreter.display, videoPitch);
    };

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    while (!quit) {
        quit = graphics.ProcessInput(interpreter.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        if (dt > CYCLEDELAY) {
            interpreter.Tick(displayFunction);
            lastCycleTime = currentTime;
        }

        auto currentFrameTime = std::chrono::high_resolution_clock::now();
        float frameDt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentFrameTime - lastFrameTime).count();
        // update timers every 1/60th of a second
        if (frameDt > FRAMEDELAY) {
            interpreter.TickTimer();
            lastFrameTime = currentFrameTime;
        }
    }
    return 0;
}
