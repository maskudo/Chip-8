#include "chip8.hpp"
#include <iostream>
using std::cout;
using std::endl;

void hello()
{
    cout << "hello from chip8" << endl;
    getchar();
}
namespace chip8
{
    void Chip8::Reset()
    {
        // pc starts at 0x200
        pc = 0x200;
        index = 0;
        sp = 0;
        opcode = 0;

        delayTimer = 0;
        soundTimer = 0;

        for (uint32_t &data : display)
        {
            data = 0;
        }
        for (uint16_t &i : registers)
        {
            i = 0;
        }
        for (uint16_t &i : stack)
        {
            i = 0;
        }
        // clear memory and insert font
        for (uint8_t &i : memory)
        {
            i = 0;
        }
        for (int i = 0; i < 80; i++)
        {
            memory[i] = chip8::fontset[i];
        }
    }
    Chip8::Chip8()
    {
        // initialize font
        for (int i = 0; i < 80; i++)
        {
            memory[i] = chip8::fontset[i];
        }
    }
}