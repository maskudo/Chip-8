
#pragma once
#include <cstdint>

struct Chip8
{
public:
    uint8_t ram[4096];
    uint8_t font[80];
};
void hello();