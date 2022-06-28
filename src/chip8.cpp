#include "chip8.hpp"
#include <fstream>
#include <iostream>
using std::cout;
using std::endl;

namespace chip8 {
void Chip8::Reset() {
    // pc starts at 0x200
    pc = 0x200;
    index = 0;
    sp = 0;
    opcode = 0;

    delayTimer = 0;
    soundTimer = 0;

    for (uint32_t &data : display) {
        data = 0;
    }
    for (uint16_t &i : registers) {
        i = 0;
    }
    for (uint16_t &i : stack) {
        i = 0;
    }
    // clear memory and insert font
    for (uint8_t &i : memory) {
        i = 0;
    }
    for (int i = 0; i < 80; i++) {
        memory[i] = font[i];
    }
}
bool Chip8::LoadROM(const char *filename) {
    std::ifstream ifile;

    ifile.open(filename, std::ios::binary);

    if (!ifile) {
        return false;
    }

    int i = 0;
    char b;

    while (ifile.get(b)) {
        // 0x200 = 500
        memory[i + 512] = b;
        cout << b << "," << memory[i + 512] << endl;
        i++;
    }

    ifile.close();

    return true;
}
void Chip8::stackPush(uint16_t data) {
    stack[sp] = data;
    sp++;
}

uint16_t Chip8::stackPop() {
    sp--;
    return stack[sp];
}
void Chip8::Tick() {
    // left shift first byte and OR second byte for 16 bit instruction
    // big endian
    opcode = memory[pc] << 8 | memory[pc + 1];
    bool invalid = false;
    pc += 2;
    switch (opcode & 0xF000) {
    case 0x0000: {
        switch (opcode) {
        // 00E0 - CLS clear screen
        case 0x00E0: {
            for (uint32_t &i : display) {
                i = 0;
            }
            break;
        }
        //  00EE - RET instruction, set pc to TOS
        case 0x00EE: {
            pc = stackPop();
            break;
        }

        default: {
            invalid = true;
            break;
        }
        }

        break;
    }
    //  1nnn - JP addr set pc to address nnn
    case 0x1000: {
        pc = opcode & 0x0FFF;
        break;
    }

    //  2nnn - CALL addr push to stack and jump to address nnn
    case 0x2000: {
        stackPush(pc);
        pc = opcode & 0x0FFF;
        break;
    }
    //  3xkk - SE Vx, byte   Skip next instruction if Vx = kk.
    case 0x3000: {
        if (registers[(0x0F00 & opcode) >> 8] == (0x00FF & opcode))
            pc += 2;
        break;
    }
    // 4xkk - SNE Vx, byte   Skip next instruction if Vx != kk.
    case 0x4000: {
        if (registers[(0x0F00 & opcode) >> 8] != (0x00FF & opcode))
            pc += 2;
        break;
    }
    // 5xy0 - SE Vx, Vy  Skip next instruction if Vx = Vy.
    case 0x5000: {
        if (registers[(0x0F00 & opcode) >> 8] == registers[0x00F0 & opcode] >> 4) {
            pc += 2;
        }
        break;
    }
    // 6xkk - LD Vx, byte   Set Vx = kk.
    case 0x6000: {
        registers[(0x0F00 & opcode) >> 8] = 0x00FF & opcode;
        break;
    }
    // 7xkk - ADD Vx, byte   Set Vx = Vx + kk.
    case 0x7000: {
        registers[(0x0F00 & opcode) >> 8] += 0x00FF & opcode;
        break;
    }
    case 0x8000: {
        switch (opcode & 0x000F) {
        // 8xy0 - LD Vx, Vy    Set Vx = Vy.
        case 0x0: {
            registers[(0x0F00 & opcode) >> 8] = registers[(0x00F0 & opcode) >> 4];
            break;
        }
        // 8xy1 - OR Vx, Vy  Set Vx = Vx OR Vy.
        case 0x1: {
            registers[(0x0F00 & opcode) >> 8] |= registers[(0x00F0 & opcode) >> 4];
            break;
        }
        // 8xy2 - AND Vx, Vy Set Vx = Vx AND Vy.
        case 0x2: {
            registers[(0x0F00 & opcode) >> 8] &= registers[(0x00F0 & opcode) >> 4];
            break;
        }
        // 8xy3 - XOR Vx, Vy    Set Vx = Vx XOR Vy.
        case 0x3: {
            registers[(0x0F00 & opcode) >> 8] ^= registers[(0x00F0 & opcode) >> 4];
            break;
        }
        // 8xy4 - ADD Vx, Vy Set Vx = Vx + Vy, set VF = carry.
        case 0x4: {
            // high chance i made a mistake
            registers[0xF] = (0xFF < (registers[(0x0F00 & opcode) >> 8] + registers[(0x00F0 & opcode) >> 4]));
            registers[(0x0F00 & opcode) >> 8] += registers[(0x00F0 & opcode) >> 4];
            break;
        }
        // 8xy5 - SUB Vx, Vy Set Vx = Vx - Vy, set VF = NOT borrow.
        case 0x5: {
            registers[0xF] = ((registers[(0x0F00 & opcode) >> 8] < registers[(0x00F0 & opcode) >> 4]));
            registers[(0x0F00 & opcode) >> 8] -= registers[(0x00F0 & opcode) >> 4];
            break;
        }
        // 8xy6 - SHR Vx {, Vy}  Set Vx = Vx SHR 1.
        // If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
        case 0x6: {
            registers[0xF] = registers[(0x0F00 & opcode) >> 8] & 0x1;
            registers[(0x0F00 & opcode) >> 8] >>= 1;
            break;
        }
        // 8xy7 - SUBN Vx, Vy    Set Vx = Vy - Vx, set VF = NOT borrow.
        // If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
        case 0x7: {
            registers[0xF] = ((registers[(0x0F00 & opcode) >> 8] < registers[(0x00F0 & opcode) >> 4]));
            registers[(0x0F00 & opcode) >> 8] = registers[(0x00F0 & opcode) >> 4] - registers[(0x0F00 & opcode) >> 8];
            break;
        }
        // 8xyE - SHL Vx {, Vy}  Set Vx = Vx SHL 1.
        // If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
        case 0xE: {
            registers[0xF] = registers[(0x0F00 & opcode) >> 8] >> 7;
            registers[(0x0F00 & opcode) >> 8] <<= 1;
            break;
        }
        default: {
            invalid = true;
            break;
        }
        }
        break;
    }
    // 9xy0 - SNE Vx, Vy Skip next instruction if Vx != Vy.
    case 0x9000: {
        if (registers[(0x0F00 & opcode) >> 8] != (registers[(0x00F0 & opcode) >> 4]))
            pc += 2;
        break;
    }
    // Annn - LD I, addr Set Index = nnn.
    case 0xA000: {
        index = opcode & 0x0FFF;
        break;
    }
    // Dxyn - DRW Vx, Vy, nibble
    // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
    case 0xD000: {
        auto vx = registers[(0x0F00 & opcode) >> 8];
        auto vy = registers[(0x00F0 & opcode) >> 4];
        auto x = vx % 64;
        auto y = vx % 32;
        uint8_t height = opcode & 0x000F;
        registers[0xF] = 0;
        for (int yLine = 0; yLine < height; yLine++) {
            auto pixel = memory[index + yLine];
            for (int xLine = 0; xLine < 8; xLine++) {
                if ((pixel & (0x8000 >> xLine)) != 0) {
                    if (display[(x + xLine + ((y + yLine) * 64))] == 1) {
                        registers[0xf] = 1;
                    }
                    display[x + xLine + ((y + yLine) * 64)] ^= 1;
                }
            }
        }
        break;
    }
    default:
        break;
    }
}
} // namespace chip8