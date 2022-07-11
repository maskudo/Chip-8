#pragma once
#include "SDL.h"
class Graphics {
private:
    SDL_Window *window{};
    SDL_Renderer *renderer{};
    SDL_Texture *texture{};

public:
    Graphics(char const *title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);

    ~Graphics();

    void Update(void const *buffer, int pitch);
    bool ProcessInput(uint8_t *keys);
};
