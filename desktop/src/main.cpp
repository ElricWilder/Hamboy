#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include <SDL.h>
#include "utils.h"
#include "cpu.h"
#include "bus.h"
#include "cart.h"
#include "ppu.h"
#include "desktop.h"
#include "logger.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <fstream>
#include <iostream>
#include <thread>
#include <windows.h>
#include <imgui_internal.h>

int main(int argc, char *argv[]) {
    using clock = std::chrono::high_resolution_clock;
    ::SetProcessDPIAware();

    Cpu* hamboy = new Cpu();
    bool romLoaded = false;
    std::string lastRomPath = "";

    // SDL init
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    float mainScale = ImGui_ImplSDL2_GetContentScaleForDisplay(0);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Hamboy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE, window_flags);
    
    if (!window) {
        throw std::runtime_error(SDL_GetError());
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!renderer) {
        throw std::runtime_error(SDL_GetError());
    }

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

    // ImGui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;

    ImFont* defaultFont = io.Fonts->AddFontFromFileTTF(
        "C:/Windows/Fonts/segoeui.ttf",
        14.0f * mainScale
    );

    io.FontDefault = defaultFont;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsLight();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(mainScale);
    style.FontScaleDpi = mainScale;

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    bool running = true;
    bool paused = false;
    bool resetClock = false;
    SDL_Event event;
    auto lastFrameTime = clock::now();

    // Main loop
    while (running) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                    break;
                }
                else if (event.key.keysym.sym == SDLK_SPACE) {
                    paused = !paused;
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

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Fullscreen DockSpace root window
        ImGuiWindowFlags dockspaceFlags =
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGui::Begin("DockSpaceRoot", nullptr, dockspaceFlags);
        ImGui::PopStyleVar(2);

        ImGuiID dockspaceId = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspaceId);

        // Menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open ROM")) {
                    std::string path = open_rom_dialog();
                    if (!path.empty()) {
                        // reset emulator before loading new ROM
                        delete hamboy;
                        hamboy = new Cpu();

                        lastRomPath = path;
                        std::vector<uint8_t> rom = loadROM(path);
                        hamboy->loadROM(rom);
                        SDL_SetWindowTitle(window, hamboy->getTitle().c_str());
                        romLoaded = true;

                        lastFrameTime = clock::now();
                    }
                }
                if (!lastRomPath.empty()) {
                    if (ImGui::MenuItem("Reload ROM")) {
                        // reset emulator before reloading ROM
                        delete hamboy;
                        hamboy = new Cpu();

                        std::vector<uint8_t> rom = loadROM(lastRomPath);
                        hamboy->loadROM(rom);
                        romLoaded = true;

                        lastFrameTime = clock::now();
                    }
                }
                else {
                    ImGui::MenuItem("Reload ROM", nullptr, false, false);
                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Game Screen
        ImGuiWindowFlags gameScreenFlags =
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove;

        ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH* SCALE, SCREEN_HEIGHT* SCALE), ImGuiCond_Always);
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Game Screen", nullptr, gameScreenFlags);
        ImGui::PopStyleVar();

        if (paused) {
            resetClock = true;
        } else {
            // reset frame timing when unpausing emulator
            if (resetClock) {
                lastFrameTime = clock::now();
                resetClock = false;
                bool romLoaded = true;
            }
            if (romLoaded) {
                // CPU tick until time to draw
                while (!hamboy->tick()) {};
            }

            // Frame timing, targets 60 FPS default
            const double targetMs = 1000.0 / 59.7275;
            auto now = clock::now();
            double elapsedMs =
                std::chrono::duration<double, std::milli>(now - lastFrameTime).count();

            if (elapsedMs < targetMs) {
                auto sleepMs = targetMs - elapsedMs;
                std::this_thread::sleep_for(
                    std::chrono::duration<double, std::milli>(sleepMs));
                now = clock::now();
            }

            lastFrameTime += std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(std::chrono::duration<double, std::milli>(targetMs));

            if (romLoaded) {
                // Render emulator frame
                std::array<uint8_t, DISPLAY_BUFFER> frame = hamboy->render();
                SDL_UpdateTexture(texture, nullptr, frame.data(), WINDOW_WIDTH);
            }
        }

        // Scale game screen to window
        ImVec2 avail = ImGui::GetContentRegionAvail();

        // Draw inside ImGui window
        ImGui::Image((void*)texture, avail);

        ImGui::End(); // Game screen
        ImGui::End(); // DockSpaceRoot

        // Final rendering 
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        
        SDL_RenderClear(renderer);
        // SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);

    }

    // Clean up
    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}