#include "chip8.hpp"
#include <iostream>
#include <fstream>
using std::cout;
using std::endl;

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
            memory[i] = font[i];
        }
    }
    bool Chip8::LoadROM(const char *filename)
    {
        std::ifstream ifile;

        ifile.open(filename, std::ios::binary);

        if (!ifile)
        {
            return false;
        }

        int i = 0;
        char b;

        while (ifile.get(b))
        {
            // 0x200 = 500
            memory[i + 512] = b;
            cout << b << "," << memory[i + 512] << endl;
            i++;
        }

        ifile.close();

        return true;
    }
    void Chip8::stackPush(uint16_t data)
    {
        stack[sp] = data;
        sp++;
    }

    uint16_t Chip8::stackPop()
    {
        sp--;
        return stack[sp];
    }
    void Chip8::Tick()
    {
        // left shift first byte and OR second byte for 16 bit instruction
        // big endian
        opcode = memory[pc] << 8 | memory[pc + 1];
        bool invalid = false;

        switch (opcode & 0xF000)
        {
        case 0x0000:
        {
            switch (opcode)
            {
            // clear screen
            case 0x00E0:
            {
                for (uint32_t &i : display)
                {
                    i = 0;
                }
                pc += 2;
                break;
            }

            default:
                break;
            }

            break;
        }

        default:
            break;
        }
    }
}