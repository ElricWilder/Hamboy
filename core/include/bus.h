#pragma once

#include <cstdint>
#include <array>
#include "cart.h"
#include "ppu.h"
#include "modes.h"
#include "io.h"
#include "wram.h"

constexpr uint16_t OAM_DMA = 0xFF46;
constexpr uint16_t HRAM_START = 0xFF80;
constexpr uint16_t HRAM_STOP = 0xFFFF;
constexpr uint16_t HRAM_SIZE = (HRAM_STOP - HRAM_START + 1);

struct Bus {
	Cart cart;
	Ppu ppu;
	IO io;
	std::array<uint8_t, 0x6000> ram;
	wram wram;
	uint8_t hram[HRAM_SIZE] = {};

	Bus();

	uint8_t readRAM(uint16_t addr) const;
	void writeRAM(uint16_t addr, uint8_t val);
	void loadROM(const std::vector <uint8_t>& romData);
	PpuUpdateResult updatePPU(uint8_t cycles);
	std::array<uint8_t, DISPLAY_BUFFER> render();
	void pressButton(Buttons button, bool pressed);
	void dmaTransfer(uint8_t val);
	bool updateTimer(uint8_t cycles);
	std::string getTitle() const;
	void renderScanline();
};