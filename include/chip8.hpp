#pragma once
#include <functional>
#include <array>
#include <cstdint>

namespace chip8 {
class Chip8 {
public:
    uint8_t font[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80, // F
    };
    std::array<uint8_t, 16> registers;
    std::array<uint16_t, 16> stack;
    std::array<uint8_t, 4096> memory;
    uint8_t keypad[16];
    uint32_t display[64 * 32];

    uint16_t index{};
    uint16_t pc{};
    // 8 bit stack pointer for 16 stack addresses
    uint8_t sp{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};

    uint16_t opcode;

    // methods
    bool LoadROM(const char *filename);
    void Reset();
    void stackPush(uint16_t data);
    uint16_t stackPop();
    void Tick(std::function<void(void)>);
    void TickTimer();
};
} // namespace chip8
