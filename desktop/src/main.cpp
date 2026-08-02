#include "../../core/include/utils.h"
#include "../../core/include/cpu.h"
#include "../../core/include/bus.h"
#include "../../core/include/cart.h"
#include "../../core/include/ppu.h"
#include "desktop.h"
#include <filesystem>
#include "logger.h"
#include <cstdint>
#include <SDL.h>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
    initLogger();

    std::string romPath = "C:/Users/elric/source/repos/gameboyemu/roms/dmg-acid2.gb";
    //std::cout << "Trying to load ROM from: " << std::filesystem::absolute(romPath) << "\n";
    bool running = true;
    SDL_Event event;

    // SDL window setup 
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error(SDL_GetError());
    }

    const int WINDOW_WIDTH = 160 * 4;
    const int WINDOW_HEIGHT = 144 * 4;

    SDL_Window* window = SDL_CreateWindow(
        "HamBoy",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL
    );

    if (!window) {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    if (!texture) {
        throw std::runtime_error(SDL_GetError());
    }

    std::vector<uint8_t> rom = loadROM(romPath);
    //std::cout << "ROM size = " << rom.size() << " bytes\n";
    Cpu* hamboy = new Cpu();
    hamboy->loadROM(rom);
    std::string title = hamboy->getTitle();
    SDL_SetWindowTitle(window, title.c_str());
    Uint32 fmt;
    SDL_QueryTexture(texture, &fmt, nullptr, nullptr, nullptr);
    SDL_Log("Texture format: %s", SDL_GetPixelFormatName(fmt));
    // Main loop
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                        break;
                    }
                    else {
                        if (auto btn = key2btn(event.key.keysym.sym)) {
                            hamboy->pressButton(*btn, true);
                        }
                    }
                    break;
                case SDL_KEYUP:
                    if (auto btn = key2btn(event.key.keysym.sym)) {
                        hamboy->pressButton(*btn, false);
                    }
            }
        }

        // CPU tick until time to draw
         while (!hamboy->tick()) {};
        // Frame buffer contains RGBA vals for for each of the 160x144 pixels
        std::array<uint8_t, DISPLAY_BUFFER> frame = hamboy->render();
        drawScreen(frame, renderer, texture);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    closeLogger();
}

