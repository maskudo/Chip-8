#include "chip8.hpp"
#include <fstream>
#include <iostream>
#include <stdlib.h>
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

    for (auto &data : display) {
        data = 0;
    }
    for (auto &i : registers) {
        i = 0;
    }
    for (auto &i : stack) {
        i = 0;
    }
    for (auto &i : keypad) {
        i = 0;
    }
    // clear memory and insert font
    for (auto &i : memory) {
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
void Chip8::Tick(Graphics *graphicsPtr) {
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
            for (auto &i : display) {
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
        if (registers[(0x0F00 & opcode) >> 8] == registers[(0x00F0 & opcode) >> 4]) {
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
            uint8_t Vx = (opcode & 0x0F00u) >> 8u;
            uint8_t Vy = (opcode & 0x00F0u) >> 4u;

            uint16_t sum = registers[Vx] + registers[Vy];
            auto carry = 0;
            if (sum > 255U) {
                carry = 1;
            }
            registers[Vx] = sum & 0xFFu;
            registers[0xF] = carry;
            break;
        }
        // 8xy5 - SUB Vx, Vy Set Vx = Vx - Vy, set VF = NOT borrow.
        case 0x5: {
            uint8_t Vx = (opcode & 0x0F00u) >> 8u;
            uint8_t Vy = (opcode & 0x00F0u) >> 4u;
            auto borrow = 0;
            if (registers[Vx] > registers[Vy]) {
                borrow = 1;
            }
            registers[Vx] -= registers[Vy];
            registers[0xF] = borrow;
            // registers[0xF] = ((registers[(0x0F00 & opcode) >> 8] < registers[(0x00F0 & opcode) >> 4]));
            // registers[(0x0F00 & opcode) >> 8] -= registers[(0x00F0 & opcode) >> 4];
            break;
        }
        // 8xy6 - SHR Vx {, Vy}  Set Vx = Vx SHR 1.
        // If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
        case 0x6: {
            auto shifted = registers[(0x0F00 & opcode) >> 8] & 0x1;
            registers[(0x0F00 & opcode) >> 8] >>= 1;
            registers[0xF] = shifted;
            break;
        }
        // 8xy7 - SUBN Vx, Vy    Set Vx = Vy - Vx, set VF = NOT borrow.
        // If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
        case 0x7: {
            auto shifted = ((registers[(0x0F00 & opcode) >> 8] < registers[(0x00F0 & opcode) >> 4]));
            registers[(0x0F00 & opcode) >> 8] = registers[(0x00F0 & opcode) >> 4] - registers[(0x0F00 & opcode) >> 8];
            registers[0xF] = shifted;
            break;
        }
        // 8xyE - SHL Vx {, Vy}  Set Vx = Vx SHL 1.
        // If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
        case 0xE: {
            auto shifted = registers[(0x0F00 & opcode) >> 8] >> 7;
            registers[(0x0F00 & opcode) >> 8] <<= 1;
            registers[0xF] = shifted;
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
    // Bnnn - JP V0, addr
    // Jump to location nnn + V0.
    case 0xB000: {
        pc = registers[0x0000] + (opcode & 0x0FFF);
        break;
    }
    // Cxkk - RND Vx, byte
    // Set Vx = random byte AND kk.
    case 0xC000: {
        registers[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
        break;
    }
    // Dxyn - DRW Vx, Vy, nibble
    // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
    case 0xD000: {
        auto vx = registers[(0x0F00 & opcode) >> 8];
        auto vy = registers[(0x00F0 & opcode) >> 4];
        auto x = vx % 64;
        auto y = vy % 32;
        uint8_t height = opcode & 0x000F;
        registers[0xF] = 0;
        for (int row = 0; row < height; row++) {
            auto pixel = memory[index + row];
            for (int col = 0; col < 8; col++) {
                if ((pixel & (0x80 >> col))) {
                    // change 1 to 0xFFFFFFFF for SDL2
                    if (display[((x + col) + ((y + row) * 64))] == 0xFFFFFFFF) {
                        registers[0xF] = 1;
                    }
                    display[((x + col) + ((y + row) * 64))] ^= 0xFFFFFFFF;
                }
            }
        }
        int videoPitch = sizeof(display[0]) * 64;
        graphicsPtr->Update(display, videoPitch);
        break;
    }
    // Ex9E - SKP Vx
    // Skip next instruction if key with the value of Vx is pressed.
    case 0xE000: {
        switch (opcode & 0x00FF) {
        case 0x9E: {
            auto key = registers[(0x0F00 & opcode) >> 8];
            if (keypad[key]) {
                pc += 2;
            }
            break;
        }
        // ExA1 - SKNP Vx
        // Skip next instruction if key with the value of Vx is not pressed.
        case 0x00A1: {
            auto key = registers[(0x0F00 & opcode) >> 8];
            if (!keypad[key]) {
                pc += 2;
            }
            break;
        }
        default: {
            invalid = true;
            break;
        }
        }

        break;
    }

    case 0xF000: {
        switch (0x00FF & opcode) {
        // Fx07 - LD Vx, DT
        // Set Vx = delay timer value.
        case 0x0007: {
            registers[(0x0F00 & opcode) >> 8] = delayTimer;
            break;
        }
        // Fx0A - LD Vx, K
        // Wait for a key press, store the value of the key in Vx.
        case 0x000A: {
            bool keypress = false;
            for (int i = 0; i < 16; i++) {
                if (keypad[i]) {
                    registers[(opcode & 0x0F00) >> 8] = i;
                    keypress = true;
                }
            }
            if (!keypress) {
                pc -= 2;
            }
            break;
        }
        // Fx15 - LD DT, Vx
        // Set delay timer = Vx.
        case 0x0015: {
            delayTimer = registers[(opcode & 0x0F00) >> 8];
            break;
        }
        // Fx18 - LD ST, Vx
        // Set sound timer = Vx.
        case 0x0018: {
            soundTimer = registers[(opcode & 0x0F00) >> 8];
            break;
        }
        // Fx1E - ADD I, Vx
        // Set I = I + Vx.
        // some interpreters set VF to 1 if I “overflows” from 0FFF to above 1000
        // Spacefight 2091!, relies on this behavior.
        case 0x001E: {
            if (index + registers[(opcode & 0x0F00) >> 8] > 0xFFF) {
                registers[0xF] = 1;
            } else {
                registers[0xF] = 0;
            }
            index += registers[(opcode & 0x0F00) >> 8];
            break;
        }
        // Fx29 - LD F, Vx
        // Set I = location of sprite for digit Vx.
        case 0x0029: {
            // Sprites are stored from 0x0000 to 0x0200. Each sprite is of 5 bytes
            index = registers[(opcode & 0x0F00) >> 8] * 0x5;
            break;
        }
        // Fx33 - LD B, Vx
        // Store BCD representation of Vx in memory locations I, I+1, and I+2.
        case 0x0033: {
            auto value = registers[(opcode & 0x0F00) >> 8];
            memory[index + 2] = value % 10;
            value /= 10;
            memory[index + 1] = value % 10;
            value /= 10;
            memory[index] = value % 10;
            break;
        }
        // Fx55 - LD [I], Vx
        // Store registers V0 through Vx in memory starting at location I.
        case 0x0055: {
            for (int i = 0; i <= (opcode & 0x0F00) >> 8; i++) {
                memory[index + i] = registers[i];
            }
            break;
        }
        // Fx65 - LD Vx, [I]
        // Read registers V0 through Vx from memory starting at location I.
        case 0x0065: {
            for (int i = 0; i <= (opcode & 0x0F00) >> 8; i++) {
                registers[i] = memory[index + i];
            }
            break;
        }
        default: {
            invalid = true;
            break;
        }
        }
        break;
    }
    default: {
        invalid = true;
        break;
    }
    }
    if (invalid) {
        std::cerr << "Invalid opcode: " << opcode << std::endl;
    }
    // std::cout << std::hex << pc << " " << opcode << std::endl;
}
void Chip8::TickTimer() {
    if (delayTimer > 0) {
        delayTimer--;
    }
    if (soundTimer > 0) {
        soundTimer--;
    }
}
} // namespace chip8