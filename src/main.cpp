#include <iostream>
#include "chip8.hpp"
int main(int argc, char **argv)
{
    chip8::Chip8 interpreter = chip8::Chip8();
    interpreter.Reset();
    if (!interpreter.LoadROM(argv[1]))
    {
        std::cerr << "Unable to load " << argv[1] << std::endl;
        return -1;
    }
}
