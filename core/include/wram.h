#pragma once

#include <cstdint>

constexpr uint16_t WRAM_START = 0xC000;
constexpr uint16_t WRAM_STOP = 0xDFFF;
constexpr uint16_t ECHO_START = 0xE000;
constexpr uint16_t ECHO_STOP = 0xFDFF;
constexpr uint16_t ECHO_SIZE = 1E00;

struct wram {
	uint8_t wRAM[0x2000] = {};

	uint8_t readU8(uint16_t addr) const;
	void writeU8(uint16_t addr, uint8_t val);

};

inline uint8_t wram::readU8(uint16_t addr) const {
	if (addr >= WRAM_START && addr <= WRAM_STOP) {
		uint16_t relativeAddr = addr - WRAM_START;
		return wRAM[relativeAddr];
	}

	else if (addr >= ECHO_START && addr < ECHO_START + ECHO_SIZE) {
		uint16_t relativeAddr = addr - ECHO_START;
		return wRAM[relativeAddr];
	}

	else {
		return 0;
	}
}

inline void wram::writeU8(uint16_t addr, uint8_t val) {
	if (addr >= WRAM_START && addr <= WRAM_STOP) {
		uint16_t relativeAddr = addr - WRAM_START;
		wRAM[relativeAddr] = val;
	}

	else if (addr >= ECHO_START && addr < ECHO_START + ECHO_SIZE) {
		uint16_t relativeAddr = addr - ECHO_START;
		wRAM[relativeAddr] = val;
	}
}