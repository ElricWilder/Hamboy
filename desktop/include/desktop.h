#pragma once

#include <vector>
#include <cstdint>
#include <iterator>
#include <fstream>
#include <stdexcept>
#include <SDL.h>
#include <optional>

static const uint32_t SCALE = 3;
static const uint32_t WINDOW_WIDTH = SCREEN_WIDTH * SCALE;
static const uint32_t WINDOW_HEIGHT = SCREEN_HEIGHT * SCALE;

std::vector<uint8_t> loadROM(const std::string& romPath) {
    std::ifstream file(romPath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Error opening ROM file");
    }

    return std::vector<uint8_t>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

void drawScreen(std::array<uint8_t, DISPLAY_BUFFER>& data, 
                SDL_Renderer* renderer,
                SDL_Texture* texture) {

    SDL_UpdateTexture(texture, nullptr, data.data(), SCREEN_WIDTH * 4);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

std::optional<Buttons> key2btn(SDL_Keycode key) {
        switch (key) {
        case (SDLK_s):
            return Buttons::Down;
        case (SDLK_w):
            return Buttons::Up;
        case (SDLK_a):
            return Buttons::Left;
        case (SDLK_d):
            return Buttons::Right;
        case (SDLK_BACKSPACE):
            return Buttons::Select;
        case (SDLK_RETURN):
            return Buttons::Start;
        case (SDLK_j):
            return Buttons::A;
        case (SDLK_k):
            return Buttons::B;
        default:
            return std::nullopt;
        }
}